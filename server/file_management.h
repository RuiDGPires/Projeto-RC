#ifndef FILE_MANAGEMENT_H__
#define FILE_MANAGEMENT_H__

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "../common/util.h"
#include "../common/debug.h"

char *create_filesystem(char *);
void destroy_filesystem(char **);

void create_directory(char *, char *);
void delete_directory(char *);


#endif
