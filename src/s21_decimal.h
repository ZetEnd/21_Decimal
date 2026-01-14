#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <math.h>      // Математические функции
#include <stddef.h>    // Определения типов size_t, NULL и др.
#include <stdint.h>    // Типы фиксированной ширины (uint32_t, etc.)
#include <stdlib.h>    // Стандартные функции (malloc, free, etc.)

typedef struct {
  int bits[4];  // Массив из 4-х 32-битных слов для хранения decimal
} s21_decimal;

// Маски для работы с bits[3] (знак и масштаб)
#define S21_SIGN_MASK 0x80000000u   // Маска для знака (старший бит)
#define S21_SCALE_MASK 0x00FF0000u  // Маска для масштаба (биты 16-23)
#define S21_SCALE_SHIFT 16          // Сдвиг для масштаба из bits[3]
#define S21_SCALE_MAX 28            // Максимальный масштаб

// Получить масштаб decimal числа
int s21_get_scale(const s21_decimal *d);

// Установить масштаб decimal числа
void s21_set_scale(s21_decimal *d, int s);

// Получить знак decimal числа
int s21_get_sign(const s21_decimal *d);

// Установить знак decimal числа
void s21_set_sign(s21_decimal *d, int neg);




// обнулить мантиссу и оставить только знак и масштаб
void s21_reset_value(s21_decimal *d);

// проверка равно ли число нулю
int s21_is_zero(const s21_decimal *d);

// удалить лишние нули в конце числа
void s21_strip_trailing_zeros(s21_decimal *d);

// 2 числа к общему масштабу
void s21_to_common_scale(s21_decimal *a, s21_decimal *b);

// уменьшить масштаб на 1 с банковским округлением
void s21_scale_down_one_banker(s21_decimal *d);

// увеличить масштаб на 1 - умножить на 10
int s21_scale_up_one(s21_decimal *d);




// получить мантиссу из децималь
void u96_from_dec(const s21_decimal *d, uint32_t a[3]);

// копировать 96 битное число в децималь
void u96_to_dec(const uint32_t a[3], s21_decimal *d);

// равно ли число нулю
int u96_is_zero(const uint32_t a[3]);

// сравнить 2 числа и вернуть a - b
int u96_compare(const uint32_t a[3], const uint32_t b[3]);

// сложить 2 числа по 96 бит а = а + b
int u96_add(uint32_t a[3], const uint32_t b[3]);

// вычесть 2 числа по 96 бит a = a - b
int u96_sub(uint32_t a[3], const uint32_t b[3]);

// умножить 96 бит число на 10 (ретюрн старший бит)
uint32_t u96_mul10(uint32_t a[3]);

// разделить 96 бит на 10 (ретюрн остаток) 
uint32_t u96_div10(uint32_t a[3]);

// комировать куда откуда 96 бит
void u96_copy(uint32_t dst[3], const uint32_t src[3]);




// Сложение 2х дециамаль чисел
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// вычитание децималь чисел
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// умножение
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// деление
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);




// меньше ли 1е число 2го - 1 если val1 < val2 иначе 0
int s21_is_less(s21_decimal value1, s21_decimal value2);

// меньше либо равно
int s21_is_less_or_equal(s21_decimal value1, s21_decimal value2);

// больше ли 1е число 2го - 1 1 если val1 > val2
int s21_is_greater(s21_decimal value1, s21_decimal value2);

// больше либо равно
int s21_is_greater_or_equal(s21_decimal value1, s21_decimal value2);

// равны ли 2 числа - 1 если равны
int s21_is_equal(s21_decimal value1, s21_decimal value2);

// не равны ли 2 числа - 1 если не равны
int s21_is_not_equal(s21_decimal value1, s21_decimal value2);




// конвертация инт в децималь 0 - успех
int s21_from_int_to_decimal(int src, s21_decimal *dst);

// конвертация флоат в децималь 0 - успех
int s21_from_float_to_decimal(float src, s21_decimal *dst);

// децималь в инт 0 - успех
int s21_from_decimal_to_int(s21_decimal src, int *dst);

// децималь в флоат 0 - успех
int s21_from_decimal_to_float(s21_decimal src, float *dst);




// округлить число вниз
int s21_floor(s21_decimal value, s21_decimal *result);

// округлить до ближайшего целого (банковское оургление)
int s21_round(s21_decimal value, s21_decimal *result);

// отбросить дробную часть числа(окргуление к нулю)
int s21_truncate(s21_decimal value, s21_decimal *result);

// изменить знак числа на противоположный
int s21_negate(s21_decimal value, s21_decimal *result);

#endif