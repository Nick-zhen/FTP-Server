#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include "dir.h"
#include "usage.h"
#include <pthread.h>
#include <netinet/in.h>
#include <string.h>
#include "command.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <errno.h>

// Here is an example of how to use the above function. It also shows
// one how to get the arguments passed on the command line.
/* this function is run by the second thread */


char dir[256];

void *recv_handler(void* socket_fd) {
  int* client_fd = (int*) socket_fd;
  printf("client socket fd: %d\n", *client_fd);

  chdir(dir);
  printf("root directory: %s\n", dir);
  char msg_buf[1024];
  int logged = 0;
  int isPasv = 0;
  int data_socket, data_client_fd;
  int openType = 0; // default 0 for A, 1 for Image


  // for user can keeping enter
  while (1) {
    bzero(msg_buf, 1024);

    if (recv(*client_fd, msg_buf, 1024, 0) == -1) {
      perror("reading from socket failed:%d\n");
      close(*client_fd);
      // logout();
      break;
    }

    // trim the string
    char* cmd_str = trimwhitespace(msg_buf);
    printf("command from client: %s\n", cmd_str);
    // tokenize the msg received from client
    char* tok = strtok(cmd_str, " ");
    // check empty msg
    if (tok == NULL) {
      continue;
    }
    char *args;

    // when user enter nothing 
    if(!strcasecmp(tok,"")){
      continue;
    } else if (strcasecmp(tok, "QUIT") == 0) {
      send_msg(*client_fd, "221 quit");
      logged = 0; // logout 
      close(*client_fd);
      break;

    } else if (strcasecmp(tok, "USER") == 0) {
       args = strtok(NULL, " ");
      // printf("args from client user: %s\n", args); 
      if (args && strtok(NULL, " ") == NULL) {
        logged = user(args, client_fd,logged);
        continue;
      } else {
        send_msg(*client_fd, "501 Syntax error in parameters or arguments.");
        continue;
      }
    }if (strcasecmp(tok, "CWD") == 0) {
      // CWD  <SP> <pathname> <CRLF>
      if (!logged) {
        send_msg(*client_fd, "530 cs317 is the only username. Not logged in.");
        continue;
      }

      args = strtok(NULL, " ");
      // printf("Printed %d directory entries\n", listFiles(1, "."));
      if (args && strtok(NULL, " ") == NULL) {
        if (check_valid_dir(args)) {
          int success = chdir(args);
          if (success == 0) {
            send_msg(*client_fd, "250 Requested file action okay, completed.");
          } else {
            send_msg(*client_fd, "550 Requested action not taken.");
          }
        } else {
          send_msg(*client_fd, "550 Requested action not taken. Don't includ ./ and ../");
        }
      } else {
        send_msg(*client_fd, "501 Syntax error in parameters or arguments.");
      }
    } else if (strcasecmp(tok, "CDUP") == 0) {
      // CDUP <CRLF>
      if (!logged) {
        send_msg(*client_fd, "530 cs317 is the only username. Not logged in.");
        continue;
      }
      if (strtok(NULL, " ") == NULL) {
        char cur_dir[256];
        getcwd(cur_dir, 256);
        if (strcmp(cur_dir, dir) == 0) {
          send_msg(*client_fd, "550 Requested action not taken.");
        } else {
          int success = chdir("..");
          if (success == 0) {
            send_msg(*client_fd, "250 Requested file action okay, completed.");
          } else {
            send_msg(*client_fd, "550 Requested action not taken.");
          }
        }
      } else {
        send_msg(*client_fd, "501 Syntax error in parameters or arguments.");
      }

    } else if (strcasecmp(tok, "TYPE") == 0) {
      if (!logged) {
        send_msg(*client_fd, "530 cs317 is the only username. Not logged in.");
        continue;
      }

      args = strtok(NULL, " ");
      if (args && strtok(NULL, " ") == NULL) {
        if (strcasecmp(args,"I") == 0) {
          openType = 1;
          send_msg(*client_fd, "200, command okay. Image mode");
        } else if  (strcasecmp(args,"A") == 0) {
          openType = 1;
          send_msg(*client_fd, "200, command okay. ASCII mode");
        } else if  (strcasecmp(args,"E") == 0){
          send_msg(*client_fd, "500, unsupported commands.");
        } else if  (strcasecmp(args,"L") == 0){
          send_msg(*client_fd, "500, unsupported commands.");
        } else {
          send_msg(*client_fd, "504 Command not implemented for that parameter. Only 'I' and 'A' ");
        }
      } else{
        send_msg(*client_fd, "501 Syntax error in parameters or arguments.");
      }
    } else if (strcasecmp(tok, "MODE") == 0) {
      // MODE <SP> <mode-code> <CRLF>
      if (!logged) {
        send_msg(*client_fd, "530 cs317 is the only username. Not logged in.");
        continue;
      }
      args = strtok(NULL, " ");
      if (args && strtok(NULL, " ") == NULL) {
        if (strcasecmp(args, "S") == 0) {
          send_msg(*client_fd, "200 mode changed to stream.");
        } else if (strcasecmp("B", args) == 0) {
          send_msg(*client_fd, "500, unsupported commands.");
        } else if (strcasecmp("C", args) == 0) {
          send_msg(*client_fd, "500, unsupported commands.");
        } else {
          send_msg(*client_fd, "504 Command not implemented for that parameter. Only support for 'S' ");
        }
      } else{
        send_msg(*client_fd, "501 Syntax error in parameters or arguments.");
      }
    } else if (strcasecmp(tok, "STRU") == 0) {
      // STRU <SP> <structure-code> <CRLF>
      // <structure-code> ::= F | R | P
      if (!logged) {
        send_msg(*client_fd, "530 cs317 is the only username. Not logged in.");
        continue;
      }

      args = strtok(NULL, " ");
      if (args && strtok(NULL, " ") == NULL) {
        if (strcasecmp("F", args) == 0) {
          send_msg(*client_fd, "200 data structure changed to file.");
        } else if (strcasecmp("R", args) == 0) {
          send_msg(*client_fd, "500, unsupported commands.");
        } else if (strcasecmp("P", args) == 0) {
          send_msg(*client_fd, "500, unsupported commands.");
        } else {
          send_msg(*client_fd, "504 Command not implemented for that parameter. Only support for 'F' ");
        }
      } else{
        send_msg(*client_fd, "501 Syntax error in parameters or arguments.");
      }

    } else if (strcasecmp(tok, "RETR") == 0) {
      // RETR <SP> <pathname> <CRLF>
      if (!logged) {
        send_msg(*client_fd, "530 cs317 is the only username. Not logged in.");
        continue;
      }
      args = strtok(NULL, " ");
      if (args && strtok(NULL, " ") == NULL) {
        if (isPasv == 0) {
          send_msg(*client_fd, "425 please change to pasv mode");
          continue;
        }

        // check dir
        DIR* check_dir = opendir(args);
        if (check_dir) {
            /* Directory exists. */
            closedir(check_dir);
        } else if (ENOENT == errno) {
            /* Directory does not exist. */
            send_msg(*client_fd, "550 Requested action not taken. File does not exist");
        } else {
            /* opendir() failed for some other reason. */
            send_msg(*client_fd, "550 Requested action not taken. Open file failed.");
        }

        FILE* curr_file;
        if (openType == 1) {
          curr_file = fopen(args, "rb");
        } else { // default case
          curr_file = fopen(args, "r");
        }
        
        send_msg(*client_fd, "150 File status okay; about to open data connection.");
        struct sockaddr_in data_addr;
        socklen_t data_addr_len = sizeof(data_addr);
        int data_client_fd = accept(data_socket, (struct sockaddr *) &data_addr, &data_addr_len);

        if (data_client_fd < 0) {
          send_msg(*client_fd, "425 please change to pasv mode");
          continue;
        }
        
        char read_buf[4096];
        int count = fread(&read_buf, sizeof(char), 4096, curr_file);
        while (count != 0) {
          if (ferror(curr_file)) {
            send_msg(*client_fd, "451 File open error");
            break;
          }
          dprintf(data_client_fd, "%s", read_buf);
          count = fread(&read_buf, sizeof(char), 4096, curr_file);
          if (count < 0) {
            send_msg(*client_fd, "451 File open error");
          }
        }
        fclose(curr_file);
        send_msg(*client_fd, "226 closing the data connection.");
        close(data_client_fd);
        close(data_socket);
        isPasv = 0;
      } else{
        send_msg(*client_fd, "501 Syntax error in parameters or arguments.");
      }

    } else if (strcasecmp(tok, "PASV") == 0) {
      // PASV <CRLF>
      if (!logged) {
        send_msg(*client_fd, "530 cs317 is the only username. Not logged in.");
        continue;
      }

      if (strtok(NULL, " ") == NULL) {
        // create data socket
        data_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (data_socket == -1) {
          perror("create data socket failed");
          exit(0);
        }

        // ref for get ip for interface: https://stackoverflow.com/questions/4139405/how-can-i-get-to-know-the-ip-address-for-interfaces-in-c

        struct ifaddrs *ifap, *ifa;
        struct sockaddr_in *sa;
        char *addr;

        // creates a linked list of structures
        //  describing the network interfaces of the local system, and stores
        //  the address of the first item of the list in *ifap.
        getifaddrs (&ifap);
        for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && strncmp(ifa->ifa_name, "lo", 2)) {
                sa = (struct sockaddr_in *) ifa->ifa_addr;
                addr = inet_ntoa(sa->sin_addr);
                printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
            }
        }
        freeifaddrs(ifap);
        

        struct sockaddr_in bind_addr;
        bzero(&bind_addr, sizeof(bind_addr));
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_port = htons(0);
        bind_addr.sin_addr.s_addr = inet_addr(addr);
        

        //bind
        if (bind(data_socket, (struct sockaddr*) &bind_addr, sizeof(bind_addr)) != 0) {
          perror("bind failed:%d\n");
          exit(0);
        } else {
          printf("Binding data socket successfully.\n");
        }
        // get data_socket port num
        // ref: https://stackoverflow.com/questions/4046616/sockets-how-to-find-out-what-port-and-address-im-assigned
        struct sockaddr_in sin;
        socklen_t len = sizeof(sin);
        if (getsockname(data_socket, (struct sockaddr *)&sin, &len) == -1) {
            perror("getsockname failed.");
        } else {
            printf("port number %d\n", ntohs(sin.sin_port));
        }

        int data_socket_port = ntohs(sin.sin_port);

        // listen for data transfer
        if (listen(data_socket, 3) < 0) {
          perror("listen failed");
          exit(0);
        }


        char msg[1024];
        char* ip1 = strtok(addr, ".");
        char* ip2 = strtok(NULL, ".");
        char* ip3 = strtok(NULL, ".");
        char* ip4 = strtok(NULL, ".");
        sprintf(msg, "227 passive mode (%s.%s.%s.%s. %d,%d)", ip1, ip2, ip3, ip4, data_socket_port / 256, data_socket_port % 256);

        send_msg(*client_fd, msg);
        isPasv = 1;
      } else {
        send_msg(*client_fd, "501 Syntax error in parameters or arguments.");
      }

    } else if (strcasecmp(tok, "NLST") == 0) {
      if (!logged) {
        send_msg(*client_fd, "530 cs317 is the only username. Not logged in.");
        continue;
      }

      if (strtok(NULL, " ") == NULL) {
        if (isPasv == 0) {
          send_msg(*client_fd, "425 please change to pasv mode");
          continue;
        }
        char cur_dir[1024];
        if (getcwd(cur_dir, 1024) == NULL) {
          send_msg(*client_fd, "550 Requested action not taken.");
        } else {
          struct sockaddr_in data_addr;
          socklen_t data_addr_len = sizeof(data_addr);
          int data_client_fd = accept(data_socket, (struct sockaddr *) &data_addr, &data_addr_len);

          if (data_client_fd < 0) {
            send_msg(*client_fd, "425 please change to pasv mode");
            continue;
          }
          printf("data_client requested accepted from %s %d\n", inet_ntoa(data_addr.sin_addr), data_addr.sin_port);

          send_msg(*client_fd, "150 File status okay. Open data connection.");
          listFiles(data_client_fd, cur_dir);
          send_msg(*client_fd, "226 closing the data connection.");
          close(data_client_fd);
          close(data_socket);
          isPasv = 0;
        }
      } else {
        send_msg(*client_fd, "501 Syntax error in parameters or arguments.");
      } 
    } else {
      send_msg(*client_fd, "500 Syntax error, command unrecognized.");
    }
   } return NULL;
  }
 




