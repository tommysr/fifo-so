#include "common.h"

int client_pid;
int server_fifo_descriptor;
int client_fifo_descriptor;
struct Message msg_buff;
char client_fifo_path[256];
char client_indicator[64];

int open_server_fifo();
void write_to_server_fifo();
void read_message_from_user();
int open_client_fifo();
void create_client_fifo();
void read_from_client_fifo();

int main(int argc, char **argv)
{
  client_pid = getpid();

  sprintf(client_fifo_path, CLIENT_FIFO_FORMAT, client_pid);
  sprintf(client_indicator, CLIENT_INDICATOR_FORMAT, client_pid);

  server_fifo_descriptor = open_server_fifo();
  create_client_fifo();
  read_message_from_user();
  write_to_server_fifo();

  client_fifo_descriptor = open_client_fifo();
  read_from_client_fifo();

  my_exit(client_indicator, 0, server_fifo_descriptor, client_fifo_descriptor, client_fifo_path);
}

void read_from_client_fifo()
{
  if (read(client_fifo_descriptor, &msg_buff, sizeof(struct Message)) == -1)
  {
    fprintf(stderr, "[%s] Error reading from fifo: %s\n", client_indicator, strerror(errno));
    my_exit(client_indicator, 1, server_fifo_descriptor, client_fifo_descriptor, client_fifo_path);
  }
  else
  {
    printf("[%s] Read message: %s\n", client_indicator, msg_buff.content);
  }
}

int open_client_fifo()
{
  int client_fifo_descriptor = open(client_fifo_path, O_RDONLY);

  if (client_fifo_descriptor == -1)
  {
    fprintf(stderr, "[%s] open client fifo error: %s\n", client_indicator, strerror(errno));
    my_exit(client_indicator, 1, server_fifo_descriptor, -1, client_fifo_path);
  }

  return client_fifo_descriptor;
}

void write_to_server_fifo()
{
  if (write(server_fifo_descriptor, &msg_buff, sizeof(struct Message)) == -1)
  {
    if (errno == EPIPE)
    {
      fprintf(stderr, "[%s] reading end of server fifo closed: %s \n", client_indicator, strerror(errno));
    }
    else
    {
      fprintf(stderr, "[%s] write to server fifo error: %s \n", client_indicator, strerror(errno));
    }

    my_exit(client_indicator, 1, server_fifo_descriptor, -1, client_fifo_path);
  }
  else
  {
    printf("[%s] Sent message: %s \n", client_indicator, msg_buff.content);
  }
}

void read_message_from_user()
{
  printf("Enter the text, it'll be shrink to %d chars:\n", MAX - 1);

  if (fgets(msg_buff.content, MAX, stdin) == NULL)
  {
    printf("[%s] read text error.\n", client_indicator);
    my_exit(client_indicator, 1, server_fifo_descriptor, -1, client_fifo_path);
  }

  msg_buff.client_pid = client_pid;
  msg_buff.content[strcspn(msg_buff.content, "\n")] = '\0';
}

void create_client_fifo()
{
  if (create_fifo(client_fifo_path, client_indicator) == -1)
  {
    my_exit(client_indicator, 1, server_fifo_descriptor, -1, NULL);
  }
}

int open_server_fifo()
{
  int server_fifo_descriptor = open(SERVER_FIFO, O_WRONLY);

  if (server_fifo_descriptor == -1)
  {
    if (errno == ENOENT)
    {
      fprintf(stderr, "[%s]  server fifo not found: %s\n", client_indicator, strerror(errno));
    }
    else
    {
      fprintf(stderr, "[%s] open server fifo error: %s\n", client_indicator, strerror(errno));
    }

    my_exit(client_indicator, 1, -1, -1, NULL);
  }
  else
  {
    printf("[%s] opened server fifo in write mode.\n", client_indicator);
  }

  return server_fifo_descriptor;
}