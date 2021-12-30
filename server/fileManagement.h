#ifndef FILEMANAGEMENT_H__
#define FILEMANAGEMENT_H__

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "../common/util.h"
#include "../common/debug.h"

void init_fileSystem(char *);
void close_fileSystem();

void create_directory(char *, char *);
void close_directory(char *);


#endif