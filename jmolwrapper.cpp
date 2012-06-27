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

static const char* command_prefix = "{\"type\":command, \"command\":";


JmolWrapper::JmolWrapper(std::string jhost, int jport) {
	host = jhost;
	port = jport;
	if ((sock = jsock(host.c_str(), port)) < 1)
		printf("error initializing socket\n");
}

JmolWrapper::~JmolWrapper() {
	jclose(sock);
}

void JmolWrapper::rotate(float x, float y, bool selected) {
	std::ostringstream command;
	if (selected)
		command << command_prefix << "\"rotateSelected x " << x << "; rotateSelected y " << y << "\"}";
	else
		command << command_prefix << "\"rotate x " << x << "; rotate y " << y << "\"}";
	jsend(command.str().c_str(), sock);
}

void JmolWrapper::translate(float x, float y, bool selected) {
	std::ostringstream command;
	if (selected)
		command << command_prefix << "\"translateSelected x " << x << "; translateSelected y " << y << "\"}";
	else
		command << command_prefix << "\"translate x " << x << "; translate y " << y << "\"}";
	jsend(command.str().c_str(), sock);
}

void JmolWrapper::drawPoint3D(const char* name, float x, float y, float z) {
	std::ostringstream command;
	command << command_prefix << "\"draw " << name << " vertices {" << x << " " << y << " " << z << "}\"}";
	jsend(command.str().c_str(), sock);
}

void JmolWrapper::drawPoint2D(float x, float y) {
	std::ostringstream command;
	command << command_prefix << "\"draw pt [" << x << " " << y << "]\"}";
	jsend(command.str().c_str(), sock);
}

void JmolWrapper::selectAll() {
	std::ostringstream command;
	command << command_prefix << "\"select all\"}";
	jsend(command.str().c_str(), sock);
}

void JmolWrapper::selectNone() {
	std::ostringstream command;
	command << command_prefix << "\"select none\"}";
	jsend(command.str().c_str(), sock);
}

// select all atoms within x_distance of x and y_distance of y
void JmolWrapper::selectWithinDistance(float x, float y, float x_distance, float y_distance) {
	std::ostringstream command;
	command << command_prefix << "\"select sx > " << x-x_distance << " && sx < " << x+x_distance << " && sy > " << y-y_distance << " && sy < " << y+y_distance << "\"}";
	jsend(command.str().c_str(), sock);
}

// select all molecules with atoms within x_distance of x and y_distance of y
void JmolWrapper::selectMoleculeWithinDistance(float x, float y, float x_distance, float y_distance) {
	std::ostringstream command;
	command << command_prefix << "\"select within(molecule, sx > " << x-x_distance << " && sx < " << x+x_distance << " && sy > " << y-y_distance << " && sy < " << y+y_distance << ")\"}";
	jsend(command.str().c_str(), sock);
}
