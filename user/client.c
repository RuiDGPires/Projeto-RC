/*----------------------------------------------
 *
 * client.c
 * -------
 * Includes the entry point for the application
 * and base functionality
 *
 *---------------------------------------------*/

#include <string.h>
#include <ctype.h>

#include "../common/util.h"
#include "../common/debug.h"
#include "../common/libio.h"
#include "../common/constants.h"
#include "connection.h"
#include "commands.h"



bool parse_input (connection_context_t *context, char str[]){
  if (strlen(str) == 0) return 1;
  char *command = get_word(&str);
  int error = 1;

  if (strcmp(command, "reg") == 0){
    error = reg(context, str);
  }else if (strcmp(command, "unregister") == 0){
    error = unregister(context, str);
  }else if (strcmp(command, "login") == 0){
    error = login_(context, str);
  }else if (strcmp(command, "logout") == 0){
    error = logout_(context, str);
  }else if (strcmp(command, "showuid") == 0 || strcmp(command, "su") == 0){
    error = showuid(context, str);
  }else if (strcmp(command, "exit") == 0){
    if (is_logged(context->session))
        error = logout_(context, str);
    return 0;
  }else if (strcmp(command, "groups") == 0 || strcmp(command, "gl") == 0){
    error = groups(context, str);
  }else if (strcmp(command, "subscribe") == 0 || strcmp(command, "s") == 0){
    error = subscribe(context, str);
  }else if (strcmp(command, "unsubscribe") == 0 || strcmp(command, "u") == 0){
    error = unsubscribe(context, str);
  }else if (strcmp(command, "my_groups") == 0 || strcmp(command, "mgl") == 0){
    error = my_groups(context, str);
  }else if (strcmp(command, "select") == 0 || strcmp(command, "sag") == 0){
    error = select_(context, str);
  }else if (strcmp(command, "showgid") == 0 || strcmp(command, "sg") == 0){
    error = showgid(context, str);
  }else if (strcmp(command, "ulist") == 0 || strcmp(command, "ul") == 0){
    error = ulist(context, str);
  }else if (strcmp(command, "post") == 0){
    error = post(context, str);
  }else if (strcmp(command, "retrieve") == 0 || strcmp(command, "r") == 0){
    error = retrieve(context, str);
  }else throw_error("Unkown command");

  return error;
}

#define CLEAR(var) var[0] = '\0'
#define DEFAULT(var, str) if (var[0] == '\0') strcpy(var, str)

void parse_args(char *dsip, char *dsport, int argc, char *argv[]){
  DEBUG_MSG_SECTION("ARGS");
  ASSERT(argc % 2 != 0, "Invalid number of arguments");

  CLEAR(dsip);
  CLEAR(dsport);

  // Parse args
  for (int i = 1; i < argc; i += 2){
    if (strcmp(argv[i], "-n") == 0)
      strcpy(dsip, argv[i+1]);
    else if (strcmp(argv[i], "-p") == 0)
      strcpy(dsport, argv[i+1]);
    else throw_error("Unkown command line argument");
  }

  DEFAULT(dsip, DEFAULT_DSIP);
  DEFAULT(dsport, DEFAULT_DSPORT);

  DEBUG_MSG("dsip set to: %s\n", dsip);
  DEBUG_MSG("dsport set to: %s\n", dsport);
}

int main(int argc, char *argv[]){
  DEBUG_MSG_SECTION("MAIN");
  
  char dsip[BUFFER_SIZE], dsport[BUFFER_SIZE];
  parse_args(dsip, dsport, argc, argv); // Recebe command line arguments e guarda em dsip e dsport

  connection_context_t *context = init_connection(dsip, dsport); // Inicializa conecção e guarda informação em context

  char input_buffer[BUFFER_SIZE];

  int keep_prompt;
  do{
    DEBUG_MSG_SECTION("INPT");
    DEBUG_MSG("Awaiting input...\n");

    get_line(input_buffer, stdin);

    keep_prompt = parse_input(context, input_buffer);
  }while(keep_prompt);

  close_connection(&context);
  DEBUG_MSG_SECTION("MAIN");
  DEBUG_MSG("Client Closed\n");
}
