/*
 * CS214: Systems Programming, Fall 2016
 * Assignment 3
 * Baljit Kaur */

#include "libnetfiles.h"
int filmode;

void error(char *msg)
{
    perror(msg);
    //exit(0);
}

int main(int argc, char *argv[]) {
	char *buf;
	char buf2[] = "hello";
	netserverinit("grep.cs.rutgers.edu", exclusive);
	int fd = netopen("/.autofs/ilab/ilab_users/bk404/test.txt", O_RDWR);
	int bytesread = netread(fd, buf, 2); 
	int byteswritten = netwrite(fd, buf2, 3);
	printf("File Descriptor: %d\n", fd);
	printf("Bytes Read: %d\n", bytesread);
	printf("Bytes Written: %d\n", byteswritten);
	if(errno > 0){
		printf("ERROR: %s\n", strerror(errno));
	}
	//netclose(fd);
	return 0;
}

int netopen(const char *pathname, int flags){
	flag f = flags;
	if (host == NULL){
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	if (f == O_RDONLY){
		int portno = port;	
    		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			error("ERROR creating socket");
			return -1;
		}
		bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
		serverAddressInfo.sin_port = htons(portno);
		serverAddressInfo.sin_family = AF_INET;
		bcopy((char *)host->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, host->h_length);
		int connecterr = connect(sockfd,(struct sockaddr *)&serverAddressInfo,sizeof(serverAddressInfo));
		if (connecterr < 0) 
       			error("ERROR connecting");
        	char buffer[256] = "open_ro_";
		char fm[2];
		sprintf(fm, "%d", filmode);
		strcat(buffer, fm);
		strcat(buffer, "_");
		strcat(buffer, pathname);
		int n = write(sockfd,buffer,strlen(buffer));	// try to write it out to the server
		if (n < 0)
       			error("ERROR writing to socket");
		bzero(buffer,256);	// sent message to the server, zero the buffer back out to read the server's response
		n = read(sockfd,buffer,255);	// read a message from the server into the buffer
	    	if (n < 0)
        		error("ERROR reading from socket");
       		//printf("%s\n",buffer);
		char *test = strstr(buffer, "_");
		if(test == NULL){
			int b = atoi(buffer);
			errno = b;
			return -1;
		}else{
			char bytes[5];
			const char c[2] = "_";
			char *token;
			token = strtok(buffer, c);
			strcpy(bytes, token);
			int fd = atoi(bytes);
			return fd;
		}
	}else if(f == O_WRONLY){
		int portno = port;	
	    	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			error("ERROR creating socket");
			return -1;
		}
		bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
		serverAddressInfo.sin_port = htons(portno);
		serverAddressInfo.sin_family = AF_INET;
		bcopy((char *)host->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, host->h_length);
		int connecterr = connect(sockfd,(struct sockaddr *)&serverAddressInfo,sizeof(serverAddressInfo));
		if (connecterr < 0) 
       			error("ERROR connecting");
	        char buffer[256] = "open_wo_";
		char fm[2];
		sprintf(fm, "%d", filmode);
		strcat(buffer, fm);
		strcat(buffer, "_");
		strcat(buffer, pathname);
		int n = write(sockfd,buffer,strlen(buffer));	// try to write it out to the server
		if (n < 0)
        		error("ERROR writing to socket");
		bzero(buffer,256);	// sent message to the server, zero the buffer back out to read the server's response
		n = read(sockfd,buffer,255);	// read a message from the server into the buffer
    		if (n < 0)
        		error("ERROR reading from socket");
       		//printf("%s\n",buffer);
       		char *test = strstr(buffer, "_");
		if(test == NULL){
			int b = atoi(buffer);
			errno = b;
			return -1;
		}else{
			char bytes[5];
			const char c[2] = "_";
			char *token;
			token = strtok(buffer, c);
			strcpy(bytes, token);
			int fd = atoi(bytes);
			return fd;
		}
	}else if(f == O_RDWR){
		int portno = port;	
    		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			error("ERROR creating socket");
			return -1;
		}
		bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
		serverAddressInfo.sin_port = htons(portno);
		serverAddressInfo.sin_family = AF_INET;
		bcopy((char *)host->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, host->h_length);
		//printf("%d\n", host->h_addr);
		int connecterr = connect(sockfd,(struct sockaddr *)&serverAddressInfo,sizeof(serverAddressInfo));
		//printf("%d\n", connecterr);
		if (connecterr < 0) 
        		error("ERROR connecting");
        	char buffer[256] = "open_rw_";
		char fm[2];
		sprintf(fm, "%d", filmode);
		strcat(buffer, fm);
		strcat(buffer, "_");
		strcat(buffer, pathname);
		int n = write(sockfd,buffer,strlen(buffer));	// try to write it out to the server
		if (n < 0)
        		error("ERROR writing to socket");
		bzero(buffer,256);	// sent message to the server, zero the buffer back out to read the server's response
		n = read(sockfd,buffer,255);	// read a message from the server into the buffer
    		if (n < 0)
       			error("ERROR reading from socket");
        	//printf("%s\n",buffer);
        	char *test = strstr(buffer, "_");
		if(test == NULL){
			//printf("check 1\n");
			int b = atoi(buffer);
			//printf("b: %d\n", b);
			errno = b;
			return -1;
		}else{
			char bytes[5];
			const char c[2] = "_";
			char *token;
			token = strtok(buffer, c);
			strcpy(bytes, token);
			int fd = atoi(bytes);
			return fd;
		}
	}
	return -1;
}

