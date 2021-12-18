#include <string.h>
#include <ctype.h>

#include "util.h"
#include "debug.h"
#include "libio.h"
#include "connection.h"

#define DEFAULT_DSIP "localhost"
#define DEFAULT_DSPORT "58065" // 58000 + GN

size_t get_line(char buffer[], FILE *stream){
  (void) fscanf(stream, "%[^\n]", buffer);
  (void) getc(stdin);
  return strlen(buffer);
}

void check_uid(const char str[]){
  size_t size = strlen(str);
  ASSERT(size == 5, "Invalid user name size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i])) throw_error("Invalid user name characters");
}

void check_pass(const char str[]){
  size_t size = strlen(str);
  ASSERT(size == 8, "Invalid password size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i]) && !isalpha(str[i])) throw_error("Invalid password characters");
}

void check_gid(const char str[]){
  size_t size = strlen(str);
  ASSERT(size == 2, "Group number");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i])) throw_error("Invalid group id chars");
}

void check_gname(const char str[]){
  size_t size = strlen(str);
  ASSERT(size == 2, "Group number");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i]) && !isalpha(str[i]) && str[i] != '-' && str[i] != '_') throw_error("Invalid group id chars");
}

char *get_word(char *str[]){
  DEBUG_MSG_SECTION("WRD");
  char command_buffer[BUFFER_SIZE];
  sscanf(*str, "%[^ ]", command_buffer);

  size_t size = strlen(command_buffer);

  (*str)[size-((*str)[size-1] == '\n')*2] = '\0';

  char *ret = *str;
  *str = &((*str)[size+1]);

  return ret;
}

bool parse_input (connection_context_t *context, char str[]){
  char *command = get_word(&str);

  if (strcmp(command, "reg") == 0){
    char buffer[BUFFER_SIZE];
    char *uid, *pass;
    uid = get_word(&str);
    pass = get_word(&str);

    check_uid(uid);
    check_pass(pass);

    sprintf(buffer, "%s %s %s\n", "REG", uid, pass);

    send_message(context, buffer, buffer);
    return 1;
  }else if (strcmp(command, "unregister") == 0){
    char buffer[BUFFER_SIZE];
    char *uid, *pass;
    uid = get_word(&str);
    pass = get_word(&str);

    check_uid(uid);
    check_pass(pass);

    sprintf(buffer, "%s %s %s\n", "UNR", uid, pass);

    send_message(context, buffer, buffer);
    return 1;
  }else if (strcmp(command, "login") == 0){
    char buffer[BUFFER_SIZE];
    char *uid, *pass;
    uid = get_word(&str);
    pass = get_word(&str);

    check_uid(uid);
    check_pass(pass);

    sprintf(buffer, "%s %s %s\n", "LOG", uid, pass);
    
    session_context_t *session = context->session;

    session->is_logged = 1;
    strcpy(session->uid, uid);
    strcpy(session->pass, pass);

    send_message(context, buffer, buffer);
    return 1;
  }else if (strcmp(command, "logout") == 0){
    char buffer[BUFFER_SIZE];

    session_context_t *session = context->session;
    sprintf(buffer, "%s %s %s\n", "OUT", session->uid, session->pass);

    send_message(context, buffer, buffer);
    session->is_logged = 0;
  }else if (strcmp(command, "showuid") == 0 || strcmp(command, "su") == 0){
  }else if (strcmp(command, "exit") == 0){
    return 0;
  }else if (strcmp(command, "groups") == 0 || strcmp(command, "gl") == 0){
  }else if (strcmp(command, "subscribe") == 0 || strcmp(command, "s") == 0){
  }else if (strcmp(command, "unsubscribe") == 0 || strcmp(command, "u") == 0){
  }else if (strcmp(command, "my_groups") == 0 || strcmp(command, "mgl") == 0){
  }else if (strcmp(command, "select") == 0 || strcmp(command, "sag") == 0){
  }else if (strcmp(command, "showgid") == 0 || strcmp(command, "sg") == 0){
  }else if (strcmp(command, "ulist") == 0 || strcmp(command, "ul") == 0){
  }else if (strcmp(command, "post") == 0){
  }else if (strcmp(command, "retrieve") == 0 || strcmp(command, "r") == 0){
  }else throw_error("Unkown command");
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

    size_t size = get_line(input_buffer, stdin);

    keep_prompt = parse_input(context, input_buffer);
  }while(keep_prompt);

  close_connection(&context);
}
