#include "../common/debug.h"
#include "../common/util.h"
#include "commands.h"
#include "file_management.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

#define TSIZE_SIZE 240
#define FNAME_SIZE 24
#define FSIZE_SIZE 24

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

int check_pass(const char str[]){
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

int check_gid(const char str[]){
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

int check_gname(const char str[]){
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

char *reg(connection_context_t *connection, char *args, char *fs){
  char *uid = get_word(&args);
  char *pass= get_word(&args);

  char buffer[BUFFER_SIZE];

  if (check_uid(uid) == FERROR || check_pass(pass) == FERROR){
    sprintf(buffer, "RRG NOK\n");
    send_udp_message(connection, buffer);
    return NULL;
  }

  char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(uid)) + 3);

  sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, uid);

  if (directory_exists(user_path)){
    sprintf(buffer, "RRG DUP\n");
  }else{
    create_directory_abs(user_path);
    create_file(user_path, "pass.txt", pass);
    sprintf(buffer, "RRG OK\n");
  }

  send_udp_message(connection, buffer);

  free(user_path);

  return uid;
}

char *unregister(connection_context_t *connection, char *args, char *fs){
    char *uid = get_word(&args);
    char *pass= get_word(&args);

    char buffer[BUFFER_SIZE];

    if(check_uid(uid) == FERROR || check_pass(pass) == FERROR){
      sprintf(buffer, "RUN NOK\n");
      send_udp_message(connection, buffer);
      return NULL;
    }

    char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(uid)) + 3);

    sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, uid);

    if (check_credencials(uid, pass, fs) == 0){
      delete_directory(user_path);

        char *groups_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_GROUPS_NAME) + 2));
        sprintf(groups_path, "%s/%s", fs, SERVER_GROUPS_NAME);

        sll_link_t groups_list = list_subdirectories(groups_path);

        FOR_ITEM_IN_LIST(char *group, groups_list)

            char *group_path = (char *) malloc(sizeof(char) * (strlen(groups_path) + strlen(group) + 2));

            sprintf(group_path, "%s/%s", groups_path, group);

            if (file_exists(group_path, uid)){
                delete_file(group_path, uid);
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

    return uid;
}

char *login_(connection_context_t *connection, char *args, char *fs){
    char *uid = get_word(&args);
    char *pass= get_word(&args);

    char buffer[BUFFER_SIZE];

    if(check_uid(uid) == FERROR || check_pass(pass) == FERROR){
      sprintf(buffer, "RLO NOK\n");
      send_udp_message(connection, buffer);
      return NULL;
    }

    char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(uid)) + 3);

    sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, uid);

    if (check_credencials(uid, pass, fs) == 0){
      if(!(is_logged_in(uid, fs))){
        create_file(user_path, "login", NULL);
      }
      sprintf(buffer, "RLO OK\n");
      
    }else{
        sprintf(buffer, "RLO NOK\n");
    }

    free(user_path);

    send_udp_message(connection, buffer);

    return uid;
}

char *logout_(connection_context_t *connection, char *args, char *fs){
    char *uid = get_word(&args);
    char *pass= get_word(&args);

    char buffer[BUFFER_SIZE];

    if(check_uid(uid) == FERROR || check_pass(pass) == FERROR){
      sprintf(buffer, "ROU NOK\n");
      send_udp_message(connection, buffer);
      return NULL;
    }

    char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(uid)) + 3);

    sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, uid);

    if (check_credencials(uid, pass, fs) == 0){
        if(is_logged_in(uid, fs)){
            delete_file(user_path, "login");
        }
        sprintf(buffer, "ROU OK\n");
    }else{
        sprintf(buffer, "ROU NOK\n");
    }

    send_udp_message(connection, buffer);

    return uid;
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

        char *group_path = (char *) malloc(sizeof(char) * (strlen(groups_path) + strlen(group)));
        sprintf(group_path, "%s/%s", groups_path, group);

        char *group_msgs_path = (char *) malloc(sizeof(char)*(strlen(group_path) + 8));
        sprintf(group_msgs_path, "%s/MSG/", group_path);
        sll_link_t msg_list = list_subdirectories(group_msgs_path);


        char *name_path = (char *) malloc(sizeof(char) * (strlen(group_msgs_path) + strlen("name.txt") + 3));
        sprintf(name_path, "%s/%s", group_path, "name.txt");
        FILE *file = fopen(name_path, "r");
        free(name_path);

        char group_name[25];

        fscanf(file, "%s", group_name);
        fclose(file);

        sprintf(top, " %s %s %04ld", group, group_name, sll_size(msg_list));

        sll_destroy(&msg_list);
        free(group_path);
        free(group_msgs_path);
    
    END_FIIL()

    msg_buffer[strlen(msg_buffer)] = '\n';
    msg_buffer[strlen(msg_buffer)+1] = '\0';
    send_udp_message(connection, msg_buffer);

    free(msg_buffer);

    sll_destroy(&group_list);
}

