/**
 * @file s21_is_not_equal.c
 * @brief Реализация функции проверки неравенства decimal чисел
 * @details Использует отрицание функции равенства
 */

#include "s21_decimal.h"

// Объявление функции равенства
int s21_is_equal(s21_decimal, s21_decimal);

/**
 * @brief Проверить, не равны ли два decimal числа
 * @param a Первое число
 * @param b Второе число
 * @return 1 если числа не равны, 0 если равны
 * @details Реализовано как отрицание результата функции s21_is_equal
 */
int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  return !s21_is_equal(a, b);  // Инвертируем результат проверки на равенство
}
