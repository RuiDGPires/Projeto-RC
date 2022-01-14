#include "../common/debug.h"
#include "../common/util.h"
#include "../common/constants.h"
#include "connection.h"
#include "commands.h"
#include "file_management.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void parse_udp_message(connection_context_t *connection, char *msg, char *fs, bool verbose){
  char *command = get_word(&msg);
  char *uid_gid;

  if (strcmp(command, "REG") == 0){
    uid_gid = reg(connection, msg, fs);
    if(verbose){
      info("Register a new user");
      info("UID %s", uid_gid);
    }
  }else if (strcmp(command, "UNR") == 0){
    uid_gid = unregister(connection, msg, fs);
    if(verbose){
      info("Unregister a user");
      info("UID %s", uid_gid);
    }
  }else if (strcmp(command, "LOG") == 0){
    uid_gid = login_(connection, msg, fs);
    if(verbose){
      info("Validate the user's credentials");
      info("UID %s", uid_gid);
    }
  }else if (strcmp(command, "OUT") == 0){
    uid_gid = logout_(connection, msg, fs);
    if(verbose){
      info("Forgets the credentials of a previously logged in user");
      info("UID %s", uid_gid);
    }
  }else if (strcmp(command, "GLS") == 0){
    groups(connection, msg, fs);
    if(verbose){
      info("Lists available groups");
    }
  }else if (strcmp(command, "GSR") == 0){
    uid_gid = subscribe(connection, msg, fs);
    if(verbose){
      info("Request to subscribe to a group or create a new one");
      info(" UID  | GID ");
      info("%s", uid_gid);
    }
    free(uid_gid);
  }else if (strcmp(command, "GUR") == 0){
    uid_gid = unsubscribe(connection, msg, fs);
    if(verbose){
      info("Request to unsubscribe to a group");
      info(" UID  | GID ");
      info("%s", uid_gid);
    }
    free(uid_gid);
  }else if (strcmp(command, "GLM") == 0){
    uid_gid = my_groups(connection, msg, fs);
    if(verbose){
      info("List the groups to which the user is subscribed");
      info("UID %s", uid_gid);
    }
  }else throw_error("Unkown command");
  if(verbose){
    info("IP %s", inet_ntoa(connection->udp_info->addr.sin_addr ));
    info("PORT %s\n", connection->port);
  }
}

void parse_tcp_message(connection_context_t *connection, char *fs,bool verbose){
    char command[4];
    command[3] = '\0';
    char *uid_gid;
   
    read_fd(connection->tcp_info->fd, command, 3);
    read_fd(connection->tcp_info->fd, NULL, 1); // throw away the space after

    if (strcmp(command, "ULS") == 0){
        uid_gid = ulist(connection, fs);
        if(verbose){
          info("Lists users currently subscribed to the same group as the user");
          info("GID %s", uid_gid);
        }
        free(uid_gid);
    }else if (strcmp(command, "PST") == 0){
        uid_gid = post(connection, fs);
        if(verbose){
          info("User sends a message containing text and possibly also a file to a selected group");
          info(" UID  | GID ");
          info("%s", uid_gid);
        }
        free(uid_gid);
    }else if (strcmp(command, "RTV") == 0){
        uid_gid = retrieve(connection, fs);
        if(verbose){
          info("Request to receive up to 20 messages");
          info(" UID  | GID ");
          info("%s", uid_gid);
        }
        free(uid_gid);
    }else throw_error("Unkown command");
    if(verbose){
      info("IP %s", inet_ntoa(connection->udp_info->addr.sin_addr ));
      info("PORT %s\n", connection->port);
    }
}

void parse_args(char *dsport, bool *verbose, int argc, char *argv[]){
  DEBUG_MSG_SECTION("ARGS");

  CLEAR(dsport);
  *verbose = FALSE;
  // Parse args
  for (int i = 1; i < argc; i++){
    if (strcmp(argv[i], "-v") == 0)
      *verbose = TRUE;
    else if (strcmp(argv[i], "-p") == 0){
      ASSERT_NOR(argc != i + 1, "-p requires another argument");
      strcpy(dsport, argv[++i]);

    }else throw_error("Unkown command line argument");
  }

  DEFAULT(dsport, DEFAULT_DSPORT);

  DEBUG_MSG("verbose set to: %d\n", *verbose);
  DEBUG_MSG("dsport set to: %s\n", dsport);
}

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}


int main(int argc, char *argv[]){
  DEBUG_MSG_SECTION("MAIN");
  DEBUG_MSG("Init Process %d\n", getpid());

  char dsport[PORT_SIZE];
  bool verbose;

  char buffer_udp[BUFFER_SIZE];

  int maxfd1, ret, running = TRUE;
  fd_set rset;

  struct sigaction act;
  memset(&act, 0, sizeof act);
  
  if (sigaction(SIGCHLD, &act, NULL)==-1)/*error*/exit(1);

  parse_args(dsport, &verbose, argc, argv);

  connection_context_t *context = (connection_context_t *) malloc(sizeof(connection_context_t));
  strcpy(context->port, dsport);

  if(init_udp(context) == FERROR) exit(0);
  if(init_tcp(context) == FERROR) exit(0); 

  char *fs = create_filesystem(".");

  FD_ZERO(&rset);

  maxfd1 = max(context->tcp_info->fd, context->udp_info->fd) + 1;

  while (running) {
    FD_SET(context->udp_info->fd, &rset);
    FD_SET(context->tcp_info->fd, &rset);

    DEBUG_MSG("Waiting Select... \n");
    select(maxfd1, &rset, NULL, NULL, NULL);
    DEBUG_MSG("... Select Completed \n");

    if(FD_ISSET(context->udp_info->fd, &rset)){
        wait_udp_message(context, buffer_udp, BUFFER_SIZE);
        parse_udp_message(context, buffer_udp, fs, verbose);
    }
    else{
      if(FD_ISSET(context->tcp_info->fd, &rset)){
        int newfd = accept_tcp_message(context);
        int pid = fork();
        if(pid == 0){
            //update fd
            DEBUG_MSG("Init Process %d\n", getpid());
            close(context->tcp_info->fd);
            context->tcp_info->fd = newfd;
            
            parse_tcp_message(context, fs, verbose);
            close(context->tcp_info->fd);
            running = FALSE;
        }
        else if(pid == -1){
          DEBUG_MSG("Error Creating Fork %d\n", getpid());
          running = FALSE;
        }

        do ret=close(newfd); while(ret==-1 && errno==EINTR);

        if(ret==-1){
          /*error*/
          DEBUG_MSG("Error \n");
          running = FALSE;
        }
      }
    }
  }

  close_udp(context);
  close_tcp(context);

  free(fs);
  fs = NULL;

  free(context);
  context = NULL;
  
  DEBUG_MSG("Close Process %d\n", getpid());
  return 0;
}
