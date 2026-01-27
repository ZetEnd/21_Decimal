/**
 * @file tests_utilities_extra.c
 * @brief Дополнительные тесты для вспомогательных функций s21_decimal
 * @details Содержит юнит-тесты для проверки корректности работы
 *          вспомогательных функций: banker's rounding, масштабирование,
 *          обработка нулевых значений и общие утилиты
 */

#include "tests.h"

/**
 * @brief Вспомогательная функция для создания decimal значений в тестах
 * @param lo Младшие 32 бита мантиссы
 * @param mid Средние 32 бита мантиссы
 * @param hi Старшие 32 бита мантиссы
 * @param scale Масштаб (количество цифр после запятой)
 * @param sign Знак (0 = положительный, 1 = отрицательный)
 * @return Созданное decimal значение
 */
static s21_decimal mk(unsigned lo, unsigned mid, unsigned hi, int scale,
                      int sign) {
  s21_decimal d = {{(int)lo, (int)mid, (int)hi, 0}};  // Инициализация мантиссы
  s21_set_scale(&d, scale);                           // Установка масштаба
  s21_set_sign(&d, sign);                             // Установка знака
  return d;  // Возврат готового decimal
}

/**
 * @brief Тест banker's rounding с остатком больше 5
 * @details Проверяет: 26 / 10 = 2.6 → 3 (округление вверх, так как остаток > 5)
 *          Функция s21_scale_down_one_banker должна правильно округлить
 */
START_TEST(util_banker_rem_gt5) {
  s21_decimal d = mk(26, 0, 0, 1, 0);      // decimal = 2.6 (26 с масштабом 1)
  s21_scale_down_one_banker(&d);           // Применение banker's rounding
  ck_assert_int_eq(s21_get_scale(&d), 0);  // Масштаб должен уменьшиться до 0
  ck_assert_int_eq(d.bits[0], 3);          // 2.6 → 3 (округление вверх)
}
END_TEST

/**
 * @brief Тест banker's rounding с остатком равным 5 (четное число)
 * @details Проверяет: 25 / 10 = 2.5 → 2 (округление к четному числу)
 *          При остатке ровно 5 banker's rounding округляет к ближайшему четному
 */
START_TEST(util_banker_rem_eq5_even) {
  s21_decimal d = mk(25, 0, 0, 1, 0);      // decimal = 2.5 (25 с масштабом 1)
  s21_scale_down_one_banker(&d);           // Применение banker's rounding
  ck_assert_int_eq(s21_get_scale(&d), 0);  // Масштаб должен уменьшиться до 0
  ck_assert_int_eq(d.bits[0], 2);          // 2.5 → 2 (к четному)
}
END_TEST

/**
 * @brief Тест banker's rounding с остатком равным 5 (нечетное число)
 * @details Проверяет: 15 / 10 = 1.5 → 2 (округление к четному числу)
 *          При остатке ровно 5 banker's rounding округляет к ближайшему четному
 */
START_TEST(util_banker_rem_eq5_odd) {
  s21_decimal d = mk(15, 0, 0, 1, 0);      // decimal = 1.5 (15 с масштабом 1)
  s21_scale_down_one_banker(&d);           // Применение banker's rounding
  ck_assert_int_eq(s21_get_scale(&d), 0);  // Масштаб должен уменьшиться до 0
  ck_assert_int_eq(d.bits[0], 2);          // 1.5 → 2 (к четному)
}
END_TEST

/**
 * @brief Тест переполнения при увеличении масштаба
 * @details Проверяет поведение при попытке увеличить масштаб максимального
 * числа Умножение на 10 должно привести к переполнению
 */
START_TEST(util_scale_up_overflow) {
  s21_decimal max =
      mk(0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);  // Максимальное значение
  int rc = s21_scale_up_one(&max);  // Попытка увеличить масштаб (× 10)
  ck_assert_int_eq(rc, 1);          // Должна вернуть ошибку переполнения
}
END_TEST

/**
 * @brief Тест успешного увеличения масштаба
 * @details Проверяет: 123 → 1230 с увеличением масштаба на 1
 *          Увеличение масштаба эквивалентно умножению на 10
 */
