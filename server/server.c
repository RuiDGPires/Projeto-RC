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

void parse_message(connection_context_t *connection, char *msg, char *fs){
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
  }else if (strcmp(command, "ULS") == 0){
    ulist(connection, msg, fs);
  }else if (strcmp(command, "PST") == 0){
    post(connection, msg, fs);
  }else if (strcmp(command, "RTV") == 0){
    retrieve(connection, msg, fs);
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

int main(int argc, char *argv[]){
  DEBUG_MSG_SECTION("MAIN");

  char dsport[PORT_SIZE];
  bool verbose;

  char buffer[BUFFER_SIZE];

  parse_args(dsport, &verbose, argc, argv);

  connection_context_t *context = (connection_context_t *) malloc(sizeof(connection_context_t));
  strcpy(context->port, dsport);

  init_udp(context);
  //Init tcp?

  char *fs = create_filesystem(".");

  while (1) {
    wait_udp_message(context, buffer, BUFFER_SIZE);
    parse_message(context, buffer);
  }

  close_udp(context);
  
  return 0;
}
