 /*
 * CS214: Systems Programming, Fall 2016
 * Assignment 3
 * Baljit Kaur */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define port 16896

void *thread(void *arg);

typedef struct File{
	int fildescrip;
	int read;
	int write;
	int isTra;
	int isExc;
	char *path;
}file_t;

file_t filarr[256];

void error(char *msg)
{
    perror(msg);
    //exit(1);
}

int main(int argc, char *argv[]) {
	// create socket descriptor
	int portno = port;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    	if (sockfd < 0){
     		error("ERROR opening socket");
    	}

	// eliminate "address already in use" error from bind
    	int optval = 1;
    	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)) < 0) {
    		return -1;
    		error("ERROR address already in use");
    	}
	
	struct sockaddr_in serverAddressInfo;
	bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_port = htons(portno);
    	serverAddressInfo.sin_family = AF_INET;	
   	serverAddressInfo.sin_addr.s_addr = INADDR_ANY;
    	int binderr = bind(sockfd, (struct sockaddr *) &serverAddressInfo, sizeof(serverAddressInfo));
    	if (binderr < 0){
		error("ERROR on binding");	
	}
	
	// make listening socket ready to accept connection requests
	int listenerr = listen(sockfd, 5);
	if (listenerr < 0){
		error("ERROR on listening");		
	}	
	pthread_t tid;
	struct sockaddr_in clientAddressInfo;

	// wait for connection requests
	while(1){
    		int clilen = sizeof(clientAddressInfo);	// determine the size of a clientAddressInfo struct
    		int *newsockfd = malloc(sizeof(int));
		*newsockfd = accept(sockfd, (struct sockaddr *) &clientAddressInfo, &clilen);	// block until a client connects, when it does, create a client socket
    		if (newsockfd < 0){
			error("ERROR on accept");
		}
		// spawn worker threads
		pthread_create(&tid, NULL, thread, (void *)newsockfd);
	}
	
	return 0;
}

