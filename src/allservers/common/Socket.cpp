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
//  Class Socket_T:
//  Class Socket_T:
//  Class UdpReceiver_T:
//---------------------------------------------------------------------------
#include "common/Socket.h"
#include "common/Utils.h"

InetAddress_T::InetAddress_T(uint32_t host, uint16_t port)
{
    init(host, port);
}
InetAddress_T::InetAddress_T(const char* host, uint16_t port)
{
    init(InetAddress_T::toInetHost(host), port);
}
InetAddress_T::InetAddress_T(const string& host, uint16_t port)
{
    init(InetAddress_T::toInetHost(host.c_str()), port);
}
InetAddress_T::InetAddress_T(struct sockaddr& saddr)
{
    memcpy(&_addr, &saddr, sizeof(_addr));
}
InetAddress_T::InetAddress_T(struct sockaddr_in& saddrin)
    : _addr(saddrin)
{

}
void InetAddress_T::init(uint32_t host, uint16_t port)
{
    _addr.sin_addr.s_addr = host;
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
}

const char* InetAddress_T::toStringHost(uint32_t ipnet)
{
    struct in_addr addr;
    addr.s_addr = ipnet;
    return inet_ntoa(addr);
}

const char* InetAddress_T::toStringHost(struct sockaddr& saddr)
{
    return toStringHost(*((struct sockaddr_in*)&saddr));
}

const char* InetAddress_T::toStringHost(struct sockaddr_in& saddrin)
{
    return toStringHost(saddrin.sin_addr.s_addr);
}

uint32_t InetAddress_T::toInetHost(const char* host)
{
    uint32_t nhost;
    /* First try it as aaa.bbb.ccc.ddd. */
    nhost = inet_addr(host);
    if (nhost != ULONG_MAX)
    {
        return nhost;
    }

    struct hostent *hostt;
    if ((hostt = gethostbyname(host)) == NULL)
    {
        return 0;
    }
    return (*(struct in_addr *) *hostt->h_addr_list).s_addr;
}

uint32_t InetAddress_T::toInetHost(const string& host)
{
    return InetAddress_T::toInetHost(host.c_str());
}

uint32_t InetAddress_T::toInetHost(struct sockaddr& saddr)
{
    return InetAddress_T::toInetHost(*((struct sockaddr_in*)&saddr));
}

uint32_t InetAddress_T::toInetHost(struct sockaddr_in& saddrin)
{
    return saddrin.sin_addr.s_addr;
}

struct sockaddr_in* InetAddress_T::sockAddrIn()
{
    return &_addr;
}

struct sockaddr* InetAddress_T::sockAddr()
{
    return (struct sockaddr*)&_addr;
}

const char* InetAddress_T::strIp()
{
    return toStringHost(_addr);
}

uint32_t InetAddress_T::netIp()
{
    return _addr.sin_port;
}

uint16_t InetAddress_T::hostPort()
{
    return ntohs(_addr.sin_port);
}

uint16_t InetAddress_T::netPort()
{
    return _addr.sin_port;
}

string InetAddress_T::strPort()
{
    char port[10];
    sprintf(port, "%d", ntohs(_addr.sin_port));
    return port;
}


//---------------------------------------------------------------------------
//  SocketImpl_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the SocketImpl_T object
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
SocketImpl_T::SocketImpl_T()
    : _fd(-1), _pLocalAddr(0), _pRemoteAddr(0)
{

}

//---------------------------------------------------------------------------
//  SocketImpl_T()-Destructure
//---------------------------------------------------------------------------
//  Description:  Destructure the object and realeas the recources
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
SocketImpl_T::~SocketImpl_T(void)
{
    this->close();
}

