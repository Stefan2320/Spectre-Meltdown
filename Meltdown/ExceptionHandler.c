#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

/* jumpbuf saves the 'state'/ stack, to do that we need to have a non-zero argument in the sigsetjmp and siglongjmp */
static sigjmp_buf jumpbuf;

static void catch_sigsegv()
{

  /* set the result of sigsetjmp to non-zero */
  siglongjmp(jumpbuf, 1);
}

int main()
{
  /* secret address */
  char *kernel_secret_addr = (char *)0xf8791000;

  /* Signal handler */
  signal(SIGSEGV, catch_sigsegv);

  /* where to return from siglongjmp, first time it returns zero because it's a direct invocation */
  if (sigsetjmp(jumpbuf, 1) == 0)
  {
    char kernel_data = *(char *)kernel_secret_addr;
    printf("I got executed.\n");
  }
  else
  {
    printf("Denied access!\n");
  }

  printf("Program was not killed!\n");

  return 0;
}
