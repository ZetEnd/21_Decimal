/**
 * @file s21_decimal.h
 * @brief Заголовочный файл библиотеки для работы с decimal числами
 * @details Содержит структуры, макросы и объявления функций для работы
 *          с числами типа decimal по стандарту C# decimal
 */

#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

// Подключение стандартных библиотек
#include <math.h>      // Математические функции
#include <stddef.h>    // Определения типов size_t, NULL и др.
#include <stdint.h>    // Типы фиксированной ширины (uint32_t, etc.)
#include <stdlib.h>    // Стандартные функции (malloc, free, etc.)

/**
 * @brief Структура для представления decimal числа
 * @details Decimal число состоит из 128 бит (4 блока по 32 бита):
 *          bits[0], bits[1], bits[2] - мантисса (96 бит)
 *          bits[3] - информация о знаке и масштабе (32 бита)
 */
typedef struct {
  int bits[4];  // Массив из 4-х 32-битных слов для хранения decimal
} s21_decimal;

// Маски для работы с bits[3] (знак и масштаб)
#define S21_SIGN_MASK 0x80000000u   // Маска для извлечения знака (старший бит)
#define S21_SCALE_MASK 0x00FF0000u  // Маска для извлечения масштаба (биты 16-23)
#define S21_SCALE_SHIFT 16          // Сдвиг для получения масштаба из bits[3]
#define S21_SCALE_MAX 28            // Максимальное значение масштаба

// ============================================================================
// ФУНКЦИИ ДЛЯ РАБОТЫ С МАСШТАБОМ И ЗНАКОМ
// ============================================================================

/**
 * @brief Получить масштаб decimal числа
 * @param d Указатель на decimal число
 * @return Значение масштаба (0-28)
 */
int s21_get_scale(const s21_decimal *d);

/**
 * @brief Установить масштаб decimal числа
 * @param d Указатель на decimal число
 * @param s Новое значение масштаба (0-28)
 */
void s21_set_scale(s21_decimal *d, int s);

/**
 * @brief Получить знак decimal числа
 * @param d Указатель на decimal число
 * @return 1 если число отрицательное, 0 если положительное
 */
int s21_get_sign(const s21_decimal *d);

/**
 * @brief Установить знак decimal числа
 * @param d Указатель на decimal число
 * @param neg 1 для отрицательного, 0 для положительного знака
 */
void s21_set_sign(s21_decimal *d, int neg);

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ РАБОТЫ С DECIMAL
// ============================================================================

/**
 * @brief Обнулить значение decimal числа (оставить только знак и масштаб)
 * @param d Указатель на decimal число
 */
void s21_reset_value(s21_decimal *d);

/**
 * @brief Проверить, равно ли decimal число нулю
 * @param d Указатель на decimal число
 * @return 1 если число равно нулю, 0 иначе
 */
int s21_is_zero(const s21_decimal *d);

/**
 * @brief Удалить незначащие нули в конце числа
 * @param d Указатель на decimal число
 */
void s21_strip_trailing_zeros(s21_decimal *d);

/**
 * @brief Привести два числа к общему масштабу
 * @param a Указатель на первое число
 * @param b Указатель на второе число
 */
void s21_to_common_scale(s21_decimal *a, s21_decimal *b);

/**
 * @brief Уменьшить масштаб на 1 с банковским округлением
 * @param d Указатель на decimal число
 */
void s21_scale_down_one_banker(s21_decimal *d);

/**
 * @brief Увеличить масштаб на 1 (умножить на 10)
 * @param d Указатель на decimal число
 * @return 0 при успехе, не 0 при переполнении
 */
int s21_scale_up_one(s21_decimal *d);

// ============================================================================
// ФУНКЦИИ ДЛЯ РАБОТЫ С 96-БИТНЫМИ БЕЗЗНАКОВЫМИ ЧИСЛАМИ
// ============================================================================

/**
 * @brief Извлечь 96-битное значение из decimal
 * @param d Decimal число
 * @param a Массив для хранения 96-битного числа
 */
void u96_from_dec(const s21_decimal *d, uint32_t a[3]);

/**
 * @brief Сохранить 96-битное значение в decimal
 * @param a 96-битное число
 * @param d Decimal число для записи
 */
void u96_to_dec(const uint32_t a[3], s21_decimal *d);

/**
 * @brief Проверить, равно ли 96-битное число нулю
 * @param a 96-битное число
 * @return 1 если равно нулю, 0 иначе
 */
int u96_is_zero(const uint32_t a[3]);

/**
 * @brief Сравнить два 96-битных числа
 * @param a Первое число
 * @param b Второе число
 * @return <0 если a<b, 0 если a==b, >0 если a>b
 */
int u96_cmp(const uint32_t a[3], const uint32_t b[3]);

/**
 * @brief Сложить два 96-битных числа (a = a + b)
 * @param a Первое число (результат)
 * @param b Второе число
 * @return 0 при успехе, не 0 при переполнении
 */
int u96_add(uint32_t a[3], const uint32_t b[3]);

/**
 * @brief Вычесть 96-битные числа (a = a - b)
 * @param a Уменьшаемое (результат)
 * @param b Вычитаемое
 * @return 0 при успехе, не 0 при переполнении
 */
int u96_sub(uint32_t a[3], const uint32_t b[3]);