ssize_t netread(int fildes, void *buf, size_t nbyte){
	int portno = port;	
    	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR creating socket");
		return -1;
	}
	bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_port = htons(portno);
	serverAddressInfo.sin_family = AF_INET;
	bcopy((char *)host->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, host->h_length);
	//printf("%d\n", host->h_addr);
	int connecterr = connect(sockfd,(struct sockaddr *)&serverAddressInfo,sizeof(serverAddressInfo));
	//printf("%d\n", connecterr);
	if (connecterr < 0) 
        	error("ERROR connecting");
        char buffer[256] = "read_";
	char fd[5];
	sprintf(fd, "%d", fildes);
	strcat(buffer, fd);
	char sepr[] = "_";
	strcat(buffer, sepr);
	char nbyt[5];
	sprintf(nbyt, "%d", nbyte);
	strcat(buffer, nbyt);
	int n = write(sockfd,buffer,strlen(buffer));	// try to write it out to the server
	if (n < 0)
        	error("ERROR writing to socket");
	bzero(buffer,256);	// sent message to the server, zero the buffer back out to read the server's response
	n = read(sockfd,buffer, 255);	// read a message from the server into the buffer
    	if (n < 0)
       		error("ERROR reading from socket");
        //printf("here: %s\n",buffer);
	char *test = strstr(buffer, "_");
	if(test == NULL){
		int b = atoi(buffer);
		errno = b;
	}else{
		char bytes[5];
		const char c[2] = "_";
		char *token;
		token = strtok(buffer, c);
		strcpy(bytes, token);
		int byt = atoi(bytes);
		return byt;
	}
	return -1;	
}

ssize_t netwrite(int fildes, const void *buf, size_t nbyte){
	int portno = port;	
    	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR creating socket");
		return -1;
	}
	bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_port = htons(portno);
	serverAddressInfo.sin_family = AF_INET;
	bcopy((char *)host->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, host->h_length);
	//printf("%d\n", host->h_addr);
	int connecterr = connect(sockfd,(struct sockaddr *)&serverAddressInfo,sizeof(serverAddressInfo));
	//printf("%d\n", connecterr);
	if (connecterr < 0) 
        	error("ERROR connecting");
        char buffer[1000] = "write_";
	char fd[5];
	sprintf(fd, "%d", fildes);
	strcat(buffer, fd);
	char sepr[] = "_";
	strcat(buffer, sepr);
	char nbyt[5];
	sprintf(nbyt, "%d", nbyte);
	strcat(buffer, nbyt);
	strcat(buffer, sepr);
	int len = strlen(buf);
	char l[5];
	sprintf(l, "%d", len);
	strcat(buffer, l);
	strcat(buffer, sepr);
	strcat(buffer, buf);
	int n = write(sockfd,buffer,strlen(buffer));	// try to write it out to the server
	if (n < 0)
        	error("ERROR writing to socket");
	bzero(buffer,1000);	// sent message to the server, zero the buffer back out to read the server's response
	n = read(sockfd,buffer, 999);	// read a message from the server into the buffer
    	if (n < 0)
       		error("ERROR reading from socket");
        //printf("here: %s\n",buffer);
	char *test = strstr(buffer, "_");
	if(test == NULL){
		int b = atoi(buffer);
		errno = b;
	}else{
		char bytes[5];
		const char c[2] = "_";
		char *token;
		token = strtok(buffer, c);
		strcpy(bytes, token);
		int byt = atoi(bytes);
		return byt;
	}
	return -1;
}

int netclose(int fd){
	int portno = port;	
    	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR creating socket");	
		return -1;
	}
	bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_port = htons(portno);
	serverAddressInfo.sin_family = AF_INET;
	bcopy((char *)host->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, host->h_length);
	//printf("%d\n", host->h_addr);
	int connecterr = connect(sockfd,(struct sockaddr *)&serverAddressInfo,sizeof(serverAddressInfo));
	//printf("%d\n", connecterr);
	if (connecterr < 0) 
        	error("ERROR connecting");
        char buffer[100] = "close_";
	char fildes[5];
	sprintf(fildes, "%d", fd);
	strcat(buffer, fildes);
	int n = write(sockfd,buffer,strlen(buffer));	// try to write it out to the server
	if (n < 0)
        	error("ERROR writing to socket");
	bzero(buffer,100);	// sent message to the server, zero the buffer back out to read the server's response
	n = read(sockfd,buffer,99);	// read a message from the server into the buffer
    	if (n < 0)
       		error("ERROR reading from socket");
        //printf("%s\n",buffer);
	if(strcmp(buffer, "success") == 0){
		return 0;
	}
	else{
		int i = atoi(buffer);
		errno = i;
	}
	return -1;
}

int netserverinit(char * hostname, int filemode){
	host = malloc(sizeof(struct hostent));
	host = gethostbyname(hostname);
	printf("Host: %s\n", host->h_name); //check hostname
	if(host->h_name == NULL){
		return -1;
	}
	filmode = filemode;
	return 0;
}
