#ifndef CONNECTION_H__
#define CONNECTION_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../common/util.h"
#define PORT_SIZE 6

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

#endif
