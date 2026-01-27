/**
 * @file tests_comparison.c
 * @brief Тесты функций сравнения decimal чисел
 * @details Содержит юнит-тесты для проверки корректности работы
 *          всех функций сравнения (>, <, >=, <=, ==, !=)
 */

#include "tests.h"

/**
 * @brief Основной тест функций сравнения
 * @details Проверяет различные сценарии сравнения:
 *          - Сравнение разных чисел (5 и 10)
 *          - Сравнение равных чисел (7 и 7)
 *          - Сравнение чисел с разными масштабами (1.00 и 1)
 *          - Сравнение нулей с разными знаками (+0 и -0)
 */
START_TEST(test_comparison_case) {
  s21_decimal a, b;

  // Тест: 5 < 10, 10 > 5
  s21_from_int_to_decimal(5, &a);
  s21_from_int_to_decimal(10, &b);
  ck_assert_int_eq(s21_is_less(a, b), 1);     // 5 < 10 должно быть true
  ck_assert_int_eq(s21_is_greater(b, a), 1);  // 10 > 5 должно быть true

  // Тест: 7 == 7
  s21_from_int_to_decimal(7, &a);
  s21_from_int_to_decimal(7, &b);
  ck_assert_int_eq(s21_is_equal(a, b), 1);      // 7 == 7 должно быть true
  ck_assert_int_eq(s21_is_not_equal(a, b), 0);  // 7 != 7 должно быть false

  ck_assert_int_eq(s21_is_greater_or_equal(a, b),
                   1);                              // 7 >= 7 должно быть true
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);  // 7 <= 7 должно быть true

  // Тест: 1.00 == 1 (разные масштабы, но равные значения)
  s21_decimal x = {{100, 0, 0, (2 << 16)}};  // 1.00 (масштаб 2)
  s21_decimal y = {{1, 0, 0, 0}};            // 1 (масштаб 0)
  ck_assert_int_eq(s21_is_equal(x, y), 1);   // Должны быть равны

  // Тест: +0 == -0 (нули с разными знаками должны быть равны)
  s21_decimal zpos = {{0, 0, 0, 0}};                       // +0
  s21_decimal zneg = {{0, 0, 0, (5 << 16) | 0x80000000}};  // -0 с масштабом 5
  ck_assert_int_eq(s21_is_equal(zpos, zneg), 1);           // Должны быть равны
}
END_TEST

/**
 * @brief Тест сравнения чисел с разными знаками
 * @details Проверяет, что отрицательные числа всегда меньше положительных
 */
START_TEST(is_less_mixed_signs_fn) {
  s21_decimal p = {{1, 0, 0, 0}};  // Положительное число +1
  s21_decimal n = {{1, 0, 0, (1u << 31)}};
  ck_assert_int_eq(s21_is_less(n, p), 1);
  ck_assert_int_eq(s21_is_less(p, n), 0);
}
END_TEST

START_TEST(is_less_zero_signed_fn) {
  s21_decimal pz = {{0, 0, 0, 0}};
  s21_decimal nz = {{0, 0, 0, (1u << 31)}};
  ck_assert_int_eq(s21_is_less(pz, nz), 0);
  ck_assert_int_eq(s21_is_less(nz, pz), 0);
}
END_TEST

START_TEST(is_less_diff_scales_equal_value_fn) {
  s21_decimal a = {{1230, 0, 0, 3 << 16}};
  s21_decimal b = {{123, 0, 0, 2 << 16}};
  ck_assert_int_eq(s21_is_less(a, b), 0);
  ck_assert_int_eq(s21_is_less(b, a), 0);
}
END_TEST

Suite* test_comparison(void) {
  Suite* s = suite_create("s21_comparison");

  TCase* tc = tcase_create("comparison_TC");
  tcase_add_test(tc, test_comparison_case);
  tcase_add_test(tc, is_less_mixed_signs_fn);
  tcase_add_test(tc, is_less_zero_signed_fn);
  tcase_add_test(tc, is_less_diff_scales_equal_value_fn);
  suite_add_tcase(s, tc);
  return s;
}
