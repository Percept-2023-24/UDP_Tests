#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "rapidjson/document.h" 
#include "rapidjson/filewritestream.h" 
#include "rapidjson/writer.h" 
#include <fstream> 
#include <iostream> 

#define IP		"127.0.0.1"
#define PORT 	8080
#define SIZE 	1024

using namespace rapidjson;
using namespace std;

void write_json(char* fname, int angle, int range) {
	Document d; 
	d.SetObject(); 

	// Add data to the JSON document 
	d.AddMember("angle", angle, d.GetAllocator()); 
	d.AddMember("range", range, d.GetAllocator());

	// Open the output file 
	FILE* fp = fopen(fname, "w");

	// Write the JSON data to the file
	char writeBuffer[65536]; 
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer)); 
    Writer<FileWriteStream> writer(os); 
    d.Accept(writer);

	fclose(fp);
}

void send_file_data(int sockfd, struct sockaddr_in addr) {
	int n;
	char buffer[SIZE];

	// Collect sample frame data
	cout << "Enter filename: ";
	char* fname;
	cin >> fname;
	fname = strcat(fname, ".json");

	cout << "Enter angle: ";
	int angle_in;
	cin >> angle_in;

	cout << "Enter range: ";
	int range_in;
	cin >> range_in;

	// Create JSON file with frame data
	write_json(fname, angle_in, range_in);
	FILE* fp_in = fopen(fname, "r");

	// Reading the text file
	if (fp_in == NULL) {
		perror("[ERROR] reading the file");
		exit(1);
	}
	
	// Sending the data
	while (fgets(buffer, SIZE, fp_in) != NULL) {
		printf("[SENDING] Data: %s", buffer);

		n = sendto(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));
		if (n == -1) {
			perror("[ERROR] sending data to the server.");
			exit(1);
		}
		bzero(buffer, SIZE);
	}

	// Sending the 'END'
	strcpy(buffer, "END");
	sendto(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));

	fclose(fp_in);
}

int main(void) {

	// Defining variables
	int server_sockfd;
	struct sockaddr_in server_addr;

	// Creating a UDP socket
	server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_sockfd < 0) {
		perror("[ERROR] socket error");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = PORT;
	server_addr.sin_addr.s_addr = inet_addr(IP);

	// Sending the file data to the server
	send_file_data(server_sockfd, server_addr);

	printf("\n[SUCCESS] Data transfer complete.\n");
	printf("[CLOSING] Disconnecting from the server.\n");

	close(server_sockfd);

	return 0;
}