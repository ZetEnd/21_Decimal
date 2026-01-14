/**
 * @file tests_from_decimal_to_float.c
 * @brief Тесты функции конвертации decimal в float
 * @details Содержит юнит-тесты для проверки корректности конвертации
 *          decimal чисел в числа с плавающей точкой, включая граничные
 *          случаи и потерю точности
 */

#include "tests.h"

/**
 * @brief Тест конвертации простого положительного decimal
 * @details Проверяет: decimal(100) → float(100.0)
 *          Базовый случай конвертации целого числа
 */
START_TEST(to_float_1) {
  s21_decimal d = {{100, 0, 0, 0}};  // decimal = 100
  float result;
  ck_assert_int_eq(s21_from_decimal_to_float(d, &result),
                   0);  // Операция должна пройти успешно
  ck_assert_float_eq_eps(result, 100.0f,
                         1e-6f);  // Результат = 100.0 ± погрешность
}
END_TEST

/**
 * @brief Тест конвертации отрицательного decimal
 * @details Проверяет: decimal(-42) → float(-42.0)
 *          Проверка корректной обработки знака
 */
START_TEST(to_float_2) {
  s21_decimal d = {
      {42, 0, 0, 0x80000000}};  // decimal = -42 (знак в старшем бите)
  float result;
  ck_assert_int_eq(s21_from_decimal_to_float(d, &result),
                   0);  // Операция должна пройти успешно
  ck_assert_float_eq_eps(result, -42.0f,
                         1e-6f);  // Результат = -42.0 ± погрешность
}
END_TEST

/**
 * @brief Тест конвертации нуля
 * @details Проверяет: decimal(0) → float(0.0)
 *          Специальный случай - нулевое значение
 */
START_TEST(to_float_3) {
  s21_decimal d = {{0, 0, 0, 0}};  // decimal = 0
  float result;
  ck_assert_int_eq(s21_from_decimal_to_float(d, &result),
                   0);  // Операция должна пройти успешно
  ck_assert_float_eq_eps(result, 0.0f, 1e-6f);  // Результат = 0.0 ± погрешность
}
END_TEST

/**
 * @brief Тест конвертации дробного числа
 * @details Проверяет: decimal(0.1) → float(0.1)
 *          Конвертация числа с дробной частью и масштабом 1
 */
START_TEST(to_float_4) {
  s21_decimal d = {
      {1, 0, 0, 1 << 16}};  // decimal = 0.1 (мантисса 1, масштаб 1)
  float result;
  ck_assert_int_eq(s21_from_decimal_to_float(d, &result),
                   0);  // Операция должна пройти успешно
  ck_assert_float_eq_eps(result, 0.1f, 1e-6f);  // Результат = 0.1 ± погрешность
}
END_TEST

/**
 * @brief Тест конвертации очень малого числа
 * @details Проверяет: decimal(1e-7) → float(1e-7)
 *          Малое число с масштабом 7
 */
START_TEST(to_float_5) {
  s21_decimal d = {
      {1, 0, 0, 7 << 16}};  // decimal = 1e-7 (мантисса 1, масштаб 7)
  float result;
  ck_assert_int_eq(s21_from_decimal_to_float(d, &result),
                   0);  // Операция должна пройти успешно
  ck_assert_float_eq_eps(result, 1e-7f,
                         1e-9f);  // Результат = 1e-7 ± погрешность
}
END_TEST

/**
 * @brief Тест конвертации сверхмалого числа
 * @details Проверяет: decimal(1e-28) → float(~1e-28)
 *          Число на границе точности float (может потребовать грубое сравнение)
 */
START_TEST(to_float_6) {
  s21_decimal d = {
      {1, 0, 0, 28 << 16}};  // decimal = 1e-28 (максимальный масштаб)
  float result;
  ck_assert_int_eq(s21_from_decimal_to_float(d, &result),
                   0);  // Операция должна пройти успешно
  ck_assert_float_eq_eps(result, 1e-28f,
                         1e-30f);  // Результат ≈ 1e-28 (грубое сравнение)
}
END_TEST

