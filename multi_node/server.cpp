// Server side implementation of UDP client-server model 
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <string>

#define IP			"169.231.210.52"	// server IP
#define PORT	    1200 
#define SIZE     	1024 

using namespace std;

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
	const char *init_msg = "Server Up";
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
	printf("Checking Connection ...\n");

	bool connection = false;
	string comp;

	// Receive client responses and print to console
	socklen_t len = sizeof(cliaddr); // len is value/result
    int n = recvfrom(sockfd, (char *)response, SIZE, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len); 
	response[n] = '\0'; 
	printf("%s\n", response);
	if (strcmp(response, "Node 1 Up")==0) {
		connection = true;
		comp = "--> Node 2 Up";
	}
	else if (strcmp(response, "Node 2 Up")==0) {
		connection = true;
		comp = "--> Node 1 Up";
	}

	n = recvfrom(sockfd, (char *)response, SIZE, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len); 
	response[n] = '\0'; 
	printf("%s\n", response);
	if (strcmp(response, comp.c_str()) != 0) {
		connection = false;
	}

	// Send response to clients
	if (connection) {
		sendto(sockfd, (const char *)init_msg, strlen(init_msg), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 
		printf("Connection Successful\n");
		cout << "[Enter] to run demo ...";
		if (cin.get() == '\n') {
			printf("Running demo ...\n");
		}
	}
	
	return 0; 
}