START_TEST(util_scale_up_ok) {
  s21_decimal d = mk(123, 0, 0, 0, 0);     // decimal = 123
  int rc = s21_scale_up_one(&d);           // Увеличение масштаба на 1
  ck_assert_int_eq(rc, 0);                 // Операция должна пройти успешно
  ck_assert_int_eq(s21_get_scale(&d), 1);  // Масштаб должен увеличиться до 1
  ck_assert_int_eq((unsigned)d.bits[0], 1230u);  // 123 × 10 = 1230
}
END_TEST

/**
 * @brief Тест удаления завершающих нулей
 * @details Проверяет: 1200 с масштабом 3 (1.200) → 12 с масштабом 1 (1.2)
 *          Функция должна удалить незначащие нули и скорректировать масштаб
 */
START_TEST(util_strip_zeros_basic) {
  s21_decimal d = mk(1200, 0, 0, 3, 0);  // decimal = 1.200 (1200 с масштабом 3)
  s21_strip_trailing_zeros(&d);          // Удаление завершающих нулей

  ck_assert_int_eq(s21_get_scale(&d), 1);  // Масштаб должен стать 1
  ck_assert_int_eq(d.bits[0], 12);         // Мантисса должна стать 12 (1.2)
}
END_TEST

/**
 * @brief Тест удаления нулей при масштабе 0
 * @details Проверяет поведение функции для целого числа без дробной части
 *          Число без дробной части не должно изменяться
 */
START_TEST(util_strip_zeros_scale0) {
  s21_decimal d = mk(500, 0, 0, 0, 0);     // decimal = 500 (масштаб = 0)
  s21_strip_trailing_zeros(&d);            // Попытка удаления нулей
  ck_assert_int_eq(s21_get_scale(&d), 0);  // Масштаб должен остаться 0
  ck_assert_int_eq(d.bits[0], 500);        // Значение не должно измениться
}
END_TEST

/**
 * @brief Тест безопасности при передаче NULL
 * @details Проверяет, что функция не падает при передаче NULL указателя
 */
START_TEST(util_strip_zeros_null_safe) {
  s21_strip_trailing_zeros(NULL);  // Передача NULL
  ck_assert_int_eq(1, 1);          // Тест пройден, если нет сбоя
}
END_TEST

/**
 * @brief Тест приведения к общему масштабу (увеличение масштаба)
 * @details Проверяет: a=1 (масштаб 0), b=1 (масштаб 2) → оба с масштабом 2
 *          a должно стать 100, b остается 1
 */
START_TEST(util_common_scale_grow_ok) {
  s21_decimal a = mk(1, 0, 0, 0, 0);  // a = 1.0
  s21_decimal b = mk(1, 0, 0, 2, 0);  // b = 0.01
  s21_to_common_scale(&a, &b);        // Приведение к общему масштабу
  ck_assert_int_eq(s21_get_scale(&a),
                   s21_get_scale(&b));  // Масштабы должны быть равны

  ck_assert_int_eq(s21_get_scale(&a), 2);  // Общий масштаб = 2
  ck_assert_int_eq(a.bits[0], 100);        // a = 1.00 (мантисса 100)
  ck_assert_int_eq(b.bits[0], 1);          // b = 0.01 (мантисса 1)
}
END_TEST

/**
 * @brief Тест приведения к общему масштабу с переполнением
 * @details Проверяет случай, когда увеличение масштаба приводит к переполнению
 *          В этом случае должно происходить уменьшение масштаба другого числа
 */
START_TEST(util_common_scale_grow_overflow_then_shrink_other) {
  s21_decimal a =
      mk(0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);  // Максимальное число
  s21_decimal b = mk(123, 0, 0, 2, 0);                  // b = 1.23
  s21_to_common_scale(&a, &b);  // Приведение к общему масштабу
  ck_assert_int_eq(s21_get_scale(&a),
                   s21_get_scale(&b));  // Масштабы должны быть равны

  ck_assert_int_eq(s21_get_scale(&a),
                   0);  // Общий масштаб = 0 (из-за переполнения)
  ck_assert_int_eq(s21_get_scale(&b), 0);  // b тоже должно иметь масштаб 0
}
END_TEST

/**
 * @brief Тест безопасности при передаче NULL в приведении масштаба
 * @details Проверяет, что функция не падает при передаче NULL указателей
 */