//---------------------------------------------------------------------------
//  SocketImpl_T::atoaddr
//---------------------------------------------------------------------------
//  Description:  Set the struct sockaddr_in object according to host typed
//                              char*
//  Parameters:   host: ip address like xxx.xxx.xxx.xxx IN
//                              address: struct sockaddr_in OUT
//  Return:       0: success; -1: failure
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::atoaddr(const char* host, struct sockaddr_in* address)
{
    /* First try it as aaa.bbb.ccc.ddd. */
    if ((address->sin_addr.s_addr = inet_addr(host)) == 0)
    {
      return 0;
    }

    struct hostent *hostt;

    if ((hostt = gethostbyname(host)) < 0)
    {
        return -1;
    }
    address->sin_addr.s_addr = (*(struct in_addr *) *hostt->h_addr_list).s_addr;

    return 0;
}
//---------------------------------------------------------------------------
//  SocketImpl_T::accept
//---------------------------------------------------------------------------
//  Description:  Accept a remote connection request
//  Parameters:   socketImpl: the acceptor

//  Return:       A new file descriptor holding in _fd
//  Exception:    None
//  MT Safe:      false
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::accept(SocketImpl_T* socketImpl)
{
    while ((_fd = ::accept(socketImpl->sfd(), NULL, NULL)) < 0)
    {
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
        {   // Either a real error occured, or blocking was interrupted for
            // some reason.  Only abort execution if a real error occured.
            // EINTR is 'interruped system call' error
            // don't return - do the accept again
            continue;
        }
        return -1;
    }
    return _fd;
}

int SocketImpl_T::bind(InetAddress_T& inetAddr)
{
    if (_pLocalAddr)
    {
        delete _pLocalAddr;
    }
    _pLocalAddr = new InetAddress_T(inetAddr);
    if (!_pLocalAddr)
    {
        return -1;
    }
    return ::bind(_fd, _pLocalAddr->sockAddr(), sizeof(struct sockaddr));
}

//---------------------------------------------------------------------------
//  SocketImpl_T::bind
//---------------------------------------------------------------------------
//  Description:  Bind _localAddr to the parameters supplied
//  Parameters:   host: the ip address
//                              port:   the port
//  Return:       0: success; -1: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::bind(const char* host, int port)
{
    InetAddress_T inetAddr(host, port);
    return this->bind(inetAddr);
}

//---------------------------------------------------------------------------
//  SocketImpl_T::bind
//---------------------------------------------------------------------------
//  Description:  Bind _localAddr to the parameters supplied
//  Parameters:   host: the ip address
//                              port:   the port
//  Return:       0: success; -1: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::bind(const string& host, int port)
{
    InetAddress_T inetAddr(host, port);
    return this->bind(inetAddr);
}

//---------------------------------------------------------------------------
//  SocketImpl_T::bind
//---------------------------------------------------------------------------
//  Description:  Bind _localAddr to the parameters supplied
//  Parameters:   host: the ip address
//                              port:   the port
//  Return:       0: success; -1: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::bind(uint32_t host, int port)
{
    InetAddress_T inetAddr(host, port);
    return this->bind(inetAddr);
}

//---------------------------------------------------------------------------
//  SocketImpl_T::bind
//---------------------------------------------------------------------------
//  Description:  Bind _localAddr to the parameters supplied
//  Parameters:   port: the port
//  Return:       0: success; -1: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::bind(int port)
{
    InetAddress_T inetAddr(INADDR_ANY, port);
    return this->bind(inetAddr);
}

//---------------------------------------------------------------------------
//  SocketImpl_T::close
//---------------------------------------------------------------------------
//  Description:  Close the file descriptor
//  Parameters:   None
//  Return:       0: success; other: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::close()
{
    if (_fd)
    {
        cout<<" Closing fd "<< _fd <<endl;
        return ::close(_fd);
    }
    return 0;
}

int SocketImpl_T::connect(InetAddress_T& inetAddr)
{
    if (_pRemoteAddr)
    {
        delete _pRemoteAddr;
    }
    _pRemoteAddr = new InetAddress_T(inetAddr);
    if (!_pRemoteAddr)
    {
        return -1;
    }
    return ::connect(_fd, _pRemoteAddr->sockAddr(), sizeof(struct sockaddr));
}

