/**
 * @file tests_div_extra.c
 * @brief Дополнительные тесты функции деления decimal для покрытия кода
 * @details Содержит специфичные юнит-тесты для проверки редких случаев
 *          и улучшения покрытия кода функции деления decimal чисел
 */

#include "tests.h"

/**
 * @brief Тест деления на ноль
 * @details Проверяет: 1 / 0 → ошибка (код 3 = деление на ноль)
 *          Дублирует тест из других файлов для полноты покрытия
 */
START_TEST(div_by_zero) {
  s21_decimal v1 = {{1, 0, 0, 0}};  // v1 = 1
  s21_decimal v2 = {{0, 0, 0, 0}};  // v2 = 0
  s21_decimal res = {{0}};
  int ret = s21_div(v1, v2, &res);
  ck_assert_int_eq(ret, 3);  // Должна вернуть ошибку деления на ноль
}
END_TEST

/**
 * @brief Тест деления нуля на число
 * @details Проверяет: 0 / 123 = 0
 *          Деление нуля на любое ненулевое число должно давать ноль
 */
START_TEST(div_zero_numerator) {
  s21_decimal v1 = {{0, 0, 0, 0}};    // v1 = 0
  s21_decimal v2 = {{123, 0, 0, 0}};  // v2 = 123
  s21_decimal res = {{0}};
  s21_decimal expected = {{0, 0, 0, 0}};  // expected = 0
  int ret = s21_div(v1, v2, &res);
  ck_assert_int_eq(ret, 0);  // Операция должна пройти успешно
  ck_assert_int_eq(s21_is_equal(res, expected), 1);  // Результат должен быть 0
}
END_TEST

/**
 * @brief Тест деления малого положительного на отрицательное с нулевым
 * результатом
 * @details Проверяет случай, когда результат деления настолько мал, что равен
 * нулю При этом проверяется корректность обработки знака результата
 */
START_TEST(div_result_is_zero_neg_sign) {
  s21_decimal v1, v2, res;
  s21_from_int_to_decimal(1, &v1);    // v1 = 1
  s21_set_scale(&v1, 28);             // v1 = 1e-28 (очень малое число)
  s21_from_int_to_decimal(-10, &v2);  // v2 = -10

  int ret = s21_div(v1, v2, &res);

  ck_assert_int_eq(ret, 0);                 // Операция должна пройти успешно
  ck_assert_int_eq(s21_is_zero(&res), 1);   // Результат должен быть нулем
  ck_assert_int_eq(s21_get_sign(&res), 0);  // Ноль должен быть положительным
}
END_TEST

/**
 * @brief Тест переполнения мантиссы в цикле деления
 * @details Проверяет: 2 / (3e-28) = большое число
 *          Тест специально создан для проверки обработки переполнения
 *          мантиссы в процессе деления
 */
START_TEST(div_mantissa_overflow_in_loop) {
  s21_decimal v1, v2, res;
  s21_from_int_to_decimal(2, &v1);  // v1 = 2
  s21_from_int_to_decimal(3, &v2);  // v2 = 3
  s21_set_scale(&v2, 28);           // v2 = 3e-28 (очень малое число)

  int ret = s21_div(v1, v2, &res);

  ck_assert_int_eq(ret, 0);  // Операция должна пройти успешно

  float fres;
  s21_from_decimal_to_float(res, &fres);
  ck_assert_float_eq_eps(fres, 0.6666666e28, 1e22);  // Результат ≈ 6.67e27
}
END_TEST

/**
 * @brief Тест корректировки масштаба с положительным переполнением
 * @details Проверяет случай автоматической корректировки масштаба
 *          при делении числа с большим масштабом
 */
START_TEST(div_scale_adjust_positive_overflow) {
  s21_decimal v1, v2, res;

  s21_from_int_to_decimal(1, &v1);  // v1 = 1
  s21_set_scale(&v1, 15);           // v1 = 1e-15
  s21_from_int_to_decimal(7, &v2);  // v2 = 7

  int ret = s21_div(v1, v2, &res);
  ck_assert_int_eq(ret, 0);  // Операция должна пройти успешно
  ck_assert_int_eq(s21_get_scale(&res),
                   28);  // Масштаб должен быть максимальным
}
END_TEST

/**
 * @brief Тест banker's rounding и покрытие сложения
 * @details Проверяет деление максимального decimal на 2 с применением
 *          banker's rounding и проверкой операций сложения в алгоритме
 */
START_TEST(div_banker_and_add_coverage) {
  s21_decimal v1, v2, res;

  v1.bits[0] = 0xFFFFFFFF;  // Максимальное decimal значение
  v1.bits[1] = 0xFFFFFFFF;  // (все биты мантиссы установлены)
  v1.bits[2] = 0xFFFFFFFF;
  v1.bits[3] = 0;

  s21_from_int_to_decimal(2, &v2);  // v2 = 2

  int ret = s21_div(v1, v2, &res);
  ck_assert_int_eq(ret, 0);  // Операция должна пройти успешно

  s21_decimal expected;
  s21_reset_value(&expected);     // Обнуление expected
  expected.bits[2] = 0x80000000;  // Установка ожидаемого значения
  ck_assert_int_eq(s21_is_equal(res, expected), 1);  // Проверка результата
}
END_TEST

/**
 * @brief Создание тестового набора для дополнительного покрытия деления
 * @return Указатель на созданный Suite
 * @details Объединяет все тесты для улучшения покрытия кода функции деления
 */
Suite* test_div_extra(void) {
  Suite* s = suite_create("S21_div_extra_coverage");  // Создание набора тестов
  TCase* tc = tcase_create("div_extra_TC");           // Создание группы тестов

  // Добавление всех тестовых случаев в группу
  tcase_add_test(tc, div_by_zero);         // Деление на ноль
  tcase_add_test(tc, div_zero_numerator);  // Деление нуля
  tcase_add_test(
      tc,
      div_result_is_zero_neg_sign);  // Нулевой результат с отрицательным знаком
  tcase_add_test(tc, div_mantissa_overflow_in_loop);  // Переполнение в цикле
  tcase_add_test(tc,
                 div_scale_adjust_positive_overflow);  // Корректировка масштаба
  tcase_add_test(tc,
                 div_banker_and_add_coverage);  // Banker's rounding и сложение

  suite_add_tcase(s, tc);  // Добавление группы в набор тестов
  return s;                // Возврат готового набора
}