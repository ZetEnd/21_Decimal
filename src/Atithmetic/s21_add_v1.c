/*#include <stdint.h>  // Типы фиксированной ширины
#include <string.h>  // Функции работы с памятью (memset)

#include "s21_decimal.h"

//Банковское округление после деления на 10 для 96-битного числа
static void bankers_round_after_div10_96(uint32_t a[3], uint32_t remains){

    if(remains > 5 || (remains == 5u && (a[0] & 1u))){
        uint32_t one[3] = {1,0,0}
        (void)u96_add(a, one);
    }
}
*/