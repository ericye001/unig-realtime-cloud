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
//  Class Socket_T:
//    A class actioning as a client socket that tries to connect to a
//    server socket
//  Class Socket_T:
//    A class that do reading and writing through a socket fd
//---------------------------------------------------------------------------
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <limits.h>
#include <fcntl.h>

using namespace std;

class SocketInterface_T;
class SocketImpl_T;
class ServerSocket_T;
class Socket_T;
class SocketException_T;
class InetAddress_T;

class InetAddress_T
{
public:
    InetAddress_T(uint32_t host, uint16_t port);
    InetAddress_T(const char* host, uint16_t port);
    InetAddress_T(const string& host, uint16_t port);
    InetAddress_T(struct sockaddr& saddr);
    InetAddress_T(struct sockaddr_in& saddrin);

    static const char* toStringHost(uint32_t ipnet);
    static const char* toStringHost(struct sockaddr& saddr);
    static const char* toStringHost(struct sockaddr_in& saddrin);

    static uint32_t toInetHost(const char* host);
    static uint32_t toInetHost(const string& host);
    static uint32_t toInetHost(struct sockaddr& saddr);
    static uint32_t toInetHost(struct sockaddr_in& saddrin);

    struct sockaddr_in* sockAddrIn();
    struct sockaddr* sockAddr();

    const char* strIp();
    uint32_t netIp();
    uint16_t hostPort();
    uint16_t netPort();
    string strPort();

private:
    void init(uint32_t host, uint16_t port);

    struct sockaddr_in _addr;
};

class SocketImpl_T
{
public:
    SocketImpl_T();
    ~SocketImpl_T();

    int accept(SocketImpl_T* socketImpl);

    int bind(InetAddress_T& inetAddr);
    int bind(const char* host, int port);
    int bind(const string& host, int port);
    int bind(uint32_t host, int port);
    int bind(int port);

    int close();

    int connect(InetAddress_T& inetAddr);
    int connect(const char* host, int port);
    int connect(const string& host, int port);
    int connect(uint32_t host, int port);

    int listen(int count = 1000);

    int create(bool stream);

    int sfd();

    const char* localHost();
    const char* remoteHost();

    uint16_t localPort();
    uint16_t remotePort();

    int option(int option);
    int option(int option, int value);

protected:
    int _fd;
    InetAddress_T* _pLocalAddr;
    InetAddress_T* _pRemoteAddr;

private:
    //SocketImpl_T(const SocketImpl_T& rhs);
    //SocketImpl_T& operator=(const SocketImpl_T& rhs);

    int atoaddr(const char* host, struct sockaddr_in* address);
};

class SocketInterface_T
{
public:
    SocketInterface_T();
SocketInterface_T(SocketImpl_T* pImpl);
    virtual ~SocketInterface_T();

    virtual int sfd() = 0;
    virtual const char* localHost() = 0;
    virtual const char* remoteHost() = 0;
    virtual uint16_t localPort() = 0;
    virtual uint16_t remotePort() = 0;
    int option(int option, int value);
    int option(int option);
        
    int configureBlocking(bool mode);
    
protected:
    SocketImpl_T* _pImpl;
    bool _blocking;
};
//---------------------------------------------------------------------------
//  declaration of the ServerSocket_T class
//---------------------------------------------------------------------------
class ServerSocket_T : public SocketInterface_T
{
public:
    ServerSocket_T(const char* host, int port);
    ServerSocket_T(const string& host, int port);
    ServerSocket_T(uint32_t host, int port);
    ServerSocket_T(int port);
    ServerSocket_T();
    virtual ~ServerSocket_T(void);
    
    void bind(int port);
    Socket_T* accept();

    int close();

    virtual int sfd();

    const char* localHost();
    const char* remoteHost();
    uint16_t localPort();
    uint16_t remotePort();

private:
    //  Disallow to use
    ServerSocket_T(const ServerSocket_T& rhs);
    ServerSocket_T& operator=(const ServerSocket_T& rhs);

    void init(InetAddress_T& inetAddr);
};

//---------------------------------------------------------------------------
//  declaration of the Socket_T class
//---------------------------------------------------------------------------
class Socket_T : public SocketInterface_T
{
public:
    Socket_T();
    Socket_T(InetAddress_T& inetAddr);
    Socket_T(const char* host, int port);
    Socket_T(const string& host, int port);
    Socket_T(uint32_t host, int port);
    Socket_T(SocketImpl_T* pImpl);
    virtual ~Socket_T();

    void close();
    void connect(InetAddress_T& inetAddr);
    void connect(const char* host, int port);
    void connect(const string& host, int port);
    void connect(uint32_t host, int port);
    virtual int sfd();

    const char* localHost();
    const char* remoteHost();
    uint16_t localPort();
    uint16_t remotePort();
    
    
private:
    //  Disallow to use
    Socket_T(const Socket_T& rhs);
    Socket_T& operator= (const Socket_T& rhs);

    void create();
};

class SocketException_T : public exception
{
public:
    SocketException_T(const char* what) throw();
    SocketException_T(const string& what) throw();
    virtual ~SocketException_T() throw() {};

    virtual const char* what() const throw();
private:
    string _what;
};

#endif                                        // #ifndef
