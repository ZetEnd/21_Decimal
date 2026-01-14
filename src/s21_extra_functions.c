#include "s21_decimal.h"

// получение значения масштаба
int s21_get_scale(const s21_decimal* d){

  int scale = 0;

  if( d != NULL){
  uint32_t scale_bit = d->bits[3] & S21_SCALE_MASK;

  scale = (int)(scale_bit >> S21_SCALE_SHIFT);
  }

  return scale;
}

// установка значения масштаба
void s21_set_scale(s21_decimal* d, int s){

  if(d != NULL){

    if(s < 0) s = 0;
    if(s > S21_SCALE_MAX) s = S21_SCALE_MAX;

    uint32_t clear_bit = (uint32_t)d->bits[3] & ~S21_SCALE_MASK;
    uint32_t new_scale = (uint32_t)s << S21_SCALE_SHIFT;

    d->bits[3] = (int)(clear_bit | new_scale);
  }
}

// получение знака десималь
int s21_get_sign(const s21_decimal* d) {

  int sign = 0;

  if(d != NULL){
    if(((uint32_t)d->bits[3] & S21_SIGN_MASK) != 0u) sign = 1;
  }
  return sign;
}

// установка знака 1 - для отрицательного
void s21_set_sign(s21_decimal* d, int negative){

  if(d != NULL){

    if (negative){
      d->bits[3] = (int)((uint32_t)d->bits[3] | S21_SIGN_MASK);
    } else {
      d->bits[3] = (int)((uint32_t)d->bits[3] & ~S21_SIGN_MASK);
    }
  }
}

// извлечь мантиссу из десималь
void u96_from_dec(const s21_decimal* d, uint32_t a[3]){
  a[0] = (uint32_t)d->bits[0];
  a[1] = (uint32_t)d->bits[1];
  a[2] = (uint32_t)d->bits[2];
}

// сохранить 96 бит в десималь
void u96_to_dec(const uint32_t a[3], s21_decimal* d) {
  d->bits[0] = (int)a[0];
  d->bits[1] = (int)a[1];
  d->bits[2] = (int)a[2];
}

// комировать 96 бит
void u96_copy(uint32_t dst[3], const uint32_t src[3]){
  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
}

// проверка 96 бит числа на 0
int u96_is_zero(const uint32_t a[3]){
  return a[0] == 0u && a[1] == 0u && a[2] == 0u;
}

// сравнение a > b => 1
int u96_compare(const uint32_t a[3], const uint32_t b[3]){

  int result = 0;
  if(a[2] != b[2]){
    if(a[2] > b[2]) result = 1;
    else result = -1;

  } else if(a[1] != b[1]){
    if(a[1] > b[1]) result = 1;
    else result = -1;
    
  } else if(a[0] != b[0]){
    if(a[0] > b[0]) result = 1;
    else result = -1;
  }

  return result;
}

// функция для сложения 96 бит и возвращения переноса(старшего разряда) при сложении 2х больших чисел
int u96_add(uint32_t a[3], const uint32_t b[3]) {
  uint64_t с = (uint64_t)a[0] + b[0];
  a[0] = (uint32_t)с;

  с = (с >> 32) + (uint64_t)a[1] + b[1];
  a[1] = (uint32_t)с;

  с = (с >> 32) + (uint64_t)a[2] + b[2];
  a[2] = (uint32_t)с;

  return (int)(с >> 32);
}

// Функция для вычитания из a величину b и сохранение переноса
int u96_sub(uint32_t a[3],const uint32_t b[3]){
  uint64_t c = (uint64_t)a[0] - b[0];
  a[0] = (uint32_t)c;

  c = (c >> 32) + (uint64_t)a[1] - b[1];
  a[1] = (uint32_t)c;

  c = (c >> 32) + (uint64_t)a[2] - b[2];
  a[2] = (uint32_t)c;

  return (c >> 32) ? 1 : 0;
}

// умножение 96 бит на 10 возврат переноса
uint32_t u96_mul10(uint32_t a[3]){
  uint64_t c = (uint64_t)a[0] * 10u;
  a[0] = (uint32_t)c;

  c = (c >> 32) + (uint64_t)a[1] * 10u;
  a[1] = (uint32_t)c;

  c = (c >> 32) + (uint64_t)a[2] * 10u;
  a[2] = (uint32_t)c;

  return (uint32_t)(c >> 32);
}

