/**
 * @file s21_is_less_or_equal.c
 * @brief Реализация функции проверки "меньше или равно" для decimal чисел
 * @details Использует комбинацию функций "меньше" и "равно"
 */

#include "s21_decimal.h"

/**
 * @brief Проверить, меньше или равно ли первое число второму
 * @param a Первое число
 * @param b Второе число
 * @return 1 если a ≤ b, 0 иначе
 * @details Реализовано как логическое ИЛИ: a ≤ b ≡ (a < b) ∨ (a = b)
 */
int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  return s21_is_less(a, b) || s21_is_equal(a, b);
}
