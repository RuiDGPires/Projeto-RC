#include "../../common/debug.h"
#include "../../common/util.h"
#include "../../server/connection.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define CLEAR(var) var[0] = '\0'
#define DEFAULT(var, str) if (var[0] == '\0') strcpy(var, str)
#define DEFAULT_DSPORT "58065" // 58000 + GN

void parse_args(char *dsport, bool *verbose, int argc, char *argv[]){
  DEBUG_MSG_SECTION("ARGS");

  CLEAR(dsport);
  *verbose = FALSE;
  // Parse args
  for (int i = 1; i < argc; i++){
    if (strcmp(argv[i], "-v") == 0)
      *verbose = TRUE;
    else if (strcmp(argv[i], "-p") == 0){
      ASSERT(argc != i + 1, "-p requires another argument");
      strcpy(dsport, argv[++i]);

    }else throw_error("Unkown command line argument");
  }

  DEFAULT(dsport, DEFAULT_DSPORT);

  DEBUG_MSG("verbose set to: %d\n", *verbose);
  DEBUG_MSG("dsport set to: %s\n", dsport);
}

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}

int main(int argc, char *argv[]){
  DEBUG_MSG_SECTION("MAIN");

  char dsport[PORT_SIZE];
  bool verbose;

  char buffer_udp[BUFFER_SIZE];
  char buffer_tcp[BUFFER_SIZE];

  int maxfd1, nready;
  fd_set rset;

  parse_args(dsport, &verbose, argc, argv);

  connection_context_t *context = (connection_context_t *) malloc(sizeof(connection_context_t));
  strcpy(context->port, dsport);

  init_udp(context);
  init_tcp(context);

  FD_ZERO(&rset);

  maxfd1 = max(context->tcp_info->fd, context->udp_info->fd) + 1;


  //SELECT
  while(1){

    FD_SET(context->tcp_info->fd, &rset);
    FD_SET(context->udp_info->fd, &rset);

    nready = select(maxfd1, &rset, NULL, NULL, NULL);

    if(fork() == 0){
      if(FD_ISSET(context->udp_info->fd, &rset)){
        wait_udp_message(context, buffer_udp, BUFFER_SIZE);
        send_udp_message(context, buffer_udp);
      }
      else{
        if(FD_ISSET(context->tcp_info->fd, &rset)){
          //Close listen fd?
          int nfd = wait_tcp_message(context, buffer_tcp, BUFFER_SIZE);
          send_tcp_message(context, buffer_tcp, BUFFER_SIZE, nfd);
        }
      }
      exit(0);
    }
  }

  close_udp(context);
  close_tcp(context);
  
  return 0;
}
