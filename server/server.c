#include "../common/debug.h"
#include "../common/util.h"
#include "connection.h"
#include "commands.h"
#include "file_management.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define CLEAR(var) var[0] = '\0'
#define DEFAULT(var, str) if (var[0] == '\0') strcpy(var, str)
#define DEFAULT_DSPORT "58065" // 58000 + GN

void parse_udp_message(connection_context_t *connection, char *msg, char *fs){
  char *command = get_word(&msg);

  if (strcmp(command, "REG") == 0){
    reg(connection, msg, fs);
  }else if (strcmp(command, "UNR") == 0){
    unregister(connection, msg, fs);
  }else if (strcmp(command, "LOG") == 0){
    login_(connection, msg, fs);
  }else if (strcmp(command, "OUT") == 0){
    logout_(connection, msg, fs);
  }else if (strcmp(command, "GLS") == 0){
    groups(connection, msg, fs);
  }else if (strcmp(command, "GSR") == 0){
    subscribe(connection, msg, fs);
  }else if (strcmp(command, "GUR") == 0){
    unsubscribe(connection, msg, fs);
  }else if (strcmp(command, "GLM") == 0){
    my_groups(connection, msg, fs);
  }else throw_error("Unkown command");
}

void parse_tcp_message(connection_context_t *connection, char *fs){
    char command[4];
    command[3] = '\0';
   
    read_fd(connection->tcp_info->fd, command, 3);
    read_fd(connection->tcp_info->fd, NULL, 1); // throw away the space after

    if (strcmp(command, "ULS") == 0){
        ulist(connection, fs);
    }else if (strcmp(command, "PST") == 0){
        post(connection, fs);
    }else if (strcmp(command, "RTV") == 0){
        retrieve(connection, fs);
    }else throw_error("Unkown command");
}

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

  int maxfd1;
  fd_set rset;



  parse_args(dsport, &verbose, argc, argv);

  connection_context_t *context = (connection_context_t *) malloc(sizeof(connection_context_t));
  strcpy(context->port, dsport);

  init_udp(context);
  init_tcp(context);

  char *fs = create_filesystem(".");

  FD_ZERO(&rset);

  maxfd1 = max(context->tcp_info->fd, context->udp_info->fd) + 1;

  while (1) {
    FD_SET(context->udp_info->fd, &rset);
    FD_SET(context->tcp_info->fd, &rset);

    DEBUG_MSG("Waiting Select... \n");
    select(maxfd1, &rset, NULL, NULL, NULL);
    DEBUG_MSG("... Select Completed \n");

    if(FD_ISSET(context->udp_info->fd, &rset)){
        wait_udp_message(context, buffer_udp, BUFFER_SIZE);
        parse_udp_message(context, buffer_udp, fs);
    }
    else{
      if(FD_ISSET(context->tcp_info->fd, &rset)){
        if(fork() == 0){
            accept_tcp_message(context);
            parse_tcp_message(context, fs); //Do TCP stuff
            close(context->tcp_info->fd);
            DEBUG_MSG("Close Fork %d\n", getpid());
            exit(0);
        }
      }
    }
  }

  close_udp(context);
  
  return 0;
}
