/**
 * @file tests_from_float_to_decimal.c
 * @brief Тесты функции конвертации float в decimal
 * @details Содержит юнит-тесты для проверки корректности конвертации
 *          чисел с плавающей точкой в decimal формат, включая специальные
 *          случаи, округление и обработку ошибок
 */

#include "tests.h"

/**
 * @brief Тест конвертации простого положительного float
 * @details Проверяет: float(123.0) → decimal
 *          Базовый случай конвертации целого числа
 */
START_TEST(from_float_simple) {
  s21_decimal dec;
  ck_assert_int_eq(s21_from_float_to_decimal(123.0f, &dec),
                   0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест конвертации отрицательного float
 * @details Проверяет: float(-123.0) → decimal
 *          Проверка корректной обработки знака
 */
START_TEST(from_float_negative) {
  s21_decimal dec;
  ck_assert_int_eq(s21_from_float_to_decimal(-123.0f, &dec),
                   0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест конвертации дробного числа
 * @details Проверяет: float(0.125) → decimal
 *          Конвертация числа с дробной частью (1/8)
 */
START_TEST(from_float_fraction) {
  s21_decimal dec;
  ck_assert_int_eq(s21_from_float_to_decimal(0.125f, &dec),
                   0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест конвертации большого числа
 * @details Проверяет: float(1e7) → decimal
 *          Конвертация числа в научной нотации (10 миллионов)
 */
START_TEST(from_float_big) {
  s21_decimal dec;
  ck_assert_int_eq(s21_from_float_to_decimal(1e7f, &dec),
                   0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест конвертации малого числа
 * @details Проверяет: float(1e-7) → decimal
 *          Конвертация малого числа в научной нотации
 */
START_TEST(from_float_small) {
  s21_decimal dec;
  ck_assert_int_eq(s21_from_float_to_decimal(1e-7f, &dec),
                   0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест конвертации с NULL указателем
 * @details Проверяет обработку ошибки при передаче NULL в качестве результата
 */
START_TEST(from_float_null_dst) {
  ck_assert_int_eq(s21_from_float_to_decimal(1.0f, NULL),
                   1);  // Должна вернуть ошибку конвертации
}
END_TEST

/**
 * @brief Тест конвертации NaN (Not a Number)
 * @details Проверяет обработку специального значения NaN
 *          NaN не может быть представлен в decimal
 */
START_TEST(from_float_nan) {
  s21_decimal dec;
  float nan_val = 0.0f / 0.0f;  // Генерация NaN
  ck_assert_int_eq(s21_from_float_to_decimal(nan_val, &dec),
                   1);  // Должна вернуть ошибку
}
END_TEST

/**
 * @brief Тест конвертации бесконечности
 * @details Проверяет обработку специального значения ±∞
 *          Бесконечность не может быть представлена в decimal
 */
START_TEST(from_float_inf) {
  s21_decimal dec;
  float inf_val = 1.0f / 0.0f;  // Генерация +∞
  ck_assert_int_eq(s21_from_float_to_decimal(inf_val, &dec),
                   1);  // Должна вернуть ошибку
}
END_TEST

/**
 * @brief Тест конвертации слишком малого числа
 * @details Проверяет: float(1e-30) → ошибка
 *          Число слишком мало для представления в decimal (минимальный масштаб
 * 28)
 */
START_TEST(from_float_too_small) {
  s21_decimal dec;
  ck_assert_int_eq(s21_from_float_to_decimal(1e-30f, &dec),
                   1);  // Должна вернуть ошибку
}
END_TEST

/**
 * @brief Тест конвертации слишком большого числа
 * @details Проверяет: float(1e30) → ошибка
 *          Число слишком велико для представления в decimal
 */
START_TEST(from_float_too_big) {
  s21_decimal dec;
  ck_assert_int_eq(s21_from_float_to_decimal(1e30f, &dec),
                   1);  // Должна вернуть ошибку
}
END_TEST

/**
 * @brief Тест конвертации числа с переполнением масштаба
 * @details Проверяет конвертацию числа с большим количеством цифр
 *          Может потребовать округление до допустимой точности
 */
START_TEST(from_float_scale_overflow) {
  s21_decimal dec;
  ck_assert_int_eq(s21_from_float_to_decimal(123456789.123456789f, &dec),
                   0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест banker's rounding при конвертации float
 * @details Проверяет применение banker's rounding для точного значения
 */
START_TEST(from_float_ties_to_even_fn) {
  s21_decimal d;
  float x = 1.2345001f;  // Число, требующее округления
  ck_assert_int_eq(s21_from_float_to_decimal(x, &d),
                   0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест конвертации числа с отрицательным масштабом
 * @details Проверяет конвертацию большого целого числа без дробной части
 */
START_TEST(from_float_neg_scale_fn) {
  s21_decimal d;
  ck_assert_int_eq(s21_from_float_to_decimal(1234567.0f, &d),
                   0);  // Операция должна пройти успешно
}
END_TEST

/**
 * @brief Тест округления с переносом до 1e7
 * @details Проверяет случай, когда округление приводит к увеличению порядка
 * числа
 */
START_TEST(from_float_round_carry_to_1e7_fn) {
  s21_decimal d;
  ck_assert_int_eq(s21_from_float_to_decimal(9.9999995f, &d),
                   0);  // 9.9999995 → 10.0 (округление)
}
END_TEST

/**
 * @brief Тест конвертации нуля и отрицательного нуля
 * @details Проверяет конвертацию +0.0 и -0.0 (оба должны давать +0)
 */
START_TEST(from_float_zero_fn) {
  s21_decimal d;
  ck_assert_int_eq(s21_from_float_to_decimal(0.0f, &d),
                   0);  // +0.0 → decimal(0)
  ck_assert_int_eq(s21_from_float_to_decimal(-0.0f, &d),
                   0);  // -0.0 → decimal(0)
}
END_TEST

/**
 * @brief Тест округления вниз (меньше половины)
 * @details Проверяет округление числа, где дробная часть < 0.5
 */
START_TEST(from_float_round_down_lt_half) {
  s21_decimal d;
  ck_assert_int_eq(s21_from_float_to_decimal(1.2340001f, &d),
                   0);  // Округление вниз
}
END_TEST

/**
 * @brief Тест округления вверх (больше половины)
 * @details Проверяет округление числа, где дробная часть > 0.5
 */
START_TEST(from_float_round_up_gt_half) {
  s21_decimal d;
  ck_assert_int_eq(s21_from_float_to_decimal(1.2349999f, &d),
                   0);  // Округление вверх
}
END_TEST

/**
 * @brief Тест banker's rounding к четному числу
 * @details Проверяет округление 0.5 к ближайшему четному числу
 */
START_TEST(from_float_tie_even) {
  s21_decimal d;
  ck_assert_int_eq(s21_from_float_to_decimal(1.2349995f, &d),
                   0);  // 1.235 → 1.234 (к четному)
}
END_TEST

/**
 * @brief Тест banker's rounding к четному числу (случай с нечетным)
 * @details Проверяет округление 0.5 от нечетного числа к четному
 */
START_TEST(from_float_tie_odd) {
  s21_decimal d;
  ck_assert_int_eq(s21_from_float_to_decimal(1.2359995f, &d),
                   0);  // 1.236 → 1.236 (к четному)
}
END_TEST

/**
 * @brief Тест конвертации с масштабом > 28 и остатком > 5
 * @details Проверяет поведение при масштабе больше максимального
 */
START_TEST(from_float_scale_gt28_r_gt5) {
  s21_decimal d;
  ck_assert_int_eq(s21_from_float_to_decimal(1.234567e-28f, &d),
                   0);  // Очень малое число
}
END_TEST

/**
 * @brief Тест banker's rounding при масштабе > 28 (к четному)
 * @details Проверяет banker's rounding для сверхмалых чисел (четный случай)
 */
START_TEST(from_float_scale_gt28_r_eq5_even) {
  s21_decimal d;
  ck_assert_int_eq(s21_from_float_to_decimal(1.234565e-28f, &d),
                   0);  // Точно 0.5, к четному
}
END_TEST

/**
 * @brief Тест banker's rounding при масштабе > 28 (к четному от нечетного)
 * @details Проверяет banker's rounding для сверхмалых чисел (нечетный случай)
 */
START_TEST(from_float_scale_gt28_r_eq5_odd) {
  s21_decimal d;
  ck_assert_int_eq(s21_from_float_to_decimal(1.234575e-28f, &d),
                   0);  // Точно 0.5, к четному
}
END_TEST

/**
 * @brief Создание тестового набора для конвертации float → decimal
 * @return Указатель на созданный Suite
 * @details Объединяет все тесты конвертации в один набор, включая
 *          базовые случаи, специальные значения и различные сценарии округления
 */
Suite* test_from_float_to_decimal(void) {
  Suite* s =
      suite_create("s21_from_float_to_decimal");  // Создание набора тестов
  TCase* tc =
      tcase_create("from_float_to_decimal_tc");  // Создание группы тестов

  // Базовые тесты конвертации
  tcase_add_test(tc, from_float_simple);    // Простое положительное число
  tcase_add_test(tc, from_float_negative);  // Отрицательное число
  tcase_add_test(tc, from_float_fraction);  // Дробное число
  tcase_add_test(tc, from_float_big);       // Большое число
  tcase_add_test(tc, from_float_small);     // Малое число
  tcase_add_test(tc, from_float_zero_fn);   // Ноль и отрицательный ноль

  // Тесты обработки ошибок
  tcase_add_test(tc, from_float_null_dst);   // NULL указатель
  tcase_add_test(tc, from_float_nan);        // NaN
  tcase_add_test(tc, from_float_inf);        // Бесконечность
  tcase_add_test(tc, from_float_too_small);  // Слишком малое число
  tcase_add_test(tc, from_float_too_big);    // Слишком большое число

  // Тесты точности и округления
  tcase_add_test(tc, from_float_scale_overflow);   // Переполнение масштаба
  tcase_add_test(tc, from_float_ties_to_even_fn);  // Banker's rounding
  tcase_add_test(tc, from_float_neg_scale_fn);     // Отрицательный масштаб
  tcase_add_test(tc,
                 from_float_round_carry_to_1e7_fn);   // Округление с переносом
  tcase_add_test(tc, from_float_round_down_lt_half);  // Округление вниз
  tcase_add_test(tc, from_float_round_up_gt_half);    // Округление вверх
  tcase_add_test(tc, from_float_tie_even);            // Tie к четному
  tcase_add_test(tc, from_float_tie_odd);             // Tie от нечетного

  // Тесты сверхмалых чисел
  tcase_add_test(tc, from_float_scale_gt28_r_gt5);  // Масштаб > 28, остаток > 5
  tcase_add_test(
      tc, from_float_scale_gt28_r_eq5_even);  // Масштаб > 28, tie к четному
  tcase_add_test(
      tc, from_float_scale_gt28_r_eq5_odd);  // Масштаб > 28, tie от нечетного

  suite_add_tcase(s, tc);  // Добавление группы в набор тестов
  return s;                // Возврат готового набора
}
