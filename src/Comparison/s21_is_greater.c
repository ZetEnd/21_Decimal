
#include "s21_decimal.h"

// функция больше
/*
 a > b -> 1
 */

 int s21_is_greater(s21_decimal a, s21_decimal b){
  // a > b <=> !(a <= b)
  return !s21_is_less_or_equal(a,b);
 }