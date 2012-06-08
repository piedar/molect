/*
 * jmolwrapper.h
 * 
 * Copyright 2012 Benn Snyder <benn.snyder@gmail.com>
 * 
 */

#ifndef _JMOLWRAPPER_H
#define _JMOLWRAPPER_H

#include <string>


class JmolWrapper {
public:
	JmolWrapper(std::string host, int port);
	~JmolWrapper();
	
	void rotate(float x, float y);
	void translate(float x, float y);
	
private:
	std::string host;
	int port;
	int sock;
};


#endif