//---------------------------------------------------------------------------
//  SocketImpl_T::connect
//---------------------------------------------------------------------------
//  Description:  Try to connect to host at port
//  Parameters:   host: endpoint ip address
//                              port: endpoint port
//  Return:       0: success; other: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::connect(const char* host, int port)
{
    InetAddress_T inetAddr(host, port);
    return this->connect(inetAddr);
}

//---------------------------------------------------------------------------
//  SocketImpl_T::connect
//-------------------------------------------------Fail to c--------------------------
//  Description:  Try to connect to host at port
//  Parameters:   host: endpoint ip address
//                              port: endpoint port
//  Return:       0: success; other: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::connect(const string& host, int port)
{
    InetAddress_T inetAddr(host, port);
    return this->connect(inetAddr);
}

//---------------------------------------------------------------------------
//  SocketImpl_T::connect
//---------------------------------------------------------------------------
//  Description:  Try to connect to host at port
//  Parameters:   host: endpoint ip address
//                              port: endpoint port
//  Return:       0: success; other: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::connect(uint32_t host, int port)
{
    InetAddress_T inetAddr(host, port);
    return this->connect(inetAddr);
}
//---------------------------------------------------------------------------
//  SocketImpl_T::create
//---------------------------------------------------------------------------
//  Description:  Create a socket typed SOCK_STREAM or SOCK_DGRAM according to
//                              stream parameter
//  Parameters:   stream: true for SOCK_STREAM, false for SOCK_DGRAM
//  Return:       0: success, -1: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::create(bool stream)
{
    if (stream)
    {
        /* step1: create a socket */
        if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            return -1;
        }
    }
    else
    {
        if ((_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            return -1;
        }
    }

    if (option(SO_REUSEADDR, 1) < 0)
    {
        return -1;
  }
  if (option(SO_KEEPALIVE, 1) < 0)
    {
        return -1;
    }
    return _fd;
}

//---------------------------------------------------------------------------
//  SocketImpl_T::listen
//---------------------------------------------------------------------------
//  Description:  Listen the socket file descriptor
//  Parameters:   None
//  Return:       0: success, other: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::listen(int count)
{
    return ::listen (_fd, count);
}

//---------------------------------------------------------------------------
//  SocketImpl_T::sfd
//---------------------------------------------------------------------------
//  Description:  Return the file descriptor
//  Parameters:   None
//  Return:       Return the file descriptor
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::sfd()
{
    return _fd;
}

//---------------------------------------------------------------------------
//  SocketImpl_T::localHost
//---------------------------------------------------------------------------
//  Description:  Return the local host
//  Parameters:   None
//  Return:       Return the local host

//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
const char* SocketImpl_T::localHost()
{
    struct sockaddr addr;
    socklen_t len = sizeof(struct sockaddr);
    getsockname(_fd, (struct sockaddr*)&addr, &len);

    return InetAddress_T::toStringHost(addr);
}
//---------------------------------------------------------------------------
//  SocketImpl_T::remoteHost
//---------------------------------------------------------------------------
//  Description:  Return the remote host
//  Parameters:   None
//  Return:       Return the remote host
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
const char* SocketImpl_T::remoteHost()
{
    struct sockaddr addr;
    socklen_t len = sizeof(struct sockaddr);
    getpeername(_fd, (struct sockaddr*)&addr, &len);

    return InetAddress_T::toStringHost(addr);
}
//---------------------------------------------------------------------------
//  SocketImpl_T::localPort
//---------------------------------------------------------------------------
//  Description:  Return the local port
//  Parameters:   None
//  Return:       Return the local port
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
uint16_t SocketImpl_T::localPort()
{
    struct sockaddr addr;
    socklen_t len = sizeof(struct sockaddr);
    getsockname(_fd, (struct sockaddr*)&addr, &len);

    return ntohs(((struct sockaddr_in*)&addr)->sin_port);
}

