#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include "dir.h"
#include "usage.h"
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


int main(int argc, char **argv) {
  char* ip = "127.0.0.1";
  int port = 8888;
  int e;

  int sockfd;
  struct sockaddr_in server_addr;
  FILE* fp;
  char* filename = "file1.txt";

	sockfd = socket(AF_INET , SOCK_STREAM , 0);
	
	if (sockfd == -1) {
		perror("Could not create socket");
    exit(0);
	} else {
    printf("Server socket created.\n");
  }
  
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port); 
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr));
  if (e == -1) {
    perror("Error in connecting");
    exit(1);
  } else {
    printf("connected to the server.\n");
  }

  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("Error in opening file");
    exit(1);
  }
}
