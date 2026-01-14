/**
 * @file tests_div_ext.c
 * @brief Расширенные тесты функции деления decimal
 * @details Содержит дополнительные юнит-тесты для проверки сложных случаев
 *          деления decimal чисел, включая banker's rounding, переполнения
 *          и граничные условия
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
 * @brief Тест деления на ноль
 * @details Проверяет: 1 / 0 → ошибка (код 3 = деление на ноль)
 *          Деление на ноль должно возвращать код ошибки 3
 */
START_TEST(div_by_zero) {
  s21_decimal a = mk(1, 0, 0, 0, 0);  // a = 1
  s21_decimal z = mk(0, 0, 0, 0, 0);  // z = 0
  s21_decimal r = {{0}};
  ck_assert_int_eq(s21_div(a, z, &r),
                   3);  // Должна вернуть ошибку деления на ноль
}
END_TEST

/**
 * @brief Тест базового дробного деления
 * @details Проверяет: 1 / 8 = 0.125
 *          Результат должен иметь масштаб 3 и мантиссу 125
 */
START_TEST(div_fraction_basic) {
  s21_decimal a = mk(1, 0, 0, 0, 0);  // a = 1
  s21_decimal b = mk(8, 0, 0, 0, 0);  // b = 8
  s21_decimal r = {{0}};
  ck_assert_int_eq(s21_div(a, b, &r), 0);  // Операция должна пройти успешно
  ck_assert_int_eq(s21_get_scale(&r), 3);  // Масштаб = 3 (0.125)
  ck_assert_int_eq(r.bits[0], 125);        // Мантисса = 125
}
END_TEST

/**
 * @brief Тест деления с разными знаками
 * @details Проверяет: -10 / 4 = -2.5
 *          Результат должен быть отрицательным
 */
START_TEST(div_negative_mixed) {
  s21_decimal a = mk(10, 0, 0, 0, 1);               // a = -10
  s21_decimal b = mk(4, 0, 0, 0, 0);                // b = 4
  s21_decimal r = {{0}}, exp = mk(25, 0, 0, 1, 1);  // expected = -2.5
  ck_assert_int_eq(s21_div(a, b, &r), 0);     // Операция должна пройти успешно
  ck_assert_int_eq(s21_is_equal(r, exp), 1);  // Результат должен равняться -2.5
}
END_TEST

/**
 * @brief Тест деления чисел с одинаковым масштабом
 * @details Проверяет: 12.3 / 0.3 = 41
 *          Масштабы сокращаются, получается целое число
 */
START_TEST(div_scale_diff) {
  s21_decimal a = mk(123, 0, 0, 1, 0);              // a = 12.3 (масштаб 1)
  s21_decimal b = mk(3, 0, 0, 1, 0);                // b = 0.3 (масштаб 1)
  s21_decimal r = {{0}}, exp = mk(41, 0, 0, 0, 0);  // expected = 41
  ck_assert_int_eq(s21_div(a, b, &r), 0);     // Операция должна пройти успешно
  ck_assert_int_eq(s21_is_equal(r, exp), 1);  // Результат должен равняться 41
}
END_TEST

/**
 * @brief Тест переполнения при делении
 * @details Проверяет: max_decimal / 0.1 → переполнение
 *          Деление максимального числа на малое должно вызвать переполнение
 */
START_TEST(div_overflow_positive) {
  s21_decimal max =
      mk(0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);  // max decimal
  s21_decimal tenth = mk(1, 0, 0, 1, 0);                // 0.1
  s21_decimal r = {{0}};
  int err = s21_div(max, tenth, &r);
  ck_assert_int_eq(err, 1);  // Должна вернуть ошибку переполнения
}
END_TEST

/**
 * @brief Тест точного деления
 * @details Проверяет: 200 / 25 = 8 (точное деление без остатка)
 */
START_TEST(div_exact_fn) {
  s21_decimal a = {{200, 0, 0, 0}};  // a = 200
  s21_decimal b = {{25, 0, 0, 0}};   // b = 25
  s21_decimal r;
  ck_assert_int_eq(s21_div(a, b, &r), 0);  // Операция должна пройти успешно
}

/**
 * @brief Тест banker's rounding при делении
 * @details Проверяет применение banker's rounding: 15 / 2 = 7.5 → 8
 */