//---------------------------------------------------------------------------
//  SocketImpl_T::remotePort
//---------------------------------------------------------------------------
//  Description:  Return the remote port
//  Parameters:   None
//  Return:       Return the remote port
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
uint16_t SocketImpl_T::remotePort()
{
    struct sockaddr addr;
    socklen_t len = sizeof(struct sockaddr);
    getpeername(_fd, (struct sockaddr*)&addr, &len);

    return ntohs(((struct sockaddr_in*)&addr)->sin_port);
}

//---------------------------------------------------------------------------
//  SocketImpl_T::option
//---------------------------------------------------------------------------
//  Description:  Return the option status
//  Parameters:   None
//  Return:       the option status: 1, on; 0, off
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::option(int option)
{
    int value;
    socklen_t size = sizeof(value);
    if (getsockopt(
    _fd,
        SOL_SOCKET,
    option,
    &value,
    &size) < 0)
    {
        return -1;
    }
    return value;
}

//---------------------------------------------------------------------------
//  SocketImpl_T::option
//---------------------------------------------------------------------------
//  Description:  Set the option status
//  Parameters:   option: option to be set; value: the option value
//  Return:       0: success; -1: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketImpl_T::option(int option, int value)
{
    return setsockopt(
    _fd,
    SOL_SOCKET,
    option,
    &value,
    sizeof(value)
    );
}
//---------------------------------------------------------------------------
//  SocketInterface_T::Constructure
//---------------------------------------------------------------------------
//  Description:  Create a new SocketImpl_T object
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
SocketInterface_T::SocketInterface_T()
    : _blocking(true)
{
    _pImpl = new SocketImpl_T;
    if (!_pImpl)
    {
        throw SocketException_T(
                string("Fail to create SocketImpl_T\n"
                        "\tat SocketInterface_T::SocketInterface_T")+
                SrcLocation_T(__FILE__, __LINE__));
    }
}
//---------------------------------------------------------------------------
//  SocketInterface_T::Constructure
//---------------------------------------------------------------------------
//  Description:  Assign _pImpl to paraFail to create SocketImpl_Tmeter pImpl
//  Parameters:   pImpl: a SocketImpl_T object
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
SocketInterface_T::SocketInterface_T(SocketImpl_T* pImpl)
    : 
    _pImpl(pImpl),
    _blocking(true)
{
    // No body
}
//---------------------------------------------------------------------------
//  SocketImpl_T::Destructure
//---------------------------------------------------------------------------
//  Description:  Delete _pImpl object
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
SocketInterface_T::~SocketInterface_T()
{
  if (_pImpl)
  {
    delete _pImpl;
  }
}

int SocketInterface_T::option(int option)
{
    return _pImpl->option(option);
}

int SocketInterface_T::option(int option, int value)
{
    return _pImpl->option(option, value);
}

//---------------------------------------------------------------------------
//  SocketInterface_T::setBlocking
//---------------------------------------------------------------------------
//  Description:  Set the file descriptor working as block or non-block mode
//  Parameters:   true: block mode; false non-block mode
//  Return:       0: success; -1: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketInterface_T::configureBlocking(bool blocking)
{
    _blocking = blocking;
    int flags;
    
    /* Get socket blocking status */
    if ((flags = fcntl(sfd(), F_GETFL, 0)) < 0)
    {
        /* Handle error */
        return flags;
    }
    
    if (blocking)
    {
        /* Blocking */
        if ((flags = fcntl(sfd(), F_SETFL, flags & (~O_NONBLOCK))) < 0)
        {
          /* Handle error */
          return flags;
        }
    }
    else
    {
        /* NoBlocking */
        if ((flags = fcntl(sfd(), F_SETFL, flags | O_NONBLOCK)) < 0)
        {
          /* Handle error */
          return flags;
        }
    }
    
    return 0;
}

