
/**
 * @file s21_from_int_to_decimal.c
 * @brief Реализация функции конвертации int в decimal
 * @details Конвертирует 32-битное целое число в decimal формат
 */

#include <limits.h>  // Константы пределов типов (INT_MIN, INT_MAX)

#include "s21_decimal.h"

/**
 * @brief Конвертировать int в decimal
 * @param src Исходное целое число
 * @param dst Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке (некорректный указатель)
 * @details Особенности:
 *          - Обрабатывает специальный случай INT_MIN
 *          - Устанавливает масштаб в 0 (целое число)
 *          - Правильно обрабатывает знак числа
 */
int s21_from_int_to_decimal(int src, s21_decimal* dst) {
  // Проверка корректности указателя
  if (!dst) return 1;

  // Обнуляем целевое decimal число
  s21_reset_value(dst);

  // Определяем знак числа
  int neg = (src < 0);

  // Получаем модуль числа с обработкой особого случая INT_MIN
  unsigned int mag;
  if (src == INT_MIN) {
    // Особый случай: -INT_MIN не помещается в int
    // INT_MIN = -2147483648, |INT_MIN| = 2147483648 = INT_MAX + 1
    mag = (unsigned int)INT_MAX + 1u;
    neg = 1;
  } else {
    // Обычный случай: берем модуль числа
    mag = (unsigned int)(neg ? -src : src);
  }

  // Записываем модуль в младший разряд decimal
  dst->bits[0] = (int)mag;
  dst->bits[1] = 0;  // Старшие разряды = 0
  dst->bits[2] = 0;

  // Устанавливаем параметры decimal числа
  s21_set_scale(dst, 0);   // Масштаб = 0 (целое число)
  s21_set_sign(dst, neg);  // Устанавливаем знак

  return 0;  // Успех
}
