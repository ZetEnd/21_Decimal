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
  } else (remains == 5 && (a[0] & 1u) != 0){
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

  if(dst != NULL){
    
  }

  
}