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

#include "common/IOStream.h"
#include "common/Utils.h"

//---------------------------------------------------------------------------
//  IOStream_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the ServerSocket_T object and initSocket
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
IOStream_T::IOStream_T(int fd, bool blocking)
    : 
    _fd(fd),
    _blocking(blocking)
{
    ignoreSigPipe();
    configureBlocking(blocking);
    _state[0] = _state[1] = 0;
}

//---------------------------------------------------------------------------
//  IOStream_T()-Destructure
//---------------------------------------------------------------------------
//  Description:  Destructure the object and realeas the recources
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
IOStream_T::~IOStream_T(void)
{
    // No body
   
    // close(_fd);
    if (_state[0]) {
        delete _state[0];
    }
    if (_state[1]) {
        delete _state[1];
    }    
}

int IOStream_T::getFd()
{
    return _fd;
}

IOState_T* IOStream_T::getState()
{
    if (!_state[_blocking]) {
        if (_blocking) {
            _state[_blocking] = new IOBlocking_T;
        } else {
            _state[_blocking] = new IONonblocking_T;
        }
    }
    return _state[_blocking];
}

        
//---------------------------------------------------------------------------
//  IOStream_T::read
//---------------------------------------------------------------------------
//  Description:  Same as read system call
//  Parameters:   None
//  Return:       the number of bytes read
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int IOStream_T::read(void* buf, int len)
{
 

    return getState()->read(getFd(), buf, len);
}

//---------------------------------------------------------------------------
//  IOStream_T::readBytes
//---------------------------------------------------------------------------
//  Description:  Read from file descriptor until len bytes read or some error
//                              occured
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
void IOStream_T::readBytes(void* buf, int len)
{

  // cout<<"void IOStream_T::readBytes(void* buf, int len)"<<endl;
    int bytes_read = 0;
    int this_read;

    while (bytes_read < len)    {
        this_read = read((char*)buf + bytes_read, len - bytes_read);
        
       if (this_read < 0)  {
            throw StrException_T(
                    string( "ReadFault:") + strerror(errno) +
                            "at IOStream_T::readBytes"+
                            SrcLocation_T(__FILE__, __LINE__));
        }
        bytes_read += this_read;
    }
}

//---------------------------------------------------------------------------
//  IOStream_T::write
//---------------------------------------------------------------------------
//  Description:  Same as write system call
//  Parameters:   None
//  Return:       The number of bytes written
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int IOStream_T::write(void* buf, int len)
{
    return getState()->write(getFd(), buf, len);
}

//---------------------------------------------------------------------------
//  IOStream_T::writeBytes
//---------------------------------------------------------------------------
//  Description:  Write to file descriptor until len bytes written or some
//                              error occured
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
void IOStream_T::writeBytes(void* buf, int len)
{
    int bytes_sent = 0;
    int this_write;
  
    while (bytes_sent < len)    {
        this_write = write((char*)buf + bytes_sent, len - bytes_sent);
        if (this_write < 0)        {
            throw StrException_T(
                    string( "WriteFault")+strerror(errno)+
                            "at IOStream_T::writeBytes"+
                            SrcLocation_T(__FILE__, __LINE__));
        }        
        bytes_sent += this_write;
    }
    // cout<<"IOStream_T::writeBytes  Length = "<<bytes_sent<<endl;
   
}

void IOStream_T::configureBlocking(bool blocking)
{   

  //cout<<"void IOStream_T::configureBlocking(bool blocking) :"<<blocking <<endl ;
    _blocking = blocking;
    int flags;
    /* Get socket blocking status */
    if ((flags = fcntl(getFd(), F_GETFL, 0)) < 0) {
        /* Handle error */
        throw StrException_T(
                string(strerror(errno))+
                " at IOStream_T::configureBlocking"+
                SrcLocation_T(__FILE__, __LINE__));
                
    }    
    if (blocking) {
        /* Blocking */
        if (fcntl(getFd(), F_SETFL, flags & (~O_NONBLOCK)) < 0) {
            /* Handle error */
            throw StrException_T(
                string(strerror(errno))+
                " at IOStream_T::configureBlocking"+
                SrcLocation_T(__FILE__, __LINE__));
        }
    } else {
        /* NoBlocking */
        if (fcntl(getFd(), F_SETFL, flags | O_NONBLOCK) < 0) {
            /* Handle error */
            throw StrException_T(
                string(strerror(errno))+
                " at IOStream_T::configureBlocking"+
                SrcLocation_T(__FILE__, __LINE__));
        }
    }    
}
//---------------------------------------------------------------------------
//  IOStream_T::ignoreSigPipe
//---------------------------------------------------------------------------
//  Description:  Set the file descriptor working as block or non-block mode
//  Parameters:   true: block mode; false non-block mode
//  Return:       0: success; -1: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
void IOStream_T::ignoreSigPipe()
{
    struct sigaction stSig;

    stSig.sa_handler = SIG_IGN;
    stSig.sa_flags = 0;
    sigemptyset(&stSig.sa_mask);
    sigaction(SIGPIPE,&stSig,NULL);
}


int IOBlocking_T::read(int fd, void* buf, int len)
{

  //cout<<"int IOBlocking_T::read(int fd, void* buf, int len) "<<endl;
    int bytes;
    do {
        bytes = ::read(fd, buf, len);
    } while (bytes <= 0 && errno == EINTR);    
    
    return bytes > 0 ? bytes : -1;
}

int IOBlocking_T::write(int fd, void* buf, int len)
{
    int bytes;
    do {
        bytes = ::write(fd, buf, len);
    } while (bytes < 0 && errno == EINTR);
    
    return bytes > 0 ? bytes : -1;
    
}

int IONonblocking_T::read(int fd, void* buf, int len)
{

  
    int bytes;
    do {
        bytes = ::read(fd, buf, len);
       
    } while (bytes < 0 && errno == EINTR);

    if(bytes==0)
      {
        
        cout<<"erron :"<<errno<<endl;
    
        cout<<" fd  "<<      fd    <<endl;
      }
    if (bytes <0 && errno == EAGAIN) {
     
      
        return 0;
    }
    
    return bytes > 0 ? bytes : -1;
}

int IONonblocking_T::write(int fd, void* buf, int len)
{
    int bytes;
    do {
        bytes = ::write(fd, buf, len);
    } while (bytes < 0 && errno == EINTR);
    
    if (bytes <0 && errno == EAGAIN) {
        return 0;
    }
    return bytes > 0 ? bytes : -1;    
}
   
