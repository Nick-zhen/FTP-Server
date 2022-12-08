#ifndef _DIRH__
#define _DIRH__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>

//function prototypes
int listFiles(int, char*);
int check_valid_dir(char* dir);
char *trimwhitespace(char *str);
void send_msg(int socket_fd, char* msg);
#endif