void *thread(void *arg){
	int newsockfd = *((int *)arg);
	//pthread_detach(pthread_self());
	char buffer[256];
    	bzero(buffer, 256);
    	int n = read(newsockfd, buffer, 255);
    	if (n < 0)
		error("ERROR reading from socket");
    	printf("Here is the message: %s\n",buffer);
	char op[] = "open";
	char rd[] = "read";
	char wr[] = "write";
	char cl[] = "close";
	if(strncmp(op, buffer, 4) == 0){
		char filepath[256];
		strcpy(filepath, buffer + 10);
		char filemode[3];
		strncpy(filemode, buffer + 5, 2);
		filemode[2] = '\0';
		char filperm[2];
		strncpy(filperm, buffer + 8, 1);
		filperm[1] = '\0';
		//printf("file perm: %s\n", filperm);
		chdir("/..");
		int fdes = -1;
		if(strcmp(filemode, "ro") == 0){
			//printf("path %s\n", filepath);
			int h = fileExists(filepath);
			//printf("h: %d\n",h);
			if(h == -1){
				fdes = open(filepath, O_RDONLY);
				if(fdes < 0){
					int err = errno;
					char stre[4];
					sprintf(stre, "%d", err);
					n = write(newsockfd, stre, 3);
					error("ERROR file could not be opened");
				}else{
					filarr[fdes].fildescrip = fdes;
					filarr[fdes].read = -1;
					filarr[fdes].write = -1;
					filarr[fdes].isTra = -1;
					filarr[fdes].isExc = -1;
					filarr[fdes].path = malloc(sizeof(strlen(filepath)));
					strcpy(filarr[fdes].path, filepath);
					//printf("filepath: %s\n", filarr[fdes].path);
				}
			}else if(h > -1){
				//fdes = filarr[h].fildescrip;
				if(strcmp(filperm, "0") == 0){	
					if(filarr[h].isTra == 1){
						int err = 1; //EPERM
						char stre[4];
						sprintf(stre, "%d", err);
						n = write(newsockfd, stre, 3);
						error("ERROR file could not be opened");
					}
				}
			}
			//set file_t info

			filarr[h].read = 1;

			if(filarr[h].write == -1){
				filarr[h].write = 0;
			}
			if(filarr[h].isTra == -1){
				filarr[h].isTra = 0;
			}
			if(filarr[h].isExc == -1){
				filarr[h].isExc = 0;
			}
		}else if(strcmp(filemode, "wo") == 0){	
			//printf("path %s\n", filepath);
			int h = fileExists(filepath);
			//printf("h: %d\n",h);
			if(h == -1){
				fdes = open(filepath, O_WRONLY);
				if(fdes < 0){
					int err = errno;
					char stre[4];
					sprintf(stre, "%d", err);
					n = write(newsockfd, stre, 3);
					error("ERROR file could not be opened");
				}else{
					filarr[fdes].fildescrip = fdes;
					filarr[fdes].read = -1;
					filarr[fdes].write = -1;
					filarr[fdes].isTra = -1;
					filarr[fdes].isExc = -1;
					filarr[fdes].path = malloc(sizeof(strlen(filepath)));
					strcpy(filarr[fdes].path, filepath);
					//printf("filepath: %s\n", filarr[fdes].path);
				}
			}else if(h > -1){
				//fdes = filarr[h].fildescrip;
				if(strcmp(filperm, "0") == 0){	
					if(filarr[h].isTra == 1){
						int err = 1; //EPERM
						char stre[4];
						sprintf(stre, "%d", err);
						n = write(newsockfd, stre, 3);
						error("ERROR file could not be opened");
					}else if((filarr[h].isExc == 1) && (filarr[h].write == 1)){
						int err = 1; //EPERM
						char stre[4];
						sprintf(stre, "%d", err);
						n = write(newsockfd, stre, 3);
						error("ERROR file could not be opened");
					}
				}else if(strcmp(filperm, "1") == 0){
					if(filarr[h].isTra == 1){
						int err = 1; //EPERM
						char stre[4];
						sprintf(stre, "%d", err);
						n = write(newsockfd, stre, 3);
						error("ERROR file could not be opened");
					}else if(filarr[h].write == 1){
						int err = 1; //EPERM
						char stre[4];
						sprintf(stre, "%d", err);
						n = write(newsockfd, stre, 3);
						error("ERROR file could not be opened");
					}
				}else if(strcmp(filperm, "2") == 0){
					if((filarr[h].write == 1) || (filarr[h].read == 1)){
						int err = 1; //EPERM
						char stre[4];
						sprintf(stre, "%d", err);
						n = write(newsockfd, stre, 3);
						error("ERROR file could not be opened");
					}
				}
			}
			//set file_t info
			filarr[h].write = 1;

			if(filarr[h].read == -1){
				filarr[h].read = 0;
			}
			if(filarr[h].isTra == -1 && (strcmp(filperm, "2") == 0)){ //
				filarr[h].isTra = 1;
			}else if(strcmp(filperm, "2") == 0){
				filarr[h].isTra = 1;
			}else if(filarr[h].isTra == -1){
				filarr[h].isTra = 0;
			}
			if(filarr[h].isExc == -1 && (strcmp(filperm, "1") == 0)){
				filarr[h].isExc = 1;
			}else if(strcmp(filperm, "1") == 0){
				filarr[h].isExc = 1;
			}else if(filarr[h].isExc == -1){
				filarr[h].isExc = 0;
			}
		}else if(strcmp(filemode, "rw") == 0){
			//printf("path %s\n", filepath);
			int h = fileExists(filepath);
			//printf("h: %d\n",h);
			if(h == -1){
				fdes = open(filepath, O_RDWR);
				if(fdes < 0){
					int err = errno;
					char stre[4];
					sprintf(stre, "%d", err);
					n = write(newsockfd, stre, 3);
					error("ERROR file could not be opened");
				}else{
					filarr[fdes].fildescrip = fdes;
					filarr[fdes].read = -1;
					filarr[fdes].write = -1;
					filarr[fdes].isTra = -1;
					filarr[fdes].isExc = -1;
					filarr[fdes].path = malloc(sizeof(strlen(filepath)));
					strcpy(filarr[fdes].path, filepath);
					//printf("filepath: %s\n", filarr[fdes].path);
				}
			}else if(h > -1){
				//fdes = filarr[h].fildescrip;
				if(strcmp(filperm, "0") == 0){	
					if(filarr[h].isTra == 1){
						int err = 1; //EPERM
						char stre[4];
						sprintf(stre, "%d", err);
						n = write(newsockfd, stre, 3);
						error("ERROR file could not be opened");
					}else if((filarr[h].isExc == 1) && (filarr[h].write == 1)){
						int err = 1; //EPERM
						char stre[4];
						sprintf(stre, "%d", err);
						n = write(newsockfd, stre, 3);
						error("ERROR file could not be opened");
					}
				}else if(strcmp(filperm, "1") == 0){
					if(filarr[h].isTra == 1){
						int err = 1; //EPERM
						char stre[4];
						sprintf(stre, "%d", err);
						n = write(newsockfd, stre, 3);
						error("ERROR file could not be opened");
					}else if(filarr[h].write == 1){
						int err = 1; //EPERM
						char stre[4];
						sprintf(stre, "%d", err);
						n = write(newsockfd, stre, 3);
						error("ERROR file could not be opened");
					}
				}else if(strcmp(filperm, "2") == 0){
					if((filarr[h].write == 1) || (filarr[h].read == 1)){
						int err = 1; //EPERM
						char stre[4];
						sprintf(stre, "%d", err);
						n = write(newsockfd, stre, 3);
						error("ERROR file could not be opened");
					}
				}
			}
			//set file_t info
			filarr[h].write = 1;
			filarr[h].read = 1;
			if(filarr[h].isTra == -1 && (strcmp(filperm, "2") == 0)){
				filarr[h].isTra = 1;
			}else if(strcmp(filperm, "2") == 0){
				filarr[h].isTra = 1;
			}else if(filarr[h].isTra == -1){
				filarr[h].isTra = 0;
			}
			if(filarr[h].isExc == -1 && (strcmp(filperm, "1") == 0)){
				filarr[h].isExc = 1;
			}else if(strcmp(filperm, "1") == 0){
				filarr[h].isExc = 1;
			}else if(filarr[h].isExc == -1){
				filarr[h].isExc = 0;
			}
		}else{
			error("ERROR invalid file mode");
			n = write(newsockfd,"I got your message",18);
		}
		char str[10];
		sprintf(str, "%d", fdes);
		strcat(str, "_");
		n = write(newsockfd, str, 10);
		if (n < 0)
			error("ERROR writing to socket");
	}else if(strncmp(rd, buffer, 4) == 0){
		char fildes[5];
		char bytes[5];
		const char c[2] = "_";
		char *token;
		token = strtok(buffer + 5, c);
		strcpy(fildes, token);
		token = strtok(NULL, c);
		strcpy(bytes, token);
		int fd = atoi(fildes);
		//printf("fd: %d\n", fd);
		int byt = atoi(bytes);
		//printf("byt: %d\n", byt);
		char *buf = malloc(sizeof(char) * byt);
		int r = read(fd, buf, byt);
		if (r < 0){
			int err = errno;
			char stre[4];
			sprintf(stre, "%d", err);
			n = write(newsockfd, stre, 3);
			error("ERROR file could not be read");
		}else{
			char str[5 + byt];
			sprintf(str, "%d", r);
			strcat(str, "_");
			strcat(str, buf);
			n = write(newsockfd, str, 5);
			//printf("bytes read: %d\n", r);
			//printf("buf: %s\n", buf);
		}
	}else if(strncmp(wr, buffer, 5) == 0){
		char fildes[5];
		char bytes[5];
		char length[5];
		char *buf;
		const char c[2] = "_";
		char *token;
		token = strtok(buffer + 5, c);
		strcpy(fildes, token);
		token = strtok(NULL, c);
		strcpy(bytes, token);
		token = strtok(NULL, c);
		strcpy(length, token);
		token = strtok(NULL, c);
		int len = atoi(length);
		buf = malloc(sizeof(char) * len);
		strcpy(buf, token);
		int fd = atoi(fildes);
		int byt = atoi(bytes);
		int wr = write(fd, buf, byt);
		if (wr < 0){
			int err = errno;
			char stre[4];
			sprintf(stre, "%d", err);
			n = write(newsockfd, stre, 3);
			error("ERROR could not write to file");
		}else{
			char str[6];
			sprintf(str, "%d", wr);
			strcat(str, "_");
			n = write(newsockfd, str, 5);
		}
	}else if(strncmp(cl, buffer, 5) == 0){
		char fildes[5];
		strcpy(fildes, buffer + 6);
		printf("close fildes: %s\n", fildes); 
		int fd = atoi(fildes);
		printf("fd: %d\n", fd);
		int succ = close(fd);
		printf("succ: %d\n", succ);
		if(succ < 0){
			int err = errno;
			char stre[4];
			sprintf(stre, "%d", err);
			n = write(newsockfd, stre, 3);
			error("ERROR file could not be closed");
		}else{
			n = write(newsockfd, "success", 7);
			if (n < 0)
			error("ERROR writing to socket");
		}
	}else{
		n = write(newsockfd,"Invalid operation requested",29);
	}
	if (n < 0){
		error("ERROR writing to socket");
	}
	close(newsockfd);
	free(arg);
	return NULL;
}

int fileExists(char *path){
	int i;
	for(i = 0; i < 100; i++){	
		if(filarr[i].path != NULL){	
			if(strcmp(filarr[i].path, path) == 0){
				return i;
			}
		}
	}
	return -1;
}
