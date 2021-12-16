#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "util.h"

#define DEFAULT_DSIP "localhost"
#define DEFAULT_DSPORT "58065" // 58000 + GN
#define BUFFER_SIZE 128

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
  char dsip[BUFFER_SIZE], dsport[BUFFER_SIZE];

  parse_args(dsip, dsport, argc, argv);
  DEBUG_MSG_SECTION("MAIN");

  int fd, errcode;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints, *res;
  struct sockaddr_in addr;
  char buffer[BUFFER_SIZE];

  char commandLineBuffer[BUFFER_SIZE];
  char nLine = '\n';

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  ASSERT(fd != -1, "Unable to create socket");
  DEBUG_MSG("Socket created\n");

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  errcode = getaddrinfo(dsip, dsport, &hints, &res);
  ASSERT(errcode == 0, "Unable to get address info");
  DEBUG_MSG("Got address info\n");

  do{
    DEBUG_MSG_SECTION("INPT");
    DEBUG_MSG("Awaiting input...\n");
    size_t size = get_line(commandLineBuffer, stdin);

    n = sendto(fd, commandLineBuffer, size, 0, res->ai_addr, res->ai_addrlen);
    ASSERT(n != -1, "Unable to send message");
    DEBUG_MSG_SECTION("UDP");
    DEBUG_MSG("Message sent: %s", commandLineBuffer);

    DEBUG_MSG("Awaiting response...\n");
    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
    if(n ==-1) /*error*/ THROW_ERROR("");
    buffer[n] = '\0';
    DEBUG_MSG("Message received: %s\n", buffer);
  }while(1);

  freeaddrinfo(res);
  close(fd);
  exit(0);
}
