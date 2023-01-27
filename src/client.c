#include <unistd.h>
#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include <string.h>

#include "common.h"

void abort_with_client_fd(int server_fd, int client_fd, const char *client_fifo_name);
void abort_without_client_fd(int server_fd, const char *client_fifo_name);

int main(int argc, char **argv)
{
  int client_pid = getpid();
  int server_fifo_descriptor;
  int client_fifo_descriptor;
  struct Message msg_buff;
  char client_fifo_name[256];
  char client_indicator[64];

  //prepare fifo name
  sprintf(client_fifo_name, CLIENT_FIFO_FORMAT, client_pid);
  //prepare indicator
  sprintf(client_indicator, CLIENT_INDICATOR_FORMAT, client_pid);

  //open server fifo fd
  server_fifo_descriptor = open(SERVER_FIFO, O_WRONLY); // failure on open error, calls exit
  if (server_fifo_descriptor == -1)
  {
    fprintf(stderr, "[%s] open server fifo error: %s\n", client_indicator, strerror(errno));
    exit(EXIT_FAILURE);
  }

  printf("[%s] Opened server fifo in write mode.\n", client_indicator);

  //create client fifo
  if (create_fifo(client_fifo_name, client_indicator) == -1)
  {
    close(server_fifo_descriptor); // close server fd
    exit(EXIT_FAILURE);
  }

  printf("Enter the text:\n");
  if (fgets(msg_buff.content, MAX, stdin) == NULL)
  {
    printf("[%s] Read text error\n.", client_indicator);
    abort_without_client_fd(server_fifo_descriptor, client_fifo_name);
  }

  // prepare message buffer
  msg_buff.client_pid = client_pid;
  msg_buff.content[strcspn(msg_buff.content, "\n")] = '\0';

  // write to server fifo
  if (write(server_fifo_descriptor, &msg_buff, sizeof(struct Message)) == -1)
  {
    fprintf(stderr, "[%s] write to server fifo error: %s \n", client_indicator, strerror(errno));
    abort_without_client_fd(server_fifo_descriptor, client_fifo_name);
  }

  printf("[%s] Sent message: %s\n", client_indicator, msg_buff.content);

  // open client fifo in read mode
  client_fifo_descriptor = open(client_fifo_name, O_RDONLY);
  if (client_fifo_descriptor == -1)
  {
    fprintf(stderr, "[%s] open client fifo error: %s\n", client_indicator, strerror(errno));
    abort_without_client_fd(server_fifo_descriptor, client_fifo_name);
  }

  //read from client fifo
  if (read(client_fifo_descriptor, &msg_buff, sizeof(struct Message)) == -1)
  {
    fprintf(stderr, "[%s] Error reading from fifo: %s\n", client_indicator, strerror(errno));
    abort_with_client_fd(server_fifo_descriptor, client_fifo_descriptor, client_fifo_name);
  }

  printf("[%s] Read message: %s\n", client_indicator, msg_buff.content);

  // end clean up and exit
  close(server_fifo_descriptor);
  close(client_fifo_descriptor);
  unlink(client_fifo_name);
  exit(EXIT_SUCCESS);
}

void abort_with_client_fd(int server_fd, int client_fd, const char *client_fifo_name)
{
  close(server_fd);
  close(client_fd);
  unlink(client_fifo_name);
  exit(EXIT_FAILURE);
}

void abort_without_client_fd(int server_fd, const char *client_fifo_name)
{
  close(server_fd);
  unlink(client_fifo_name);
  exit(EXIT_FAILURE);
}
