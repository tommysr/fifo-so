#include "common.h"

void exit_with_failure(int status, int server_fd, int client_fd, const char *fifo_path)
{
  switch (status)
  {
  case 1: // client with not opened client fd
    close(server_fd);
    unlink(fifo_path);
    exit(EXIT_FAILURE);
    break;

  case 2: // client with opened client fd
    close(server_fd);
    close(client_fd);
    unlink(fifo_path);
    exit(EXIT_FAILURE);

    break;

  case 3: // client with opened client fd
    close(server_fd);
    exit(EXIT_FAILURE);
    break;

  case 4: // client with opened client fd
    unlink(fifo_path);
    exit(EXIT_FAILURE);
    break;

  default:
    break;
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
