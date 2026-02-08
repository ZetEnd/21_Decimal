#include <stdint.h>  // Типы фиксированной ширины
#include <string.h>  // Функции работы с памятью (memset)

#include "../s21_decimal.h"

//Функция сложения десималь чисел

//Бановское округление после деления на 10 для 96 бит
/*
Если остаток > 5 или равен 5 и число нечет 
То округляем вверх*/

static void bankers_round_after_div10_96(uint32_t a[3], uint32_t rem){

    if(rem > 5 || (rem == 5u && (a[0] & 1u))){
        // Число 1 в формате 96 бит
        uint32_t one[3] = {1, 0, 0};
        // Прибавляем 1 и игнорируем переполнение
        (void)u96_add(a,one);
    }
}

//Удаление незначащих нулей в конце числа
/*
Уменьшает масштаб, деля число на 10, пока это возможно*/

static void strip_trailing_zeros(uint32_t a[3], int* scale){

    uint32_t tmp[3] = {0, 0, 0};
    u96_copy(tmp, a);

    while(*scale > 0 && u96_div10(tmp) == 0){
        //сохраняем результат деления
        u96_copy(a, tmp);
        //уменьшаем масштаб
        (*scale)--;
        // готовим к следующей итерации
        u96_copy(tmp,a);
    }
}

// Деление 128 бит числа на 10
/*
Выполняет деление "в столбик" от старших разрядов к младшим*/
static uint32_t u128_div10(uint32_t a[4]){

    uint64_t rem = 0;

    // от старшего разряда к младшему
    for(int i = 3; i >= 0; i--) {
        // текущий число - остаток + новый разряд
        uint64_t curr = (rem << 32) | a[i];
        // частное записываем обратно
        a[i] = (uint32_t)(curr / 10);
        // остаток для следующей итерации
        rem = curr % 10;
    }

    return (uint32_t)rem;
}

// banks round after div 10 a 128 bit number
/*
adding a one to number if we need to upper round
*/

static void bankers_round_after_div10_128(uint32_t a[4], uint32_t rem){

    if(rem > 5u || (a[0] & 1u)){

        // We add 1, taking into account the transfer between digits
        uint64_t s = (uint64_t)a[0] + 1u;
        a[0] = (uint32_t)s;
        //transfer to the senior category
        uint64_t c = s >> 32; // transfir is a (0 or 1)
        // if there is a transfer

        /*
        Если есть перенос (c == 1), переполнение в a[0], нужно добавить 1 к a[1] и т.д.
        */
        if(c){
            // we add it to the next category
            s = (uint64_t)a[1] + 1u; // so if we have a transfer = 1 we add a 1 for next bit
            a[1] = (uint32_t)s;
            c = s >> 32; 

            //if there is a transfer again
            if(c){
                s = (uint64_t)a[2] + 1u;
                a[2] = (uint32_t)s;
                c = s >> 32;

                // we add it to the highest level
                if(c) a[3] += 1u;

                /*текущая реализация с явными проверками может быть эффективнее, 
                так как в большинстве случаев переноса дальше первого разряда не будет.
                */
            }
        }
    }
}

// Приведение двух чисел к одинаковому масштабу
//Увеличивает масштаб меньшего числа или уменьшает масштаб большего
static void align_scales(uint32_t ax[3], int* sa, uint32_t bx[3], int* sb){

    if(*sa != *sb){

        // Убеждаемся, что *sa <= *sb (меняем местами если нужно)
        if(*sa > *sb){
            uint32_t tmp[3];
            // Меняем числа местами
            u96_copy(tmp, ax);
            u96_copy(ax, bx);
            u96_copy(bx, tmp);

            // Меняем масштабы местами
            int ts = *sa;
            *sa = *sb;
            *sb = ts;
        }
    }
}