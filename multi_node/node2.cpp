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
	const char *init_msg = "Node 2 Up"; 
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
	
	sendto(sockfd, (const char *)init_msg, strlen(init_msg), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
	printf("Pinging Server\n"); 
		
	n = recvfrom(sockfd, (char *)response, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len); 
	response[n] = '\0'; 
	printf("%s\n", response);

	close(sockfd); 
	return 0; 
}