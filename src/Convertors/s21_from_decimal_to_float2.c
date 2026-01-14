/**
 * @file s21_from_decimal_to_float.c
 * @brief Реализация функции конвертации decimal в float
 * @details Конвертирует decimal число в 32-битное число с плавающей точкой
 */

#include <math.h>  // Математические функции (pow)

#include "../s21_decimal.h"

/**
 * @brief Конвертировать decimal в float
 * @param src Исходное decimal число
 * @param dst Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке (некорректный указатель)
 * @details Алгоритм:
 *          1. Собирает 96-битную мантиссу в double
 *          2. Применяет масштаб (деление на 10^scale)
 *          3. Применяет знак
 *          4. Приводит к float (возможна потеря точности)
 */
int s21_from_decimal_to_float(s21_decimal src, float* dst) {
  if (dst) {
    double v = 0.0;  // Используем double для промежуточных вычислений

    // Собираем 96-битную мантиссу в double
    v += (double)(uint32_t)src.bits[0];  // Младшие 32 бита
    v += (double)(uint32_t)src.bits[1] *
         pow(2.0, 32.0);  // Средние 32 бита * 2^32
    v += (double)(uint32_t)src.bits[2] *
         pow(2.0, 64.0);  // Старшие 32 бита * 2^64

    // Применяем масштаб (количество знаков после запятой)
    int scale = s21_get_scale(&src);
    if (scale > 0) {
      v /= pow(10.0, (double)scale);  // Делим на 10^scale
    }

    // Применяем знак
    if (s21_get_sign(&src)) v = -v;

    // Приводим к float (возможна потеря точности)
    *dst = (float)v;
  }

  // Возвращаем 0 при успехе, 1 при ошибке
  return !dst;
}
