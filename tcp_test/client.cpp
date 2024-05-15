#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>

//#define IP			"127.0.0.1"	// localhost
#define IP			"169.231.222.39"	
#define PORT	    1200  
#define MAXLINE    	1024

using namespace std;

//Client side
int main(int argc, char *argv[])
{
    //create a message buffer 
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr;

    //bzero((char*)&servaddr, sizeof(servaddr)); 
    memset(&servaddr, 0, sizeof(servaddr)); 

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(IP);
    servaddr.sin_port = htons(PORT);

    int clientSd = socket(AF_INET, SOCK_STREAM, 0);

    //try to connect...    
    if(connect(clientSd, (sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        printf("Error Connecting To Socket!");
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server!\n");

    while(1) {
        cout << "> ";
        string data;
        getline(cin, data);
        memset(&buffer, 0, sizeof(buffer)); //clear the buffer
        strcpy(buffer, data.c_str());

        if(data == "exit") {
            send(clientSd, (char*)&buffer, strlen(buffer), 0);
            break;
        }

        send(clientSd, (char*)&buffer, strlen(buffer), 0);
        printf("\nAwaiting Server Response...\n");
        memset(&buffer, 0, sizeof(buffer));//clear the buffer

        recv(clientSd, (char*)&buffer, sizeof(buffer), 0);
        
        if(!strcmp(buffer, "exit")) {
            printf("\nServer has quit the session\n");
            break;
        }
        cout << "Server: " << buffer << endl;
    }
    close(clientSd);
    printf("Connection closed...\n");
    return 0;    
}
