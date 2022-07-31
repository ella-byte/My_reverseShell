#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <readline/readline.h> // better input system
#define MAX 4048 // 4kb buffer
#define SA struct sockaddr

void handle(int sockfd) {
	char *input;
    char buff[MAX];
	ssize_t bytes;
    for (;;) {
		// get user input allowing using arrow keys
		input = readline("$ ");

		// send cmd to socket
        write(sockfd, input, strlen(input));

		// if send exit then close
        if ((strncmp(input, "exit", 4)) == 0) {
			printf("exiting...\n");
			free(input);
            break;
        }

		// if send STOP then close
        if ((strncmp(input, "STOP", 4)) == 0) {
			printf("shutting down the server...\n");
            free(input);
			break;
        }

		// free the non needed input
		free(input);

		// read cmd output from socket
		bytes = read(sockfd, buff, sizeof(buff));
        printf("%.*s", (int)bytes, buff);
    }
}
   
int main(int argc, char **argv) {
	// check arguments
	if (argc != 3) {
		printf("Usage: %s [ip] [port]\n", argv[0]);
		return 1;
	}

    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
    	printf("Socket couldn't be created..\n");
		return 2;
    }
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET; // ipv4
	const unsigned long ip = inet_addr(argv[1]);
	if (ip == INADDR_NONE) {
		printf("%s: wrong format ipv4\n", argv[1]);
		return 4;
	}
    servaddr.sin_addr.s_addr = ip; // all interfaces/addresses
    servaddr.sin_port = htons(atoi(argv[2])); // port
   
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        return 3;
    }
	printf("connected to the server..\n");
   
    // function for chat
    handle(sockfd);
   
    // close the socket
    close(sockfd);
}
