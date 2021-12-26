#include "connection.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void init_udp(connection_context_t *connection){
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
}
