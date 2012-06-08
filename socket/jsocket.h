/*
 * jsocket.h
 * 
 * Set of utility functions for sending JSON strings to a socket to control Jmol.
 * 
 * Copyright 2012 Benn Snyder <benn.snyder@gmail.com>
 */

#ifndef _JSOCKET_H
#define _JSOCKET_H


int jsock(const char* jhost, int jport);
void jsend(const char* jcommand, int sock);
void jsendh(const char* jcommand, const char* jhost, int jport);


#endif
