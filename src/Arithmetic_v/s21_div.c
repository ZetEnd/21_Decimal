/**
 * @file s21_div.c
 * @brief Реализация функции деления decimal чисел
 * @details Содержит алгоритм деления с поддержкой точности до 28 знаков
 *          после запятой и банковского округления
 */

#include "../s21_decimal.h"

typedef uint32_t u32;  // Сокращение для 32-битного беззнакового типа

/**
 * @brief Сдвиг 96-битного числа влево на 1 бит
 * @param a 96-битное число (изменяется)
 * @return Бит, который "выпал" из старшего разряда
 * @details Используется в алгоритме деления для сдвига остатка
 */
static inline u32 u96_shl1(u32 a[3]) {
  u32 c0 = (a[0] >> 31) & 1u;  // Бит перехода из 0-го разряда в 1-й
  u32 c1 = (a[1] >> 31) & 1u;  // Бит перехода из 1-го разряда во 2-й
  u32 c2 = (a[2] >> 31) & 1u;  // Бит, который "выпадает" из числа

  a[0] <<= 1;               // Сдвигаем младший разряд
  a[1] = (a[1] << 1) | c0;  // Сдвигаем средний + бит от младшего
  a[2] = (a[2] << 1) | c1;  // Сдвигаем старший + бит от среднего

  return c2;  // Возвращаем "выпавший" бит
}

/**
 * @brief Установка младшего бита 96-битного числа
 * @param a 96-битное число
 * @param bit Значение бита (0 или 1)
 * @details Используется для записи очередного бита результата деления
 */
static inline void u96_or1(u32 a[3], u32 bit) {
  a[0] |= (bit & 1u);  // Устанавливаем младший бит
}

/**
 * @brief Деление 96-битного числа на 96-битное с получением частного и остатка
 * @param a_in Делимое (96 бит)
 * @param b_in Делитель (96 бит)
 * @param q_out Частное (результат, 96 бит)
 * @param r_out Остаток (96 бит)
 * @details Реализует алгоритм деления "в столбик" для двоичных чисел
 */
static void u96_divmod(const u32 a_in[3], const u32 b_in[3], u32 q_out[3],
                       u32 r_out[3]) {
  // Создаем рабочие копии входных данных
  u32 a[3] = {a_in[0], a_in[1], a_in[2]};  // Делимое
  u32 b[3] = {b_in[0], b_in[1], b_in[2]};  // Делитель
  u32 q[3] = {0, 0, 0};                    // Частное
  u32 r[3] = {0, 0, 0};                    // Остаток

  // Обрабатываем каждый бит делимого от старшего к младшему
  for (int bit = 95; bit >= 0; --bit) {
    u32 carry = u96_shl1(r);  // Сдвигаем остаток влево (получаем перенос)
    (void)carry;              // Игнорируем перенос (не используется)

    // Определяем позицию текущего бита в массиве
    int limb = bit / 32;  // Номер элемента массива
    int off = bit % 32;   // Позиция бита в элементе

    // Извлекаем текущий бит делимого
    u32 abit = (a[limb] >> off) & 1u;
    u96_or1(r, abit);  // Добавляем его к остатку

    // Если остаток >= делителя, то можем вычесть
    if (u96_compare(r, b) >= 0) {
      (void)u96_sub(r, b);     // Вычитаем делитель из остатка
      q[limb] |= (1u << off);  // Устанавливаем соответствующий бит частного
    }
  }

  // Копируем результаты в выходные параметры
  q_out[0] = q[0];
  q_out[1] = q[1];
  q_out[2] = q[2];
  r_out[0] = r[0];
  r_out[1] = r[1];
  r_out[2] = r[2];
}

/**
 * @brief Определение необходимости банковского округления
 * @param R Остаток от деления
 * @param D Делитель
 * @param curQ Текущее частное
 * @return 1 если нужно округлить вверх, 0 иначе
 * @details Реализует правило банковского округления для деления
 */
static int banker_should_increment(const u32 R[3], const u32 D[3],
                                   const u32 curQ[3]) {
  int result = 0;
  u32 tmp[3] = {R[0], R[1], R[2]};  // Копия остатка
  (void)u96_mul10(tmp);             // Умножаем остаток на 10

  u32 qd[3], rd[3];
  u96_divmod(tmp, D, qd, rd);
  u32 remain = qd[0];
  int tail_nonzero = !(rd[0] == 0u && rd[1] == 0u && rd[2] == 0u);
  if (remain < 5u)
    result = 0;
  else if (remain > 5u || tail_nonzero)
    result = 1;
  else
    result = (curQ[0] & 1u) != 0u;
  return result;
}

