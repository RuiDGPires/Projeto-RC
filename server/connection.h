#ifndef CONNECTION_H__
#define CONNECTION_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../common/util.h"
#define PORT_SIZE 6
#define BACKLOG_NUM 99

typedef struct {
  int fd;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints, *res;
  struct sockaddr_in addr;
} udp_info_t;

typedef struct {
  int fd;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints, *res;
  struct sockaddr_in addr;
} tcp_info_t;

typedef struct connection_context_s {
  char port[PORT_SIZE];
  udp_info_t *udp_info;
  tcp_info_t *tcp_info;
} connection_context_t;

void init_udp(connection_context_t *);
void close_udp(connection_context_t *);

void init_tcp(connection_context_t *);
void close_tcp(connection_context_t *);

void wait_udp_message(connection_context_t *, char *, size_t);
void send_udp_message_size(connection_context_t *, char *, size_t);

#define send_udp_message(con, buffer) send_udp_message_size(con, buffer, strlen(buffer))

int accept_tcp_message(connection_context_t *);
void wait_tcp_message(connection_context_t *, char *, size_t, int);
void send_tcp_message(connection_context_t *, char *, size_t, int);

#endif
