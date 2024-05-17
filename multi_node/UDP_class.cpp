#include <stdio.h>
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <string>
#include "rapidjson/document.h" 
#include "rapidjson/filewritestream.h" 
#include "rapidjson/writer.h" 
#include <fstream> 
#include <iostream>

#define IP				"169.231.222.39" // server IP
#define SERVER_PORT		1200 
#define MAXLINE 		1024

using namespace std;
using namespace std::chrono;
using namespace rapidjson;

// Class for multi-node server comms
class JSON_UDP {
    int frame = 1;
    const char *node = "Mike";    // Patrick or Mike
    int clientSd;
    struct sockaddr_in servaddr;
	socklen_t addr_size;
    Value s;
	FILE* fp;
	FILE* fp_in;
	string fname;
	char buffer[MAXLINE];
	int n;
	const char *exit_msg = "Demo Complete";
    
	public:
		void write_json(string fname, float angle, auto duration) {
		    Document d; 
		    d.SetObject();
		    s.SetString(StringRef(node));

		    // Add data to the JSON document
		    d.AddMember("Node", s, d.GetAllocator());
		    d.AddMember("Frame Number", frame, d.GetAllocator());
		    d.AddMember("Elapsed Time (ms)", duration.count(), d.GetAllocator());
		    d.AddMember("Angle", angle, d.GetAllocator()); 
		    //d.AddMember("range", range, d.GetAllocator());

		    // Open the output file
		    fp = fopen(fname.c_str(), "w");

		    // Write the JSON data to the file
		    char writeBuffer[65536]; 
		    FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer)); 
		    Writer<FileWriteStream> writer(os); 
		    d.Accept(writer);

		    fclose(fp);
		}

		void send_file_data(string fname, float angle, auto duration) {
			write_json(fname, angle, duration);    	// Write JSON file with angle data
		    fp_in = fopen(fname.c_str(), "r");		// Read JSON file with angle data

		    // Read the text file
		    if (fp_in == NULL) {
		        perror("[ERROR] reading the file\n");
		        exit(EXIT_FAILURE);
		    }
		    
		    // Send the data
		    memset(&buffer, 0, sizeof(buffer));
		    while (fgets(buffer, MAXLINE, fp_in) != NULL) {
		        printf("\nSending: %s", buffer);

		        n = sendto(clientSd, buffer, MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
		        if (n == -1) {
		            perror("[ERROR] sending data to the server.\n");
		            exit(EXIT_FAILURE);
		        }
		        memset(&buffer, 0, sizeof(buffer));
		    }

		    // Send the 'END'
		    strcpy(buffer, "END");
		    sendto(clientSd, buffer, MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
		    fclose(fp_in);
		}
		
		int socket_setup() {
			memset(&servaddr, 0, sizeof(servaddr));
		    
		    // Socket address properties
		    servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = inet_addr(IP);
		    servaddr.sin_port = htons(SERVER_PORT);
		    
		    // Create a TCP socket
		    clientSd = socket(AF_INET, SOCK_STREAM, 0);
		    if (clientSd < 0) {
		        perror("[ERROR] socket error\n");
		        exit(EXIT_FAILURE);
		    }
		    printf("\nClient Setup Complete...\n");
		    
		    if (connect(clientSd, (sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		    	printf("Error Connecting To Socket!\n");
		    	exit(EXIT_FAILURE);
		    }
		    printf("Connected To Server!\n\n");
		    return 1;
		}

		int get_frames() {
			memset(&buffer, 0, sizeof(buffer));
			addr_size = sizeof(servaddr);
			n = recvfrom(clientSd, buffer, MAXLINE, 0, (struct sockaddr*)&servaddr, &addr_size);
			printf("Capturing %s Frames...\n\n", buffer);
			return stoi(buffer);
		}

		void process(float angle, auto start_time) {
		    auto stop = chrono::high_resolution_clock::now();
		    auto duration_udp_process = duration_cast<milliseconds>(stop - start_time);		    
		    
			fname = format("%s_Frame%d.json", node, frame);
		    send_file_data(fname, angle, duration_udp_process); // Send file to server
		    printf("\nFrame Data Sent To Server\n\n");
		    
		    frame++;
		}
		
		void end_stream() {
			memset(&buffer, 0, sizeof(buffer));
			strcpy(buffer, exit_msg);
			n = sendto(clientSd, buffer, MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
			close(clientSd);
			printf("Demo Complete!\n");
			printf("Connection Closed...\n\n");
		}
};