static int handle_div_overflow_and_round(s21_decimal* out, int S, int sign,
                                         const u32 R[3], const u32 D[3]) {
  int result = 0;
  int inc = banker_should_increment(
      R, D, (u32[]){(u32)out->bits[0], (u32)out->bits[1], (u32)out->bits[2]});
  if (inc) {
    u32 one[3] = {1, 0, 0};
    if (u96_add((u32*)&out->bits[0], one)) {
      if (S) {
        s21_scale_down_one_banker(out);
        u96_add((u32*)&out->bits[0], one);
      } else
        result = sign ? 2 : 1;
    }
  }
  return result;
}

/**
 * @brief Деление двух decimal чисел
 * @param value_1 Делимое
 * @param value_2 Делитель
 * @param result Указатель для сохранения результата
 * @return 0 - успех, 1 - слишком большое число, 2 - слишком маленькое число, 3
 * - деление на ноль
 * @details Выполняет точное деление с поддержкой до 28 знаков после запятой
 */
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  int err = 0;

  // Проверка корректности указателя результата
  if (!result) return 1;

  // Проверка деления на ноль
  if (value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0) {
    return 3;  // Ошибка: деление на ноль
  }

  // Если делимое равно нулю, результат тоже ноль
  if (value_1.bits[0] == 0 && value_1.bits[1] == 0 && value_1.bits[2] == 0) {
    s21_reset_value(result);  // Обнуляем результат
    return 0;
  }

  // Вычисляем знак результата (XOR знаков операндов)
  int sign = s21_get_sign(&value_1) ^ s21_get_sign(&value_2);

  // Удаляем незначащие нули у обоих чисел
  s21_strip_trailing_zeros(&value_1);
  s21_strip_trailing_zeros(&value_2);

  // Получаем масштабы чисел
  int Sa = s21_get_scale(&value_1);  // Масштаб делимого
  int Sb = s21_get_scale(&value_2);  // Масштаб делителя
  int E = Sa - Sb;

  u32 Q[3] = {0, 0, 0};
  u32 R[3] = {0, 0, 0};
  u32 N[3], D[3];
  u96_from_dec(&value_1, N);
  u96_from_dec(&value_2, D);

  u96_divmod(N, D, Q, R);

  s21_decimal out = {{0, 0, 0, 0}};
  u96_to_dec(Q, &out);
  int S = 0;

  while (!u96_is_zero(R) && S < S21_SCALE_MAX) {
    u32 tmp[3] = {(u32)out.bits[0], (u32)out.bits[1], (u32)out.bits[2]};
    if (u96_mul10(tmp)) {
      err = handle_div_overflow_and_round(&out, S, sign, R, D);
      if (err) return err;
      break;
    }

    u32 r10[3] = {R[0], R[1], R[2]};
    (void)u96_mul10(r10);

    u32 qd[3], rd[3];
    u96_divmod(r10, D, qd, rd);
    u32 digit = qd[0];

    u32 digit_u96[3] = {digit, 0, 0};
    if (u96_add(tmp, digit_u96)) {
      err = handle_div_overflow_and_round(&out, S, sign, R, D);
      if (err) return err;
      break;
    }
    u96_to_dec(tmp, &out);
    S++;
    u96_copy(R, rd);
  }

  if (E > 0) {
    int targetS = S + E;
    while (targetS > S21_SCALE_MAX) {
      s21_scale_down_one_banker(&out);
      targetS--;
    }
    S = targetS;
  } else if (E < 0) {
    int need = -E;
    while (need > 0) {
      u32 a[3] = {(u32)out.bits[0], (u32)out.bits[1], (u32)out.bits[2]};
      if (u96_mul10(a)) {
        if (S == 0) {
          return sign ? 2 : 1;
        }

        s21_set_scale(&out, S);
        s21_scale_down_one_banker(&out);
        S = s21_get_scale(&out);
      } else {
        u96_to_dec(a, &out);
        need--;
      }
    }
  }

  s21_set_scale(&out, S);
  s21_set_sign(&out, sign);

  if (s21_is_zero(&out)) {
    s21_set_sign(&out, 0);
  }

  *result = out;
  return 0;
}
