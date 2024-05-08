// Client side implementation of UDP client-server model 
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define IP			"169.231.210.52"
#define PORT	    1200
#define MAXLINE     1024 

// Driver code 
int main() { 
	int sockfd; 
	char response[MAXLINE]; 
	const char *hello = "Node 2 Up"; 
	struct sockaddr_in	 servaddr; 

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = inet_addr(IP); 
	
	int n;
	socklen_t len; 
	
	sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
	std::cout<<"Connection Confirmed"<<std::endl; 
		
	n = recvfrom(sockfd, (char *)response, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len); 
	response[n] = '\0'; 
	std::cout<<"Server: "<<response<<std::endl; 

	close(sockfd); 
	return 0; 
}