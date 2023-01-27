#include "common.h"

void my_exit(const char *indicator, int failure, int server_fd, int client_fd, const char *fifo_name)
{
  if (server_fd != -1)
  {
    if (close(server_fd) == -1)
    {
      fprintf(stderr, "[%s] server fd close error %s\n", indicator, strerror(errno));
    }
  }

  if (client_fd != -1)
  {
    if (close(server_fd) == -1)
    {
      fprintf(stderr, "[%s] client fd close error %s\n", indicator, strerror(errno));
    }
  }

  if (fifo_name)
  {
    if (unlink(fifo_name) == -1)
    {
      fprintf(stderr, "[%s] unlink fifo %s failed: %s\n", indicator, *fifo_name, strerror(errno));
    }
  }

  if (failure)
  {
    exit(EXIT_FAILURE);
  }
  else
  {
    exit(EXIT_SUCCESS);
  }
}

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

      return -1; // meaning error
    }
  }
  else
  {
    printf("[%s] fifo was created.\n", indicator);
  }

  return 0; // meaning success
}
