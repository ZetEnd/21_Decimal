#include <limits.h>  // Константы пределов типов (INT_MIN, INT_MAX)

#include "../s21_decimal.h"

/*
0 при успехе, 1 при ошибке (некорректный указатель или выход за пределы int)
Алгоритм:
1. Отбрасывает дробную часть (truncate)
2. Проверяет, помещается ли результат в диапазон int
3. Обрабатывает особый случай INT_MIN
 */

int s21_from_decimal_to_int(s21_decimal src, int* dst) {

  int result = 1;

  if(dst != NULL){

    s21_decimal truncated = {{0}};

    // обрезаем число
    if(s21_truncate(src, &truncated) == 0){

      if(truncated.bits[1] == 0 && truncated.bits[2] == 0){

        int negative = s21_get_sign(&src);

        uint32_t value = (uint32_t)truncated.bits[0];

        if(negative && value < (uint32_t)INT_MAX + 1u){ *dst = -(int)value;
          result = 0;
        } else 
        if(negative && value == ((uint32_t)INT_MAX + 1u)){ *dst = INT_MIN;
          result = 0;
        } else 
        if(!negative && value <= (uint32_t)INT_MAX){ *dst = (int)value;
          result = 0;
        }


      }
    }
  }

  return result;
}