char *subscribe(connection_context_t *connection, char *args, char *fs){
    char *uid = get_word(&args);
    char *gid = get_word(&args);
    char *gname = get_word(&args);

    char msg_buffer[BUFFER_SIZE];

    if(check_uid(uid) == FERROR){
      sprintf(msg_buffer, "RGS E_USR\n");
      send_udp_message(connection, msg_buffer);
      return NULL;
    } else if(check_gid(gid) == FERROR){
      sprintf(msg_buffer, "RGS E_GRP\n");
      send_udp_message(connection, msg_buffer);
      return NULL;
    } else if(check_gname(gname) == FERROR){
      sprintf(msg_buffer, "RGS E_GNAME\n");
      send_udp_message(connection, msg_buffer);
      return NULL;
    }

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

            if (new_id > 99){
                sprintf(msg_buffer, "RGS E_FULL\n");
            }else{
                sprintf(group_dir, "%s/%02d", groups_dir, new_id);

                create_directory_abs(group_dir);
                create_directory(group_dir, "MSG");
                create_file(group_dir, "name.txt", gname);

                sll_destroy(&group_list);
                sprintf(gid, "%02d", new_id);
            }
        }

        sprintf(group_dir, "%s/%s", groups_dir, gid);

        if (!(directory_exists(groups_dir))){
            sprintf(msg_buffer, "RGS E_GRP\n");
        }else{
            char *group_name_path = malloc(sizeof(char) * (strlen(group_dir) + strlen("name.txt") + 2));
            sprintf(group_name_path, "%s/name.txt", group_dir);
            char group_name[BUFFER_SIZE];
            FILE *file = fopen(group_name_path, "r");
            fscanf(file, "%s", group_name);
            free(group_name_path);
            fclose(file);
            if (strcmp(group_name, gname) != 0){
                sprintf(msg_buffer, "RGS E_GNAME\n");
            }else{
                create_file(group_dir, uid, NULL);
                sprintf(msg_buffer, "RGS OK\n");
            }

        }

        free(group_dir);
        free(groups_dir);
    }

    send_udp_message(connection, msg_buffer);
    free(user_dir);

    char *uid_gid = (char *) malloc(sizeof(char)*(UID_SIZE + GID_SIZE + 3));
    sprintf(uid_gid, "%s | %s", uid, gid);
    return uid_gid;
}

char *unsubscribe(connection_context_t *connection, char *args, char *fs){
    char *uid = get_word(&args);
    char *gid = get_word(&args);

    char msg_buffer[BUFFER_SIZE];

    if(check_uid(uid) == FERROR){
      sprintf(msg_buffer, "RGU E_USR\n");
      send_udp_message(connection, msg_buffer);
      return NULL;
    } else if(check_gid(gid) == FERROR){
      sprintf(msg_buffer, "RGU E_GRP\n");
      send_udp_message(connection, msg_buffer);
      return NULL;
    }

    char *user_dir = malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(uid) + 3));
    sprintf(user_dir, "%s/%s/%s", fs, SERVER_USERS_NAME, uid);

    if (directory_exists(user_dir)){
        char *group_dir = malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_GROUPS_NAME) + strlen(gid) + 3));
        sprintf(group_dir, "%s/%s/%s", fs, SERVER_GROUPS_NAME, gid);
        if (directory_exists(group_dir)){
            if (file_exists(group_dir, uid)){
                delete_file(group_dir, uid);
                sprintf(msg_buffer, "RGU OK\n");
            }else{
                sprintf(msg_buffer, "RGU NOK\n");
            }
        }else{
            sprintf(msg_buffer, "RGU E_GRP\n");
        }
        free(group_dir);
    }else{
        sprintf(msg_buffer, "RGU E_USR\n");
    }
     
    free(user_dir);
    send_udp_message(connection, msg_buffer);

    char *uid_gid = (char *) malloc(sizeof(char)*(UID_SIZE + GID_SIZE + 3));
    sprintf(uid_gid, "%s | %s", uid, gid);
    return uid_gid;
}

