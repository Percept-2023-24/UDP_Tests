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

#define IP			"127.0.0.1"	// localhost
#define PORT	    8080 
#define MAXLINE    	1024

using namespace std;

//Server side
int main(int argc, char *argv[])
{
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;

    // Creating socket file descriptor 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) { 
		perror("Socket Creation Failed"); 
		exit(EXIT_FAILURE);
	} 

    //setup a socket and connection tools
    //bzero((char*)&servaddr, sizeof(servaddr));
    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(IP);
    servaddr.sin_port = htons(PORT);

    //bind the socket to its local address
    if(bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        perror("Bind Failed"); 
		exit(EXIT_FAILURE);
    }

    printf("Waiting for a client to connect...\n");

    //listen for up to 5 requests at a time
    listen(sockfd, 5);

    //receive a request from client using accept
    //we need a new address to connect with the client
    len = sizeof(cliaddr);

    //accept, create a new socket descriptor to 
    //handle the new connection with client
    int newsockfd = accept(sockfd, (sockaddr *)&cliaddr, &len);
    
    if(newsockfd < 0) {
        perror("Error Accepting Client Request!");
        exit(EXIT_FAILURE);
    }
    printf("Connected with client!\n");
    
    while(1) {
        //receive a message from the client (listen)
        printf("\nAwaiting Client Response...\n");

        memset(&buffer, 0, sizeof(buffer));//clear the buffer
        recv(newsockfd, (char*)&buffer, sizeof(buffer), 0);

        if(!strcmp(buffer, "exit")) {
            printf("\nClient has quit the session\n");
            break;
        }

        cout << "Client: " << buffer << endl;
        cout << "> ";
        string data;
        getline(cin, data);
        memset(&buffer, 0, sizeof(buffer)); //clear the buffer
        strcpy(buffer, data.c_str());

        if(data == "exit") {
            //Close the connection
            send(newsockfd, (char*)&buffer, strlen(buffer), 0);
            break;
        }

        //Send the message to client
        send(newsockfd, (char*)&buffer, strlen(buffer), 0);
    }

    //we need to close the socket descriptors after we're all done
    close(newsockfd);
    close(sockfd);
    printf("Connection closed...\n");
    return 0;   
}