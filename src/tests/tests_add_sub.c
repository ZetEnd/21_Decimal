/**
 * @file tests_add_sub.c
 * @brief Тесты функций сложения и вычитания decimal чисел
 * @details Содержит юнит-тесты для проверки корректности работы
 *          функций s21_add и s21_sub в различных сценариях
 */

#include "tests.h"

/**
 * @brief Вспомогательная функция для создания decimal числа
 * @param lo Младшие 32 бита мантиссы
 * @param mid Средние 32 бита мантиссы
 * @param hi Старшие 32 бита мантиссы
 * @param scale Масштаб (количество знаков после запятой)
 * @param sign Знак (0 - положительное, 1 - отрицательное)
 * @return Созданное decimal число
 * @details Упрощает создание тестовых decimal чисел с заданными параметрами
 */
static s21_decimal mk(unsigned lo, unsigned mid, unsigned hi, int scale,
                      int sign) {
  s21_decimal d = {
      {(int)lo, (int)mid, (int)hi, 0}};  // Создаем число с заданной мантиссой
  s21_set_scale(&d, scale);              // Устанавливаем масштаб
  s21_set_sign(&d, sign);                // Устанавливаем знак
  return d;
}

/**
 * @brief Тест простого сложения целых чисел
 * @details Проверяет: 5 + 3 = 8
 */
START_TEST(test_add_simple) {
  s21_decimal a, b, res, expected;
  s21_from_int_to_decimal(5, &a);         // a = 5
  s21_from_int_to_decimal(3, &b);         // b = 3
  s21_from_int_to_decimal(8, &expected);  // expected = 8

  ck_assert_int_eq(s21_add(a, b, &res), 0);  // Операция должна пройти успешно
  ck_assert_int_eq(s21_is_equal(res, expected),
                   1);  // Результат должен быть равен ожидаемому
}
END_TEST

/**
 * @brief Тест простого вычитания целых чисел
 * @details Проверяет: 5 - 3 = 2
 */
START_TEST(test_sub_simple) {
  s21_decimal a, b, res, expected;
  s21_from_int_to_decimal(5, &a);         // a = 5
  s21_from_int_to_decimal(3, &b);         // b = 3
  s21_from_int_to_decimal(2, &expected);  // expected = 2

  ck_assert_int_eq(s21_sub(a, b, &res), 0);  // Операция должна пройти успешно
  ck_assert_int_eq(s21_is_equal(res, expected),
                   1);  // Результат должен быть равен ожидаемому
}
END_TEST

START_TEST(add_diff_scales) {
  s21_decimal a = mk(120, 0, 0, 2, 0);
  s21_decimal b = mk(3, 0, 0, 2, 0);
  s21_decimal r = {{0}}, exp = mk(123, 0, 0, 2, 0);
  ck_assert_int_eq(s21_add(a, b, &r), 0);
  ck_assert_int_eq(s21_is_equal(r, exp), 1);
}
END_TEST

START_TEST(add_opposite_signs_subtract_path) {
  s21_decimal a, b, r, exp;
  s21_from_int_to_decimal(5, &a);
  s21_from_int_to_decimal(-3, &b);
  s21_from_int_to_decimal(2, &exp);
  ck_assert_int_eq(s21_add(a, b, &r), 0);
  ck_assert_int_eq(s21_is_equal(r, exp), 1);
}
END_TEST

START_TEST(add_bankers_shrink_one_digit) {
  s21_decimal a, b, r, one;
  s21_from_int_to_decimal(1, &a);
  for (int i = 0; i < 27; ++i) ck_assert_int_eq(s21_scale_up_one(&a), 0);

  s21_from_int_to_decimal(1, &b);
  for (int i = 0; i < 27; ++i) ck_assert_int_eq(s21_scale_up_one(&b), 0);

  ck_assert_int_eq(s21_sub(a, b, &a), 0);
  ck_assert_int_eq(s21_add(a, b, &r), 0);

  s21_from_int_to_decimal(1, &one);
  ck_assert_int_eq(s21_is_equal(r, one), 1);
}
END_TEST

START_TEST(add_carry_and_scale_fn) {
  s21_decimal a = {{9999999, 0, 0, 6 << 16}};
  s21_decimal b = {{1, 0, 0, 6 << 16}};
  s21_decimal r;
  ck_assert_int_eq(s21_add(a, b, &r), 0);
}
END_TEST

START_TEST(add_align_scales_long_fn) {
  s21_decimal a = {{123, 0, 0, 0 << 16}};
  s21_decimal b = {{1, 0, 0, 15 << 16}};
  s21_decimal r;
  ck_assert_int_eq(s21_add(a, b, &r), 0);
}

START_TEST(add_bankers_tie_even_fn) {
  s21_decimal a = {{5, 0, 0, 20 << 16}};
  s21_decimal b = {{1, 0, 0, 9 << 16}};
  s21_decimal r;
  ck_assert_int_eq(s21_add(a, b, &r), 0);
}

START_TEST(add_bankers_tie_odd_fn) {
  s21_decimal a = {{5, 0, 0, 20 << 16}};
  s21_decimal b = {{3, 0, 0, 9 << 16}};
  s21_decimal r;
  ck_assert_int_eq(s21_add(a, b, &r), 0);
}

START_TEST(add_overflow_pos_fn) {
  s21_decimal a = {{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal r;
  ck_assert_int_eq(s21_add(a, b, &r), 1);
}
END_TEST

START_TEST(add_align_bankers_tie_odd) {
  s21_decimal a = {{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0}};
  s21_decimal b = {{15, 0, 0, 20 << 16}};
  s21_decimal r;
  ck_assert_int_eq(s21_add(a, b, &r), 0);
}
END_TEST

START_TEST(add_align_bankers_tie_even) {
  s21_decimal a = {{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0}};
  s21_decimal b = {{25, 0, 0, 20 << 16}};
  s21_decimal r;
  ck_assert_int_eq(s21_add(a, b, &r), 0);
}
END_TEST

START_TEST(add_overflow_shrink128) {
  s21_decimal a = {{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 1 << 16}};
  s21_decimal b = {{1, 0, 0, 1 << 16}};
  s21_decimal r;
  ck_assert_int_eq(s21_add(a, b, &r), 0);
}
END_TEST

Suite* test_add_sub(void) {
  Suite* s = suite_create("s21_add_sub");
  TCase* tc = tcase_create("add_sub");

  tcase_add_test(tc, test_add_simple);
  tcase_add_test(tc, test_sub_simple);
  tcase_add_test(tc, add_diff_scales);
  tcase_add_test(tc, add_opposite_signs_subtract_path);
  tcase_add_test(tc, add_bankers_shrink_one_digit);
  tcase_add_test(tc, add_carry_and_scale_fn);
  tcase_add_test(tc, add_align_scales_long_fn);
  tcase_add_test(tc, add_bankers_tie_even_fn);
  tcase_add_test(tc, add_bankers_tie_odd_fn);
  tcase_add_test(tc, add_overflow_pos_fn);
  tcase_add_test(tc, add_align_bankers_tie_odd);
  tcase_add_test(tc, add_align_bankers_tie_even);
  tcase_add_test(tc, add_overflow_shrink128);

  suite_add_tcase(s, tc);
  return s;
}
