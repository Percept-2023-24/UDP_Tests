#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <typeinfo>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <string>
#include <fcntl.h>

#define IP			"127.0.0.1"			// localhost
//#define IP		"169.231.210.52" 	// server
#define PORT 		1200
#define MAXLINE		1024

void write_file(int sockfd, struct sockaddr_in addr) {
	const char *filename = "test.json";
	int n;
	char buffer[MAXLINE];
	socklen_t addr_size;

	// Creating a file.
	FILE* fp = fopen(filename, "w");

	// Receiving the data and writing it into the file.
	memset(&buffer, 0, sizeof(buffer));
	while (1) {
		addr_size = sizeof(addr);
		n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr*)&addr, &addr_size);
		if (strcmp(buffer, "END") == 0) {
			break;
		}
		printf("[RECEVING] Data: %s", buffer);
		fprintf(fp, "%s", buffer);
		memset(&buffer, 0, sizeof(buffer));
	}
	fclose(fp);
}

int main() {
	// Defining variables
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len;

	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(IP);
	servaddr.sin_port = htons(PORT);

	// Create a TCP socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("[ERROR] socket error");
		exit(EXIT_FAILURE);
	}
	if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		perror("Bind Failed"); 
		exit(EXIT_FAILURE);
	}

	printf("Waiting for a client to connect...\n");

	//listen for up to 5 requests at a time
    listen(sockfd, 5);
	len = sizeof(cliaddr);
	
    //accept, create a new socket descriptor to 
    //handle the new connection with client
    int newsockfd = accept(sockfd, (sockaddr *)&cliaddr, &len);
    if(newsockfd < 0) {
        perror("Error Accepting Client Request!");
        exit(EXIT_FAILURE);
    }
    printf("Connected with client!\n");

	write_file(newsockfd, cliaddr);

	printf("\n[SUCCESS] Data transfer complete.\n");
	printf("[CLOSING] Closing the server.\n");

	close(sockfd);
	return 0;
}