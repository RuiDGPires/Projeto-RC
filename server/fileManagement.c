#include "fileManagement.h"
#include <string.h>

#define SERVER_DIRECTORY_NAME "SERVER"
#define SERVER_GROUPS_NAME "GROUPS"
#define SERVER_USER_NAME "USERS"

/* -SERVER
        -USERS
            -(users folders)
        -GROUPS
            -(groups folders) */
char *init_filesystem(char *path){
    //TODO Dont' Close if dir already created, just reset them?
    DEBUG_MSG_SECTION("FSYS");

    DEBUG_MSG("Initiating file system\n");

    char *server_path = (char*) malloc(sizeof(char)*(sizeof(path)+6));

    sprintf(server_path,"%s/SERVER", path);
    if(mkdir(server_path, 0700) == -1) exit(1); //error
    DEBUG_MSG("Server Main Directory Created (%s)\n", server_path);

    create_directory(server_path, "USERS");
    create_directory(server_path, "GROUPS");

    return server_path;
}

/* Deletes everything */
void delete_filesystem(char **path){
    DEBUG_MSG_SECTION("FSYS");
    
    close_directory(*path);
    free(*path);
    *path = NULL;
}

void create_directory(char *path, char *name){
    DEBUG_MSG_SECTION("FSYS");

    char *dir_path = (char*) malloc(sizeof(char)*(sizeof(path)+sizeof(name)));
    sprintf(dir_path,"%s/%s", path, name);
    if(mkdir(dir_path, 0700) == -1) exit(1); //error
    DEBUG_MSG("%s Directory Created (%s)\n", name, dir_path);

    free(dir_path);
}

/* Closes a directory recursively */
void close_directory(char *path){
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
                file_path = (char*) malloc (sizeof(char) * (sizeof(path) + sizeof(dir->d_name)));
                sprintf(file_path, "%s/%s", path, dir->d_name);
                close_directory(file_path);

                free(file_path);
                continue;
            }
        
            file_path = (char*) malloc (sizeof(char) * (sizeof(path) + sizeof(dir->d_name)));
            sprintf(file_path, "%s/%s", path, dir->d_name);
            ASSERT(remove(file_path) != -1, "Couldn't remove %s", file_path); 

            DEBUG_MSG("%s closed\n", file_path);

            free(file_path);
        }
    }
    ASSERT(rmdir(path) != -1, "Couldn't remove %s", path);

    DEBUG_MSG("%s closed\n", path);
}

void create_file(char *path, char* name){

    DEBUG_MSG_SECTION("FSYS");

    char *file_path = (char*) malloc(sizeof(char)*(sizeof(path)+sizeof(name)));
    sprintf(file_path,"%s/%s", path, name);
    //create file?????????????????????????????? help
    DEBUG_MSG("%s File Created (%s)\n", name, file_path);

    free(file_path);
}

