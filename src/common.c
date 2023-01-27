#include "common.h"

int create_fifo(const char *name, const char *indicator)
{
  if (mkfifo(name, 0600) == -1)
  {
    if (errno == EEXIST)
    {
      printf("[%s] fifo was already created.\n", indicator);
    }
    else
    {
      fprintf(stderr, "[%s] fifo creation error %s\n", indicator, strerror(errno));

      return -1; //meaning error
    }
  }
  else
  {
    printf("[%s] fifo was created.\n", indicator);
  }

  return 0; // meaning success
}
