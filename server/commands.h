#ifndef COMMANDS_H__
#define COMMANDS_H__

#include "connection.h"

// UDP
char *reg(connection_context_t *, char *, char *);
char *unregister(connection_context_t *, char *, char *);
char *login_(connection_context_t *, char *, char *);
char *logout_(connection_context_t *, char *, char *);
void groups(connection_context_t *, char *, char *);
char *subscribe(connection_context_t *, char *, char *);
char *unsubscribe(connection_context_t *, char *, char *);
char *my_groups(connection_context_t *, char *, char *);

// TCP
char *ulist(connection_context_t *, char *);
char *post(connection_context_t *, char *);
char *retrieve(connection_context_t *, char *);

#endif