START_TEST(util_common_scale_null_safe) {
  s21_decimal x = mk(1, 0, 0, 0, 0);
  s21_to_common_scale(&x, NULL);  // Один NULL
  s21_to_common_scale(NULL, &x);  // Другой NULL
  ck_assert_int_eq(1, 1);         // Тест пройден, если нет сбоя
}
END_TEST

/**
 * @brief Тест безопасности сброса значения при NULL
 * @details Проверяет, что функция сброса не падает при передаче NULL
 */
START_TEST(util_reset_null_safe) {
  s21_reset_value(NULL);   // Передача NULL
  ck_assert_int_eq(1, 1);  // Тест пройден, если нет сбоя
}
END_TEST

/**
 * @brief Тест безопасности проверки на ноль при NULL
 * @details Проверяет, что функция проверки на ноль корректно обрабатывает NULL
 */
START_TEST(util_is_zero_null_safe) {
  ck_assert_int_eq(s21_is_zero(NULL), 0);  // NULL не является нулем
}
END_TEST

/**
 * @brief Тест установки различных значений масштаба
 * @details Проверяет корректность установки масштаба от 0 до 28
 */
START_TEST(set_scale_cases_fn) {
  s21_decimal d = {{0, 0, 0, 0}};

  s21_set_scale(&d, 0);  // Установка масштаба 0
  ck_assert_int_eq(s21_get_scale(&d), 0);

  s21_set_scale(&d, 28);  // Установка максимального масштаба
  ck_assert_int_eq(s21_get_scale(&d), 28);

  s21_set_scale(&d, 17);  // Установка промежуточного масштаба
  ck_assert_int_eq(s21_get_scale(&d), 17);
}
END_TEST

/**
 * @brief Тест определения нулевого значения
 * @details Проверяет корректность определения нулевых и ненулевых значений
 */
START_TEST(is_zero_paths_fn) {
  s21_decimal z = (s21_decimal){{0, 0, 0, 0}};   // Нулевое значение
  s21_decimal nz = (s21_decimal){{1, 0, 0, 0}};  // Ненулевое значение

  ck_assert_int_eq(s21_is_zero(&z), 1);  // Ноль должен определяться как ноль
  ck_assert_int_eq(s21_is_zero(&nz),
                   0);  // Ненулевое значение не должно быть нулем
}
END_TEST

/**
 * @brief Создание тестового набора для дополнительных утилит
 * @return Указатель на созданный Suite
 * @details Объединяет все тесты вспомогательных функций в один набор
 */
Suite* test_utilities_extra(void) {
  Suite* s = suite_create("s21_utilities_extra");  // Создание набора тестов
  TCase* tc = tcase_create("utilities_extra_TC");  // Создание группы тестов

  // Тесты banker's rounding
  tcase_add_test(tc, util_banker_rem_gt5);       // Остаток > 5
  tcase_add_test(tc, util_banker_rem_eq5_even);  // Остаток = 5, к четному
  tcase_add_test(tc, util_banker_rem_eq5_odd);   // Остаток = 5, от нечетного
  tcase_add_test(tc, is_zero_paths_fn);          // Проверка на ноль

  // Тесты масштабирования
  tcase_add_test(tc, util_scale_up_overflow);  // Переполнение при увеличении
  tcase_add_test(tc, util_scale_up_ok);        // Успешное увеличение

  // Тесты удаления нулей
  tcase_add_test(tc, util_strip_zeros_basic);      // Базовое удаление нулей
  tcase_add_test(tc, util_strip_zeros_scale0);     // При масштабе 0
  tcase_add_test(tc, util_strip_zeros_null_safe);  // Безопасность NULL

  // Тесты общего масштаба
  tcase_add_test(tc, util_common_scale_grow_ok);  // Успешное приведение
  tcase_add_test(
      tc,
      util_common_scale_grow_overflow_then_shrink_other);  // С переполнением
  tcase_add_test(tc, util_common_scale_null_safe);         // Безопасность NULL

  // Тесты безопасности
  tcase_add_test(tc, util_reset_null_safe);    // Сброс NULL
  tcase_add_test(tc, util_is_zero_null_safe);  // Проверка NULL на ноль

  // Тесты установки масштаба
  tcase_add_test(tc, set_scale_cases_fn);  // Различные значения масштаба

  suite_add_tcase(s, tc);  // Добавление группы в набор тестов
  return s;                // Возврат готового набора
}
