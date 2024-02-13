#include <emmintrin.h>
#include <x86intrin.h>
#include <stdint.h>
#include <stdio.h>

uint8_t array[10 * 128];

int main()
{
  int junk = 0;

  register uint64_t time1, time2;

  volatile uint8_t *addr;
  int i;

  for (i = 0; i < 10; i++)
    array[i * 128] = 1;

  printf("Addresses cached for array[3*128] should be:%p-%p\n", (array + 3 * 128), (array + 3 * 128 + 64));
  printf("Addresses cached for array[7*128] should be:%p-%p\n", (array + 7 * 128), (array + 7 * 128 + 64));
  for (i = 0; i < 10; i++)
    _mm_clflush(&array[i * 128]);

  array[3 * 128] = 100;
  array[7 * 128] = 200;

  for (i = 0; i < 10; i++)
  {
    addr = &array[i * 128];
    time1 = __rdtscp(&junk);
    junk = *addr;
    time2 = __rdtscp(&junk) - time1;
    printf("Access time for array[%d*128],with address %p: %d CPU cycles\n", i, (array + i * 128), (int)time2);
  }
  return 0;
}
