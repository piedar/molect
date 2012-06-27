/*
 * jsocket.c
 *
 *
 * Copyright 2012 Benn Snyder <benn.snyder@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include "jsocket.h"

#ifdef _WIN32
	#include <winsock.h>
	#define close(sock) closesocket(sock)
#else
	#include <netdb.h>
	#include <unistd.h>
#endif


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
	
	memset(&server_addr, 0, sizeof(server_addr));
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

// dumb little function so we don't have to do OS-specific stuff higher-up
void jclose(int sock) {
	close(sock);
}
