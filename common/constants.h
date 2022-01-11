//client.c
#define DEFAULT_DSIP "localhost"
#define DEFAULT_DSPORT "58065" // 58000 + GN

//client/commands.c
//server/commands.c
#define SUCCESS 1
#define WARNING 1
#define FERROR 0

//util.h
#define BUFFER_SIZE 128
#define UID_SIZE 6
#define PASS_SIZE 9
#define GID_SIZE 3
#define MID_SIZE 5

//util.h
#define TRUE 1
#define FALSE 0

//libio.h
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define NC "\033[0m" 

//server.c
#define CLEAR(var) var[0] = '\0'
#define DEFAULT(var, str) if (var[0] == '\0') strcpy(var, str)
#define DEFAULT_DSPORT "58065" // 58000 + GN

//connection.h
#define PORT_SIZE 6
#define BACKLOG_NUM 99

//server/commands.c
#define TSIZE_SIZE 240
#define FNAME_SIZE 24
#define FSIZE_SIZE 24