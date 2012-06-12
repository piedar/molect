/*
 * jmolwrapper.cpp
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

#include <sstream>
#include <stdio.h>
#include "jmolwrapper.h"
extern "C" {
	#include "socket/jsocket.h"
}


JmolWrapper::JmolWrapper(std::string jhost, int jport) {
	host = jhost;
	port = jport;
	if ((sock = jsock(host.c_str(), port)) < 1)
		printf("error initializing socket\n");
}

JmolWrapper::~JmolWrapper() {
	close(sock);
}

void JmolWrapper::rotate(float x, float y, bool selected) {
	std::ostringstream command;
	command << "{\"type\":move, \"style\":rotate, \"x\":" << x << ", \"y\":" << y << "}";
	jsend(command.str().c_str(), sock);
}

void JmolWrapper::translate(float x, float y, bool selected) {
	std::ostringstream command;
	command << "{\"type\":move, \"style\":translate, \"x\":" << x << ", \"y\":" << y << "}";
	jsend(command.str().c_str(), sock);
}

void JmolWrapper::selectMolecule(float distance, float x, float y, float z) {
	std::ostringstream command;
	command << "{\"type\":command, \"command\":\"select within(molecule, within(" << distance << ", {" << x << " " << y << " " << z << "}))\"}";
	jsend(command.str().c_str(), sock);
}

void JmolWrapper::drawVertex(const char* name, float x, float y, float z) {
	std::ostringstream command;
	command << "{\"type\":command, \"command\":\"draw " << name << " vertices {" << x << " " << y << " " << z << "}\"}";
	jsend(command.str().c_str(), sock);
}
