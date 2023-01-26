#include "common.h"

int open_fifo(const char *name, const int flags)
{
  int open_res = open(name, flags);

  if (open_res == -1)
  {
    perror("[S] Open fifo error\n");
    exit(EXIT_FAILURE);
  }

  return open_res;
}

void create_fifo(const char *name)
{
  if (mkfifo(name, 0777) == -1)
  {
    if (errno == EEXIST)
    {
      printf("[S] Fifo was already created.\n");
    }
    else
    {
      perror("[S] Error during fifo creation.\n");
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    printf("[S] Fifo was created.\n");
  }
}
