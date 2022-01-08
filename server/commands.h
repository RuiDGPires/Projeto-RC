#ifndef COMMANDS_H__
#define COMMANDS_H__

#include "connection.h"

// UDP
void reg(connection_context_t *, char *, char *);
void unregister(connection_context_t *, char *, char *);
void login_(connection_context_t *, char *, char *);
void logout_(connection_context_t *, char *, char *);
void groups(connection_context_t *, char *, char *);
void subscribe(connection_context_t *, char *, char *);
void unsubscribe(connection_context_t *, char *, char *);
void my_groups(connection_context_t *, char *, char *);

// TCP
void ulist(connection_context_t *, char *);
void post(connection_context_t *, char *);
void retrieve(connection_context_t *, char *);

#endif
