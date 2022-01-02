#include "fileManagement.h"
#include <string.h>

char *server_path;
/* -SERVER
        -USERS
            -(users folders)
        -GROUPS
            -(groups folders) */
void init_fileSystem(char *path){
    //TODO Dont' Close if dir already created, just reset them?
    DEBUG_MSG_SECTION("FSYS");

    DEBUG_MSG("Initiating file system\n");

    server_path = (char*) malloc(sizeof(char)*(sizeof(path)+6));
    sprintf(server_path,"%s/SERVER", path);
    if(mkdir(server_path, 0700) == -1) exit(1); //error
    DEBUG_MSG("Server Main Directory Created (%s)\n", server_path);

    create_directory(server_path, "USERS");
    create_directory(server_path, "GROUPS");
}


/* Deletes everything */
void close_fileSystem(){
    DEBUG_MSG_SECTION("FSYS");
    
    close_directory(server_path);
}


void create_directory(char *path, char *name){
    DEBUG_MSG_SECTION("FSYS");

    char *dir_path = (char*) malloc(sizeof(char)*(sizeof(path)+sizeof(name)));
    sprintf(dir_path,"%s/%s", server_path, name);
    if(mkdir(dir_path, 0700) == -1) exit(1); //error
    DEBUG_MSG("%s Directory Created (%s)\n", name, dir_path);

    free(dir_path);
}


/* Closes a directory recursively */
void close_directory(char *path){
    DEBUG_MSG_SECTION("FSYS");

    DIR *d;
    struct dirent *dir;
    char *filePath;

    d = opendir(path);

    if(d){
        while((dir = readdir(d)) != NULL){
            if(dir->d_name[0] == '.') continue;

            if (dir->d_type == DT_DIR ){
                //DIR
                filePath = (char*) malloc (sizeof(char) * (sizeof(path) + sizeof(dir->d_name)));
                sprintf(filePath, "%s/%s", path, dir->d_name);
                close_directory(filePath);

                free(filePath);
                continue;
            }
        
            filePath = (char*) malloc (sizeof(char) * (sizeof(path) + sizeof(dir->d_name)));
            sprintf(filePath, "%s/%s", path, dir->d_name);
            if(remove(filePath) == -1) exit(1); //error

            DEBUG_MSG("%s closed\n", filePath);

            free(filePath);
        }
    }
    if(rmdir(path) == -1) exit(1); //error

    DEBUG_MSG("%s closed\n", path);

    if(strlen(path) == strlen(server_path)){
        free(server_path);
    }
}

void create_file(char* name, char *path){

    DEBUG_MSG_SECTION("FSYS");

    char *file_path = (char*) malloc(sizeof(char)*(sizeof(path)+sizeof(name)));
    sprintf(file_path,"%s/%s", server_path, name);
    //create file?????????????????????????????? help
    DEBUG_MSG("%s File Created (%s)\n", name, file_path);

    free(file_path);
}

