/**
 * @file tests_rounding.c
 * @brief Тесты функций округления decimal чисел
 * @details Содержит юнит-тесты для проверки корректности работы
 *          функций floor, round, truncate для различных чисел
 */

#include "tests.h"

/**
 * @brief Основной тест функций округления
 * @details Проверяет различные виды округления для положительных и
 * отрицательных чисел:
 *          - floor: округление к отрицательной бесконечности
 *          - round: банковское округление до ближайшего целого
 *          - truncate: отбрасывание дробной части (округление к нулю)
 */
START_TEST(test_rounding_case) {
  s21_decimal d, res;
  int i;

  // Тесты для положительного числа 3.7
  s21_from_float_to_decimal(3.7f, &d);

  // floor(3.7) = 3 (округление вниз)
  s21_floor(d, &res);
  s21_from_decimal_to_int(res, &i);
  ck_assert_int_eq(i, 3);

  // round(3.7) = 4 (округление до ближайшего целого)
  s21_round(d, &res);
  s21_from_decimal_to_int(res, &i);
  ck_assert_int_eq(i, 4);

  // truncate(3.7) = 3 (отбрасывание дробной части)
  s21_truncate(d, &res);
  s21_from_decimal_to_int(res, &i);
  ck_assert_int_eq(i, 3);

  // Тесты для отрицательного числа -2.3
  s21_from_float_to_decimal(-2.3f, &d);

  // floor(-2.3) = -3 (округление к отрицательной бесконечности)
  s21_floor(d, &res);
  s21_from_decimal_to_int(res, &i);
  ck_assert_int_eq(i, -3);

  // round(-2.3) = -2 (округление до ближайшего целого)
  s21_round(d, &res);
  s21_from_decimal_to_int(res, &i);
  ck_assert_int_eq(i, -2);
}
END_TEST

Suite* test_rounding(void) {
  Suite* s = suite_create("s21_round_floor_truncate");
  TCase* tc = tcase_create("round_floor_truncate_TC");
  tcase_add_test(tc, test_rounding_case);
  suite_add_tcase(s, tc);
  return s;
}
