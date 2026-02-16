#include <math.h>    // Математические функции (isnan, isinf)
#include <stdint.h>  // Типы фиксированной ширины

#include "../s21_decimal.h"

// Записывает число в младшие 64 бита, старшие 32 бита - 0
static inline void u96_from_u64(uint64_t v, uint32_t a[3]){

  a[0] = (uint32_t)( v & 0xFFFFFFFFu);
  a[1] = (uint32_t)( (v >> 32) & 0xFFFFFFFFu );
  a[2] = 0u;
}

// Используется для округления и коррекции
static uint32_t u96_add_small(uint32_t a[3], uint32_t add){

  uint64_t carry = add;
  uint64_t c;

  for(int i = 0; i < 3 && carry; i++){
    c = (uint64_t)a[i] + carry; // прибавляем перенос
    a[i] = (uint32_t)(c & 0xFFFFFFFFu); // младшые 32 бита
    carry = c >> 32; // новый перенос
  }

  return (uint32_t)carry;
}

// умножить на 10
static uint32_t u96_mul_small(uint32_t a[3], uint32_t k){

  uint64_t carry = 0u;
  uint64_t c;

  for(int i = 0; i < 3; i++){
    // умножаем каждый разряд на К
    c = (uint64_t)a[i] * (uint64_t)k + carry;
    a[i] = (uint32_t)(c & 0xFFFFFFFF);
    carry = c >> 32;
  }

  return (uint32_t)carry;
}

// деление с округлением по правилам банковского округления
static void u96_round_div10(uint32_t a[3]){

  uint32_t remains = u96_div10(a);

  if(remains > 5) {
    (void)u96_add_small(a,1u);
  } else if (remains == 5 && (a[0] & 1u) != 0) {
    (void)u96_add_small(a,1u);
  }
}

// используется для увеличения масштаба числа
static int u96_mul_pow10(uint32_t a[3], int k){

  uint32_t result = 0;;
  for(int i = 0; i < k; i++){
    result = u96_mul_small(a, 10u);
    if(result != 0u){
      result = 1;
      break;
    }
  }

  return result;
}

//
static uint64_t round_ties_to_even(double x){

  const double eps =1e-12;

  // округляем в нижнюю сторону
  double f = floor(x);
  uint64_t result;

  // остаток
  double diff = x - f;

  if (diff > 0.5 + eps) result = (uint64_t)f + 1u;
  else if (diff < 0.5 - eps) result = (uint64_t)f;
  else if(((uint64_t)f & 1u) != 0) result = (uint64_t)f + 1u;
  else result = (uint64_t)f;

  return result;
}

int s21_from_float_to_decimal(float src, s21_decimal* dst){

  if(dst != NULL && isnan(src) != 1 && isinf(src) != 1){

    
    const double DECIMAL_MAX_D = 7.922816251426434e28;
    const double DECIMAL_MIN_D = 1e-28;
    
    // обнулить значение десималь
    s21_reset_value(dst);

    int sign;
    // устанавливаем знак числа десималь
    if(src < 0.0f) sign = 1;
    else sign = 0;

    // берем модуль числа
    double abs_src = fabs((double)src);

    if(abs_src == 0.0) return 0;
    
    if(abs_src < DECIMAL_MIN_D || abs_src > DECIMAL_MAX_D) return 1;
    

    int exp10 = (int)floor(log10(abs_src));
    double pow_neg = pow(10.0, -exp10);
    double norm = abs_src * pow_neg;

    // проверяем на нормированное значение
    if(norm >= 10.0) {
      norm /= 10.0;
      exp10 +=1;
    }
    if(norm < 1.0){

      norm *= 10.0;
      exp10 -= 1;
    }

    // извлечение 7ми цифр
    const int digits = 7;
    double scaled = norm * pow(10.0, digits - 1);
    uint64_t mantissa7 = round_ties_to_even(scaled);

    // проверка на переполнение в коде, 8 цифр
    if(mantissa7 == 10000000ull){
      mantissa7 = 1000000ull;
      exp10 += 1;
    }

    // рассчитываем значение масштаба числа десималб
    int scale = (digits - 1) - exp10;

    uint32_t number[3];
    u96_from_u64(mantissa7, number);

    // помещаем размер масштаба в диапазон от 0 до 28
    if(scale < 0){
      int k = -scale;
      if(u96_mul_pow10(number, k) != 0) return 1;

      scale = 0;
    }

    while(scale > 28){
      u96_round_div10(number);
      scale -= 1;
    }

    // устанавливаем мантиссу масштаб и знак десималь числа
    u96_to_dec(number, dst);
    s21_set_scale(dst, scale);
    s21_set_sign(dst, sign);

    // если мантисса нулевая - делаем полное обнуление числа деималь
    if(u96_is_zero(number)){
      s21_reset_value(dst);
    }

  } else {
    return 1;
  }
  return 0;
}