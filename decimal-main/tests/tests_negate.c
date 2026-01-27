/**
 * @file tests_negate.c
 * @brief Тесты функции смены знака decimal
 * @details Содержит юнит-тесты для проверки корректности функции s21_negate,
 *          которая изменяет знак decimal числа на противоположный
 */

#include "tests.h"

/**
 * @brief Тест простой смены знака
 * @details Проверяет: negate(5) → -5
 *          Базовый случай изменения знака с положительного на отрицательный
 */
START_TEST(test_negate_simple) {
  s21_decimal a, res, expected;
  s21_from_int_to_decimal(5, &a);            // a = 5
  s21_from_int_to_decimal(-5, &expected);    // expected = -5
  ck_assert_int_eq(s21_negate(a, &res), 0);  // Операция должна пройти успешно
  ck_assert_int_eq(s21_is_equal(res, expected), 1);  // res должно равняться -5
}
END_TEST

/**
 * @brief Создание тестового набора для функции смены знака
 * @return Указатель на созданный Suite
 * @details Объединяет все тесты функции negate в один набор
 */
Suite* test_negate(void) {
  Suite* s = suite_create("s21_negate");  // Создание набора тестов
  TCase* tc = tcase_create("negate");     // Создание группы тестов
  tcase_add_test(tc,
                 test_negate_simple);  // Добавление теста простой смены знака
  suite_add_tcase(s, tc);              // Добавление группы в набор тестов
  return s;                            // Возврат готового набора
}
