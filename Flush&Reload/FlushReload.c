#include <emmintrin.h>
#include <x86intrin.h>
#include <stdint.h>
#include <stdio.h>

#define CACHE_HIT_THRESHOLD (100)
#define DELTA 64

uint8_t array[256 * 4096];
int temp;

void flushCache()
{

  /* Bring it to RAM. Prevent Copy-on-write.
   COW is a memory management technique that allows multiple processes to share the same memory pages for an array. We initialize the array so we 'force' the creation  // of a page. We want to ensure that the array exists in the physical memory */
  for (int i = 0; i < 256; i++)
    array[i * 4096 + DELTA] = 1;

  for (int i = 0; i < 256; i++)
    _mm_clflush(&array[i * 4096 + DELTA]);
}

void victim(uint8_t secret)
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
int main(int argc, const char **argv)
{

  uint8_t secret = 23;
  flushCache();
  victim(secret);
  reloadCache();
  return 0;
}