// Деление на 10 для 96 бит
uint32_t u96_div10(uint32_t a[3]){

  uint32_t remain = 0;
  uint64_t current = 0;

  for(int i = 2; i >= 0; i--){
    //берем число и добавляем остаток(старший разряд)
    current = ((uint64_t)remain << 32) | a[i];

    a[i] = (uint32_t)(current / 10); // целая часть
    remain = (uint32_t)(current % 10); //остаток для следующего разряда
  }

  return remain;
}

// проверяет делится ла 96 бит на 10 нацело
static int u96_divisible_by_10(const uint32_t a[3]){

  uint32_t remain = 0;
  uint64_t current = 0;

  for(int i = 2; i >=0; i--){
    current = ((uint64_t)remain << 32) | a[i];
    remain = (uint32_t)(current % 10u);
  } 

  return remain == 0u;
}

/*
static int u96_divisible_by_10(const uint32_t a[3]) {

  const uint32_t main_32_mod_10 = 6u;
  uint32_t remain = 0;

  for(int i = 2; i >= 0; i--){
    remain = (main_32_mod_10*remain +a[i]) % 10u;
  }

  return remain == 0u;
}*/

// Сброс значения децимала в полный 0
void s21_reset_value(s21_decimal* d){
  if(d != NULL) 
  //memset(d, 0, sizeof(s21_decimal));
    d->bits[0] = d->bits[1] = d->bits[2] = d->bits[3] = 0;
}

// Првоерка значеняи децималь на 0
int s21_is_zero(const s21_decimal *d){
  return (d != NULL && d->bits[0] == 0 && d->bits[1] == 0 && d->bits[2] == 0);
}

// уменьшение масштаба с банковским округлением
void s21_scale_down_one_banker(s21_decimal* d){

  uint32_t a[3];
  u96_from_dec(d, a);
  uint32_t remain = u96_div10(a); // это уже изменило наше 96 бит число а
  //число 1 в 96 бит формате
  uint32_t one[3] = {1u, 0, 0};


  if((remain > 5u) || (remain == 5u && ((a[0] & 1u) != 0u))) // если нечет
    (void)u96_add(a, one);

  u96_to_dec(a, d);
  s21_set_scale(d, s21_get_scale(d) - 1);

}

// увеличение масштаба на 1 (умножение мантисы на 10) 0 - хорошо
int s21_scale_up_one(s21_decimal* d){

  uint32_t a[3];
  u96_from_dec(d, a);
  //модифицируем а и смотрим на перенос
  uint32_t carry = u96_mul10(a);

  if(!carry){
    u96_to_dec(a, d);
    s21_set_scale(d,s21_get_scale(d)+1);
  }
  return  carry & 1u;
}

// Удаление незначащих нулей из мантиссы
void s21_strip_trailing_zeros(s21_decimal* d){

  if(d != NULL){
    int current_scale = s21_get_scale(d);

    if(current_scale > 0){

      uint32_t a[3];
      u96_from_dec(d,a);

      while(current_scale > 0 && u96_divisible_by_10(a)){
        (void)u96_div10(a);// убираем нуль
        current_scale--;
      }

      u96_to_dec(a,d);
      s21_set_scale(d,current_scale);
    }

  }
}

// приводит 2 числа к общему масштабу
void s21_to_common_scale(s21_decimal* a, s21_decimal* b){

  if(a != NULL && b != NULL){

    s21_strip_trailing_zeros(a);
    s21_strip_trailing_zeros(b);

    // если разные масштбы
    while(s21_get_scale(a) != s21_get_scale(b)){

      if(s21_get_scale(a) < s21_get_scale(b))
      //если нет переполнения увеличиваем мантиссу a(0 в конец) и масштаб иначе - округляем b
        if(s21_scale_up_one(a)) s21_scale_down_one_banker(b);

      if(s21_get_scale(a) > s21_get_scale(b))
        if(s21_scale_up_one(b)) s21_scale_down_one_banker(a);

    }
  }

}