//---------------------------------------------------------------------------
//  ServerSocket_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the ServerSocket_T object
//  Parameters:   port: server socket listening port
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
ServerSocket_T::ServerSocket_T(const char* host, int port)
{
    InetAddress_T inetAddr(host, port);
    init(inetAddr);
}

//---------------------------------------------------------------------------
//  ServerSocket_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the ServerSocket_T object
//  Parameters:   port: server socket listening port
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
ServerSocket_T::ServerSocket_T(const string& host, int port)
{
  InetAddress_T inetAddr(host, port);
    init(inetAddr);
}

//---------------------------------------------------------------------------
//  ServerSocket_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the ServerSocket_T object
//  Parameters:   port: server socket listening port
//  Return:       None
//  Exception:    None
//  MT Safe:      FalseFail to create SocketImpl_T
//  Note:         None
//---------------------------------------------------------------------------
ServerSocket_T::ServerSocket_T(uint32_t host, int port)
{
  InetAddress_T inetAddr(host, port);
    init(inetAddr);
}


//---------------------------------------------------------------------------
//  ServerSocket_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the ServerSocket_T object
//  Parameters:   port: server socket listening port
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
ServerSocket_T::ServerSocket_T(int port)
{
  InetAddress_T inetAddr(INADDR_ANY, port);
    init(inetAddr);
}

ServerSocket_T::ServerSocket_T()
{
  if (_pImpl->create(true) < 0)
  {
    throw SocketException_T(
                string( "Creating socket failed(")+
                strerror(errno)+")\n"+
                "\tat ServerSocket_T::init"+
                SrcLocation_T(__FILE__, __LINE__));
  }
}


void ServerSocket_T::init(InetAddress_T& inetAddr)
{
  if (_pImpl->create(true) < 0)
  {
    throw SocketException_T(
                string( "Creating socket failed(")+
                strerror(errno)+")\n"+
                "\tat ServerSocket_T::init"+
                SrcLocation_T(__FILE__, __LINE__));
  }
  if (_pImpl->bind(inetAddr) < 0)
  {
    throw SocketException_T(
        string("Binding socket failed(")+
        strerror(errno)+")\n"+
        "\tat ServerSocket_T::init"+
        SrcLocation_T(__FILE__, __LINE__));
  }
  if (_pImpl->listen() < 0)
  {
    throw SocketException_T(
                string( "Listening on socket faild(")+
                strerror(errno)+")\n"+
                "\tat ServerSocket_T::init"+
                SrcLocation_T(__FILE__, __LINE__));
  }
}

void ServerSocket_T::bind(int port)
{
  InetAddress_T inetAddr(INADDR_ANY, port);
  
  if (_pImpl->bind(inetAddr) < 0)
  {
    throw SocketException_T(
        string("Binding socket failed(")+
        strerror(errno)+")\n"+
        "\tat ServerSocket_T::init"+
        SrcLocation_T(__FILE__, __LINE__));
  }
  if (_pImpl->listen() < 0)
  {
    throw SocketException_T(
                string( "Listening on socket faild(")+
                strerror(errno)+")\n"+
                "\tat ServerSocket_T::init"+
                SrcLocation_T(__FILE__, __LINE__));
  }
}