/**
 * @brief Тест конвертации максимального decimal
 * @details Проверяет: decimal(max) → float(~7.9228163e28)
 *          Максимальное decimal значение с потерей точности в float
 *          Float имеет только ~7 значащих цифр
 */
START_TEST(to_float_7) {
  s21_decimal d = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};  // Максимальное decimal
  float result;
  ck_assert_int_eq(s21_from_decimal_to_float(d, &result),
                   0);  // Операция должна пройти успешно
  /* Для огромных чисел у float всего ~7 знаков, ставим широкий eps */
  ck_assert_float_eq_eps(result, 7.9228163e28f,
                         1e22f);  // Результат с большой погрешностью
}
END_TEST

/**
 * @brief Тест конвертации максимального decimal с масштабом
 * @details Проверяет: decimal(max/10^28) → float(~7.9228163)
 *          Максимальное decimal значение с максимальным масштабом
 */
START_TEST(to_float_8) {
  s21_decimal d = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 28 << 16}};  // max decimal / 10^28
  float result;
  ck_assert_int_eq(s21_from_decimal_to_float(d, &result),
                   0);  // Операция должна пройти успешно
  /* max / 10^28 ≈ 7.9228163 — это нормально для float */
  ck_assert_float_eq_eps(result, 7.9228163f, 1e-5f);  // Результат ≈ 7.9228163
}
END_TEST

/**
 * @brief Тест конвертации отрицательного нуля
 * @details Проверяет: decimal(-0) → float(-0.0)
 *          Специальный случай - отрицательный ноль
 */
START_TEST(to_float_9) {
  s21_decimal d = {
      {0, 0, 0, 0x80000000}};  // decimal = -0 (ноль с отрицательным знаком)
  float result;
  ck_assert_int_eq(s21_from_decimal_to_float(d, &result),
                   0);  // Операция должна пройти успешно
  ck_assert_float_eq_eps(result, -0.0f, 1e-6f);  // Результат = -0.0
}
END_TEST

/**
 * @brief Тест конвертации с NULL указателем
 * @details Проверяет обработку ошибки при передаче NULL в качестве результата
 */
START_TEST(to_float_10) {
  s21_decimal d = {{100, 0, 0, 0}};  // Любое валидное decimal значение
  ck_assert_int_eq(s21_from_decimal_to_float(d, NULL),
                   1);  // Должна вернуть ошибку конвертации
}
END_TEST

/**
 * @brief Создание тестового набора для конвертации decimal → float
 * @return Указатель на созданный Suite
 * @details Объединяет все тесты конвертации в один набор, включая
 *          различные масштабы, знаки и граничные случаи
 */
Suite* test_to_float(void) {
  Suite* s = suite_create("S21_to_float");  // Создание набора тестов
  TCase* tc = tcase_create("to_float_TC");  // Создание группы тестов

  // Добавление всех тестовых случаев в группу
  tcase_add_test(tc, to_float_1);   // Простое положительное число
  tcase_add_test(tc, to_float_2);   // Отрицательное число
  tcase_add_test(tc, to_float_3);   // Ноль
  tcase_add_test(tc, to_float_4);   // Дробное число
  tcase_add_test(tc, to_float_5);   // Очень малое число
  tcase_add_test(tc, to_float_6);   // Сверхмалое число
  tcase_add_test(tc, to_float_7);   // Максимальное число
  tcase_add_test(tc, to_float_8);   // Максимальное с масштабом
  tcase_add_test(tc, to_float_9);   // Отрицательный ноль
  tcase_add_test(tc, to_float_10);  // Обработка ошибки (NULL)

  suite_add_tcase(s, tc);  // Добавление группы в набор тестов
  return s;                // Возврат готового набора
}
