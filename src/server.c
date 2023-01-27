#include "common.h"

int server_fifo_descriptor;
int client_fifo_descriptor;
struct Message msg_buff;
char client_fifo_path[256];

int read_from_server_fifo();
void create_and_open_server_fifo();
void write_to_client_fifo();
int open_client_fifo();
void handle_message();

int main(int argc, char **argv)
{
  create_and_open_server_fifo();

  while (read_from_server_fifo())
  {
    printf("[%s] Received message: %s, from client %d\n", SERVER_INDICATOR, msg_buff.content, msg_buff.client_pid);
    handle_message();
    sprintf(client_fifo_path, CLIENT_FIFO_FORMAT, msg_buff.client_pid);
    client_fifo_descriptor = open_client_fifo();
    write_to_client_fifo();
    close(client_fifo_descriptor);
  }

  printf("[%s] all server fifo's write ends were closed. Finishing work...\n", SERVER_INDICATOR);
  my_exit(SERVER_INDICATOR, 0, server_fifo_descriptor, -1, SERVER_FIFO);
}

int read_from_server_fifo()
{
  int read_res = read(server_fifo_descriptor, &msg_buff, sizeof(struct Message));

  if (read_res == -1)
  {
    fprintf(stderr, "[%s] Error reading from server fifo: %s\n", SERVER_INDICATOR, strerror(errno));
    my_exit(SERVER_INDICATOR, 1, server_fifo_descriptor, -1, SERVER_FIFO);
  }

  return read_res;
}

void write_to_client_fifo()
{
  if (write(client_fifo_descriptor, &msg_buff, sizeof(struct Message)) == -1)
  {
    if (errno == EPIPE)
    {
      fprintf(stderr, "[%s] reading end of %s fifo closed: %s \n", SERVER_INDICATOR, client_fifo_path, strerror(errno));
    }
    else
    {
      fprintf(stderr, "[%s] write to %s fifo error: %s \n", SERVER_INDICATOR, client_fifo_path, strerror(errno));
    }

    my_exit(SERVER_INDICATOR, 1, server_fifo_descriptor, client_fifo_descriptor, SERVER_FIFO);
  }
  else
  {
    printf("[%s] wrote to fifo of %d \n", SERVER_INDICATOR, msg_buff.client_pid);
  }
}

int open_client_fifo()
{
  int client_fifo_descriptor = open(client_fifo_path, O_WRONLY);

  if (client_fifo_descriptor == -1)
  {
    fprintf(stderr, "[%s] open %s fifo error: %s \n", SERVER_INDICATOR, client_fifo_path, strerror(errno));
    my_exit(SERVER_INDICATOR, 1, server_fifo_descriptor, -1, SERVER_FIFO);
  }

  return client_fifo_descriptor;
}

void handle_message()
{
  size_t message_size = strlen(msg_buff.content);
  for (int i = 0; i < message_size; i++)
    msg_buff.content[i] = toupper(msg_buff.content[i]);
}

void create_and_open_server_fifo()
{
  if (create_fifo(SERVER_FIFO, SERVER_INDICATOR) == -1)
  {
    my_exit(SERVER_INDICATOR, 1, -1, -1, NULL);
  }

  server_fifo_descriptor = open(SERVER_FIFO, O_RDONLY);

  if (server_fifo_descriptor == -1)
  {
    fprintf(stderr, "[%s] open server fifo error: %s \n", SERVER_INDICATOR, strerror(errno));
    my_exit(SERVER_INDICATOR, 1, -1, -1, SERVER_FIFO);
  }

  printf("[%s] opened server fifo in read mode, waiting for clients to connect...\n", SERVER_INDICATOR);
  sleep(10);
}
