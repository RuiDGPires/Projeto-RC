#include "session.h"
#include "debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


void send_message(Connection_context *context, const char message[], char response[]){
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

Connection_context *init_connection(const char dsip[], const char dsport[]){
  Connection_context *context = (Connection_context *) malloc(sizeof(Connection_context));
  
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

  return context;
}

void close_connection(Connection_context **context){
  freeaddrinfo((*context)->res);
  close((*context)->fd);

  free(*context);
  *context = NULL;
}

