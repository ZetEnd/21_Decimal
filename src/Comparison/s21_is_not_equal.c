
#include "s21_decimal.h"

// функция не равно
// a != b => 1

int s21_is_not_equal(s21_decimal a, s21_decimal b){
  // инверсия результата проверки на равенство
  return !s21_is_equal(a,b);
}