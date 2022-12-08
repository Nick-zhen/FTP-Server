#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "dir.h"
#include "usage.h"
#include <pthread.h>
#include <netinet/in.h>
#include <string.h>
#include "command.h"
#include <arpa/inet.h>


int user(char * args, int* client_fd, int logged){

        if (logged == 1) {
          send_msg(*client_fd, "530, already logged in.");
        } else {
          if (strcmp(args, "cs317") == 0) {
            logged = 1;
            send_msg(*client_fd, "230 User logged in, proceed.");
          } else {
            send_msg(*client_fd, "530 cs317 is the only username. Not logged in.");
          }
        }
        return logged;
}