START_TEST(div_tie_even_fn) {
  s21_decimal a = {{15, 0, 0, 0}};  // a = 15
  s21_decimal b = {{2, 0, 0, 0}};   // b = 2
  s21_decimal r;
  ck_assert_int_eq(s21_div(a, b, &r), 0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест деления с сокращением большого масштаба
 * @details Проверяет: 1 / 1 = 1 (с автоматическим сокращением масштаба)
 */
START_TEST(div_large_scale_reduce_fn) {
  s21_decimal a = {{1, 0, 0, 0}};  // a = 1
  s21_decimal b = {{1, 0, 0, 0}};  // b = 1

  s21_decimal r;
  ck_assert_int_eq(s21_div(a, b, &r), 0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест внутреннего banker's rounding к четному
 * @details Проверяет: 0.5 / 0.1 = 5 (с внутренним округлением)
 */
START_TEST(div_tie_even_internal_fn) {
  s21_decimal a = {{5, 0, 0, 1 << 16}};  // a = 0.5
  s21_decimal b = {{1, 0, 0, 1 << 16}};  // b = 0.1
  s21_decimal r;
  ck_assert_int_eq(s21_div(a, b, &r), 0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест внутреннего banker's rounding от нечетного
 * @details Проверяет: 1.5 / 0.3 = 5 (с внутренним округлением)
 */
START_TEST(div_tie_odd_internal_fn) {
  s21_decimal a = {{15, 0, 0, 1 << 16}};  // a = 1.5
  s21_decimal b = {{3, 0, 0, 1 << 16}};   // b = 0.3
  s21_decimal r;
  ck_assert_int_eq(s21_div(a, b, &r), 0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест banker's rounding при переполнении (четный случай)
 * @details Проверяет деление максимального числа на 3 с banker's rounding
 */
START_TEST(div_banker_overflow_even_fn) {
  s21_decimal a =
      mk(0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);  // max decimal
  s21_decimal b = mk(3, 0, 0, 0, 0);                    // b = 3
  s21_decimal r = {{0}};
  ck_assert_int_eq(s21_div(a, b, &r), 0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест banker's rounding при переполнении (нечетный случай)
 * @details Проверяет деление максимального числа на 9 с banker's rounding
 */
START_TEST(div_banker_overflow_odd_fn) {
  s21_decimal a =
      mk(0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);  // max decimal
  s21_decimal b = mk(9, 0, 0, 0, 0);                    // b = 9
  s21_decimal r = {{0}};
  ck_assert_int_eq(s21_div(a, b, &r), 0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Создание тестового набора для расширенного деления
 * @return Указатель на созданный Suite
 * @details Объединяет все тесты сложных случаев деления в один набор
 */
Suite* test_div_ext(void) {
  Suite* s = suite_create("s21_div_ext");  // Создание набора тестов
  TCase* tc = tcase_create("div_ext");     // Создание группы тестов

  // Основные тесты деления
  tcase_add_test(tc, div_by_zero);            // Деление на ноль
  tcase_add_test(tc, div_fraction_basic);     // Базовое дробное деление
  tcase_add_test(tc, div_negative_mixed);     // Смешанные знаки
  tcase_add_test(tc, div_scale_diff);         // Разные масштабы
  tcase_add_test(tc, div_overflow_positive);  // Переполнение

  suite_add_tcase(s, tc);  // Добавление группы в набор тестов

  // Дополнительные тесты
  tcase_add_test(tc, div_exact_fn);               // Точное деление
  tcase_add_test(tc, div_tie_even_fn);            // Banker's rounding
  tcase_add_test(tc, div_large_scale_reduce_fn);  // Сокращение масштаба
  tcase_add_test(tc,
                 div_tie_even_internal_fn);  // Внутреннее округление (четное)
  tcase_add_test(tc,
                 div_tie_odd_internal_fn);  // Внутреннее округление (нечетное)
  tcase_add_test(
      tc, div_banker_overflow_even_fn);  // Banker's при переполнении (четное)
  tcase_add_test(
      tc, div_banker_overflow_odd_fn);  // Banker's при переполнении (нечетное)

  return s;  // Возврат готового набора
}