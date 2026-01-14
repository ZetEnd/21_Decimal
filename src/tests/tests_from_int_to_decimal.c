/**
 * @file tests_from_int_to_decimal.c
 * @brief Тесты функции конвертации int в decimal
 * @details Содержит юнит-тесты для проверки корректности конвертации
 *          целых чисел в decimal формат, включая граничные значения
 */

#include <limits.h>  // Для INT_MIN, INT_MAX

#include "tests.h"

/**
 * @brief Тест конвертации нуля
 * @details Проверяет: int(0) → decimal(0) с масштабом 0 и положительным знаком
 */
START_TEST(from_int_1) {
  int src = 0;            // Исходное значение: ноль
  s21_decimal d = {{0}};  // Инициализация decimal структуры
  ck_assert_int_eq(s21_from_int_to_decimal(src, &d),
                   0);                     // Операция должна пройти успешно
  ck_assert_int_eq(d.bits[0], 0);          // Мантисса должна быть 0
  ck_assert_int_eq(s21_get_scale(&d), 0);  // Масштаб должен быть 0
  ck_assert_int_eq(s21_get_sign(&d), 0);   // Знак должен быть положительным
}
END_TEST

/**
 * @brief Тест конвертации положительной единицы
 * @details Проверяет: int(1) → decimal(1) с масштабом 0 и положительным знаком
 */
START_TEST(from_int_2) {
  int src = 1;            // Исходное значение: единица
  s21_decimal d = {{0}};  // Инициализация decimal структуры
  ck_assert_int_eq(s21_from_int_to_decimal(src, &d),
                   0);                     // Операция должна пройти успешно
  ck_assert_int_eq(d.bits[0], 1);          // Мантисса должна быть 1
  ck_assert_int_eq(s21_get_scale(&d), 0);  // Масштаб должен быть 0
  ck_assert_int_eq(s21_get_sign(&d), 0);   // Знак должен быть положительным
}
END_TEST

/**
 * @brief Тест конвертации отрицательной единицы
 * @details Проверяет: int(-1) → decimal(-1) с масштабом 0 и отрицательным
 * знаком
 */
START_TEST(from_int_3) {
  int src = -1;           // Исходное значение: минус единица
  s21_decimal d = {{0}};  // Инициализация decimal структуры
  ck_assert_int_eq(s21_from_int_to_decimal(src, &d),
                   0);                     // Операция должна пройти успешно
  ck_assert_int_eq(d.bits[0], 1);          // Мантисса должна быть 1 (модуль)
  ck_assert_int_eq(s21_get_scale(&d), 0);  // Масштаб должен быть 0
  ck_assert_int_eq(s21_get_sign(&d), 1);   // Знак должен быть отрицательным
}
END_TEST

/**
 * @brief Тест конвертации среднего положительного числа
 * @details Проверяет: int(123456) → decimal(123456) с масштабом 0 и
 * положительным знаком
 */
START_TEST(from_int_4) {
  int src = 123456;       // Исходное значение: 123456
  s21_decimal d = {{0}};  // Инициализация decimal структуры
  ck_assert_int_eq(s21_from_int_to_decimal(src, &d),
                   0);                     // Операция должна пройти успешно
  ck_assert_int_eq(d.bits[0], 123456);     // Мантисса должна быть 123456
  ck_assert_int_eq(s21_get_scale(&d), 0);  // Масштаб должен быть 0
  ck_assert_int_eq(s21_get_sign(&d), 0);   // Знак должен быть положительным
}
END_TEST

/**
 * @brief Тест конвертации среднего отрицательного числа
 * @details Проверяет: int(-654321) → decimal(-654321) с масштабом 0 и
 * отрицательным знаком
 */
START_TEST(from_int_5) {
  int src = -654321;      // Исходное значение: -654321
  s21_decimal d = {{0}};  // Инициализация decimal структуры
  ck_assert_int_eq(s21_from_int_to_decimal(src, &d),
                   0);                  // Операция должна пройти успешно
  ck_assert_int_eq(d.bits[0], 654321);  // Мантисса должна быть 654321 (модуль)
  ck_assert_int_eq(s21_get_scale(&d), 0);  // Масштаб должен быть 0
  ck_assert_int_eq(s21_get_sign(&d), 1);   // Знак должен быть отрицательным
}
END_TEST

/**
 * @brief Тест конвертации максимального int значения
 * @details Проверяет: INT_MAX (2147483647) → decimal с корректным
 * представлением
 */
