
#include "../s21_decimal.h"

#include <math.h>  // Математические функции (pow)

/*
0 при успехе, 1 при ошибке (некорректный указатель)
1. Собирает 96-битную мантиссу в double
2. Применяет масштаб (деление на 10^scale)
3. Применяет знак
4. Приводит к float (возможна потеря точности)
*/

int s21_from_decimal_to_float(s21_decimal src, float* dst){

  if(dst != NULL){

    double mantissa = 0.0;

    // вначале приводим к типу беззнаковому, и это число в дабл
    /*
    В двоичном: 1111 0000 0000 0000 0000 0000 0000 0000
    Как unsigned: 4,026,531,840
    Как signed: -268,435,456 (из-за знакового бита)
    */
    mantissa += (double)(uint32_t)(src.bits[0]);
    mantissa += (double)(uint32_t)(src.bits[1]) * pow(2,32);
    mantissa += (double)(uint32_t)(src.bits[2]) * pow(2,64);

    int scale = s21_get_scale(&src);

    if(scale > 0) mantissa /= pow(10. ,scale);

    if(s21_get_sign(&src)) mantissa = -mantissa;

    *dst = (float)mantissa;
  }

  return (dst == NULL);

}

