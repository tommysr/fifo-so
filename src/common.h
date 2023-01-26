#ifndef COMMON_H
#define COMMON_H

#include "sys/types.h"
#include "stdlib.h"
#include "errno.h"

#define SERVER_FIFO "./server_fifo"
#define CLIENT_FIFO "./client_fifo_%d"
#define MAX 10

int open_fifo(const char *name, const int flags);
void create_fifo(const char *name);

struct Message
{
  pid_t client_pid;
  char content[MAX];
};

#endif