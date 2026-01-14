/**
 * @file tests_from_decimal_int.c
 * @brief Тесты функции конвертации decimal в int
 * @details Содержит юнит-тесты для проверки корректности конвертации
 *          decimal чисел в целые числа с учетом масштаба и знака
 */

#include "tests.h"

/**
 * @brief Тест конвертации простого положительного decimal
 * @details Проверяет: decimal(100) → int(100)
 *          Без масштаба, прямая конвертация
 */
START_TEST(to_int_1) {
  s21_decimal d = {{100, 0, 0, 0}};  // decimal = 100
  int result = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(d, &result),
                   0);            // Операция должна пройти успешно
  ck_assert_int_eq(result, 100);  // Результат должен быть 100
}
END_TEST

/**
 * @brief Тест конвертации отрицательного decimal
 * @details Проверяет: decimal(-42) → int(-42)
 *          Проверка корректной обработки знака
 */
START_TEST(to_int_2) {
  s21_decimal d = {
      {42, 0, 0, 0x80000000}};  // decimal = -42 (знак в старшем бите)
  int result = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(d, &result),
                   0);            // Операция должна пройти успешно
  ck_assert_int_eq(result, -42);  // Результат должен быть -42
}
END_TEST

/**
 * @brief Тест конвертации нуля
 * @details Проверяет: decimal(0) → int(0)
 *          Проверка, что предыдущее значение result перезаписывается
 */
START_TEST(to_int_3) {
  s21_decimal d = {{0, 0, 0, 0}};  // decimal = 0
  int result = 1234;               // Намеренно не нулевое значение
  ck_assert_int_eq(s21_from_decimal_to_int(d, &result),
                   0);          // Операция должна пройти успешно
  ck_assert_int_eq(result, 0);  // Результат должен быть 0
}
END_TEST

/**
 * @brief Тест конвертации с масштабом (деление на 1000)
 * @details Проверяет: decimal(12345) с масштабом 3 → int(12)
 *          12345 / 10^3 = 12.345 → 12 (целая часть)
 */
START_TEST(to_int_4) {
  s21_decimal d = {{12345, 0, 0, 3 << 16}};  // decimal = 12345, scale = 3
  int result = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(d, &result),
                   0);           // Операция должна пройти успешно
  ck_assert_int_eq(result, 12);  // 12345 / 1000 = 12 (целая часть)
}
END_TEST

/**
 * @brief Тест конвертации малого числа с большим масштабом
 * @details Проверяет: decimal(1) с масштабом 6 → int(0)
 *          1 / 10^6 = 0.000001 → 0 (целая часть)
 */
START_TEST(to_int_5) {
  s21_decimal d = {{1, 0, 0, 6 << 16}};  // decimal = 1, scale = 6
  int result = -1;                       // Намеренно не нулевое значение
  ck_assert_int_eq(s21_from_decimal_to_int(d, &result),
                   0);          // Операция должна пройти успешно
  ck_assert_int_eq(result, 0);  // 1 / 10^6 ≈ 0
}
END_TEST

/**
 * @brief Тест конвертации с масштабом (деление на 100)
 * @details Проверяет: decimal(123) с масштабом 2 → int(1)
 *          123 / 10^2 = 1.23 → 1 (целая часть)
 */
START_TEST(to_int_6) {
  s21_decimal d = {{123, 0, 0, 2 << 16}};  // decimal = 123, scale = 2
  int result = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(d, &result),
                   0);          // Операция должна пройти успешно
  ck_assert_int_eq(result, 1);  // 123 / 100 = 1 (целая часть)
}
END_TEST

/**
 * @brief Тест конвертации переполнения decimal
 * @details Проверяет обработку decimal, превышающего диапазон int
 *          Значение 0xFFFFFFFF FFFFFFFF слишком велико для int
 */
START_TEST(to_int_7) {
  s21_decimal d = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};  // Очень большое число
  int result = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(d, &result),
                   1);  // Должна вернуть ошибку переполнения
}
END_TEST

/**
 * @brief Тест конвертации с NULL указателем
 * @details Проверяет обработку ошибки при передаче NULL в качестве результата
 */
START_TEST(to_int_8) {
  s21_decimal d = {{100, 0, 0, 0}};  // Любое валидное decimal значение
  int err = s21_from_decimal_to_int(d, NULL);
  ck_assert_int_eq(err, 1);  // Должна вернуть ошибку конвертации
}
END_TEST

/**
 * @brief Тест конвертации отрицательного числа с масштабом
 * @details Проверяет: decimal(-98765) с масштабом 3 → int(-98)
 *          -98765 / 10^3 = -98.765 → -98 (целая часть)
 */
START_TEST(to_int_9) {
  s21_decimal d = {{98765, 0, 0, (3 << 16) | 0x80000000}};  // -98765, scale = 3
  int result = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(d, &result),
                   0);            // Операция должна пройти успешно
  ck_assert_int_eq(result, -98);  // -98765 / 1000 = -98
}
END_TEST

/**
 * @brief Тест конвертации максимального int значения
 * @details Проверяет: decimal(2147483647) → int(2147483647)
 *          Граничное значение - максимальный int
 */
START_TEST(to_int_10) {
  s21_decimal d = {{2147483647, 0, 0, 0}};  // decimal = INT_MAX
  int result = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(d, &result),
                   0);                   // Операция должна пройти успешно
  ck_assert_int_eq(result, 2147483647);  // Результат = INT_MAX
}
END_TEST

/**
 * @brief Создание тестового набора для конвертации decimal → int
 * @return Указатель на созданный Suite
 * @details Объединяет все тесты конвертации в один набор
 */
Suite* test_to_int(void) {
  Suite* s = suite_create("s21_from_decimal_to_int");  // Создание набора тестов
  TCase* tc = tcase_create("to_int_TC");               // Создание группы тестов

  // Добавление всех тестовых случаев в группу
  tcase_add_test(tc, to_int_1);   // Простое положительное число
  tcase_add_test(tc, to_int_2);   // Отрицательное число
  tcase_add_test(tc, to_int_3);   // Ноль
  tcase_add_test(tc, to_int_4);   // Число с масштабом (деление)
  tcase_add_test(tc, to_int_5);   // Малое число с большим масштабом
  tcase_add_test(tc, to_int_6);   // Число с масштабом 2
  tcase_add_test(tc, to_int_7);   // Переполнение
  tcase_add_test(tc, to_int_8);   // Обработка ошибки (NULL)
  tcase_add_test(tc, to_int_9);   // Отрицательное с масштабом
  tcase_add_test(tc, to_int_10);  // Максимальное int значение

  suite_add_tcase(s, tc);  // Добавление группы в набор тестов
  return s;                // Возврат готового набора
}
