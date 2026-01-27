/**
 * @file s21_utilities.c
 * @brief Вспомогательные функции для работы с decimal числами
 * @details Содержит функции для работы с масштабом, знаком, 96-битными числами
 *          и другие утилиты для внутреннего использования библиотеки
 */

#include "s21_decimal.h"

// Макрос для подавления предупреждений о неиспользуемых переменных
#ifndef S21_UNUSED
#if defined(__GNUC__) || defined(__clang__)
#define S21_UNUSED __attribute__((unused))
#else
#define S21_UNUSED
#endif
#endif

/**
 * @brief Получить масштаб decimal числа
 * @param d Указатель на decimal число
 * @return Значение масштаба (0-28), или 0 если указатель некорректен
 * @details Извлекает масштаб из битов 16-23 четвертого элемента
 */
int s21_get_scale(const s21_decimal* d) {
  return (int)(d != 0) *  // Если указатель корректен
         (((uint32_t)d->bits[3] & S21_SCALE_MASK) >> S21_SCALE_SHIFT);
}

/**
 * @brief Установить масштаб decimal числа
 * @param d Указатель на decimal число
 * @param s Новое значение масштаба
 * @details Автоматически ограничивает масштаб диапазоном [0, S21_SCALE_MAX]
 */
void s21_set_scale(s21_decimal* d, int s) {
  if (d) {
    // Ограничиваем масштаб допустимым диапазоном
    if (s < 0) s = 0;
    if (s > S21_SCALE_MAX) s = S21_SCALE_MAX;

    uint32_t meta = (uint32_t)d->bits[3];  // Текущие метаданные
    // Очищаем старый масштаб и устанавливаем новый
    d->bits[3] =
        (int)(meta & ~S21_SCALE_MASK) | ((uint32_t)s << S21_SCALE_SHIFT);
  }
}

/**
 * @brief Получить знак decimal числа
 * @param d Указатель на decimal число
 * @return 1 если число отрицательное, 0 если положительное или указатель
 * некорректен
 * @details Проверяет старший бит (бит 31) четвертого элемента
 */
int s21_get_sign(const s21_decimal* d) {
  return (d != 0) * (((uint32_t)d->bits[3] & S21_SIGN_MASK) != 0u);
}

/**
 * @brief Установить знак decimal числа
 * @param d Указатель на decimal число
 * @param neg Новый знак: 1 для отрицательного, 0 для положительного
 * @details Устанавливает или сбрасывает старший бит четвертого элемента
 */
void s21_set_sign(s21_decimal* d, int neg) {
  if (d) {
    uint32_t meta = (uint32_t)d->bits[3];  // Текущие метаданные
    // Устанавливаем или сбрасываем бит знака
    d->bits[3] = (neg) ? (meta | S21_SIGN_MASK) : (meta & ~S21_SIGN_MASK);
  }
}

/**
 * @brief Извлечь 96-битное значение из decimal
 * @param d Decimal число
 * @param a Массив для хранения 96-битного числа
 * @details Копирует первые 3 элемента decimal в выходной массив
 */
void u96_from_dec(const s21_decimal* d, uint32_t a[3]) {
  a[0] = (uint32_t)d->bits[0];  // Младшие 32 бита
  a[1] = (uint32_t)d->bits[1];  // Средние 32 бита
  a[2] = (uint32_t)d->bits[2];  // Старшие 32 бита
}

/**
 * @brief Сохранить 96-битное значение в decimal
 * @param a 96-битное число
 * @param d Decimal число для записи
 * @details Копирует 96-битное число в первые 3 элемента decimal
 */
void u96_to_dec(const uint32_t a[3], s21_decimal* d) {
  d->bits[0] = (int)a[0];  // Младшие 32 бита
  d->bits[1] = (int)a[1];  // Средние 32 бита
  d->bits[2] = (int)a[2];  // Старшие 32 бита
}

/**
 * @brief Скопировать 96-битное число
 * @param dst Назначение
 * @param src Источник
 * @details Поэлементное копирование массива
 */
void u96_copy(uint32_t dst[3], const uint32_t src[3]) {
  dst[0] = src[0];  // Копируем младшие 32 бита
  dst[1] = src[1];
  dst[2] = src[2];
}

int u96_is_zero(const uint32_t a[3]) {
  return a[0] == 0u && a[1] == 0u && a[2] == 0u;
}

