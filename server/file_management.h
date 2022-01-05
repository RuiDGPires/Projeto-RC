#ifndef FILE_MANAGEMENT_H__
#define FILE_MANAGEMENT_H__

#define SERVER_DIRECTORY_NAME "SERVER"
#define SERVER_GROUPS_NAME "GROUPS"
#define SERVER_USERS_NAME "USERS"

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "../common/util.h"
#include "../common/debug.h"
#include "strlinkedlist.h"

char *create_filesystem(char *);
void destroy_filesystem(char **);

void create_directory(char *, char *);
#define create_directory_abs(dir) create_directory(dir, "/")

sll_link_t list_subdirectories(char *);
void delete_directory(char *);
bool directory_exists(char *);

void create_file(char *path, char *name, char *data);
void delete_file(char *path, char *name);
bool file_exists(char *, char *);
#endif
