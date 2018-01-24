/*
 * CS214: Systems Programming, Fall 2016
 * Assignment 3
 * Baljit Kaur
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>

#define port 16896

typedef enum{
	O_RDONLY,
	O_WRONLY, 
	O_RDWR
}flag;

typedef enum{
	unrestricted,
	exclusive,
	transaction
}filemode;

int netopen(const char *pathname, int flags);
ssize_t netread(int fildes, void *buf, size_t nbyte);
ssize_t netwrite(int fildes, const void *buf, size_t nbyte);
int netclose(int fd);
int netserverinit(char * hostname, int filemode);
struct sockaddr_in serverAddressInfo;
struct hostent *host;