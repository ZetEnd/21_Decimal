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

//
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