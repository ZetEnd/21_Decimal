#include "../s21_decimal.h"

// Проверить равны ли 2 числа
/*
a == b -> 1
*/

int s21_is_equal(s21_decimal a, s21_decimal b){
  int res = 0;
  // не равны 0
  if( s21_is_zero(&a) == 0 && s21_is_zero(&b) == 0){
    // равенство знаков
    if(s21_get_sign(&a) == s21_get_sign(&b)){

      // удаляем незначащие нули у чисел
      s21_strip_trailing_zeros(&a);
      s21_strip_trailing_zeros(&b);

    // Если совпадают масштаб и биты - значит равны
    if(s21_get_scale(&a) == s21_get_scale(&b) &&
      a.bits[0] == b.bits[0] && a.bits[1] == b.bits[1] &&
      a.bits[2] == b.bits[2]) res = 1;
    }
  } else if( s21_is_zero(&a) && s21_is_zero(&b) ) res = 1;

  return res;
}