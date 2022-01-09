#include "../common/debug.h"
#include "../common/util.h"
#include "commands.h"
#include "file_management.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#define SUCCESS 1
#define WARNING 1
#define FERROR 0

int check_uid(const char str[]){
  if (str == NULL){
    throw_error("Invalid user name");
    return FERROR;
  }
  size_t size = strlen(str);
  ASSERT(size == 5, "Invalid user name size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i])){
      throw_error("Invalid user name characters");
      return FERROR;
    }
  
  return size == 5;
}

void check_pass(const char str[]){
  if (str == NULL){
    throw_error("Invalid user name");
    return FERROR;
  }
  size_t size = strlen(str);
  ASSERT(size == 8, "Invalid password size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i]) && !isalpha(str[i])){
      throw_error("Invalid password characters");
      return FERROR;
    }
  return size == 8;
}

void check_gid(const char str[]){
  if (str == NULL){
    throw_error("Invalid user name");
    return FERROR;
  }
  size_t size = strlen(str);
  ASSERT(size == 2, "Invalid group number size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i])){
      throw_error("Invalid group id chars");
      return FERROR;
    }
  return size == 2;
}

void check_gname(const char str[]){
  if (str == NULL){
    throw_error("Invalid user name");
    return FERROR;
  }
  size_t size = strlen(str);
  ASSERT(size < 24, "Invalid group name length");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i]) && !isalpha(str[i]) && str[i] != '-' && str[i] != '_'){
      throw_error("Invalid group name chars");
      return FERROR;
    }
  return size < 24;
}

// returns:
//  0 if everything is correct
//  1 if user name does not exist
//  2 if password is wrong
int check_credencials(const char name[], const char pass[], const char fs[]){
    int ret = 0;
    DEBUG_MSG_SECTION("CHK");
    DEBUG_MSG("check credencials");

  char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(name)) + 3);
  sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, name);

  if (!(directory_exists(user_path))){
    ret = 1;
  }else{
    char *pass_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(name)) + strlen("pass.txt") + 4);

    sprintf(pass_path, "%s/%s/%s/pass.txt", fs, SERVER_USERS_NAME, name);

    FILE *file = fopen(pass_path, "r");

    char user_pass[9];
    fscanf(file, "%s", user_pass);
    fclose(file);

    if (strcmp(user_pass, pass) == 0){
      ret = 0;
    }else{
      ret = 2;
    }

    free(pass_path);
  }

  free(user_path);
  return ret;
}

bool is_logged_in(const char name[], const char fs[]){

    char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(name)) + 3);
    sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, name);  

    return (file_exists(user_path, "login"));
}

void reg(connection_context_t *connection, char *args, char *fs){
  char *name = get_word(&args);
  char *pass= get_word(&args);

  char buffer[BUFFER_SIZE];

  char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(name)) + 3);

  sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, name);

  if (directory_exists(user_path)){
    sprintf(buffer, "RRG DUP\n");
  }else{
    create_directory_abs(user_path);
    create_file(user_path, "pass.txt", pass);
    sprintf(buffer, "RRG OK\n");
  }

  send_udp_message(connection, buffer);

  free(user_path);
}

void unregister(connection_context_t *connection, char *args, char *fs){
    char *name = get_word(&args);
    char *pass= get_word(&args);

    char buffer[BUFFER_SIZE];

    char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(name)) + 3);

    sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, name);

    if (check_credencials(name, pass, fs) == 0){
      delete_directory(user_path);

        char *groups_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_GROUPS_NAME) + 2));
        sprintf(groups_path, "%s/%s", fs, SERVER_GROUPS_NAME);

        sll_link_t groups_list = list_subdirectories(groups_path);

        FOR_ITEM_IN_LIST(char *group, groups_list)

            char *group_path = (char *) malloc(sizeof(char) * (strlen(groups_path) + strlen(group) + 2));

            sprintf(group_path, "%s/%s", groups_path, group);

            if (file_exists(group_path, name)){
                delete_file(group_path, name);
            }

            free(group_path);

        END_FIIL()


        sll_destroy(&groups_list);
        free(groups_path);
      sprintf(buffer, "RUN OK\n");
    }else{
      sprintf(buffer, "RUN NOK\n");
    }

    send_udp_message(connection, buffer);
    free(user_path);
}

void login_(connection_context_t *connection, char *args, char *fs){
    char *name = get_word(&args);
    char *pass= get_word(&args);

    char buffer[BUFFER_SIZE];

    char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(name)) + 3);

    sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, name);

    if (check_credencials(name, pass, fs) == 0){
      if(!(is_logged_in(name, fs))){
        create_file(user_path, "login", NULL);
      }
      sprintf(buffer, "RLO OK\n");
      
    }else{
        sprintf(buffer, "RLO NOK\n");
    }

    free(user_path);

    send_udp_message(connection, buffer);
}

