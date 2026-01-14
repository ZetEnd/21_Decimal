/**
 * @file s21_sub.c
 * @brief Реализация функции вычитания decimal чисел
 * @details Использует простой алгоритм: a - b = a + (-b)
 */

#include "../s21_decimal.h"

/**
 * @brief Вычитание двух decimal чисел
 * @param a Уменьшаемое
 * @param b Вычитаемое
 * @param res Указатель для сохранения результата
 * @return 0 при успехе, 1 при переполнении, 2 при недополнении
 * @details Реализовано через сложение с противоположным числом:
 *          a - b = a + (-b)
 */
int s21_sub(s21_decimal a, s21_decimal b, s21_decimal* res) {
  // Меняем знак второго числа: sub = a + (-b)
  s21_set_sign(&b, !s21_get_sign(&b));  // Инвертируем знак числа b
  return s21_add(a, b, res);            // Вызываем функцию сложения
}
