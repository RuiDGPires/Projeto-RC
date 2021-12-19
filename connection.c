/*----------------------------------------------
 *
 * connection.c
 * -------
 * Implementation of connection.h
 *
 *---------------------------------------------*/

#include "connection.h"
#include "debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


void init_udp_connection(connection_context_t *connection){
  udp_info_t *info = (udp_info_t *) malloc(sizeof(udp_info_t));

  info->fd = socket(AF_INET, SOCK_DGRAM, 0);
  ASSERT(info->fd != -1, "Unable to create socket");
  DEBUG_MSG("Socket created\n");

  memset(&info->hints, 0, sizeof info->hints);
  info->hints.ai_family = AF_INET;
  info->hints.ai_socktype = SOCK_DGRAM;

  int errcode = getaddrinfo(connection->dsip, connection->dsport, &info->hints, &info->res);
  ASSERT(errcode == 0, "Unable to get address info");
  DEBUG_MSG("Got address info\n");
  
  connection->udp_info = info;  
}

void close_udp_connection(udp_info_t **info){
  freeaddrinfo((*info)->res);
  close((*info)->fd);
  free(*info); 
  *info = NULL;
}

void init_tcp_connection(connection_context_t *connection){
  tcp_info_t *info = (tcp_info_t *) malloc(sizeof(tcp_info_t));

  info->fd = socket(AF_INET, SOCK_STREAM, 0);
  ASSERT(info->fd != -1, "Unable to create socket");
  DEBUG_MSG("Socket created\n");

  memset(&info->hints, 0, sizeof info->hints);
  info->hints.ai_family = AF_INET;
  info->hints.ai_socktype = SOCK_STREAM;

  int errcode = getaddrinfo(connection->dsip, connection->dsport, &info->hints, &info->res);
  ASSERT(errcode == 0, "Unable to get address info");
  DEBUG_MSG("Got address info\n");
  
  connection->tcp_info = info;  
}

void close_tcp_connection(tcp_info_t **info){
  freeaddrinfo((*info)->res);
  close((*info)->fd);
  free(*info); 
  *info = NULL;
}

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
}

int is_logged(session_context_t *session){
  return session->uid[0] != '\0';
}

connection_context_t *init_connection(const char dsip[], const char dsport[]){
  connection_context_t *context = (connection_context_t *) malloc(sizeof(connection_context_t));
  
  strcpy(context->dsip, dsip); 
  strcpy(context->dsport, dsport); 

  init_udp_connection(context);

  context->session = init_session(context);
  return context;
}

void close_connection(connection_context_t **context){
  close_session(&((*context)->session));

  close_udp_connection(&((*context)->udp_info));

  free(*context);
  *context = NULL;
}

void send_udp_message_size(connection_context_t *context, const char message[], char response[], size_t response_size){
  context->udp_info->addrlen = sizeof(context->udp_info->addr);
  size_t size = strlen(message), n;

  n = sendto(context->udp_info->fd, message, size, 0, context->udp_info->res->ai_addr, context->udp_info->res->ai_addrlen);
  ASSERT(n != -1, "Unable to send message");
  DEBUG_MSG_SECTION("UDP");
  DEBUG_MSG("Message sent: %s\n", message);

  DEBUG_MSG("Awaiting response...\n");
  n = recvfrom(context->udp_info->fd, response, response_size, 0, (struct sockaddr*) &context->udp_info->addr, &context->udp_info->addrlen);
  ASSERT(n != -1, "Unable to receive message");
  response[n] = '\0';
  DEBUG_MSG("Response: %s\n", response);
}

void send_tcp_message_size(connection_context_t *context, const char message[], char response[], size_t response_size){
  context->tcp_info->addrlen = sizeof(context->tcp_info->addr);
  size_t size = strlen(message), n;

  n=connect(context->tcp_info->fd, context->tcp_info->res->ai_addr, context->tcp_info->res->ai_addrlen);
  ASSERT(n != -1, "Unable connect to server");
  DEBUG_MSG_SECTION("TCP");
  n = dprintf(context->tcp_info->fd, "%s\n", message);
  ASSERT(n != -1, "Unable to send message");
  DEBUG_MSG("Message sent: %s\n", message);
  DEBUG_MSG("Awaiting response...\n");
  n=read(context->tcp_info->fd, response, response_size);
  ASSERT(n != -1, "Unable to receive message");
  response[n] = '\0';

  DEBUG_MSG("Response: %s\n", response);
}