void logout_(connection_context_t *connection, char *args, char *fs){
    char *name = get_word(&args);
    char *pass= get_word(&args);

    char buffer[BUFFER_SIZE];

    char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(name)) + 3);

    sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, name);

    if (check_credencials(name, pass, fs) == 0){
        if(is_logged_in(name, fs)){
            delete_file(user_path, "login");
        }
        sprintf(buffer, "ROU OK\n");
    }else{
        sprintf(buffer, "ROU NOK\n");
    }

    send_udp_message(connection, buffer);
}

void groups(connection_context_t *connection, char *args, char *fs){
    (void) args;

    char *groups_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_GROUPS_NAME) + 3));
    sprintf(groups_path, "%s/%s/", fs, SERVER_GROUPS_NAME);

    sll_link_t group_list = list_subdirectories(groups_path);

    // Convert to string so that strlen can be used to measure string size
    char n_str[BUFFER_SIZE];
    sprintf(n_str, "%ld", sll_size(group_list));

    char *msg_buffer = (char *) malloc(sizeof(char)*(4 + strlen(n_str) + sll_size(group_list)*33));

    sprintf(msg_buffer, "RGL %s", n_str);

    FOR_ITEM_IN_LIST(char *group, group_list)
        char *top = &msg_buffer[strlen(msg_buffer)];

        char *group_msgs_path = (char *) malloc(sizeof(char)*(strlen(groups_path) + strlen(group) + 3));

        sprintf(group_msgs_path, "%s/%s/", groups_path, group);
        sll_link_t msg_list = list_subdirectories(group_msgs_path);


        char *name_path = (char *) malloc(sizeof(char) * (strlen(group_msgs_path) + strlen("name.txt") + 3));
        sprintf(name_path, "%s/%s", group_msgs_path, "name.txt");
        FILE *file = fopen(name_path, "r");
        free(name_path);

        char group_name[25];

        fscanf(file, "%s", group_name);
        fclose(file);

        sprintf(top, " %s %s %04ld", group, group_name, sll_size(msg_list));

        sll_destroy(&msg_list);
        free(group_msgs_path);
    
    END_FIIL()

    send_udp_message(connection, msg_buffer);

    free(msg_buffer);

    sll_destroy(&group_list);
}

void subscribe(connection_context_t *connection, char *args, char *fs){
    char *uid = get_word(&args);
    char *gid = get_word(&args);
    char *gname = get_word(&args);

    char msg_buffer[BUFFER_SIZE];

    char *user_dir = malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + 10));
    sprintf(user_dir, "%s/%s/%s", fs, SERVER_USERS_NAME, uid);

    if (!(directory_exists(user_dir)))
        sprintf(msg_buffer, "RGS E_USR\n");
    else{
        char *groups_dir = malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_GROUPS_NAME) + 3));
        sprintf(groups_dir, "%s/%s", fs, SERVER_GROUPS_NAME);

        char *group_dir = malloc(sizeof(char) * (strlen(groups_dir) + 4));
        if (strcmp(gid, "00") == 0){ // Create new group
            sll_link_t group_list = list_subdirectories(groups_dir);
            int new_id = sll_size(group_list) + 1;

            sprintf(group_dir, "%s/%02d", groups_dir, new_id);

            create_directory_abs(group_dir);
            create_directory(group_dir, "MSG");
            create_file(group_dir, "name.txt", gname);

            sll_destroy(&group_list);
            sprintf(gid, "%02d", new_id);
        }

        sprintf(group_dir, "%s/%s", groups_dir, gid);

        if (!(directory_exists(groups_dir))){
            sprintf(msg_buffer, "RGS E_GRP\n");
        }else{
            create_file(group_dir, uid, "");
            sprintf(msg_buffer, "RGS OK\n");
        }

        free(group_dir);
        free(groups_dir);
    }

    send_udp_message(connection, msg_buffer);
    free(user_dir);
}

