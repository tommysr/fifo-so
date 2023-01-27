#include <unistd.h>
#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "common.h"
#include <string.h>
#include "ctype.h"

void abort_with_client(int server_fd, int client_fd);
void abort_without_client(int server_fd);

int main(int argc, char **argv)
{
  int server_fifo_descriptor;
  int client_fifo_descriptor;
  struct Message msg_buff;
  char client_fifo_name[256];

  // create server fifo
  if (create_fifo(SERVER_FIFO, SERVER_INDICATOR) == -1)
  {
    exit(EXIT_FAILURE);
  }

  // open server fifo in read mode
  server_fifo_descriptor = open(SERVER_FIFO, O_RDONLY);
  if (server_fifo_descriptor == -1)
  {
    fprintf(stderr, "[%s] open server fifo error: %s \n", SERVER_INDICATOR, strerror(errno));
    exit(EXIT_FAILURE);
  }

  printf("[S] opened server fifo in read mode\n.");
  sleep(10); // wait 10 second for clients

  while (1)
  {
    // read from server fifo, break the loop if no clients are writing
    if (read(server_fifo_descriptor, &msg_buff, sizeof(struct Message)) == 0)
    {
      printf("[S] all server fifo's were closed. Finishing work.\n");
      break;
    }

    printf("[S] Received message: %s, from client %d\n", msg_buff.content, msg_buff.client_pid);

    size_t message_size = strlen(msg_buff.content);
    for (int i = 0; i < message_size; i++)
      msg_buff.content[i] = toupper(msg_buff.content[i]);

    sprintf(client_fifo_name, CLIENT_FIFO_FORMAT, msg_buff.client_pid);
    client_fifo_descriptor = open(client_fifo_name, O_WRONLY);
    if (client_fifo_descriptor == -1)
    {
      fprintf(stderr, "[%s] open %s fifo error: %s \n", SERVER_INDICATOR, client_fifo_name, strerror(errno));
      abort_without_client(server_fifo_descriptor);
    }

    if (write(client_fifo_descriptor, &msg_buff, sizeof(struct Message)) == -1)
    {
      fprintf(stderr, "[%s] write %s fifo error: %s \n", SERVER_INDICATOR, client_fifo_name, strerror(errno));
      abort_with_client(server_fifo_descriptor, client_fifo_descriptor);
    }

    close(client_fifo_descriptor);
  }

  close(server_fifo_descriptor);
  unlink(SERVER_FIFO);
  exit(EXIT_SUCCESS);
}

void abort_with_client(int server_fd, int client_fd)
{
  close(client_fd);
  close(server_fd);
  unlink(SERVER_FIFO);
  exit(EXIT_FAILURE);
}

void abort_without_client(int server_fd)
{
  close(server_fd);
  unlink(SERVER_FIFO);
  exit(EXIT_FAILURE);
}
