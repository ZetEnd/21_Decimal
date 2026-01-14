/**
 * @file s21_floor.c
 * @brief Реализация функции округления decimal числа вниз
 * @details Округляет число к отрицательной бесконечности (floor)
 */

#include "../s21_decimal.h"

/**
 * @brief Округлить число вниз (к отрицательной бесконечности)
 * @param value Исходное число
 * @param result Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке
 * @details Алгоритм:
 *          - Для положительных чисел: truncate (отбросить дробную часть)
 *          - Для отрицательных чисел с дробной частью: truncate - 1
 *          Примеры: 2.7 → 2, -2.7 → -3, 5.0 → 5, -5.0 → -5
 */
int s21_floor(s21_decimal value, s21_decimal* result) {
  if (!result) return 1;  // Проверка корректности указателя

  // Отбрасываем дробную часть
  s21_decimal trunced = {{0}};
  int err = s21_truncate(value, &trunced);
  if (err) return err;

  // Определяем, отрицательное ли число и была ли дробная часть
  int is_negative = s21_get_sign(&value);
  int had_fraction = !s21_is_equal(
      value, trunced);  // Если trunced ≠ value, была дробная часть

  if (is_negative && had_fraction) {
    // Для отрицательных чисел с дробной частью: результат = truncate - 1
    s21_decimal one = {{1, 0, 0, 0}};      // Число 1
    return s21_sub(trunced, one, result);  // trunced - 1
  } else {
    // Для положительных чисел или целых чисел: результат = truncate
    *result = trunced;
    return 0;
  }
}
