/**
 * @file s21_from_decimal_to_int.c
 * @brief Реализация функции конвертации decimal в int
 * @details Конвертирует decimal число в 32-битное целое с проверкой диапазона
 */

#include <limits.h>  // Константы пределов типов (INT_MIN, INT_MAX)

#include "../s21_decimal.h"

/**
 * @brief Конвертировать decimal в int
 * @param src Исходное decimal число
 * @param dst Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке (некорректный указатель или выход за
 * пределы int)
 * @details Алгоритм:
 *          1. Отбрасывает дробную часть (truncate)
 *          2. Проверяет, помещается ли результат в диапазон int
 *          3. Обрабатывает особый случай INT_MIN
 */
int s21_from_decimal_to_int(s21_decimal src, int* dst) {
  int result = 1;         // По умолчанию - ошибка
  s21_decimal t = {{0}};  // Временная переменная для truncate

  // Проверяем корректность указателя и выполняем truncate
  if (dst && s21_truncate(src, &t) == 0) {
    int neg = s21_get_sign(&src);  // Получаем знак исходного числа

    // Извлекаем 96-битную мантиссу
    uint32_t lo = (uint32_t)t.bits[0];   // Младшие 32 бита
    uint32_t mid = (uint32_t)t.bits[1];  // Средние 32 бита
    uint32_t hi = (uint32_t)t.bits[2];   // Старшие 32 бита

    // Проверяем, что число помещается в 32 бита
    if (mid == 0u && hi == 0u) {
      if (!neg) {
        // Положительное число: должно быть <= INT_MAX
        if (lo <= (uint32_t)INT_MAX) {
          *dst = (int)lo;
          result = 0;  // Успех
        }
      } else {
        // Отрицательное число: должно быть >= INT_MIN
        if (lo <= (uint32_t)INT_MAX + 1u) {
          // Особый случай: INT_MIN = -(INT_MAX + 1)
          *dst = (lo == (uint32_t)INT_MAX + 1u) ? INT_MIN : -(int)lo;
          result = 0;  // Успех
        }
      }
    }
    // Если средние или старшие биты != 0, число слишком большое для int
  }

  return result;
}
