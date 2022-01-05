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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../../common/util.h"
#include "../../common/debug.h"
#include "../../common/libio.h"
#include "connection.h"
//#include "commands.h"

#define DEFAULT_DSIP "localhost"
#define DEFAULT_DSPORT "58065" // 58000 + GN
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
  char buffer_tcp[BUFFER_SIZE];

  int keep_prompt;
  if(fork() == 0){
    /* do{
      //size_t size = get_line(input_buffer, stdin);
      sprintf(input_buffer, "Fuck You Mark %d\n", getpid());
      send_udp_message(context,input_buffer,input_buffer);
      DEBUG_MSG("%s\n", input_buffer);
    }while(keep_prompt); */
  }
  else{
    do{
      //size_t size = get_line(input_buffer, stdin);
      sprintf(buffer_tcp, "Bless You John %d\n", getpid());
      send_tcp_message(context,buffer_tcp,buffer_tcp);
      DEBUG_MSG("%s\n", buffer_tcp);
    }while(keep_prompt);
  }

  close_connection(&context);
}
