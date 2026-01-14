#include "../s21_decimal.h"

//Изменить знак числа на противоположный
//0 при успехе, 1 при ошибке (некорректный указатель)

int s21_negate(s21_decimal value, s21_decimal* result){

  if(result != NULL){

    *result = value;

    s21_set_sign(result, !s21_get_sign(&value));
  }

  return (int)(result == NULL);
}