/*
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 
* 文件名称：
* 文件标识：
* 摘    要：
* 
*
* 修订记录：
*       修订日期    修订人    修订内容
*/

//---------------------------------------------------------------------------
//  Copyright (c) 2002-2006, UOneNet, All right reserved
//
//  Class ServerSocket_T:
//    A class actioning as a server socket that is to accept the socket 
//    connection from a host
//  Class ClientSocket_T:
//    A class actioning as a client socket that tries to connect to a 
//    server socket
//  Class ClientSocket_T:
//    A class that do reading and writing through a socket fd
//  Class UdpReceiver_T:
//    A class actioning as a UDP server socket that reads data through
//    a port speciated
//---------------------------------------------------------------------------
#ifndef SOCKET_STREAM_HPP
#define SOCKET_STREAM_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <errno.h>
#include <string>
#include <fcntl.h>
#include <signal.h>

using namespace std;

class SocketStream_T;
class StreamException_T;

class SocketStream_T
{
public:
	SocketStream_T(int fd);
	~SocketStream_T();
	
	int read(void* buf, int len);
	int read(void* buf, int off, int len);
	void readBytes(void* buf, int len);
	void readBytes(void* buf, int off, int len);
	
	int write(void* buf, int len);
	int write(void* buf, int off, int len);
	void writeBytes(void* buf, int len);
	void writeBytes(void* buf, int off, int len);

	void ignorePipe();
private:
	int _fd;
};

class StreamException_T : public exception
{
public:
	StreamException_T(int code, const char* what) throw();
	StreamException_T(int code, const string& what) throw();
	virtual ~StreamException_T() throw() {};
	
	virtual const char* what() const throw();
	int code();
	
private:
	int _code;
	std::string _what;
};

#endif                                        // #ifndef
