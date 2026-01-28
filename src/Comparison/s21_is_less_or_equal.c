
#include "s21_decimal.h"

// меньше либо равно
// a <= b => 1
int s21_is_less_or_equal(s21_decimal a, s21_decimal b){

  int res = 0;

  if(s21_is_less(a,b) == 1 || s21_is_equal(a,b) == 1) res = 1;

  return res;
}