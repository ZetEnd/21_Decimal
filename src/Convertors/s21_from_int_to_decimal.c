#include "../s21_decimal.h"

#include "limits.h"

//0 при успехе, 1 при ошибке (некорректный указатель)

// INT_MIN = -2147483648, |INT_MIN| = 2147483648 = INT_MAX + 1
int s21_from_int_to_decimal(int src, s21_decimal* dst) {

    int result = 1;

    if(dst != NULL){

        s21_reset_value(dst);

        int negative = 0;
        if(src < 0) negative = 1;

        unsigned int mantissa;

        if(src == INT_MIN){

            mantissa = (unsigned int)INT_MAX + 1u;
        } else {
            if(negative) mantissa = (unsigned int)(-src);
            else mantissa = (unsigned int)(src);
        }

        dst->bits[0] = (int)(mantissa);
        dst->bits[1] = 0;
        dst->bits[2] = 0;
        dst->bits[3] = 0;

        s21_set_scale(dst, 0);
        s21_set_sign(dst, negative);
        result = 0; // успех

    }

    return result;
}