void unsubscribe(connection_context_t *connection, char *args, char *fs){
    char *uid = get_word(&args);
    char *gid = get_word(&args);

    char msg_buffer[BUFFER_SIZE];

    char *user_dir = malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(uid) + 3));
    sprintf(user_dir, "%s/%s/%s", fs, SERVER_USERS_NAME, uid);

    if (directory_exists(user_dir)){
        char *group_dir = malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_GROUPS_NAME) + strlen(gid) + 3));
        sprintf(group_dir, "%s/%s/%s", fs, SERVER_GROUPS_NAME, gid);
        if (directory_exists(group_dir)){
            if (file_exists(group_dir, uid)){
                delete_file(group_dir, uid);
                sprintf(msg_buffer, "RGU OK");
            }else{
                sprintf(msg_buffer, "RGU NOK");
            }
        }else{
            sprintf(msg_buffer, "RGU E_GRP");
        }
        free(group_dir);
    }else{
        sprintf(msg_buffer, "RGU E_USR");
    }

    
     
    free(user_dir);
    send_udp_message(connection, msg_buffer);
}

void my_groups(connection_context_t *connection, char *args, char *fs){
  char *uid = get_word(&args);

  char *groups_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_GROUPS_NAME) + 3));
  sprintf(groups_path, "%s/%s/", fs, SERVER_GROUPS_NAME);

  sll_link_t groups_list = list_subdirectories(groups_path);

  int n_subscribed_groups = 0;

  char n_str[BUFFER_SIZE];

  char *groups_buffer = (char *) malloc(sizeof(char)*(sll_size(groups_list)*33));

  FOR_ITEM_IN_LIST(char* group, groups_list)

      char *group_path = (char *) malloc(sizeof(char) * (strlen(groups_path) + strlen(group) + 2));

      sprintf(group_path, "%s/%s", groups_path, group);

      if (file_exists(group_path, uid)){

        n_subscribed_groups++;

        char *top = &groups_buffer[strlen(groups_buffer)];

        char *group_msgs_path = (char *) malloc(sizeof(char)*(strlen(groups_path) + strlen(group) + 3));

        sprintf(group_msgs_path, "%s/%s/", groups_path, group);
        sll_link_t msg_list = list_subdirectories(group_msgs_path);

        char *name_path = (char *) malloc(sizeof(char) * (strlen(group_msgs_path) + strlen("name.txt") + 3));
        sprintf(name_path, "%s/%s", group_msgs_path, "name.txt");
        FILE *file = fopen(name_path, "r");
        free(name_path);

        char group_name[25];

        fscanf(file, "%s", group_name);
        fclose(file);

        sprintf(top, "%s %s %04ld", group, group_name, sll_size(msg_list));

        sll_destroy(&msg_list);
        free(group_msgs_path);
      }
      free(group_path);
  END_FIIL()

  sprintf(n_str, "%d", n_subscribed_groups);
  char *msg_buffer = (char *) malloc(sizeof(char)*(4 + strlen(n_str) + strlen(groups_buffer)));


  sprintf(msg_buffer, "RGM %s %s", n_str, groups_buffer);

  send_udp_message(connection, msg_buffer);

  free(groups_buffer);

  free(msg_buffer);

  free(groups_path);

  sll_destroy(&groups_list);

}


void ulist(connection_context_t *connection, char *fs){
    char gid[3];
    gid[2] = '\0';

    int fd = connection->tcp_info->fd;
    read_fd(fd, gid, 2);

    char *msg;

    char *group_dir = malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_GROUPS_NAME) + strlen(gid) + 3));
    sprintf(group_dir, "%s/%s/%s", fs, SERVER_GROUPS_NAME, gid);
    
    if (directory_exists(group_dir)){
        // read group name
        char group_name[25];
        char *name_path = (char *) malloc(sizeof(char) * (strlen(group_dir) + strlen("name.txt") + 2));
        sprintf(name_path, "%s/%s", group_dir, "name.txt");
        FILE *file = fopen(name_path, "r");
        free(name_path);

        fscanf(file, "%s", group_name);
        fclose(file);

        sll_link_t file_list = list_files(group_dir);
        
        size_t size = sll_size(file_list) - 1; // -1 is because of the name.txt file which we do not take into account

        msg = (char *) malloc(sizeof(char) * (11 + strlen(group_name) + size * 6));

        sprintf(msg, "RUL OK %s ", group_name);

        size_t end_of_str = strlen(msg);

        FOR_ITEM_IN_LIST(char *file_name, file_list)
            if (strcmp(file_name, "name.txt") == 0) continue;
            sprintf(&msg[end_of_str], "%s ", file_name);
            end_of_str += strlen(file_name) + 1;
        END_FIIL()

        msg[end_of_str++] = '\n';
        msg[end_of_str] = '\0';

        sll_destroy(&file_list);
    }else{
        msg = strdup("RUL NOK\n");
    }

    send_tcp_message(connection, msg);

    free(msg);
}

void post(connection_context_t *connection, char *fs){

}
void retrieve(connection_context_t *connection, char *fs){
}
