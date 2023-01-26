
#include <unistd.h>
#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "common.h"
#include <string.h>

int main(int argc, char **argv)
{
  int client_pid = getpid();
  int server_fifo_descriptor, client_fifo_descriptor;
  struct Message msg_buff;
  char client_fifo_name[256];

  sprintf(client_fifo_name, CLIENT_FIFO, client_pid);
  server_fifo_descriptor = open(SERVER_FIFO, O_WRONLY);
  printf("[C-%d] Opened server fifo in write mode\n.", client_pid);

  create_fifo(client_fifo_name);

    while (1)
  {
    msg_buff.client_pid = client_pid;

    if (fgets(msg_buff.content, MAX, stdin) == NULL)
    {
      printf("[C-%d] Read text error\n.", client_pid);
      exit(EXIT_FAILURE);
    }

    msg_buff.content[strcspn(msg_buff.content, "\n")] = '\0';
    write(server_fifo_descriptor, &msg_buff, sizeof(struct Message));

    printf("[C-%d] Sent message: %s\n", msg_buff.client_pid, msg_buff.content);

    client_fifo_descriptor = open(client_fifo_name, O_RDONLY);

    if (read(client_fifo_descriptor, &msg_buff, sizeof(struct Message)) > 0)
    {
      printf("[C-%d] Read message: %s\n", msg_buff.client_pid, msg_buff.content);
    }
    else
    {
      break;
    }

    close(client_fifo_descriptor);
  }

  close(client_fifo_descriptor);
  unlink(client_fifo_name);

  exit(EXIT_SUCCESS);
}