char *my_groups(connection_context_t *connection, char *args, char *fs){
  char *uid = get_word(&args);

  char *user_dir = malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + 10));
  sprintf(user_dir, "%s/%s/%s", fs, SERVER_USERS_NAME, uid);

  if (!(directory_exists(user_dir))){
    char msg_buffer[BUFFER_SIZE];
    sprintf(msg_buffer, "RGS E_USR\n");

    send_udp_message(connection, msg_buffer);

    free(user_dir);

    return uid;
  }else if (!(is_logged_in(uid, fs))){
    char msg_buffer[BUFFER_SIZE];
    sprintf(msg_buffer, "RGS E_USR\n");

    send_udp_message(connection, msg_buffer);

    free(user_dir);

    return uid;
  }

  char *groups_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_GROUPS_NAME) + 3));
  sprintf(groups_path, "%s/%s/", fs, SERVER_GROUPS_NAME);

  sll_link_t groups_list = list_subdirectories(groups_path);

  int n_subscribed_groups = 0;

  char *groups_buffer = (char *) malloc(sizeof(char)*(sll_size(groups_list)*33));
  groups_buffer[0] = '\0';

  FOR_ITEM_IN_LIST(char* group, groups_list)
      char *group_path = (char *) malloc(sizeof(char) * (strlen(groups_path) + strlen(group) + 2));

      sprintf(group_path, "%s/%s", groups_path, group);

      if (file_exists(group_path, uid)){
        n_subscribed_groups++;

        char *top = &groups_buffer[strlen(groups_buffer)];

        char *groups_gid_path = (char *) malloc(sizeof(char)*(strlen(groups_path) + strlen(group) + 2));
        sprintf(groups_gid_path, "%s/%s", groups_path, group);

        char *group_msgs_path = (char *) malloc(sizeof(char)*(strlen(groups_gid_path) + 5));
        sprintf(group_msgs_path, "%s/MSG", groups_gid_path);

        sll_link_t msg_list = list_subdirectories(group_msgs_path);
        
        char *name_path = (char *) malloc(sizeof(char) * (strlen(groups_gid_path) + strlen("name.txt") + 3));
        sprintf(name_path, "%s/%s", groups_gid_path, "name.txt");
        FILE *file = fopen(name_path, "r");

        char group_name[25];

        fscanf(file, "%s", group_name);
        fclose(file);

        sprintf(top, " %s %s %04ld", group, group_name, sll_size(msg_list));

        sll_destroy(&msg_list);
        free(groups_gid_path);
        free(group_msgs_path);        
        free(name_path);
      }

      free(group_path);

  END_FIIL()

  if(n_subscribed_groups == 0){

    char msg_buffer[BUFFER_SIZE];
    sprintf(msg_buffer, "RGM 0\n");

    send_udp_message(connection, msg_buffer);

    free(user_dir);
    free(groups_buffer);
    free(groups_path);
    sll_destroy(&groups_list);

    return uid;
  }

  char n_str[BUFFER_SIZE];
  sprintf(n_str, "%d", n_subscribed_groups);

  char *msg_buffer = (char *) malloc(sizeof(char)*(6 + strlen(n_str) + strlen(groups_buffer)));
  sprintf(msg_buffer, "RGM %s%s\n", n_str, groups_buffer);

  send_udp_message(connection, msg_buffer);

  free(user_dir);
  free(groups_buffer);
  free(msg_buffer);
  free(groups_path);
  sll_destroy(&groups_list);

  return uid;
}


char *ulist(connection_context_t *connection, char *fs){
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

    char *_gid = (char *) malloc(sizeof(char *) * strlen(gid));
    sprintf(_gid, "%s", gid);
    return _gid;
}

