#ifndef COMMANDS_H__
#define COMMANDS_H__

#include "connection.h"

void reg(connection_context_t *, char *, char *);
void unregister(connection_context_t *, char *, char *);
void login_(connection_context_t *, char *, char *);
void logout_(connection_context_t *, char *, char *);
void showuid(connection_context_t *, char *, char *);
void groups(connection_context_t *, char *, char *);
void subscribe(connection_context_t *, char *, char *);
void unsubscribe(connection_context_t *, char *, char *);
void my_groups(connection_context_t *, char *, char *);
void select_(connection_context_t *, char *, char *);
void showgid(connection_context_t *, char *, char *);
void ulist(connection_context_t *, char *, char *);
void post(connection_context_t *, char *, char *);
void retrieve(connection_context_t *, char *, char *);

#endif
