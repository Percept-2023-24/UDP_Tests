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
#define IP		"169.231.210.52" 	// server
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

void write_file(int sockfd, struct sockaddr_in addr) {
	// Receiving the data and writing it into the file.
	while (1) {
		addr_size = sizeof(addr);
		n = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, &addr_size);
		
		if (strcmp(buffer, "END") == 0) {
			break;
		}

		printf("Receiving: %s\n", buffer);

		node = (buffer[9] == 'M') ? "Mike" : "Patrick";	// set node name based on incoming data
		fname = format("{}/{}_Frame{}.json", path, node, frame);

		fp = fopen(fname.c_str(), "w");
		fprintf(fp, "%s", buffer);
		bzero(buffer, SIZE);
	}
	fclose(fp);
}

int main() {

	// Defining variables
	int server_sockfd;
	struct sockaddr_in server_addr, client_addr;
	char buffer[SIZE];
	int e;

	// Creating a UDP socket
	server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_sockfd < 0) {
		perror("[ERROR] socket error");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP);
	server_addr.sin_port = PORT;

	e = bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	
	if (e < 0) {
		perror("[ERROR] bind error");
		exit(1);
	}

	printf("UDP File Server started.\n\n");

	while (frame <= num_frames) {
		write_file(server_sockfd, client_addr);
		frame++;
	}
	
	printf("\n\nData transfer complete.\n");
	printf("Closing the server.\n");

	close(server_sockfd);
	return 0;
}