int u96_cmp(const uint32_t a[3], const uint32_t b[3]) {
  int result = 0;
  if (a[2] != b[2])
    result = (a[2] < b[2]) ? -1 : 1;
  else if (a[1] != b[1])
    result = (a[1] < b[1]) ? -1 : 1;
  else if (a[0] != b[0])
    result = (a[0] < b[0]) ? -1 : 1;
  return result;
}

int u96_add(uint32_t a[3], const uint32_t b[3]) {
  uint64_t t = (uint64_t)a[0] + b[0];
  a[0] = (uint32_t)t;
  t = (t >> 32) + (uint64_t)a[1] + b[1];
  a[1] = (uint32_t)t;
  t = (t >> 32) + (uint64_t)a[2] + b[2];
  a[2] = (uint32_t)t;
  return (int)(t >> 32);
}

int u96_sub(uint32_t a[3], const uint32_t b[3]) {
  uint64_t t = (uint64_t)a[0] - b[0];
  a[0] = (uint32_t)t;
  t = (int64_t)(t >> 32) + (int64_t)a[1] - b[1];
  a[1] = (uint32_t)t;
  t = (int64_t)(t >> 32) + (int64_t)a[2] - b[2];
  a[2] = (uint32_t)t;
  return (t >> 32) ? 1 : 0;
}

uint32_t u96_mul10(uint32_t a[3]) {
  uint64_t t = (uint64_t)a[0] * 10u;
  a[0] = (uint32_t)t;
  t = (t >> 32) + (uint64_t)a[1] * 10u;
  a[1] = (uint32_t)t;
  t = (t >> 32) + (uint64_t)a[2] * 10u;
  a[2] = (uint32_t)t;
  return (uint32_t)(t >> 32);
}

uint32_t u96_div10(uint32_t a[3]) {
  uint32_t rem = 0;
  uint64_t cur = 0;
  for (int i = 2; i >= 0; --i) {
    cur = ((uint64_t)rem << 32) | a[i];
    a[i] = (uint32_t)(cur / 10u);
    rem = (uint32_t)(cur % 10u);
  }
  return rem;
}

static int u96_divisible_by_10(const uint32_t a_in[3]) {
  uint32_t a[3] = {a_in[0], a_in[1], a_in[2]};
  return u96_div10(a) == 0u;
}

void s21_reset_value(s21_decimal* d) {
  if (d) d->bits[0] = d->bits[1] = d->bits[2] = d->bits[3] = 0;
}

int s21_is_zero(const s21_decimal* d) {
  return d && d->bits[0] == 0 && d->bits[1] == 0 && d->bits[2] == 0;
}

void s21_scale_down_one_banker(s21_decimal* d) {
  uint32_t a[3];
  u96_from_dec(d, a);
  uint32_t rem = u96_div10(a);
  uint32_t one[3] = {1u, 0u, 0u};
  if (rem > 5u) {
    (void)u96_add(a, one);
  } else if (rem == 5u && ((a[0] & 1u) != 0u)) {
    (void)u96_add(a, one);
  }
  u96_to_dec(a, d);
  s21_set_scale(d, s21_get_scale(d) - 1);
}

int s21_scale_up_one(s21_decimal* d) {
  uint32_t a[3];
  u96_from_dec(d, a);
  uint32_t carry = u96_mul10(a);
  if (!carry) {
    u96_to_dec(a, d);
    s21_set_scale(d, s21_get_scale(d) + 1);
  }
  return carry & 1;
}

void s21_strip_trailing_zeros(s21_decimal* d) {
  if (d) {
    int s = s21_get_scale(d);
    if (s) {
      uint32_t a[3];
      u96_from_dec(d, a);
      while (s > 0 && u96_divisible_by_10(a)) {
        (void)u96_div10(a);
        --s;
      }
      u96_to_dec(a, d);
      s21_set_scale(d, s);
    }
  }
}

void s21_to_common_scale(s21_decimal* a, s21_decimal* b) {
  if (a && b) {
    s21_strip_trailing_zeros(a);
    s21_strip_trailing_zeros(b);
    while (s21_get_scale(a) != s21_get_scale(b)) {
      if (s21_get_scale(a) < s21_get_scale(b)) {
        if (s21_scale_up_one(a)) s21_scale_down_one_banker(b);
      } else if (s21_scale_up_one(b))
        s21_scale_down_one_banker(a);
    }
  }
}