/**
 * @brief Умножить 96-битное число на 10
 * @param a Число для умножения
 * @return Старшие биты результата (для определения переполнения)
 */
uint32_t u96_mul10(uint32_t a[3]);

/**
 * @brief Разделить 96-битное число на 10
 * @param a Число для деления
 * @return Остаток от деления
 */
uint32_t u96_div10(uint32_t a[3]);

/**
 * @brief Скопировать 96-битное число
 * @param dst Назначение
 * @param src Источник
 */
void u96_copy(uint32_t dst[3], const uint32_t src[3]);

// ============================================================================
// АРИФМЕТИЧЕСКИЕ ОПЕРАЦИИ
// ============================================================================

/**
 * @brief Сложение двух decimal чисел
 * @param value_1 Первое слагаемое
 * @param value_2 Второе слагаемое
 * @param result Указатель для сохранения результата
 * @return 0 - успех, 1 - слишком большое число, 2 - слишком маленькое число
 */
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Вычитание двух decimal чисел
 * @param value_1 Уменьшаемое
 * @param value_2 Вычитаемое
 * @param result Указатель для сохранения результата
 * @return 0 - успех, 1 - слишком большое число, 2 - слишком маленькое число
 */
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Умножение двух decimal чисел
 * @param value_1 Первый множитель
 * @param value_2 Второй множитель
 * @param result Указатель для сохранения результата
 * @return 0 - успех, 1 - слишком большое число, 2 - слишком маленькое число
 */
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

/**
 * @brief Деление двух decimal чисел
 * @param value_1 Делимое
 * @param value_2 Делитель
 * @param result Указатель для сохранения результата
 * @return 0 - успех, 1 - слишком большое число, 2 - слишком маленькое число, 3 - деление на ноль
 */
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// ============================================================================
// ФУНКЦИИ СРАВНЕНИЯ
// ============================================================================

/**
 * @brief Проверить, меньше ли первое число второго
 * @param value1 Первое число
 * @param value2 Второе число
 * @return 1 если value1 < value2, иначе 0
 */
int s21_is_less(s21_decimal value1, s21_decimal value2);

/**
 * @brief Проверить, меньше или равно ли первое число второму
 * @param value1 Первое число
 * @param value2 Второе число
 * @return 1 если value1 <= value2, иначе 0
 */
int s21_is_less_or_equal(s21_decimal value1, s21_decimal value2);

/**
 * @brief Проверить, больше ли первое число второго
 * @param value1 Первое число
 * @param value2 Второе число
 * @return 1 если value1 > value2, иначе 0
 */
int s21_is_greater(s21_decimal value1, s21_decimal value2);

/**
 * @brief Проверить, больше или равно ли первое число второму
 * @param value1 Первое число
 * @param value2 Второе число
 * @return 1 если value1 >= value2, иначе 0
 */
int s21_is_greater_or_equal(s21_decimal value1, s21_decimal value2);

/**
 * @brief Проверить, равны ли два числа
 * @param value1 Первое число
 * @param value2 Второе число
 * @return 1 если числа равны, иначе 0
 */
int s21_is_equal(s21_decimal value1, s21_decimal value2);

/**
 * @brief Проверить, не равны ли два числа
 * @param value1 Первое число
 * @param value2 Второе число
 * @return 1 если числа не равны, иначе 0
 */
int s21_is_not_equal(s21_decimal value1, s21_decimal value2);

// ============================================================================
// ФУНКЦИИ КОНВЕРТАЦИИ ТИПОВ
// ============================================================================

/**
 * @brief Конвертировать int в decimal
 * @param src Исходное целое число
 * @param dst Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке конвертации
 */
int s21_from_int_to_decimal(int src, s21_decimal *dst);

/**
 * @brief Конвертировать float в decimal
 * @param src Исходное число с плавающей точкой
 * @param dst Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке конвертации
 */
int s21_from_float_to_decimal(float src, s21_decimal *dst);

/**
 * @brief Конвертировать decimal в int
 * @param src Исходное decimal число
 * @param dst Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке конвертации
 */
int s21_from_decimal_to_int(s21_decimal src, int *dst);

/**
 * @brief Конвертировать decimal в float
 * @param src Исходное decimal число
 * @param dst Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке конвертации
 */
int s21_from_decimal_to_float(s21_decimal src, float *dst);

// ============================================================================
// ФУНКЦИИ ОКРУГЛЕНИЯ И ПРЕОБРАЗОВАНИЯ
// ============================================================================

/**
 * @brief Округлить число вниз (к отрицательной бесконечности)
 * @param value Исходное число
 * @param result Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке
 */
int s21_floor(s21_decimal value, s21_decimal *result);

/**
 * @brief Округлить число до ближайшего целого (банковское округление)
 * @param value Исходное число
 * @param result Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке
 */
int s21_round(s21_decimal value, s21_decimal *result);

/**
 * @brief Отбросить дробную часть числа (округление к нулю)
 * @param value Исходное число
 * @param result Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке
 */
int s21_truncate(s21_decimal value, s21_decimal *result);

/**
 * @brief Изменить знак числа на противоположный
 * @param value Исходное число
 * @param result Указатель для сохранения результата
 * @return 0 при успехе, 1 при ошибке
 */
int s21_negate(s21_decimal value, s21_decimal *result);

#endif  // S21_DECIMAL_H