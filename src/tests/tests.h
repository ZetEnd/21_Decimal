/**
 * @file tests.h
 * @brief Заголовочный файл для тестов библиотеки s21_decimal
 * @details Содержит объявления тестовых наборов и вспомогательные макросы
 */

#ifndef TESTS_H
#define TESTS_H

#include <check.h>   // Библиотека для юнит-тестирования
#include <math.h>    // Математические функции
#include <stdio.h>   // Стандартные функции ввода-вывода

#include "../s21_decimal.h"  // Тестируемая библиотека

// Макрос для сравнения чисел с плавающей точкой с заданной точностью
// Нужен для старых версий check, где этот макрос может отсутствовать
#ifndef ck_assert_float_eq_eps
#define ck_assert_float_eq_eps(actual, expected, eps)                         \
  do {                                                                        \
    float _a = (actual), _e = (expected);                                     \
    float _d = (_a > _e ? _a - _e : _e - _a);                                 \
    ck_assert_msg(_d <= (eps), "float %.9g != %.9g (|diff|=%.9g > eps=%.9g)", \
                  _a, _e, _d, (float)(eps));                                  \
  } while (0)
#endif

// Объявления тестовых наборов (суитов)

Suite *test_to_float(void);              // Тесты конвертации decimal → float
Suite *test_to_int(void);                // Тесты конвертации decimal → int
Suite *test_from_float_to_decimal(void); // Тесты конвертации float → decimal
Suite *test_from_int(void);              // Тесты конвертации int → decimal
Suite *test_rounding(void);              // Тесты функций округления
Suite *test_comparison(void);            // Тесты функций сравнения
Suite *test_add_sub(void);               // Тесты сложения и вычитания
Suite *test_mul_div(void);               // Тесты умножения и деления
Suite *test_negate(void);                // Тесты изменения знака
Suite *test_div_ext(void);               // Расширенные тесты деления
Suite *test_div_extra(void);             // Дополнительные тесты деления
Suite *test_utilities_extra(void);

#endif
