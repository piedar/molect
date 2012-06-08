/*
 * jsocket.c
 *
 * Copyright 2012 Benn Snyder <benn.snyder@gmail.com>
 */

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include "jsocket.h"


// create and return a socket to the specified host or return -1 on error
int jsock(const char* jhost, int jport) {
	int sock;
	struct hostent* host;
	struct sockaddr_in server_addr;
	
	if ((host = gethostbyname(jhost)) == NULL) {
		printf("couldn't find host\n");
		return -1;
	}
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error\n");
		return -1;
	}
	
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr = *((struct in_addr*)host->h_addr);
	server_addr.sin_port = htons(jport);
	
	if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		printf("connect error\n");
		return -1;
	}
	
	return sock;
}

// send a command to an already existing socket
void jsend(const char* jcommand, int sock) {
	send(sock, jcommand, strlen(jcommand), 0);
	send(sock, "\r\n", 2, 0); // MUST MUST MUST newline terminate JSON strings for Jmol to accept them - \r may be necessary on Windows
}

// send a command to a new socket on the specified host and port
void jsendh(const char* jcommand, const char* jhost, int jport) {
	int sock = jsock(jhost, jport);
	jsend(jcommand, sock);	
	close(sock);
}
