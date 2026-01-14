
/**
 * @file s21_from_float_to_decimal.c
 * @brief Реализация функции конвертации float в decimal
 * @details Конвертирует 32-битное число с плавающей точкой в decimal формат
 *          с сохранением максимальной точности
 */

#include <math.h>    // Математические функции (isnan, isinf)
#include <stdint.h>  // Типы фиксированной ширины

#include "../s21_decimal.h"

typedef uint32_t U32;  // Сокращение для 32-битного беззнакового типа

/**
 * @brief Конвертировать 64-битное число в 96-битный формат
 * @param v 64-битное число
 * @param a Выходной 96-битный массив
 * @details Записывает число в младшие 64 бита, старшие 32 бита = 0
 */
static inline void u96_from_u64(uint64_t v, U32 a[3]) {
  a[0] = (U32)(v & 0xFFFFFFFFu);          // Младшие 32 бита
  a[1] = (U32)((v >> 32) & 0xFFFFFFFFu);  // Старшие 32 бита
  a[2] = 0u;                              // Обнуляем самый старший разряд
}

/**
 * @brief Прибавить малое число к 96-битному числу
 * @param a 96-битное число (изменяется)
 * @param add Прибавляемое число
 * @return Перенос за пределы 96 бит
 * @details Используется для округления и коррекции
 */
static U32 u96_add_small(U32 a[3], U32 add) {
  uint64_t carry = add;  // Начальное значение переноса

  // Проходим по всем разрядам, пока есть перенос
  for (int i = 0; i < 3 && carry; ++i) {
    uint64_t cur = (uint64_t)a[i] + carry;  // Прибавляем перенос
    a[i] = (U32)(cur & 0xFFFFFFFFu);        // Младшие 32 бита
    carry = cur >> 32;                      // Новый перенос
  }
  return (U32)carry;  // Возвращаем финальный перенос
}

/**
 * @brief Умножить 96-битное число на малое число
 * @param a 96-битное число (изменяется)
 * @param k Множитель
 * @return Старшие биты произведения (переполнение)
 * @details Используется для умножения на степени 10
 */
static U32 u96_mul_small(U32 a[3], U32 k) {
  uint64_t carry = 0u;  // Перенос между разрядами

  // Умножаем каждый разряд на k с учетом переноса
  for (int i = 0; i < 3; ++i) {
    uint64_t cur = (uint64_t)a[i] * (uint64_t)k + carry;
    a[i] = (U32)(cur & 0xFFFFFFFFu);  // Младшие 32 бита результата
    carry = cur >> 32;                // Старшие 32 бита - это перенос
  }
  return (U32)carry;  // Возвращаем финальный перенос
}

/**
 * @brief Разделить 96-битное число на 10 с банковским округлением
 * @param a 96-битное число (изменяется)
 * @details Выполняет деление с округлением по правилам банковского округления
 */
static void u96_round_div10(U32 a[3]) {
  U32 r = u96_div10(a);  // Делим на 10, получаем остаток

  if (r > 5) {
    // Остаток > 5 - округляем вверх
    (void)u96_add_small(a, 1u);
  } else if (r == 5) {
    // Остаток = 5 - банковское округление (к четному)
    if (a[0] & 1u)
      (void)u96_add_small(a, 1u);  // Если нечетное - округляем вверх
  }
  // Если остаток < 5 - не округляем
}

/**
 * @brief Умножить 96-битное число на 10^k
 * @param a 96-битное число (изменяется)
 * @param k Показатель степени (количество умножений на 10)
 * @return 0 при успехе, 1 при переполнении
 * @details Используется для увеличения масштаба числа
 */
static int u96_mul_pow10(U32 a[3], int k) {
  for (int i = 0; i < k; ++i) {
    if (u96_mul_small(a, 10u) != 0u) return 1;  // Переполнение
  }
  return 0;  // Успех
}

static uint64_t round_ties_to_even(double x) {
  double f = floor(x);
  double frac = x - f;
  const double eps = 1e-12;
  if (frac > 0.5 + eps) return (uint64_t)f + 1u;
  if (frac < 0.5 - eps) return (uint64_t)f;

  uint64_t q = (uint64_t)f;
  return (q & 1u) ? (q + 1u) : q;
}

#if 0
static U32 u96_add_small(U32 a[3], U32 add) { }
static U32 u96_divmod10(U32 a[3])           { }
static U32 u96_mod10(const U32 in[3])       { }
static void u96_round_div10(U32 a[3])       { }
#endif

int s21_from_float_to_decimal(float src, s21_decimal* dst) {
  if (!dst) return 1;
  s21_reset_value(dst);

  if (isnan(src) || isinf(src)) return 1;

  int sign = (src < 0.0f);
  double y = fabs((double)src);

  if (y == 0.0) {
    return 0;
  }

  if (y < 1e-28) return 1;

  const double DECIMAL_MAX_D = 7.922816251426434e28;
  if (y > DECIMAL_MAX_D) return 1;

  int exp10 = (int)floor(log10(y));
  double pow_neg = pow(10.0, -exp10);
  double norm = y * pow_neg;

  if (norm >= 10.0) {
    norm /= 10.0;
    exp10 += 1;
  }
  if (norm < 1.0) {
    norm *= 10.0;
    exp10 -= 1;
  }

  const int digits = 7;

  double scaled = norm * pow(10.0, digits - 1);

  uint64_t m7 = round_ties_to_even(scaled);

  if (m7 == 10000000ull) {
    m7 = 1000000ull;
    exp10 += 1;
  }

  int scale = (digits - 1) - exp10;

  U32 N[3];
  u96_from_u64(m7, N);

  if (scale < 0) {
    int k = -scale;
    if (u96_mul_pow10(N, k) != 0) {
      return 1;
    }
    scale = 0;
  }

  while (scale > 28) {
    u96_round_div10(N);
    scale -= 1;
  }

  u96_to_dec(N, dst);
  s21_set_scale(dst, scale);
  s21_set_sign(dst, sign);

  if (u96_is_zero(N)) {
    s21_reset_value(dst);
  }
  return 0;
}
