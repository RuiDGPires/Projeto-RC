#ifndef SESSION_H__
#define SESSION_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define BUFFER_SIZE 128

typedef struct {
  char dsip[BUFFER_SIZE], dsport[BUFFER_SIZE];

  int fd;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints, *res;
  struct sockaddr_in addr;
} Connection_context;

typedef struct {
	int is_logged;
	char uid[BUFFER_SIZE], pass[BUFFER_SIZE];
} Login_Context;

void send_message(Connection_context *context, const char message[], char response[]);

Connection_context *init_connection(const char dsip[], const char dsport[]);

void close_connection(Connection_context **context);
#endif
