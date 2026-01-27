/**
 * @file s21_is_greater_or_equal.c
 * @brief Реализация функции проверки "больше или равно" для decimal чисел
 * @details Использует логическое отрицание функции "меньше"
 */

#include "s21_decimal.h"

// Объявления используемых функций
int s21_is_less(s21_decimal, s21_decimal);
int s21_is_equal(s21_decimal, s21_decimal);

/**
 * @brief Проверить, больше или равно ли первое число второму
 * @param a Первое число
 * @param b Второе число
 * @return 1 если a ≥ b, 0 иначе
 * @details Реализовано через логическое тождество: a ≥ b ≡ !(a < b)
 */
int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  // >= эквивалентно !(<)
  return !s21_is_less(a, b);
}