char *post(connection_context_t *connection, char *fs){
    DEBUG_MSG_SECTION("PST");
    DEBUG_MSG("posting\n");
    char uid[6];
    char gid[3];
    gid[2] = '\0';
    uid[5] = '\0';

    int fd = connection->tcp_info->fd;
    read_fd(fd, uid, 5);
    read_fd(fd, NULL, 1); // Throw away space
    read_fd(fd, gid, 2);
    read_fd(fd, NULL, 1); // Throw away space
    
    char tsize_str[TSIZE_SIZE];
    get_word_fd(fd, tsize_str);

    int tsize = atoi(tsize_str);

    char *text = (char *) malloc(sizeof(char)*(tsize + 1));
    read_fd(fd, text, tsize); 
    text[tsize] = '\0';
    read_fd(fd, NULL, 1); // Throw away space

    
    char msg_buffer[BUFFER_SIZE];

    char *group_dir = (char *) malloc(sizeof(char) * (strlen(fs) + strlen(SERVER_GROUPS_NAME) + strlen(gid)  + 3));
    sprintf(group_dir, "%s/%s/%s", fs, SERVER_GROUPS_NAME, gid);

    if (directory_exists(group_dir)){
        char *msgs_dir = (char *) malloc(sizeof(char) * (strlen(group_dir) + strlen("MSG") + 2));
        sprintf(msgs_dir, "%s/MSG", group_dir);

        sll_link_t msg_list = list_subdirectories(msgs_dir);
        size_t msg_id = sll_size(msg_list) + 1;
        sll_destroy(&msg_list);

        char msg_id_str[5];
        sprintf(msg_id_str, "%04ld", msg_id);
        create_directory(msgs_dir, msg_id_str);

        char *msg_dir = (char *) malloc(sizeof(char)*(strlen(msgs_dir) + 7));
        sprintf(msg_dir, "%s/%s", msgs_dir, msg_id_str);

        create_file(msg_dir, "author.txt", uid);
        create_file(msg_dir, "text.txt", text);

        char fname[FNAME_SIZE];

        fcntl(fd, F_SETFL, O_NONBLOCK);
        if (get_word_fd(fd, fname)){ // if there is a file
            fcntl(fd, F_SETFL, ~O_NONBLOCK);

            create_directory(msg_dir, "FILE");
            char *file_path = (char *) malloc(sizeof(char) * (strlen(msg_dir) + strlen("FILE") + strlen(fname) + 5));
            sprintf(file_path, "%s/FILE/%s", msg_dir, fname);

            char fsize_str[FSIZE_SIZE];
            get_word_fd(fd, fsize_str);
            int fsize = atoi(fsize_str);

            char *file_data = (char *) malloc(sizeof(char)*(fsize + 1));
            read_fd(fd, file_data, fsize); 
            FILE *file = fopen(file_path, "w");
            fwrite(file_data, 1, fsize, file);
            fclose(file);
        }
        sprintf(msg_buffer, "RPT OK\n");
    }else{
        sprintf(msg_buffer, "RPT NOK\n");
    }

    fcntl(fd, F_SETFL, ~O_NONBLOCK);
    send_tcp_message(connection, msg_buffer);

    char *uid_gid = (char *) malloc(sizeof(char)*(UID_SIZE + GID_SIZE + 3));
    sprintf(uid_gid, "%s | %s", uid, gid);
    return uid_gid;
}

