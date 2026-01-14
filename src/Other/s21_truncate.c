#include <stdint.h>  // Типы фиксированной ширины

#include "../s21_decimal.h"

// Отбрасывание дробной части десималь числа(округление к нулю)
// Не округляет - просто отбрасывает дробную часть

static void u96_div10_int(int bits[4]){
  
    uint32_t a[3] = {
        (uint32_t)bits[0],
        (uint32_t)bits[1],
        (uint32_t)bits[2]
    };

    (void)u96_div10(a);

    for(int i = 0; i < 3; i++){
        bits[i] = (int)a[i];
    }
}

/*Удаляет дробную часть путем деления мантиссы на 10^scale и установки масштаба в 0. Знак числа сохраняется.
Примеры: 123.456 → 123, -78.9 → -78
0 при успехе, 1 при ошибке (некорректный указатель)
*/

int s21_truncate(s21_decimal value, s21_decimal* result) { 

    if(result != NULL){

        *result = value;

        int scale = s21_get_scale(result);

        while(scale > 0){
            u96_div10_int(result->bits);
            scale--;
        }

        // обнуляем биты масштаа
        result->bits[3] = 0;
        // cтавим знак
        s21_set_sign(result, s21_get_sign(&value));
    }

    return (int)(result == NULL);

}