//---------------------------------------------------------------------------
//  ServerSocket_T()-Destructure
//---------------------------------------------------------------------------
//  Description:  Destructure the object and realeas the recources
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
ServerSocket_T::~ServerSocket_T(void)
{
  // No body
}
//---------------------------------------------------------------------------
//  ServerSocket_T::accept
//---------------------------------------------------------------------------
//  Description:  Accept connection
//  Parameters:   None
//  Return:       A new Socket_T object on success, NULL on fail
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
Socket_T* ServerSocket_T::accept()
{
    SocketImpl_T* pImpl = new SocketImpl_T;

    if (!_pImpl)
    {
        throw SocketException_T(
                string( "Fail to create SocketImpl_T\n"
                        "\tat ServerSocket_T::accept")+
                SrcLocation_T(__FILE__, __LINE__));
    }

    if (pImpl->accept(_pImpl) < 0)
    {
        throw SocketException_T(
                string( "Accepting on socket failed(")+
                strerror(errno)+")\n"+
                "\tat ServerSocket_T::accept"+
                SrcLocation_T(__FILE__, __LINE__));
    }
    Socket_T* pSocket = new Socket_T(pImpl);
    if (!pSocket)
    {
        throw SocketException_T(
                string( "Fail to create Socket_T\n"
                        "\tat ServerSocket_T::accept")+
                SrcLocation_T(__FILE__, __LINE__));
    }
    return pSocket;
}

//---------------------------------------------------------------------------
//  SocketImpl_T::localHost
//---------------------------------------------------------------------------
//  Description:  Return the local host
//  Parameters:   None
//  Return:       Return the local host
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
const char* ServerSocket_T::localHost()
{
  return _pImpl->localHost();
}
//---------------------------------------------------------------------------
//  SocketImpl_T::remoteHost
//---------------------------------------------------------------------------
//  Description:  Return the remote host
//  Parameters:   None
//  Return:       Return the remote host
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
const char* ServerSocket_T::remoteHost()
{
  return "NULL";
}
//---------------------------------------------------------------------------
//  ServerSocket_T::localPort
//---------------------------------------------------------------------------
//  Description:  Return the local listening port
//  Parameters:   None
//  Return:       The local listening port
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
uint16_t ServerSocket_T::localPort()
{
    return _pImpl->localPort();
}

//---------------------------------------------------------------------------
//  ServerSocket_T::remotePort
//---------------------------------------------------------------------------
//  Description:  Return the remote port
//  Parameters:   None
//  Return:       The remote port
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
uint16_t ServerSocket_T::remotePort()
{
    return 0;
}
//---------------------------------------------------------------------------
//  ServerSocket_T::sfd
//---------------------------------------------------------------------------
//  Description:  Return the file descriptor
//  Parameters:   None
//  Return:       The file descriptor
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int ServerSocket_T::sfd()
{
    return _pImpl->sfd();
}

//---------------------------------------------------------------------------
//  Socket_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the Socket_T object
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
Socket_T::Socket_T()
{
    create();
}
//---------------------------------------------------------------------------
//  Socket_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the Socket_T object
//  Parameters:   host: The endpoint host
//                port: The endpoint port
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
Socket_T::Socket_T(const char* host, int port)
{
    create();
    InetAddress_T inetAddr(host, port);
    this->connect(inetAddr);
}

//---------------------------------------------------------------------------
//  Socket_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the Socket_T object
//  Parameters:   host: The endpoint host
//                port: The endpoint port
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
Socket_T::Socket_T(const string& host, int port)
{
    create();
    InetAddress_T inetAddr(host, port);
    this->connect(inetAddr);
}

//---------------------------------------------------------------------------
//  Socket_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the Socket_T object
//  Parameters:   host: The endpoint host
//                port: The endpoint port
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
Socket_T::Socket_T(uint32_t host, int port)
{
    create();
    InetAddress_T inetAddr(host, port);
    this->connect(inetAddr);
}
//---------------------------------------------------------------------------
//  Socket_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the Socket_T object
//  Parameters:   socketImpl: A established socket
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
Socket_T::Socket_T(SocketImpl_T* pImpl)
    : SocketInterface_T(pImpl)
{
  // No body
}

//---------------------------------------------------------------------------
//  Socket_T()-Destructure
//---------------------------------------------------------------------------
//  Description:  Destructure
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
Socket_T::~Socket_T(void)
{
  // No body
}

void Socket_T::create()
{
    if (_pImpl->create(true) < 0)
    {
        throw SocketException_T(
                string( "Creating socket failed(")+
                strerror(errno)+")\n"+
                "\tat Socket_T::create"+                
                SrcLocation_T(__FILE__, __LINE__));
    }
}

