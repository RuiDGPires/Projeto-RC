#ifndef FILE_MANAGEMENT_H__
#define FILE_MANAGEMENT_H__

#define SERVER_DIRECTORY_NAME "SERVER"
#define SERVER_GROUPS_NAME "GROUPS"
#define SERVER_USERS_NAME "USERS"

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "../common/util.h"
#include "strlinkedlist.h"

char *create_filesystem(char *);
void destroy_filesystem(char **);

int create_directory(char *, char *);
#define create_directory_abs(dir) create_directory(dir, "/")

sll_link_t list_subdirectories(char *);
sll_link_t list_subdirectories_ord(char *);
int delete_directory(char *);
bool directory_exists(char *);

int create_file(char *path, char *name, char *data);
int delete_file(char *path, char *name);
bool file_exists(char *, char *);
sll_link_t list_files(char *);
#endif
