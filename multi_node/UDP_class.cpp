// Multi-node Application (Server side) 
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

#define IP			"169.231.210.52"	// server IP
#define PORT	    1200 
#define SIZE     	1024 

using namespace std;
using namespace std::chrono;
using namespace rapidjson;

class JSON_UDP {
    int frame = 1;
    const char *node = "Patrick";    // Patrick or Mike
    
    string write_json(float *angle_ptr, auto duration) {
        Document d; 
        d.SetObject(); 

        // Add data to the JSON document
        d.AddMember("Node", node, d.GetAllocator());
        d.AddMember("Frame Number", frame, d.GetAllocator());
        d.AddMember("Elapsed Time (us)", duration.count(), d.GetAllocator());
        d.AddMember("Angle", *angle_ptr, d.GetAllocator()); 
        //d.AddMember("range", range, d.GetAllocator());

        // Open the output file
        string fname = format("%s_Frame%d.json", node, frame); 
        FILE* fp = fopen(fname.c_str(), "w");

        // Write the JSON data to the file
        char writeBuffer[65536]; 
        FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer)); 
        Writer<FileWriteStream> writer(os); 
        d.Accept(writer);

        fclose(fp);
        return fname;
    }

    void send_file_data(int sockfd, struct sockaddr_in addr, string fname) {
        int n;
        char buffer[SIZE];

        // Read JSON file with frame data
        FILE* fp_in = fopen(fname.c_str(), "r");

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

    void process(float *angle_ptr, auto start_time) {
        auto stop = chrono::high_resolution_clock::now();
        auto duration_udp_process = duration_cast<microseconds>(stop - start_time);

        // Defining variables
        int server_sockfd;
        struct sockaddr_in server_addr;

        // Creating a UDP socket
        server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (server_sockfd < 0) {
            perror("[ERROR] socket error");
            exit(1);
        }

        // Socket address properties
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = PORT;
        server_addr.sin_addr.s_addr = inet_addr(IP);

        string fname = write_json(*angle_ptr, duration_udp_process);    // Write JSON file with angle data
        send_file_data(server_sockfd, server_addr, fname);              // Sending the file data to the server

        printf("\nFrame Data Sent To Server\n");
        close(server_sockfd);
        frame++;
    }

};