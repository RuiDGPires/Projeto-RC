#ifndef COMMANDS_H__
#define COMMANDS_H__

#include "connection.h"

int reg(connection_context_t *, char *);
int unregister(connection_context_t *, char *);
int login_(connection_context_t *, char *);
int logout_(connection_context_t *, char *);
int showuid(connection_context_t *, char *);
int groups(connection_context_t *, char *);
int subscribe(connection_context_t *, char *);
int unsubscribe(connection_context_t *, char *);
int my_groups(connection_context_t *, char *);
int select_(connection_context_t *, char *);
int showgid(connection_context_t *, char *);
int ulist(connection_context_t *, char *);
int post(connection_context_t *, char *);
int retrieve(connection_context_t *, char *);

#endif