void Socket_T::connect(InetAddress_T& inetAddr)
{
    if (_pImpl->connect(inetAddr) < 0)
    {
        throw SocketException_T(
                string( "Connecting socket(")+
                inetAddr.strIp()+
                ":"+
                inetAddr.strPort()+
                ") failed("+
                strerror(errno)+")\n"+
                "\tat Socket_T::connect"+
                SrcLocation_T(__FILE__, __LINE__));
    }
}
//---------------------------------------------------------------------------
//  Socket_T::connect
//---------------------------------------------------------------------------
//  Description:  Connect to endpoint
//  Parameters:   host: The endpoint ip address
//                port: The endpoint port
//  Return:       None
//  Exception:    None
//  MT Safe:      None
//  Note:         None
//---------------------------------------------------------------------------
void Socket_T::connect(const char* host, int port)
{
    InetAddress_T inetAddr(host, port);
    this->connect(inetAddr);
}

//---------------------------------------------------------------------------
//  Socket_T::connect
//---------------------------------------------------------------------------
//  Description:  Connect to endpoint
//  Parameters:   host: The endpoint ip address
//                port: The endpoint port
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
void Socket_T::connect(const string& host, int port)
{
    InetAddress_T inetAddr(host, port);
    this->connect(inetAddr);
}
//---------------------------------------------------------------------------
//  Socket_T::connect
//---------------------------------------------------------------------------
//  Description:  Connect to endpoint
//  Parameters:   host: The endpoint ip address
//                port: The endpoint port
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
void Socket_T::connect(uint32_t host, int port)
{
    InetAddress_T inetAddr(host, port);
    this->connect(inetAddr);
}
//---------------------------------------------------------------------------
//  Socket_T::sfd
//---------------------------------------------------------------------------
//  Description:  Return the file descriptor
//  Parameters:   The file descriptor
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int Socket_T::sfd()
{
    return _pImpl->sfd();
}


//---------------------------------------------------------------------------
//  Socket_T::localHost
//---------------------------------------------------------------------------
//  Description:  Return the local host
//  Parameters:   None
//  Return:       Return the local host

//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
const char* Socket_T::localHost()
{
    return _pImpl->localHost();
}
//---------------------------------------------------------------------------
//  Socket_T::remoteHost
//---------------------------------------------------------------------------
//  Description:  Return the remote host
//  Parameters:   None
//  Return:       Return the remote host
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
const char* Socket_T::remoteHost()
{
    return _pImpl->remoteHost();
}
//---------------------------------------------------------------------------
//  Socket_T::localPort
//---------------------------------------------------------------------------
//  Description:  Return the local port
//  Parameters:   None
//  Return:       Return the local port
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
uint16_t Socket_T::localPort()
{
    return _pImpl->localPort();
}
//---------------------------------------------------------------------------
//  Socket_T::remotePort
//---------------------------------------------------------------------------
//  Description:  Return the remote port
//  Parameters:   None
//  Return:       Return the remote port
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
uint16_t Socket_T::remotePort()
{
    return _pImpl->remotePort();
}


//---------------------------------------------------------------------------
//  SocketException_T()-Constructure
//---------------------------------------------------------------------------
//  Description:
//  Parameters:   what:
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
SocketException_T::SocketException_T(const char* what) throw()
    : _what(what)
{
    // No body
}

//---------------------------------------------------------------------------
//  SocketException_T()-Constructure
//---------------------------------------------------------------------------
//  Description:
//  Parameters:   what:
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
SocketException_T::SocketException_T(const string& what) throw()
    : _what(what)
{
    // No body
}

//---------------------------------------------------------------------------
//  SocketException_T()-Constructure
//---------------------------------------------------------------------------
//  Description:
//  Parameters:   what:
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
const char* SocketException_T::what() const throw()
{
    return _what.c_str();
}
