/**
 * @file tests.c
 * @brief Главный файл для запуска всех тестов библиотеки s21_decimal
 * @details Содержит main функцию, которая запускает все тестовые наборы
 */

#include "tests.h"

/**
 * @brief Главная функция для запуска тестов
 * @return 0 если все тесты прошли успешно, 1 если были ошибки
 * @details Запускает все тестовые наборы последовательно и подсчитывает
 *          количество неудачных тестов
 */
int main(void) {
  int fail = 0;  // Счетчик неудачных тестов

  // Массив всех тестовых наборов
  Suite* suites[] = {
      test_to_float(),               // Тесты конвертации decimal → float
      test_to_int(),                 // Тесты конвертации decimal → int
      test_from_float_to_decimal(),  // Тесты конвертации float → decimal
      test_from_int(),               // Тесты конвертации int → decimal
      test_rounding(),               // Тесты функций округления
      test_comparison(),             // Тесты функций сравнения
      test_add_sub(),                // Тесты сложения и вычитания
      test_mul_div(),                // Тесты умножения и деления
      test_negate(),                 // Тесты изменения знака
      test_div_ext(),                // Расширенные тесты деления
      test_div_extra(),              // Дополнительные тесты деления
      test_utilities_extra(),        // Тесты вспомогательных функций
      NULL                           // Маркер конца массива
  };

  // Запускаем каждый тестовый набор
  for (int i = 0; suites[i]; i++) {
    SRunner* sr = srunner_create(suites[i]);  // Создаем раннер для набора
    srunner_set_fork_status(sr, CK_NOFORK);   // Отключаем forking (для отладки)
    srunner_run_all(sr, CK_NORMAL);           // Запускаем все тесты в наборе
    fail += srunner_ntests_failed(sr);  // Добавляем количество неудачных тестов
    srunner_free(sr);                   // Освобождаем память
  }

  // Выводим итоговое количество неудачных тестов
  printf("FAILED TESTS: %d\n", fail);

  // Возвращаем 0 при успехе, 1 при наличии ошибок
  return fail ? 1 : 0;
}
