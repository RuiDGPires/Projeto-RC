#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "util.h"

//-------------------------------
// ENUM GENERATION

#define FOREACH_SERVER_COMMAND(SERVER_COMMAND) \
        SERVER_COMMAND(REG)   \
        SERVER_COMMAND(RRG)   \
        SERVER_COMMAND(UNR)   \
        SERVER_COMMAND(RUN)   \
        SERVER_COMMAND(LOG)   \
        SERVER_COMMAND(RLO)   \
        SERVER_COMMAND(OUT)   \
        SERVER_COMMAND(ROU)   \
        SERVER_COMMAND(GLS)   \
        SERVER_COMMAND(RGL)   \
        SERVER_COMMAND(GSR)   \
        SERVER_COMMAND(RGS)   \
        SERVER_COMMAND(GUR)   \
        SERVER_COMMAND(RGU)   \
        SERVER_COMMAND(GLM)   \
        SERVER_COMMAND(RGM)   

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum {
    FOREACH_SERVER_COMMAND(GENERATE_ENUM)
};

static const char *command_str[] = {
    FOREACH_SERVER_COMMAND(GENERATE_STRING)
};

//-------------------------------

#define DEFAULT_DSIP "localhost"
#define DEFAULT_DSPORT "58065" // 58000 + GN
#define BUFFER_SIZE 128

typedef struct {
  char dsip[BUFFER_SIZE], dsport[BUFFER_SIZE];

  int fd;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints, *res;
  struct sockaddr_in addr;
} Context;


size_t get_line(char buffer[], FILE *stream){
  size_t i = 0;
  char c;

  do{
    c = getc(stream);
    if (i == BUFFER_SIZE - 2) {
      printf("Line size exceeds buffer capacity");
      exit(1);
    }
    buffer[i++] = c;
  }while(c != '\n');

  buffer[i++] = '\n';
  buffer[i] = '\0';

  return i;
}

#include <ctype.h>

bool check_uid(const char str[]){
  size_t size = strlen(str);
  ASSERT(size == 5, "Invalid user name size");
  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i])) return FALSE;
}

bool check_pass(const char str[]){
  size_t size = strlen(str);
  ASSERT(size == 5, "Invalid user name size");
  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i]) && !isalpha(str[i])) return FALSE;
}

bool check_gid(const char str[]){

}

bool parse_input (Context *context, char str[]){
  char command_buffer[BUFFER_SIZE];

  sscanf(str, "%[^ ]", command_buffer);

  size_t size = strlen(command_buffer);
  str = &(str[size-1]);

  if (command_buffer[size-1] == '\n') {
    command_buffer[size-2] = '\0';
  }else {
    command_buffer[size-1] = '\0';
  }

  if (strcmp(command_buffer, "reg") == 0){

    return 1;
  }else if (strcmp(command_buffer, "unregister") == 0){
  }else if (strcmp(command_buffer, "login") == 0){
  }else if (strcmp(command_buffer, "logout") == 0){
  }else if (strcmp(command_buffer, "showuid") == 0 || strcmp(command_buffer, "su") == 0){
  }else if (strcmp(command_buffer, "exit") == 0){
    return 0;
  }else if (strcmp(command_buffer, "groups") == 0 || strcmp(command_buffer, "gl") == 0){
  }else if (strcmp(command_buffer, "subscribe") == 0 || strcmp(command_buffer, "s") == 0){
  }else if (strcmp(command_buffer, "unsubscribe") == 0 || strcmp(command_buffer, "u") == 0){
  }else if (strcmp(command_buffer, "my_groups") == 0 || strcmp(command_buffer, "mgl") == 0){
  }else if (strcmp(command_buffer, "select") == 0 || strcmp(command_buffer, "sag") == 0){
  }else if (strcmp(command_buffer, "showgid") == 0 || strcmp(command_buffer, "sg") == 0){
  }else if (strcmp(command_buffer, "ulist") == 0 || strcmp(command_buffer, "ul") == 0){
  }else if (strcmp(command_buffer, "post") == 0){
  }else if (strcmp(command_buffer, "retrieve") == 0 || strcmp(command_buffer, "r") == 0){
  }else THROW_ERROR("Unkown command");
}

Context *init_connection(const char dsip[], const char dsport[]){
  Context *context = (Context *) malloc(sizeof(Context));
  
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

void close_connection(Context **context){
  freeaddrinfo((*context)->res);
  close((*context)->fd);

  free(*context);
  *context = NULL;
}

void send_message(Context *context, const char message[], char response[]){
  context->addrlen = sizeof(context->addr);
  size_t size = strlen(message), n;

  n = sendto(context->fd, message, size, 0, context->res->ai_addr, context->res->ai_addrlen);
  ASSERT(n != -1, "Unable to send message");
  DEBUG_MSG_SECTION("UDP");
  DEBUG_MSG("Message sent: %s", message);

  DEBUG_MSG("Awaiting response...\n");
  n = recvfrom(context->fd, response, BUFFER_SIZE, 0, (struct sockaddr*) &context->addr, &context->addrlen);
  ASSERT(n != -1, "Unable to receive message");
  response[n] = '\0';
  DEBUG_MSG("Message received: %s\n", response);
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
    else THROW_ERROR("Unkown command line argument");
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

  Context *context = init_connection(dsip, dsport); // Inicializa conecção e guarda informação em context

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
