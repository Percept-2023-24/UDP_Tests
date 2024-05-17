#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <typeinfo>
#include <fmt/core.h>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>
//#define IP			"127.0.0.1"				// localhost
#define IP				"169.231.222.39" 		// server
#define SERVER_PORT1 	1200
#define SERVER_PORT2 	1210
#define MAXLINE 		1024

using namespace std;
using namespace fmt;
using namespace rapidjson;

int frame_mw = 1, frame_p = 1, num_frames;
float angle;
string fname, node;
string path = "/home/aditya/Programming/Capstone/UDP_Tests/multi_node/frame_data";
int n;
char buffer[MAXLINE];
char readBuffer[65536];
Document d;
socklen_t addr_size;
FILE* fp;
struct sockaddr_in servaddr1, servaddr2, cliaddr1, cliaddr2;
socklen_t len1, len2;
int sockfd1, sockfd2, newsockfd1, newsockfd2;
bool mw_done = false, p_done = false;

void update_pos() {
    fp = fopen(fname.c_str(), "rb"); // Open the file 
  
    // Check if the file was opened successfully 
    if (!fp) { 
		perror("Error: unable to open file");
        exit(EXIT_FAILURE);
    }
  
    // Read the file into a buffer and parse JSON 
	memset(&readBuffer, 0, sizeof(readBuffer));
    FileReadStream is(fp, readBuffer, sizeof(readBuffer)); 
    d.ParseStream(is);
  
    // Check if the document is valid 
    if (d.HasParseError()) { 
        perror("Error: failed to parse JSON document");
        fclose(fp); 
        exit(EXIT_FAILURE);
    }   
    fclose(fp);

	angle = d["Angle"].GetFloat();	// Parse angle from JSON
	//range = d["Range"].GetFloat();	// Parse range from JSON

	//depending on which node is being analyzed (Patrick or Mike) do processing to update position

}

void write_file(int sockfd, struct sockaddr_in addr) {
	// Receiving the data and writing it into the file.
	memset(&buffer, 0, sizeof(buffer));
	while (1) {
		addr_size = sizeof(addr);
		n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr*)&addr, &addr_size);
		if (strcmp(buffer, "END") == 0) {
			break;
		}
		if (strcmp(buffer, "Mike Demo Complete") == 0) {
			mw_done = true;
			printf("\nClient 1 Connection Closed...\n");
			close(newsockfd1);
			close(sockfd1);
			return;
		}
		if (strcmp(buffer, "Patrick Demo Complete") == 0) {
			p_done = true;
			printf("Client 2 Connection Closed...\n");
			close(newsockfd2);
			close(sockfd2);
			return;
		}
		printf("Receiving: %s\n", buffer);

		// Set node name based on incoming data
		if (buffer[9] == 'M') {
			node = "Mike";
			fname = format("{}/{}_Frame{}.json", path, node, frame_mw);
			frame_mw++;
			
		}
		else if (buffer[9] == 'P') {
			node = "Patrick";
			fname = format("{}/{}_Frame{}.json", path, node, frame_p);
			frame_p++;
			if (strcmp(buffer, "Demo Complete") == 0) {
				p_done = true;
				return;
			}
		}

		fp = fopen(fname.c_str(), "w");
		fprintf(fp, "%s", buffer);
		memset(&buffer, 0, sizeof(buffer));
	}
	fclose(fp);
}

void init_demo() {
	cout << "# Frames To Capture: ";
	cin >> num_frames;
	memset(&buffer, 0, sizeof(buffer));
	strcpy(buffer, to_string(num_frames).c_str());
	n = sendto(newsockfd1, buffer, MAXLINE, 0, (struct sockaddr*)&cliaddr1, sizeof(cliaddr1));
	n = sendto(newsockfd2, buffer, MAXLINE, 0, (struct sockaddr*)&cliaddr2, sizeof(cliaddr2));
}

int socket_setup() {
	memset(&servaddr1, 0, sizeof(servaddr1));
	memset(&servaddr2, 0, sizeof(servaddr2));

	servaddr1.sin_family = AF_INET;
	servaddr1.sin_addr.s_addr = inet_addr(IP);
	servaddr1.sin_port = htons(SERVER_PORT1);
	
	servaddr2.sin_family = AF_INET;
	servaddr2.sin_addr.s_addr = inet_addr(IP);
	servaddr2.sin_port = htons(SERVER_PORT2);

	// Create a TCP socket
	sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
	sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd1 < 0 || sockfd2 < 0) {
		perror("[ERROR] socket error\n");
		exit(EXIT_FAILURE);
	}	
	if (bind(sockfd1, (struct sockaddr*)&servaddr1, sizeof(servaddr1)) < 0 || bind(sockfd2, (struct sockaddr*)&servaddr2, sizeof(servaddr2)) < 0) {
		perror("[ERROR] bind failed"); 
		exit(EXIT_FAILURE);
	}
	printf("Server Setup Complete...\n");

	//listen for up to 5 requests at a time
	listen(sockfd1, 5);
	listen(sockfd2, 5);
	len1 = sizeof(cliaddr1);
	len2 = sizeof(cliaddr2);

	//accept, create a new socket descriptor to 
	//handle the new connection with client
	newsockfd1 = accept(sockfd1, (sockaddr *)&cliaddr1, &len1);
	if(newsockfd1 < 0) {
		perror("Error Accepting Client 1 Request!");
		exit(EXIT_FAILURE);
	}
	printf("Connected To Client 1!\n");

	newsockfd2 = accept(sockfd2, (sockaddr *)&cliaddr2, &len2);
	if(newsockfd2 < 0) {
		perror("Error Accepting Client 2 Request!");
		exit(EXIT_FAILURE);
	}
	printf("Connected To Client 2!\n\n");

	return 1;
}

int main() {

	if (socket_setup() == 1) {
		init_demo();
		printf("Running Demo...\n\n");
		while (mw_done == false || p_done == false) {
			if (!mw_done) {
				write_file(newsockfd1, cliaddr1);
			}
			if (!p_done) {
				write_file(newsockfd2, cliaddr2);
			}
		}
		printf("\nDemo Complete!\n");
	}
	return 0;


	
}