char *retrieve(connection_context_t *connection, char *fs){
    char uid[UID_SIZE], gid[GID_SIZE], mid_str[MID_SIZE];
    int fd = connection->tcp_info->fd;

    get_word_fd(fd, uid);
    get_word_fd(fd, gid);
    get_word_fd(fd, mid_str);
    int mid = atoi(mid_str);

    char *group_dir = (char *) malloc(sizeof(char) * (strlen(fs) + strlen(SERVER_GROUPS_NAME) + strlen(gid) + 3));
    sprintf(group_dir, "%s/%s/%s", fs, SERVER_GROUPS_NAME, gid);

    DEBUG_MSG("Group directory: %s\n", group_dir);

    char *msg;

    if (directory_exists(group_dir)){
        char *msgs_dir = (char *) malloc(sizeof(char) * (strlen(group_dir) + 8)); 
        sprintf(msgs_dir, "%s/MSG", group_dir);

        DEBUG_MSG("Messagesssss directory: %s\n", msgs_dir);

        sll_link_t msg_list = list_subdirectories(msgs_dir);

        size_t n_msgs = sll_size(msg_list);

        if (mid > n_msgs){ // There are no new messages
            msg = strdup("RRT EOF 0\n"); 
            send_tcp_message(connection, msg);
            free(msg);
        }else{
            int new_msgs = n_msgs - mid + 1;
            dprintf(fd, "RRT OK %d", 20 < new_msgs ? 20: new_msgs); 
            
            FOR_ITEM_IN_LIST(char *current_str, msg_list);
                int current = atoi(current_str);
                if (current < mid) continue; 
                if (current >= mid + 20) break;

#define BUFFER_SIZE2 BUFFER_SIZE * 2
                char buffer[BUFFER_SIZE2];
                sprintf(buffer, " %s ", current_str);
                
                char *msg_dir = (char *) malloc(sizeof(char) * strlen(msgs_dir) + 6);
                sprintf(msg_dir, "%s/%s", msgs_dir,  current_str);

                DEBUG_MSG("Message directory: %s\n", msg_dir);

                char *author_path = (char *) malloc(sizeof(char) * (strlen(msg_dir) + strlen("author.txt") + 3));
                sprintf(author_path, "%s/author.txt", msg_dir);
                DEBUG_MSG("Author path: %s\n", author_path);
                char *text_path = (char *) malloc(sizeof(char) * (strlen(msg_dir) + strlen("text.txt") + 3));
                sprintf(text_path, "%s/text.txt", msg_dir);
                DEBUG_MSG("Text path: %s\n", text_path);

                char author[UID_SIZE];

                FILE *file = fopen(author_path, "r");
                fread(author, 1, UID_SIZE - 1, file);
                author[UID_SIZE - 1] = '\0';
                fclose(file);

                file = fopen(text_path, "r");
                size_t text_size = get_file_size(file);

                sprintf(&buffer[strlen(buffer)], "%s %ld ", author, text_size);

                write(fd, buffer, strlen(buffer));

                char *new_buffer = (char *) malloc(sizeof(char) * text_size + BUFFER_SIZE);
                fread(new_buffer, 1, text_size, file);
                fclose(file);

                free(author_path);
                free(text_path);

                // CHECK IF HAS FILE
                char *file_dir = (char *) malloc(sizeof(char) * (strlen(msg_dir) + strlen("FILE") + 2));
                sprintf(file_dir, "%s/FILE", msg_dir);
                DEBUG_MSG("File directory: %s\n", file_dir);

                if (directory_exists(file_dir)){
                    sll_link_t file_lst = list_files(file_dir);
                    
                    char *file_path = (char *) malloc(sizeof(char) * (strlen(file_dir) + strlen(file_lst->str) + 3));
                    
                    sprintf(file_path, "%s/%s", file_dir, file_lst->str);

                    DEBUG_MSG("file: %s\n", file_path);

                    FILE *file = fopen(file_path, "r");
                    size_t file_size = get_file_size(file);

                    char file_size_str[11];
                    sprintf(file_size_str, "%ld", file_size);
                    sprintf(&new_buffer[text_size], " / %s %s ", file_lst->str, file_size_str);

                    size_t total_size = text_size + 5 + strlen(file_lst->str) + strlen(file_size_str);
                    for (size_t total_written_size = 0; total_written_size != total_size;)
                        total_written_size += write(fd, &new_buffer[total_written_size], total_size-total_written_size);

                    char buffer[BUFFER_SIZE];
                    for (size_t total_read = 0; total_read < file_size;){
                        size_t reading_size = (file_size - total_read) > BUFFER_SIZE ? BUFFER_SIZE : (file_size - total_read);
                        size_t n = fread(buffer, 1, reading_size, file);
                        total_read += n;
                        write(fd, buffer, n);
                    }

                    fclose(file);

                    free(file_path);
                    sll_destroy(&file_lst);
                }else{
                    for (size_t total_written_size = 0; total_written_size != text_size;)
                        total_written_size += write(fd, &new_buffer[total_written_size], text_size-total_written_size);
                }
                free(new_buffer);
                free(msg_dir);
            END_FIIL();
            write(fd, "\n", 1);
        }

        sll_destroy(&msg_list);
    }else{
        msg = strdup("RRT NOK\n");
        send_tcp_message(connection, msg);
        free(msg);
    }

    char *uid_gid = (char *) malloc(sizeof(char)*(UID_SIZE + GID_SIZE + 3));
    sprintf(uid_gid, "%s | %s", uid, gid);
    return uid_gid;
}
