#ifndef COMMANDS_H__
#define COMMANDS_H__
#include "connection.h"

void reg(connection_context_t *, char *);
void unregister(connection_context_t *, char *);
void login_(connection_context_t *, char *);
void logout_(connection_context_t *, char *);
void showuid(connection_context_t *, char *);
void groups(connection_context_t *, char *);
void subscribe(connection_context_t *, char *);
void unsubscribe(connection_context_t *, char *);
void my_groups(connection_context_t *, char *);
void select_(connection_context_t *, char *);
void showgid(connection_context_t *, char *);
void ulist(connection_context_t *, char *);

#endif
