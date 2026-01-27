#include "../s21_decimal.h"

// сравнивает 2 децимал числа
/*
a < b - 1, 0 - иначе
приводим к общему масштабу
Если знаки разные - отрицательное меньше положительного
знаки одинаковые - сравниваем мантиссы
*/

int s21_is_less(s21_decimal a, s21_decimal b){

  int result = 0;

  if(s21_is_zero(&a) != 1 || s21_is_zero(&b) != 1){

    // к общему масштабу
    s21_to_common_scale(&a, &b);

    int a_sign = s21_get_sign(&a);
    int b_sign = s21_get_sign(&b);

    if(a_sign == b_sign){

      uint32_t A[3], B[3];
      // берем мантиссу 2х чисел
      u96_from_dec(&a, A);
      u96_from_dec(&b, B);

      //a > b => 1
      int c = u96_compare(A, B);

      // если отрицательные числа и мантисса а > b
      if( a_sign == 1 && c == 1)
      // то а < b и результат - 1
      result = 1;

      // если положительные и a < b
      if( a_sign == 0 && c == -1)
      result = 1;
    } else {
      //знаки разные и А - отрицательное, то а < b
      if( a_sign == 1)
      result = 1;
    }
  }

  // если оба == 0 - то результат 0 - никто не меньше

  return result;
}
