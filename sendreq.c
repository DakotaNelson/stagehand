/* Reads in a file, then sends that file as a HTTP request to the host
 * specified in the file's "Host:" header.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

void fatal(char *);

int main() {
  // open up the file containing our request to send
  int fd, fsize;
  struct stat st;
  char *request;
  char *req_file = "./request.txt";

  // get the size of the file
  stat(req_file, &st);
  fsize = st.st_size;
  printf("file size is %i\n", fsize);

  if((request = (char *) malloc(fsize+1)) == NULL) {
    fatal("allocating memory for file");
  }

  // open the file
  fd = open(req_file, O_RDONLY);
  if(fd == -1) {
    fatal("opening file");
  }

  // read the request from the file into the 'request' variable
  read(fd, request, fsize);
  request[fsize+1] = 0;

  printf("REQUEST:\n--------------------\n%s\n", request);

  // we now have the request in memory, we don't need the file
  close(fd);

  // open up a socket
  int sockfd;
  int hostlen;
  char *hostloc;
  char *hostloc_newline;
  char *host;
  struct hostent *host_info;
  struct sockaddr_in target_addr;
  unsigned char response[8192];

  // request MUST have a "Host:" header
  if((hostloc = strstr(request, "Host: ")) == NULL) {
    fatal("no host header found");
  }

  hostloc += 6; // ignore the "Host: "

  // find the end of the header
  if((hostloc_newline = strstr(hostloc, "\n")) == NULL) {
    fatal("no newline found after host header");
  }

  hostlen = hostloc_newline-hostloc;
  if((host = (char *) malloc(hostlen+1)) == NULL) {
    fatal("allocating memory for hostname");
  }

  strncpy(host, hostloc, hostlen);

  printf("Host is: %s\n", host);

  if((host_info = gethostbyname(host)) == NULL) {
    fatal("in hostname lookup");
  }

  if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    fatal("in socket");
  }

  target_addr.sin_family = AF_INET;
  target_addr.sin_port = htons(80);
  target_addr.sin_addr = *((struct in_addr *)host_info->h_addr);
  memset(&(target_addr.sin_zero), '\0', 8);

  if(connect(sockfd,
             (struct sockaddr *)&target_addr,
             sizeof(struct sockaddr)
            ) == -1) {
    fatal("connecting to target server");
  }

  send(sockfd, request, strlen(request), 0);
  
  while(recv(sockfd, response, 8192, 0)) {
    printf("%s", response);
  }

  free(host);
  free(request);
  return 0;
}

void fatal(char *message) {
  printf("%s\n", message);
  exit(-1);
}
