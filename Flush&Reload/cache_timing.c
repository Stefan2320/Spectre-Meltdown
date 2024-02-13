#include <emmintrin.h>
#include <x86intrin.h>
#include <stdint.h>
#include <stdio.h>

/*This is used to determine the cache threshold value.*/
uint8_t array[10 * 4096];

int main()
{
  int junk = 0;

  register uint64_t time1, time2;

  volatile uint8_t *addr;
  int i;

  for (i = 0; i < 10; i++)
    array[i * 4096] = 1;

  for (i = 0; i < 10; i++)
    _mm_clflush(&array[i * 4096]);

  array[3 * 4096] = 100;
  array[7 * 4096] = 200;

  for (i = 0; i < 10; i++)
  {
    addr = &array[i * 4096];
    time1 = __rdtscp(&junk);
    junk = *addr;
    time2 = __rdtscp(&junk) - time1;
    printf("Access time for array[%d*4096]: %d CPU cycles\n", i, (int)time2);
  }
  return 0;
}