START_TEST(from_int_6) {
  int src = INT_MAX;      // Исходное значение: максимальный int
  s21_decimal d = {{0}};  // Инициализация decimal структуры
  ck_assert_int_eq(s21_from_int_to_decimal(src, &d),
                   0);                     // Операция должна пройти успешно
  ck_assert_int_eq(d.bits[0], INT_MAX);    // Мантисса должна быть равна INT_MAX
  ck_assert_int_eq(s21_get_scale(&d), 0);  // Масштаб должен быть 0
  ck_assert_int_eq(s21_get_sign(&d), 0);   // Знак должен быть положительным
}
END_TEST

/**
 * @brief Тест конвертации минимального int значения
 * @details Проверяет: INT_MIN (-2147483648) → decimal с корректным
 * представлением Специальный случай: INT_MIN имеет модуль больше INT_MAX на 1
 */
START_TEST(from_int_7) {
  int src = INT_MIN;      // Исходное значение: минимальный int
  s21_decimal d = {{0}};  // Инициализация decimal структуры
  ck_assert_int_eq(s21_from_int_to_decimal(src, &d),
                   0);  // Операция должна пройти успешно
  ck_assert_uint_eq((unsigned)d.bits[0],
                    (unsigned)INT_MAX + 1u);  // Модуль INT_MIN = INT_MAX + 1
  ck_assert_int_eq(s21_get_sign(&d), 1);      // Знак должен быть отрицательным
}
END_TEST

/**
 * @brief Тест конвертации миллиарда (положительного)
 * @details Проверяет: int(1000000000) → decimal с корректным представлением
 */
START_TEST(from_int_8) {
  int src = 1000000000;   // Исходное значение: миллиард
  s21_decimal d = {{0}};  // Инициализация decimal структуры
  ck_assert_int_eq(s21_from_int_to_decimal(src, &d),
                   0);                      // Операция должна пройти успешно
  ck_assert_int_eq(d.bits[0], 1000000000);  // Мантисса должна быть 1000000000
  ck_assert_int_eq(s21_get_scale(&d), 0);   // Масштаб должен быть 0
}
END_TEST

/**
 * @brief Тест конвертации минус миллиарда
 * @details Проверяет: int(-1000000000) → decimal с корректным представлением
 */
START_TEST(from_int_9) {
  int src = -1000000000;  // Исходное значение: минус миллиард
  s21_decimal d = {{0}};  // Инициализация decimal структуры
  ck_assert_int_eq(s21_from_int_to_decimal(src, &d),
                   0);  // Операция должна пройти успешно
  ck_assert_int_eq(d.bits[0],
                   1000000000);  // Мантисса должна быть 1000000000 (модуль)
  ck_assert_int_eq(s21_get_sign(&d), 1);  // Знак должен быть отрицательным
}
END_TEST

/**
 * @brief Тест обработки NULL указателя
 * @details Проверяет: передача NULL в качестве dst должна вернуть код ошибки 1
 */
START_TEST(from_int_10) {
  int err = s21_from_int_to_decimal(0, NULL);  // Передача NULL указателя
  ck_assert_int_eq(err, 1);                    // Должна вернуть код ошибки 1
}
END_TEST

/**
 * @brief Создание тестового набора для функции s21_from_int_to_decimal
 * @details Группирует все тестовые случаи в единый набор тестов для
 *          функции конвертации int в decimal
 * @return Указатель на созданный тестовый набор
 */
Suite* test_from_int(void) {
  Suite* s = suite_create(
      "s21_from_int_to_decimal");           // Создание основного набора тестов
  TCase* tc = tcase_create("from_int_TC");  // Создание группы тестовых случаев

  // Добавление всех тестовых случаев в группу
  tcase_add_test(tc, from_int_1);   // Тест конвертации нуля
  tcase_add_test(tc, from_int_2);   // Тест конвертации +1
  tcase_add_test(tc, from_int_3);   // Тест конвертации -1
  tcase_add_test(tc, from_int_4);   // Тест среднего положительного числа
  tcase_add_test(tc, from_int_5);   // Тест среднего отрицательного числа
  tcase_add_test(tc, from_int_6);   // Тест максимального int
  tcase_add_test(tc, from_int_7);   // Тест минимального int
  tcase_add_test(tc, from_int_8);   // Тест миллиарда
  tcase_add_test(tc, from_int_9);   // Тест минус миллиарда
  tcase_add_test(tc, from_int_10);  // Тест NULL указателя

  suite_add_tcase(s, tc);  // Добавление группы в набор
  return s;                // Возврат готового набора тестов
}
