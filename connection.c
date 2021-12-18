#include "connection.h"
#include "debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

session_context_t *init_session(connection_context_t *connection){
  session_context_t *session = (session_context_t *) malloc(sizeof(session_context_t));

  session->is_logged = 0;
  session->connection = connection;

  return session;
}

void close_session(session_context_t **session_p){
  session_context_t *session = *session_p;

  // Realizar loggout caso o utilizador esteja logged in
  if (session->is_logged){
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%s %s %s\n", "OUT", session->uid, session->pass);
    send_message(session->connection, buffer, buffer);
  }

  free(session);
  *session_p = NULL;
}

connection_context_t *init_connection(const char dsip[], const char dsport[]){
  connection_context_t *context = (connection_context_t *) malloc(sizeof(connection_context_t));
  
    strcpy(context->dsip, dsip); 
  strcpy(context->dsport, dsport); 

  context->fd = socket(AF_INET, SOCK_DGRAM, 0);
  ASSERT(context->fd != -1, "Unable to create socket");
  DEBUG_MSG("Socket created\n");

  memset(&context->hints, 0, sizeof context->hints);
  context->hints.ai_family = AF_INET;
  context->hints.ai_socktype = SOCK_DGRAM;

  int errcode = getaddrinfo(context->dsip, context->dsport, &context->hints, &context->res);
  ASSERT(errcode == 0, "Unable to get address info");
  DEBUG_MSG("Got address info\n");


  context->session = init_session(context);
  return context;
}

void close_connection(connection_context_t **context){
  close_session(&((*context)->session));
  freeaddrinfo((*context)->res);
  close((*context)->fd);

  free(*context);
  *context = NULL;
}

void send_message(connection_context_t *context, const char message[], char response[]){
  context->addrlen = sizeof(context->addr);
  size_t size = strlen(message), n;

  n = sendto(context->fd, message, size, 0, context->res->ai_addr, context->res->ai_addrlen);
  ASSERT(n != -1, "Unable to send message");
  DEBUG_MSG_SECTION("UDP");
  DEBUG_MSG("Message sent: %s\n", message);

  DEBUG_MSG("Awaiting response...\n");
  memset(response, ' ', BUFFER_SIZE);
  n = recvfrom(context->fd, response, BUFFER_SIZE, 0, (struct sockaddr*) &context->addr, &context->addrlen);
  ASSERT(n != -1, "Unable to receive message");
  response[n] = '\0';
  DEBUG_MSG("Response: %s\n", response);
}

