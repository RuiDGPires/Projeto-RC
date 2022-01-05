#include "file_management.h"
#include "strlinkedlist.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

/* -SERVER
        -USERS
            -(users folders)
        -GROUPS
            -(groups folders) */
char *create_filesystem(char *path){
    //TODO Dont' Close if dir already created, just reset them?
    DEBUG_MSG_SECTION("FSYS");

    DEBUG_MSG("Initiating file system\n");

    char *server_path = (char*) malloc(sizeof(char)*(strlen(path)+strlen(SERVER_DIRECTORY_NAME) + 2));

    sprintf(server_path,"%s/%s", path, SERVER_DIRECTORY_NAME);

    if (!(directory_exists(server_path))){
      ASSERT(mkdir(server_path, 0700) != -1, "Unable to create main directory"); //error
      DEBUG_MSG("Server Main Directory created (%s)\n", server_path);

      create_directory(server_path, SERVER_USERS_NAME);
      create_directory(server_path, SERVER_GROUPS_NAME);
    }else{
      DEBUG_MSG("Server Main Directory already exists (%s)\n", server_path);
    }

    return server_path;
}

/* Deletes everything */
void destroy_filesystem(char **path){
    DEBUG_MSG_SECTION("FSYS");
  
    delete_directory(*path);
    free(*path);
    *path = NULL;
}

void create_directory(char *path, char *name){
    DEBUG_MSG_SECTION("FSYS");

    char *dir_path = (char*) malloc(sizeof(char)*(strlen(path)+strlen(name)) + 2);
    sprintf(dir_path,"%s/%s", path, name);
    if(mkdir(dir_path, 0700) == -1) exit(1); //error
    DEBUG_MSG("Directory Created %s\n", dir_path);

    free(dir_path);
}

/* Deletes a directory recursively */
void delete_directory(char *path){
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
                delete_directory(file_path);

                free(file_path);
                continue;
            }
        
            file_path = (char*) malloc (sizeof(char) * (strlen(path) + strlen(dir->d_name) + 2));
            sprintf(file_path, "%s/%s", path, dir->d_name);
            ASSERT(remove(file_path) != -1, "Couldn't remove %s", file_path); 

            DEBUG_MSG("%s closed\n", file_path);

            free(file_path);
        }
    }
    ASSERT(rmdir(path) != -1, "Couldn't remove %s", path);

    DEBUG_MSG("%s closed\n", path);
}

bool directory_exists(char *path){
  struct stat sb;

  return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

sll_link_t list_subdirectories(char *path){
    DIR *d;
    struct dirent *dir;
    char *subdir_path;
    
    sll_link_t dir_list = sll_create();

    d = opendir(path);

    if(d){
        while((dir = readdir(d)) != NULL){
            if(dir->d_name[0] == '.') continue;

            if (dir->d_type == DT_DIR )
                sll_append(&dir_list, dir->d_name);
        }
    }

    return dir_list;
}

void create_file(char *path, char *name, char *data){
    DEBUG_MSG_SECTION("FSYS");

    char *file_path = (char*) malloc(sizeof(char)*(strlen(path)+strlen(name) + 2));
    sprintf(file_path,"%s/%s", path, name);

    FILE *file = fopen(file_path, "w");
    ASSERT(file != NULL, "Unable to open file");

    fprintf(file, "%s", data);

    fclose(file);

    DEBUG_MSG("%s File Created (%s)\n", name, file_path);

    free(file_path);
}

void delete_file(char *path, char *name){
    DEBUG_MSG_SECTION("FSYS");

    char *file_path = (char*) malloc(sizeof(char)*(strlen(path)+strlen(name) + 2));
    sprintf(file_path,"%s/%s", path, name);

    ASSERT(remove(file_path) == 0, "Unable to delete file");
}

bool file_exists(char *path, char *name){
    DEBUG_MSG_SECTION("FSYS");

    struct stat sb;
    char *fname = (char*) malloc ( sizeof(char)*(strlen(path) + strlen(name) + 2));
    sprintf(fname, "%s/%s", path, name);
    

    bool ret =  stat(fname, &sb) == 0 && S_ISREG(sb.st_mode);
    free(fname);
    return ret;
}
