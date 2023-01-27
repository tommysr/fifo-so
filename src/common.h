#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#define SERVER_FIFO "./server_fifo"
#define CLIENT_FIFO_FORMAT "./client_fifo_%d"
#define CLIENT_INDICATOR_FORMAT "client_%d"
#define SERVER_INDICATOR "SERVER"
#define MAX 10

int create_fifo(const char *name, const char *indicator);
int open_fifo(const char *name, const int flags);

struct Message
{
  pid_t client_pid;
  char content[MAX];
};

#endif