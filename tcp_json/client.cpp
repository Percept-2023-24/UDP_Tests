#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h> 
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include "rapidjson/document.h" 
#include "rapidjson/filewritestream.h" 
#include "rapidjson/writer.h" 
#include <fstream> 
#include <iostream> 
#include <fmt/core.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#define IP			"127.0.0.1"			// localhost
//#define IP		"169.231.210.52" 	// server
#define PORT 		1200
#define MAXLINE		1024

using namespace rapidjson;
using namespace std;
using namespace fmt;

void write_json(string fname, int angle, int range) {
	Document d; 
	d.SetObject();

	// Add data to the JSON document 
	d.AddMember("angle", angle, d.GetAllocator()); 
	d.AddMember("range", range, d.GetAllocator());

	// Open the output file
	FILE* fp = fopen(fname.c_str(), "w");

	// Write the JSON data to the file
	char writeBuffer[65536]; 
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer)); 
    Writer<FileWriteStream> writer(os); 
    d.Accept(writer);

	fclose(fp);
}

void send_file_data(int sockfd, struct sockaddr_in addr) {
	int n;
	char buffer[MAXLINE];

	// Collect sample frame data
	cout << "Enter filename: ";
	string fname;
	cin >> fname;
	fname = format("{}.json", fname);

	cout << "Enter angle: ";
	int angle_in;
	cin >> angle_in;

	cout << "Enter range: ";
	int range_in;
	cin >> range_in;

	// Create JSON file with frame data
	write_json(fname, angle_in, range_in);
	FILE* fp_in = fopen(fname.c_str(), "r");

	// Reading the text file
	if (fp_in == NULL) {
		perror("[ERROR] reading the file");
		exit(EXIT_FAILURE);
	}
	
	// Sending the data
	while (fgets(buffer, MAXLINE, fp_in) != NULL) {
		printf("[SENDING] Data: %s", buffer);

		n = sendto(sockfd, buffer, MAXLINE, 0, (struct sockaddr*)&addr, sizeof(addr));
		if (n == -1) {
			perror("[ERROR] sending data to the server.");
			exit(EXIT_FAILURE);
		}
		memset(&buffer, 0, sizeof(buffer));
	}

	// Sending the 'END'
	strcpy(buffer, "END");
	send(sockfd, (char*)&buffer, strlen(buffer), 0);
	fclose(fp_in);
}

int main(void) {
	// Defining variables
	struct sockaddr_in servaddr;

	memset(&servaddr, 0, sizeof(servaddr)); 

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(IP);
	servaddr.sin_port = htons(PORT);

	// Creating a TCP socket
	int clientSd = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSd < 0) {
		perror("[ERROR] socket error");
		exit(EXIT_FAILURE);
	}
	if(connect(clientSd, (sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        printf("Error Connecting To Socket!");
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server!\n");

	// Sending the file data to the server
	send_file_data(clientSd, servaddr);

	printf("\nData transfer complete.\n");
	printf("Disconnecting from the server.\n");

	close(clientSd);

	return 0;
}