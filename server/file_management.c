#include "../common/debug.h"
#include "file_management.h"
#include "strlinkedlist.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Creates filesystem.
 * SERVER(dir)
 *  |_USERS(dir)
 *  |_GROUPS(dir)
 */
char *create_filesystem(char *path){
    //TODO Dont' Close if dir already created, just reset them?
    DEBUG_MSG_SECTION("FSYS");

    DEBUG_MSG("Initiating file system\n");

    char *server_path = (char*) malloc(sizeof(char)*(strlen(path)+strlen(SERVER_DIRECTORY_NAME) + 2));

    sprintf(server_path,"%s/%s", path, SERVER_DIRECTORY_NAME);

    if (!(directory_exists(server_path))){
        if(mkdir(server_path, 0700) == -1){
            throw_error("Unable to create main directory"); //error
            exit(1);
        }

      DEBUG_MSG("Server Main Directory created (%s)\n", server_path);

      if( create_directory(server_path, SERVER_USERS_NAME) == FERROR) exit(1);
      if( create_directory(server_path, SERVER_GROUPS_NAME) == FERROR) exit(1);
    }else{
      DEBUG_MSG("Server Main Directory already exists (%s)\n", server_path);
    }

    return server_path;
}

/*
 * Deletes filesystem if needed.
 */
void destroy_filesystem(char **path){
    DEBUG_MSG_SECTION("FSYS");
  
    delete_directory(*path);
    free(*path);
    *path = NULL;
}

/*
 * Creates a directory given by the path in the first argument.
 * Directory is created with the name.
 */
int create_directory(char *path, char *name){
    DEBUG_MSG_SECTION("FSYS");

    char *dir_path = (char*) malloc(sizeof(char)*(strlen(path)+strlen(name)) + 2);
    sprintf(dir_path,"%s/%s", path, name);
    DEBUG_MSG("Creating directory %s\n", dir_path);
    ASSERT(mkdir(dir_path, 0700) != -1, FERROR, "Error while creating directory"); //error
    DEBUG_MSG("Directory Created %s\n", dir_path);

    free(dir_path);
    return SUCCESS;
}

/*
 * Deletes the directory given by the path in the first argument.
 * Everything inside the directory is removed aswell, recursevily.
 */
int delete_directory(char *path){
    DEBUG_MSG_SECTION("FSYS");

    DIR *d;
    struct dirent *dir;
    char *file_path;

    d = opendir(path);

    if(d){
        while((dir = readdir(d)) != NULL){
            if(dir->d_name[0] == '.') continue;

            if (dir->d_type == DT_DIR ){
                //DIR
                file_path = (char*) malloc (sizeof(char) * (strlen(path) + strlen(dir->d_name) + 2));
                sprintf(file_path, "%s/%s", path, dir->d_name);
                ASSERT(delete_directory(file_path) != FERROR, FERROR, "Error while deleting directory");

                free(file_path);
                continue;
            }
        
            file_path = (char*) malloc (sizeof(char) * (strlen(path) + strlen(dir->d_name) + 2));
            sprintf(file_path, "%s/%s", path, dir->d_name);
            ASSERT(remove(file_path) != -1, FERROR, "Couldn't remove %s", file_path); 

            DEBUG_MSG("%s closed\n", file_path);

            free(file_path);
        }
    }
    ASSERT(rmdir(path) != -1, FERROR, "Couldn't remove %s", path);

    DEBUG_MSG("%s closed\n", path);

    free(d);

    return SUCCESS;
}

/*
 * Checks if directory given by the path in the first argument exist
 */
bool directory_exists(char *path){
  struct stat sb;

  return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

/*
 * Sorts the subdirectories of the directory given by the path in the argument
 * Returns:
 * pointer to list of subdirectories sorted
 */
sll_link_t list_subdirectories_ord(char *path){
    DIR *d;
    struct dirent *dir;
    
    sll_link_t dir_list = sll_create();

    d = opendir(path);

    if(d){
        while((dir = readdir(d)) != NULL){
            if(dir->d_name[0] == '.') continue;

            if (dir->d_type == DT_DIR)
                sll_append_ord(&dir_list, dir->d_name, strcmp);
        }
    }

    free(d);

    return dir_list;
}

/*
 * List the subdirectories of the directory given by the path in the argument
 * Returns:
 * pointer to list of subdirectories
 */
sll_link_t list_subdirectories(char *path){
    DIR *d;
    struct dirent *dir;
    
    sll_link_t dir_list = sll_create();

    d = opendir(path);

    if(d){
        while((dir = readdir(d)) != NULL){
            if(dir->d_name[0] == '.') continue;

            if (dir->d_type == DT_DIR)
                sll_push(&dir_list, dir->d_name);
        }
    }

    free(d);

    return dir_list;
}

/*
 * Creates file in the directory given by the path in the first arg, with the name given by the
 * second arg with the data listed in the third one
 */
int create_file(char *path, char *name, char *data){
    DEBUG_MSG_SECTION("FSYS");

    char *file_path = (char*) malloc(sizeof(char)*(strlen(path)+strlen(name) + 2));
    sprintf(file_path,"%s/%s", path, name);

    FILE *file = fopen(file_path, "w");
    ASSERT(file != NULL,FERROR, "Unable to open file");

    if (data != NULL)
        fprintf(file, "%s", data);

    fclose(file);

    DEBUG_MSG("%s File Created (%s)\n", name, file_path);

    free(file_path);

    return SUCCESS;
}

/*
 * Deletes file with the name of the second arg in the directory given by the path in the first arg
 */
int delete_file(char *path, char *name){
    DEBUG_MSG_SECTION("FSYS");

    char *file_path = (char*) malloc(sizeof(char)*(strlen(path)+strlen(name) + 2));
    sprintf(file_path,"%s/%s", path, name);

    ASSERT(remove(file_path) == 0, FERROR, "Unable to delete file");
    DEBUG_MSG("File deleted\n");

    free(file_path);

    return SUCCESS;
}

/*
 * Checks if file with the name of the second arg exists in the directory given by the path in the first arg
 * Returns:
 * true if file exists
 * false if file doesn't exist
 */
bool file_exists(char *path, char *name){
    DEBUG_MSG_SECTION("FSYS");

    char *fname = (char*) malloc ( sizeof(char)*(strlen(path) + strlen(name) + 2));
    sprintf(fname, "%s/%s", path, name);
    
    //bool ret = stat(real_path, &sb) == 0 && S_ISREG(sb.st_mode);

    bool ret =  access(fname, F_OK ) == 0;
    DEBUG_MSG("Does %s exist? %s\n", fname, ret? "Yes": "No");

    free(fname);
    return ret;
}

/*
 * List the files that exist in the directory given by the path in the argument
 * Returns:
 * pointer to list of files
 */
sll_link_t list_files(char *path){
    DIR *d;
    struct dirent *dir;
    
    sll_link_t file_list = sll_create();

    d = opendir(path);

    if(d){
        while((dir = readdir(d)) != NULL){
            if (dir->d_type != DT_DIR )
                sll_append(&file_list, dir->d_name);
        }
    }

    free(d);

    return file_list;

}
