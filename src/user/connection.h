/*----------------------------------------------
 *
 * connection.h
 * -------
 * Library describing an API to perform connection
 * related operations between client and server.
 *
*---------------------------------------------*/

#ifndef CONNECTION_H__
#define CONNECTION_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../common/util.h"

typedef struct {
  int fd;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints, *res;
  struct sockaddr_in addr;
} protocol_info_t;

struct session_context_s;
struct connection_context_s;

typedef struct session_context_s {
	char uid[UID_SIZE], pass[PASS_SIZE];
  char gid[GID_SIZE];
  struct connection_context_s *connection;
} session_context_t;

typedef struct connection_context_s {
  char dsip[BUFFER_SIZE], dsport[BUFFER_SIZE];

  protocol_info_t *udp_info;
  protocol_info_t *tcp_info;
  
  session_context_t *session;
} connection_context_t;

int init_udp_connection(connection_context_t *);
void close_udp_connection(protocol_info_t **);

int init_tcp_connection(connection_context_t *);
void close_tcp_connection(protocol_info_t **);

session_context_t *init_session(connection_context_t *);
void close_session(session_context_t **);
void login(session_context_t *, char *, char *);
int is_logged(session_context_t *);
void logout(session_context_t *);
void select_group(session_context_t *,char *);
int is_group_selected(session_context_t *);

connection_context_t *init_connection(const char *, const char *);
void close_connection(connection_context_t **);

#define send_udp_message(con, mes, res, rcv) send_udp_message_size(con, mes, res, BUFFER_SIZE, rcv)
void send_udp_message_size(connection_context_t *, const char *, char *, size_t, int*);
#define send_tcp_message(con, mes, res) send_tcp_message_size(con, mes, res, BUFFER_SIZE)
void send_tcp_message_size(connection_context_t *, const char *, char *, size_t);
void send_tcp_message_sending_size(connection_context_t *context, const char message[], char response[], size_t size);

void send_tcp_message_no_answer(connection_context_t *, const char *);

#endif
