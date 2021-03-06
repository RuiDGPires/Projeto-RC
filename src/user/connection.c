/*----------------------------------------------
 *
 * connection.c
 * -------
 * Implementation of connection.h
 *
 *---------------------------------------------*/

#include "../common/debug.h"
#include "connection.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

int timer_start(int sd){
  struct timeval tmout;
  memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */

  tmout.tv_sec=15; /* Wait for 15 sec for a reply from server. */

  return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tmout,sizeof(struct timeval)));
}

int timer_stop(int sd){
  struct timeval tmout;
  memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */

  return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
}

int init_udp_connection(connection_context_t *connection){
  protocol_info_t *info = (protocol_info_t *) malloc(sizeof(protocol_info_t));

  info->fd = socket(AF_INET, SOCK_DGRAM, 0);
  ASSERT(info->fd != -1, FERROR, "Unable to create socket");
  DEBUG_MSG("Socket created\n");

  memset(&info->hints, 0, sizeof info->hints);
  info->hints.ai_family = AF_INET;
  info->hints.ai_socktype = SOCK_DGRAM;

  int errcode = getaddrinfo(connection->dsip, connection->dsport, &info->hints, &info->res);
  ASSERT(errcode == 0, FERROR, "Unable to get address info");
  DEBUG_MSG("Got address info\n");

  connection->udp_info = info;  
  return SUCCESS;
}

void close_udp_connection(protocol_info_t **info){
  freeaddrinfo((*info)->res);
  close((*info)->fd);
  free(*info); 
  *info = NULL;
}

int init_tcp_connection(connection_context_t *connection){
  protocol_info_t *info = (protocol_info_t *) malloc(sizeof(protocol_info_t));

  info->fd = socket(AF_INET, SOCK_STREAM, 0);
  ASSERT(info->fd != -1, FERROR, "Unable to create socket");
  DEBUG_MSG("Socket created\n");

  memset(&info->hints, 0, sizeof info->hints);
  info->hints.ai_family = AF_INET;
  info->hints.ai_socktype = SOCK_STREAM;

  int errcode = getaddrinfo(connection->dsip, connection->dsport, &info->hints, &info->res);
  ASSERT(errcode == 0, FERROR, "Unable to get address info");
  DEBUG_MSG("Got address info\n");
  
  connection->tcp_info = info;  
  return SUCCESS;
}

void close_tcp_connection(protocol_info_t **info){
  freeaddrinfo((*info)->res);
  close((*info)->fd);
  free(*info); 
  *info = NULL;
}

/* Initializes local client session */
session_context_t *init_session(connection_context_t *connection){
  session_context_t *session = (session_context_t *) malloc(sizeof(session_context_t));
  logout(session);

  session->connection = connection;

  return session;
}

void close_session(session_context_t **session_p){
  session_context_t *session = *session_p;
  free(session);
  *session_p = NULL;
}

void login(session_context_t *session, char uid[], char pass[]){
  strcpy(session->uid, uid);
  strcpy(session->pass, pass);
}

void logout(session_context_t *session){
  session->uid[0] = '\0';
  session->gid[0] = '\0';
}

int is_logged(session_context_t *session){
  return session->uid[0] != '\0';
}

void select_group(session_context_t *session, char gid[]){
  int gid_int = atoi(gid);
  
  sprintf(session->gid, "%02d", gid_int);
}

int is_group_selected(session_context_t *session){
  return session->gid[0] != '\0';
}

connection_context_t *init_connection(const char dsip[], const char dsport[]){
  connection_context_t *context = (connection_context_t *) malloc(sizeof(connection_context_t));
  
  strcpy(context->dsip, dsip); 
  strcpy(context->dsport, dsport); 

  if (init_udp_connection(context) == FERROR){
    context = NULL;
    return context;
  }

  context->session = init_session(context);
  return context;
}

void close_connection(connection_context_t **context){
  close_session(&((*context)->session));

  close_udp_connection(&((*context)->udp_info));

  free(*context);
  *context = NULL;
}

