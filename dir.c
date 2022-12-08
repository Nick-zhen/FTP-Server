#include "dir.h"
#include <string.h>
#include <netinet/in.h>

/* 
   Arguments: 
      fd - a valid open file descriptor. This is not checked for validity
           or for errors with it is used.
      directory - a pointer to a null terminated string that names a 
                  directory

   Returns
      -1 the named directory does not exist or you don't have permission
         to read it.
      -2 insufficient resources to perform request

 
   This function takes the name of a directory and lists all the regular
   files and directoies in the directory. 
 

 */

int listFiles(int fd, char * directory) {

  // Get resources to see if the directory can be opened for reading
  
  DIR * dir = NULL;
  
  dir = opendir(directory);
  if (!dir) return -1;
  
  // Setup to read the directory. When printing the directory
  // only print regular files and directories. 

  struct dirent *dirEntry;
  int entriesPrinted = 0;
  
  for (dirEntry = readdir(dir);
       dirEntry;
       dirEntry = readdir(dir)) {
    if (dirEntry->d_type == DT_REG) {  // Regular file
      struct stat buf;

      // This call really should check the return value
      // stat returns a structure with the properties of a file
      stat(dirEntry->d_name, &buf);

      dprintf(fd, "F    %-20s     %ld\n", dirEntry->d_name, buf.st_size);
    } else if (dirEntry->d_type == DT_DIR) { // Directory
      dprintf(fd, "D        %s\n", dirEntry->d_name);
    } else {
      dprintf(fd, "U        %s\n", dirEntry->d_name);
    }
    entriesPrinted++;
  }
  
  // Release resources
  closedir(dir);
  return entriesPrinted;
}

// For security reasons you are not accept any CWD command that starts with ./ or ../ or contains ../ in it
int check_valid_dir(char* dir) {
  if (!strncmp(dir, "./", 2) || !strncmp(dir, "../", 3) || strstr(dir, "../")) {
    return 0;
  } else {
    return 1;
  }
}

// ref: https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
// my note: it will also trim the \n
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

void send_msg(int socket_fd, char* msg) {
  // plus \n and \0
  int len = strlen(msg) + 2;
  char buf[len];
  strcpy(buf, msg);
  buf[len - 2] = '\n';
  if (send(socket_fd, buf, len-1, 0) != len-1) {
    perror("send msg failed.\n");
  }
}
