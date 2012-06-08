/*
 * jmolwrapper.cpp
 * 
 * Copyright 2012 Benn Snyder <benn.snyder@gmail.com>
 * 
 */

#include <sstream>
#include <stdio.h>
#include "socket/jsocket.h"
#include "jmolwrapper.h"


JmolWrapper::JmolWrapper(std::string jhost, int jport) {
	host = jhost;
	port = jport;
	if ((sock = jsock(host.c_str(), port)) < 1)
		printf("error initializing socket\n");
}

JmolWrapper::~JmolWrapper() {
	close(sock);
}

void JmolWrapper::rotate(float x, float y) {
	std::ostringstream command;
	command << "{\"type\":move, \"style\":rotate, \"x\":" << x << ", \"y\":" << y << "}";
	jsend(command.str().c_str(), sock);
}

void JmolWrapper::translate(float x, float y) {
	std::ostringstream command;
	command << "{\"type\":move, \"style\":translate, \"x\":" << x << ", \"y\":" << y << "}";
	jsend(command.str().c_str(), sock);
}
