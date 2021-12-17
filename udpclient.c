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
} Connection_context;

typedef struct {
	bool is_logged;
	char uid[BUFFER_SIZE], pass[BUFFER_SIZE];
} Login_Context;


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

void send_message(Connection_context *context, const char message[], char response[]){
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

#include <ctype.h>

void check_uid(const char str[]){
  size_t size = strlen(str);
  ASSERT(size == 5, "Invalid user name size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i])) THROW_ERROR("Invalid user name characters");
}

void check_pass(const char str[]){
  size_t size = strlen(str);
  ASSERT(size == 8, "Invalid password size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i]) && !isalpha(str[i])) THROW_ERROR("Invalid password characters");
}

void check_gid(const char str[]){
	size_t size = strlen(str);
	ASSERT(size == 2, "Group number");
	
	for (size_t i = 0; i < size; i++)
		if (!isdigit(str[i])) THROW_ERROR("Invalid group id chars");
}

void check_gname(const char str[]){
	size_t size = strlen(str);
	ASSERT(size == 2, "Group number");
	
	for (size_t i = 0; i < size; i++)
		if (!isdigit(str[i]) && !isalpha(str[i]) && str[i] != '-' && str[i] != '_') THROW_ERROR("Invalid group id chars");
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

bool parse_input (Connection_context *context, Login_Context *login_context, char str[]){
	char *command = get_word(&str);

  if (strcmp(command, "reg") == 0){
		char buffer[BUFFER_SIZE];
		char *uid, *pass;
		uid = get_word(&str);
		pass = get_word(&str);

		check_uid(uid);
		check_pass(pass);

		sscanf(buffer, "%s %s %s", "REG", uid, pass);

		send_message(context, buffer, buffer);
		DEBUG_MSG("Response: %s\n", buffer);
    return 1;
  }else if (strcmp(command, "unregister") == 0){
		char buffer[BUFFER_SIZE];
		char *uid, *pass;
		uid = get_word(&str);
		pass = get_word(&str);

		check_uid(uid);
		check_pass(pass);

		sscanf(buffer, "%s %s %s", "UNR", uid, pass);

		send_message(context, buffer, buffer);
		DEBUG_MSG("Response: %s\n", buffer);
    return 1;
  }else if (strcmp(command, "login") == 0){
		char buffer[BUFFER_SIZE];
		char *uid, *pass;
		uid = get_word(&str);
		pass = get_word(&str);

		check_uid(uid);
		check_pass(pass);

		sscanf(buffer, "%s %s %s", "UNR", uid, pass);

		login_context->is_logged = TRUE;
		strcpy(login_context->uid, uid);
		strcpy(login_context->pass, pass);

		send_message(context, buffer, buffer);
		DEBUG_MSG("Response: %s\n", buffer);
		return 1;
  }else if (strcmp(command, "logout") == 0){
		char buffer[BUFFER_SIZE];

		sscanf(buffer, "%s %s %s", "UNR", login_context->uid, login_context->pass);

		send_message(context, buffer, buffer);
		login_context->is_logged = FALSE;
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
  }else THROW_ERROR("Unkown command");
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

  Connection_context *context = init_connection(dsip, dsport); // Inicializa conecção e guarda informação em context

	Login_Context login_context = (Login_Context) {.is_logged = FALSE};

  char input_buffer[BUFFER_SIZE];

  int keep_prompt;
  do{
    DEBUG_MSG_SECTION("INPT");
    DEBUG_MSG("Awaiting input...\n");

    size_t size = get_line(input_buffer, stdin);

    keep_prompt = parse_input(context, &login_context, input_buffer);
  }while(keep_prompt);

  close_connection(&context);
}
