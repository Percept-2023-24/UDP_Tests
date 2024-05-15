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
#define SERVER_PORT	1200 
#define MAXLINE 	1024 

using namespace std;
using namespace std::chrono;
using namespace rapidjson;