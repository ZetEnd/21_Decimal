#include "../s21_decimal.h"

// больше либо равно for decimal numbers
/*
a >= b  => 1
*/

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b){

  //реализовано чрез логическое тождество: a >= b <=> !(a < b)
  return !s21_is_less(a,b);
}