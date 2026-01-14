/**
 * @file s21_is_greater.c
 * @brief Реализация функции проверки "больше" для decimal чисел
 * @details Использует логическое отрицание функции "меньше или равно"
 */

#include "s21_decimal.h"

/**
 * @brief Проверить, больше ли первое число второго
 * @param a Первое число
 * @param b Второе число
 * @return 1 если a > b, 0 иначе
 * @details Реализовано через логическое тождество: a > b ≡ !(a ≤ b)
 */
int s21_is_greater(s21_decimal a, s21_decimal b) {
  // a > b эквивалентно !(a <= b)
  return !s21_is_less_or_equal(a, b);
}
