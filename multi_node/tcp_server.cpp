#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <typeinfo>
#include <fmt/core.h>

#define FMT_HEADER_ONLY
#include <fmt/format.h>
//#define IP		"127.0.0.1"			// localhost
#define IP		"169.231.222.39" 		// server
#define PORT 	1200
#define SIZE 	1024

using namespace std;
using namespace fmt;

int frame = 1, num_frames = 10;
string fname, node;
string path = "/home/aditya/Programming/Capstone/UDP_Tests/multi_node/frame_data";
int n;
char buffer[SIZE];
socklen_t addr_size;
FILE* fp;
struct sockaddr_in servaddr, cliaddr;
socklen_t len;
int newsockfd;

int write_file(int sockfd, struct sockaddr_in addr) {
	// Receiving the data and writing it into the file.
	memset(&buffer, 0, sizeof(buffer));
	while (1) {
		addr_size = sizeof(addr);
		n = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, &addr_size);
		if (strcmp(buffer, "END") == 0) {
			break;
		}
		else if (strcmp(buffer, "Demo Complete") == 0) {
			return 0;
		}

		printf("Receiving: %s\n\n", buffer);

		// Set node name based on incoming data
		if (buffer[9] == 'M') {
			node = "Mike";
		}
		else if (buffer[9] == 'P') {
			node = "Patrick";
		}

		fname = format("{}/{}_Frame{}.json", path, node, frame);

		fp = fopen(fname.c_str(), "w");
		fprintf(fp, "%s", buffer);
		memset(&buffer, 0, sizeof(buffer));
	}
	fclose(fp);
	frame++;
	return 1;
}

int main() {
	
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
		perror("[ERROR] bind failed"); 
		exit(EXIT_FAILURE);
	}

	printf("Server Setup Complete...\n");
		
	//listen for up to 5 requests at a time
	listen(sockfd, 5);
	len = sizeof(cliaddr);

	//accept, create a new socket descriptor to 
	//handle the new connection with client
	newsockfd = accept(sockfd, (sockaddr *)&cliaddr, &len);
	if(newsockfd < 0) {
		perror("Error Accepting Client Request!");
		exit(EXIT_FAILURE);
	}
	printf("Connected with client!\n");

	while (1) {
		if (write_file(newsockfd, cliaddr) == 0) {
			print("Connection Closed...\n");
			close(newsockfd);
			close(sockfd);
			return 0;
		}
		else {
			;
		}
	}

	// while (frame <= num_frames) {
	// 	write_file(server_sockfd, cliaddr);
	// 	frame++;
	// }
	
	// printf("\nData transfer complete.\n");
	// printf("Closing the server.\n");

	// close(sockfd);
	return 0;
}