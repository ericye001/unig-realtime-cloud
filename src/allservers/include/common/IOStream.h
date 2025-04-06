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
#ifndef IOSTREAM_H
#define IOSTREAM_H

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

class IOStream_T;
class IOState_T;
class IOBlocking_T;
class IONonblocking_T;

class IOStream_T
{
public:
	IOStream_T(int fd, bool blocking = true);
	virtual ~IOStream_T();
	
	int read(void* buf, int len);
	void readBytes(void* buf, int len);	
	
	int write(void* buf, int len);
	void writeBytes(void* buf, int len);

	void ignoreSigPipe();
	void configureBlocking(bool blocking);
	
private:
    IOState_T* getState();
    int getFd();
    
private:
	int _fd;
	bool _blocking;
	IOState_T* _state[2];
};

class IOState_T
{
public:
    virtual int read(int fd, void* buf, int len) = 0;
    virtual int write(int fd, void* buf, int len) = 0;
};


class IOBlocking_T : public IOState_T
{
public:
    virtual int read(int fd, void* buf, int len);
	virtual int write(int fd, void* buf, int len);
};

class IONonblocking_T : public IOState_T
{
public:
    virtual int read(int fd, void* buf, int len);
	virtual int write(int fd, void* buf, int len);    
};


#endif                                        // #ifndef
