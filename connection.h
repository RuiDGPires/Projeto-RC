#ifndef CONNECTION_H__
#define CONNECTION_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define BUFFER_SIZE 128

struct session_context_s;
struct connection_context_s;

typedef struct session_context_s {
	int is_logged;
	char uid[BUFFER_SIZE], pass[BUFFER_SIZE];
  struct connection_context_s *connection;
} session_context_t;

typedef struct connection_context_s {
  char dsip[BUFFER_SIZE], dsport[BUFFER_SIZE];

  int fd;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints, *res;
  struct sockaddr_in addr;

  session_context_t *session;
} connection_context_t;


session_context_t *init_session(connection_context_t *);
void close_session(session_context_t **);

connection_context_t *init_connection(const char *, const char *);
void close_connection(connection_context_t **);
void send_message(connection_context_t *, const char *, char *);

#endif
