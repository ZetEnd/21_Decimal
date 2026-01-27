/**
 * @file tests_mul_div.c
 * @brief Тесты функций умножения и деления decimal чисел
 * @details Содержит юнит-тесты для проверки корректности работы
 *          функций s21_mul и s21_div, включая граничные случаи
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
 */
static s21_decimal mk(unsigned lo, unsigned mid, unsigned hi, int scale,
                      int sign) {
  s21_decimal d = {{(int)lo, (int)mid, (int)hi, 0}};
  s21_set_scale(&d, scale);
  s21_set_sign(&d, sign);
  return d;
}

/**
 * @brief Создать decimal число с заданной мантиссой и масштабом
 * @param mant Мантисса (32-битная)
 * @param scale Масштаб
 * @return Созданное decimal число
 * @details Упрощенная версия mk() для чисел, помещающихся в 32 бита
 */
static s21_decimal mk_mant_scale(uint32_t mant, int scale) {
  s21_decimal d = {{0, 0, 0, 0}};
  d.bits[0] = mant;
  d.bits[3] = (uint32_t)(scale & 0xFF) << 16;
  return d;
}

/**
 * @brief Тест банковского округления при умножении (четный случай)
 * @details Проверяет округление когда остаток = 5 и последняя цифра четная
 *          Ожидается округление вниз (к четному)
 */
START_TEST(mul_r5_even_fn) {
  s21_decimal a = mk_mant_scale(5, 20);  // 5 * 10^-20
  s21_decimal b = mk_mant_scale(1, 9);   // 1 * 10^-9
  s21_decimal r;
  ck_assert_int_eq(s21_mul(a, b, &r), 0);  // Операция должна пройти успешно

  // Результат должен иметь максимальный масштаб 28
  ck_assert_int_eq(s21_get_scale(&r), 28);
  // При таких малых числах результат округляется до 0
  ck_assert_int_eq(r.bits[0] | r.bits[1] | r.bits[2], 0u);
}
END_TEST

/**
 * @brief Тест банковского округления при умножении (нечетный случай)
 * @details Проверяет округление когда остаток = 5 и последняя цифра нечетная
 */
START_TEST(mul_r5_odd_fn) {
  s21_decimal a = mk_mant_scale(5, 20);  // 5 * 10^-20
  s21_decimal b = mk_mant_scale(3, 9);
  s21_decimal r;
  ck_assert_int_eq(s21_mul(a, b, &r), 0);
  ck_assert_int_eq(s21_get_scale(&r), 28);
  ck_assert_int_eq(r.bits[0], 2u);
  ck_assert_int_eq(r.bits[1], 0u);
  ck_assert_int_eq(r.bits[2], 0u);
}
END_TEST

START_TEST(test_mul_simple) {
  s21_decimal a, b, res, expected;
  s21_from_int_to_decimal(4, &a);
  s21_from_int_to_decimal(3, &b);
  s21_from_int_to_decimal(12, &expected);
  ck_assert_int_eq(s21_mul(a, b, &res), 0);
  ck_assert_int_eq(s21_is_equal(res, expected), 1);
}
END_TEST

START_TEST(test_div_simple) {
  s21_decimal a, b, res, expected;
  s21_from_int_to_decimal(12, &a);
  s21_from_int_to_decimal(3, &b);
  s21_from_int_to_decimal(4, &expected);
  ck_assert_int_eq(s21_div(a, b, &res), 0);
  ck_assert_int_eq(s21_is_equal(res, expected), 1);
}
END_TEST

START_TEST(mul_signs) {
  s21_decimal a = mk(3, 0, 0, 0, 1);
  s21_decimal b = mk(2, 0, 0, 0, 0);
  s21_decimal r = {{0}}, exp = mk(6, 0, 0, 0, 1);
  ck_assert_int_eq(s21_mul(a, b, &r), 0);
  ck_assert_int_eq(s21_is_equal(r, exp), 1);
}
END_TEST

START_TEST(mul_overflow_no_scale) {
  s21_decimal max = mk(0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  s21_decimal two = mk(2, 0, 0, 0, 0);
  s21_decimal r = {{0}};
  ck_assert_int_eq(s21_mul(max, two, &r), 1);
}
END_TEST

START_TEST(mul_reduce_when_S_gt_28) {
  s21_decimal a = mk(1, 0, 0, 20, 0);
  s21_decimal b = mk(1, 0, 0, 15, 0);
  s21_decimal r = {{0}};
  ck_assert_int_eq(s21_mul(a, b, &r), 0);
  ck_assert_int_eq(s21_get_scale(&r), 28);
  ck_assert_int_eq(r.bits[0] | r.bits[1] | r.bits[2], 0);
}
END_TEST

START_TEST(mul_u192_shrink_branch) {
  s21_decimal a = mk(0, 0, 0x80000000u, 10, 0);
  s21_decimal b = mk(0, 0, 0x80000000u, 10, 0);
  s21_decimal r = {{0}};
  ck_assert_int_eq(s21_mul(a, b, &r), 0);
  ck_assert_int_le(s21_get_scale(&r), 28);
}
END_TEST

Suite* test_mul_div(void) {
  Suite* s = suite_create("s21_mul_div");
  TCase* tc = tcase_create("mul_div");

  tcase_add_test(tc, test_mul_simple);
  tcase_add_test(tc, test_div_simple);
  tcase_add_test(tc, mul_signs);
  tcase_add_test(tc, mul_overflow_no_scale);
  tcase_add_test(tc, mul_reduce_when_S_gt_28);
  tcase_add_test(tc, mul_u192_shrink_branch);
  tcase_add_test(tc, mul_r5_even_fn);
  tcase_add_test(tc, mul_r5_odd_fn);
  suite_add_tcase(s, tc);
  return s;
}