int main(int argc, char **argv) {
  // Check the command line arguments
  if (argc != 2) {
      usage(argv[0]);
      return -1;
    }

    int portNum = atoi(argv[1]);

    if (portNum < 1024 || portNum > 65535) {
      usage(argv[0]);
      return -1;
    }

  // ref: https://www.youtube.com/watch?v=HWVVEa3seXE
  // initial socket 
  int serfd;
	serfd = socket(AF_INET , SOCK_STREAM , 0);
	
	if (serfd == -1) {
		perror("Could not create socket");
    exit(0);
	} else {
    printf("Server socket created.\n");
  }
  
  struct sockaddr_in serAddr;
  bzero(&serAddr, sizeof(serAddr));
  serAddr.sin_family = AF_INET;
  serAddr.sin_port = htons(portNum);
  serAddr.sin_addr.s_addr = INADDR_ANY;
  // char* ip = "127.0.0.1";
  // serAddr.sin_addr.s_addr = inet_addr(ip);

  //bind
  if (bind(serfd, (struct sockaddr*) &serAddr, sizeof(serAddr)) != 0) {
    perror("bind failed:%d\n");
    exit(0);
  } else {
    printf("Binding successful.\n");
  }


  // listen
  // The backlog, defines the maximum length 
  // to which the queue of pending connections for sockfd may grow.
  if(listen(serfd, 10) != 0) {
    perror("listen failed:%d\n");
    exit(0);
  } else {
    printf("Listening...\n");
  }
  

  // Determines the path name of the working directory and stores it in dir.
  getcwd(dir, 256);

  //accept
  struct sockaddr_in client_addr;
  while(1) {
    socklen_t len = sizeof(client_addr);
    int new_fd = accept(serfd, (struct sockaddr *) &client_addr, &len);
    if (new_fd < 0) {
      perror("accept failed:%d\n");
      exit(0);
      continue;
    }
    printf("Client requested accepted from %s %d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

    send_msg(new_fd, "220, Service ready for new user.");

    pthread_t child;
    if (pthread_create(&child, NULL, recv_handler, &new_fd) != 0) {
      perror("carete therad failed.\n");
      exit(0);
    }

    // wait for child thread
    pthread_join(child, NULL);
    printf("child thread finished\n");
  }

  //close socket
  close(serfd);
    // close(new_fd);

    // This is how to call the function in dir.c to get a listing of a directory.
    // It requires a file descriptor, so in your code you would pass in the file descriptor 
    // returned for the ftp server's data connection
    
    printf("Printed %d directory entries\n", listFiles(1, "."));
    return 0;
}