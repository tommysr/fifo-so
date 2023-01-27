#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>

#define SERVER_FIFO "./server_fifo"
#define CLIENT_FIFO_FORMAT "./client_fifo_%d"
#define CLIENT_INDICATOR_FORMAT "client_%d"
#define SERVER_INDICATOR "SERVER"
#define MAX 10

void my_exit(const char *indicator, int failure, int server_fd, int client_fd, const char *fifo_name);
int create_fifo(const char *name, const char *indicator);

struct Message
{
  pid_t client_pid;
  char content[MAX];
};
// sizeof(struct Message) should not be bigger than PIPE_BUF variable !!!!

#endif