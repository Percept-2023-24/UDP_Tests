// Server side implementation of UDP client-server model 
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define IP			"169.231.210.52"	// server IP
#define PORT	    1200 
#define SIZE     	1024 

void write_file(int sockfd, struct sockaddr_in addr) {
	const char *filename = "server_data.json";
	int n;
	char response[SIZE];
	socklen_t addr_size;

	// Creating a file.
	FILE* fp = fopen(filename, "w");

	// Receiving the data and writing it into the file.
	while (1) {
		addr_size = sizeof(addr);
		n = recvfrom(sockfd, response, SIZE, 0, (struct sockaddr*)&addr, &addr_size);
		
		if (strcmp(response, "END") == 0) {
			break;
		}

		printf("[RECEVING] Data: %s", response);
		fprintf(fp, "%s", response);
		bzero(response, SIZE);
	}

	fclose(fp);
}

// Driver code 
int main() { 
	int sockfd; 
	char response[SIZE];
	const char *hello = "Start Application";
	struct sockaddr_in servaddr, cliaddr;
	
	// Creating socket file descriptor 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = inet_addr(IP); // inet_addr(IP) for specific IP
	servaddr.sin_port = htons(PORT); 
	
	// Bind the socket with the server address 
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 

	// Print connection checking to console
	std::cout<<"Checking Connection"<<std::endl;

	bool connection = false;
	char *comp;

	// Receive client responses and print to console
	socklen_t len = sizeof(cliaddr); // len is value/result
    int n = recvfrom(sockfd, (char *)response, SIZE, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len); 
	response[n] = '\0'; 
	printf("%s\n", response);
	if (response == "Node 1 Up") {
		connection = true;
		comp = "Node 2 Up";
	}
	else if (response == "Node 2 Up") {
		connection = true;
		comp = "Node 1 Up";
	}

	n = recvfrom(sockfd, (char *)response, SIZE, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len); 
	response[n] = '\0'; 
	printf("%s\n", response);
	if (response != comp) {
		connection = false;
	}

	// Send response to clients
	if (connection) {
		sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 
		std::cout<<"Application Running"<<std::endl; 
	}
	std::cout<<"Test"<<std::endl; 
	
	return 0; 
}