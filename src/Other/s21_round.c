#include "../s21_decimal.h"

//Округляет число до ближайшего целого с применением банковского округления
//0 при успехе, 1 при ошибке (некорректный указатель)
// 123.456 - 123.46 - 123.5 - 124??

int s21_round(s21_decimal value, s21_decimal* result) {

  if(result != NULL){

    *result = value;

    int scale = s21_get_scale(result);

    if(scale > 0){
        
        //идем поразрядно по дробной части и округляем с конца
        for(int i = 0; i < scale; i++){
            s21_scale_down_one_banker(result);
        }

        s21_set_scale(result, 0);
    }
  }

  return (int)(result == NULL);
}