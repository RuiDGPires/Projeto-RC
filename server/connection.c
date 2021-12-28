#include "connection.h"
#include "../common/debug.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void init_udp(connection_context_t *connection){
  connection->udp_info = (udp_info_t *) malloc(sizeof(udp_info_t));
  if((connection->udp_info->fd=socket(AF_INET,SOCK_DGRAM,0))==-1)exit(1);//error
  memset(&connection->udp_info->hints,0,sizeof connection->udp_info->hints);
  connection->udp_info->hints.ai_family=AF_INET;//IPv4
  connection->udp_info->hints.ai_socktype=SOCK_DGRAM;//UDP socket
  connection->udp_info->hints.ai_flags=AI_PASSIVE;
  if(getaddrinfo(NULL, connection->port, &connection->udp_info->hints,&connection->udp_info->res)!=0)/*error*/exit(1);
  if(bind(connection->udp_info->fd,connection->udp_info->res->ai_addr,connection->udp_info->res->ai_addrlen)==-1)/*error*/exit(1);
}

void close_udp(connection_context_t *connection){
  freeaddrinfo(connection->udp_info->res);
  close(connection->udp_info->fd);
  free(connection->udp_info); //Should?
  connection->udp_info = NULL;
}

void init_tcp(connection_context_t *connection){
  connection->tcp_info = (tcp_info_t *) malloc(sizeof(tcp_info_t));

  if((connection->tcp_info->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1); //error
  memset(&connection->tcp_info->hints, 0, sizeof connection->tcp_info->hints);
  connection->tcp_info->hints.ai_family = AF_INET;
  connection->tcp_info->hints.ai_socktype = SOCK_STREAM;
  connection->tcp_info->hints.ai_flags = AI_PASSIVE;

  if( getaddrinfo(NULL, connection->port,&connection->tcp_info->hints,&connection->tcp_info->res) != 0) /*error*/ exit(1);
  if( bind(connection->tcp_info->fd, connection->tcp_info->res->ai_addrlen) == -1) exit(1); //error

  //listen?
}

void close_tcp(connection_context_t *connection){
  freeaddrinfo(connection->tcp_info->res);
  close(connection->tcp_info->fd);
  free(connection->tcp_info);
  connection->tcp_info = NULL;
}
