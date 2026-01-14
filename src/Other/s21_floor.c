#include "../s21_decimal.h"

//Округлить число вниз (к отрицательной бесконечности)

/*
0 при успехе, 1 при ошибке
- Для положительных чисел: truncate (отбросить дробную часть)
- Для отрицательных чисел с дробной частью: truncate - 1
Примеры: 2.7 → 2, -2.7 → -3, 5.0 → 5, -5.0 → -5
*/

int s21_floor(s21_decimal value, s21_decimal* result) {

    int response = 0;

    if(result != NULL){

        s21_decimal truncated = {{0}};
        int truncate_error = s21_truncate(value, &truncated);
        
        response |= truncate_error;

        if(!truncate_error){

            int negative = s21_get_sign(&value);
            int remains = !s21_is_equal(value, truncated);

            // если число отрицательное и имеет остаток
            if(negative && remains){
                s21_decimal one = {{1, 0, 0, 0}};
                // минус один
                response |= s21_sub(truncated, one, result);
            } else {
                *result = truncated;
            }
        }
    } else {
        response |= 1;
    }

    return response;
}