/* Send and receive udp message specifying response buffer size */
void send_udp_message_size(connection_context_t *context, const char message[], char response[], size_t response_size, int* rcv_success){
  context->udp_info->addrlen = sizeof(context->udp_info->addr);
  size_t size = strlen(message), n;

  n = sendto(context->udp_info->fd, message, size, 0, context->udp_info->res->ai_addr, context->udp_info->res->ai_addrlen);
  ASSERT_NOR(n != -1, "Unable to send message");
  DEBUG_MSG("Message sent: %s\n", message);

  ASSERT_NOR(timer_start(context->udp_info->fd) != -1, "Unable to set socket timeout");

  DEBUG_MSG("Awaiting response...\n");
  n = recvfrom(context->udp_info->fd, response, response_size, 0, (struct sockaddr*) &context->udp_info->addr, &context->udp_info->addrlen);
  
  if(n == -1){
    *rcv_success = 0;
    DEBUG_MSG_SECTION("UDP");
    DEBUG_MSG("Unable to receive message\n");
    return;
  }

  *rcv_success = 1;

  response[n] = '\0';
  DEBUG_MSG("Response: %s\n", response);

  ASSERT_NOR(timer_stop(context->udp_info->fd) != -1, "Unable to reset socket timeout");
}

/* Send and receive a tcp message specifying the response buffer size */
void send_tcp_message_size(connection_context_t *context, const char message[], char response[], size_t response_size){
  DEBUG_MSG_SECTION("TCP");
  init_tcp_connection(context);

  context->tcp_info->addrlen = sizeof(context->tcp_info->addr);
  size_t n;

  n=connect(context->tcp_info->fd, context->tcp_info->res->ai_addr, context->tcp_info->res->ai_addrlen);
  ASSERT_NOR(n != -1, "Unable connect to server");
  
  n = dprintf(context->tcp_info->fd, "%s", message);
  ASSERT_NOR(n != -1, "Unable to send message");
  DEBUG_MSG("Message sent: %s\n", message);

  DEBUG_MSG("Awaiting response...\n");
  n=read(context->tcp_info->fd, response, response_size);
  ASSERT_NOR(n != -1, "Unable to receive tcp message");
  response[n] = '\0';

  DEBUG_MSG("Response: %s\n", response);

  close_tcp_connection(&context->tcp_info);
}

/* Send and receive a tcp message specifying the buffer size */
void send_tcp_message_sending_size(connection_context_t *context, const char message[], char response[], size_t size){
  DEBUG_MSG_SECTION("TCP");

  init_tcp_connection(context);

  context->tcp_info->addrlen = sizeof(context->tcp_info->addr);
  size_t n;

  n=connect(context->tcp_info->fd, context->tcp_info->res->ai_addr, context->tcp_info->res->ai_addrlen);
  ASSERT_NOR(n != -1, "Unable connect to server");

  size_t total_written_size = 0;
  while(total_written_size != size)
    total_written_size += write(context->tcp_info->fd, &message[total_written_size], size-total_written_size);

  DEBUG_MSG("Message sent: %s\n", message);

  DEBUG_MSG("Awaiting response...\n");
  n=read(context->tcp_info->fd, response, BUFFER_SIZE);
  ASSERT_NOR(n != -1, "Unable to receive tcp message");
  response[n] = '\0';

  DEBUG_MSG("Response: %s\n", response);

  close_tcp_connection(&context->tcp_info);
}

/* Send a tcp message */
void send_tcp_message_no_answer(connection_context_t *context, const char *message){
  DEBUG_MSG_SECTION("TCP");
  init_tcp_connection(context);

  context->tcp_info->addrlen = sizeof(context->tcp_info->addr);
  size_t  n;

  n=connect(context->tcp_info->fd, context->tcp_info->res->ai_addr, context->tcp_info->res->ai_addrlen);
  ASSERT_NOR(n != -1, "Unable connect to server");
  n = dprintf(context->tcp_info->fd, "%s", message);
  ASSERT_NOR(n != -1, "Unable to send tcp message");
  DEBUG_MSG("Message sent: %s\n", message);
}
