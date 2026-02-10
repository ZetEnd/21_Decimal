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

        // Приводим к общему масштабу
        while(*sa < *sb){

            uint32_t tmp[3];
            u96_copy(tmp, ax);

            // пытаемся увеличить масштаб меньшего числа
            // умножаем на 10
            if(u96_mul10(tmp) == 0){
                // если нет переполнения - принимаем результат
                u96_copy(ax, tmp);
                (*sa)++;
            } else {
                // Иначе уменьшаем масштаб большего числа
                // Делим на 10
                uint32_t rem = u96_div10(bx);
                // округляем результат и уменьшаем масштаб
                bankers_round_after_div10_96(bx, rem);
                (*sb)--;
            }
        }
    }
}

// Сложение двух decimal чисел
// Основная функция сложения, учитывающая знаки и масштабы чисел

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result ){

    memset(result, 0, sizeof(s21_decimal));

    int scale_a = s21_get_scale(&value_1);
    int scale_b = s21_get_scale(&value_2);

    uint32_t a96[3], b96[3];
    u96_from_dec(&value_1, a96);
    u96_from_dec(&value_2, b96);

    align_scales(a96, &scale_a, b96, &scale_b);

    int sign_a = s21_get_sign(&value_1);
    int sign_b = s21_get_sign(&value_2);

    uint32_t res96[3] = {0};
    int sign_res = 0;

    // если знаки одинаковые
    if(sign_a == sign_b) {
        u96_copy(res96, a96);
        // прибавялем 2е число и получаем перенос
        int carry = u96_add(res96, b96);

        // если есть переполение
        if(carry){

            // нельзя уменьшить масштаб - выход
            if(scale_a == 0) return 1;

            // создаем 128 бит число для устранения переполнения
            uint32_t ext[4] = {res96[0], res96[1], res96[2], (uint32_t)carry};

            do{
                // если нельзя больше уменьшать масштаб - выход
                if(scale_a == 0) return 1;

                // делим на 10 и округляем уменьшая масштаб
                uint32_t rem = u128_div10(ext);
                bankers_round_after_div10_128(ext, rem);
            } while(ext[3] != 0); // пока есть переполнение

            // Копируем обратно в 96-битный формат
            res96[0] = ext[0];
            res96[1] = ext[1];
            res96[2] = ext[2];
        }
        // Знак результата = знаку слагаемых
        sign_res = sign_a;
    } else {
        // Если знаки разные - вычитаем

        // Сравниваем модули чисел
        int cmp = u96_compare(a96, b96);

        // Если числа равны по модулю - результат 0
        if(cmp ==0){
            res96[0] = res96[1] = res96[2] = 0;
            scale_a = 0;
            sign_res = 0;
        } else if (cmp > 0){ // Если |a| > |b|
            u96_copy(res96, a96);
            // res = a - b
            u96_sub(res96, b96);
            sign_res = sign_a;
        } else {
            u96_copy(res96, b96);
            // res = b - a
            u96_sub(res96, a96);
            sign_res = sign_b;
        }
    }

    //Удаляем незначащие нули
    strip_trailing_zeros(res96, &scale_a);

    //Формируем финальный результат
    u96_to_dec(res96, result);
    s21_set_scale(result, scale_a);
    s21_set_sign(result, sign_res);

    return 0;

}