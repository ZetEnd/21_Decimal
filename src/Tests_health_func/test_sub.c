#include <math.h>      // Математические функции
#include <stddef.h>    // Определения типов size_t, NULL и др.
#include <stdint.h>    // Типы фиксированной ширины (uint32_t, etc.)
#include <stdlib.h>    // Стандартные функции (malloc, free, etc.)
#include <stdio.h>

int u96_sub(uint32_t a[3], uint32_t b[3]){
  uint64_t c = (uint64_t)a[0] - b[0];
  a[0] = (uint32_t)c;

  c = (c >> 32) + (uint64_t)a[1] - b[1];
  a[1] = (uint32_t)c;

  c = (c >> 32) + (uint64_t)a[2] - b[2];
  a[2] = (uint32_t)c;

  return (c >> 32) ? 1 : 0;
}

int main(){

    uint32_t a[3] = {1, 3, 0};
    uint32_t b[3] = {2, 1, 0};

    uint64_t c = 4294967295;


    int rem = u96_sub(a,b);

        printf("-1 %d %u-> 0x%016llx 0x%016llx 0x%016llx (%llu)  (%08x)\n",(uint32_t)a[0] , (uint32_t)c,a[0], a[1], a[2], (int)a[1], (uint32_t)(a[0] - b[0]));
}