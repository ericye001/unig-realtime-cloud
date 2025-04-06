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

#include "common/SocketStream.h"
#include "common/Utils.h"

//---------------------------------------------------------------------------
//  SocketStream_T()-Constructure
//---------------------------------------------------------------------------
//  Description:  Construct the ServerSocket_T object and initSocket
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
SocketStream_T::SocketStream_T(int fd)
    : _fd(fd)
{
    ignorePipe();
}

//---------------------------------------------------------------------------
//  SocketStream_T()-Destructure
//---------------------------------------------------------------------------
//  Description:  Destructure the object and realeas the recources
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
SocketStream_T::~SocketStream_T(void)
{
    // No body
}

//---------------------------------------------------------------------------
//  SocketStream_T::read
//---------------------------------------------------------------------------
//  Description:  Same as read system call
//  Parameters:   None
//  Return:       the number of bytes read
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketStream_T::read(void* buf, int len)
{
    int bytes;
    do 
      {
        bytes = ::read(_fd, buf, len);
        
      } while (bytes <= 0 && errno == EINTR);
    
    if(! bytes )
      {
        cout<<"  SocketStream_T::read(void* buf, int len) "<< bytes <<endl;
      }
    
    if (bytes <= 0 && errno == EAGAIN) 
      {
        return 0;
      }
    
    return bytes > 0 ? bytes : -1;
}

//---------------------------------------------------------------------------
//  SocketStream_T::read
//---------------------------------------------------------------------------
//  Description:  Same as above except that the buf starts from off
//  Parameters:   None
//  Return:       Same as above
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketStream_T::read(void* buf, int off, int len)
{
    return this->read((char*)buf+off, len);
}

//---------------------------------------------------------------------------
//  SocketStream_T::readBytes
//---------------------------------------------------------------------------
//  Description:  Read from file descriptor until len bytes read or some error
//                              occured
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
void SocketStream_T::readBytes(void* buf, int len)
{
  int bytes_read = 0;
    int this_read;

    while (bytes_read < len)
    {
        if ((this_read = this->read(buf, bytes_read, len - bytes_read)) < 0)
       //removeed by moah
        {
          if (errno ==EINTR||this_read<=0)
            {
              //cout<<" this_read = "<<this_read <<endl;
              cout<<" catch read error code = "<<strerror(errno)<<endl;
              //continue;
            }
          
          throw StreamException_T(
                                  errno, 
                                  string( "Fail to read from socket\n"
                                          "\tat SocketStream_T::readBytes")+
                                  SrcLocation_T(__FILE__, __LINE__));
        }
        bytes_read += this_read;
       
          break;
    }
   // cout<<"--########   bytes_read is ####"<<  bytes_read<<endl;
}

//---------------------------------------------------------------------------
//  SocketStream_T::readBytes
//---------------------------------------------------------------------------
//  Description:  Same as above except that the buf starts from off
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
void SocketStream_T::readBytes(void* buf, int off, int len)
{
    this->readBytes((char*)buf+off, len);
}

//---------------------------------------------------------------------------
//  SocketStream_T::write
//---------------------------------------------------------------------------
//  Description:  Same as write system call
//  Parameters:   None
//  Return:       The number of bytes written
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketStream_T::write(void* buf, int len)
{
    int bytes;
    do {
        bytes = ::write(_fd, buf, len);
    } while ((bytes < 0) && (errno == EINTR || errno == EAGAIN));
    
    // cout<<"SocketStream_T::write() len = "<<bytes <<endl;
    return bytes > 0 ? bytes : -1;
}

//---------------------------------------------------------------------------
//  SocketStream_T::write
//---------------------------------------------------------------------------
//  Description:  Same as above except that the buf starts from off
//  Parameters:   None
//  Return:       Same as above
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int SocketStream_T::write(void* buf, int off, int len)
{
    return this->write((char*)buf+off, len);
}

//---------------------------------------------------------------------------
//  SocketStream_T::writeBytes
//---------------------------------------------------------------------------
//  Description:  Write to file descriptor until len bytes written or some
//                              error occured
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
void SocketStream_T::writeBytes(void* buf, int len)
{
    int bytes_sent = 0;
    int this_write;

    while (bytes_sent < len)
    {
        if ((this_write = this->write(buf, bytes_sent, len - bytes_sent)) < 0)
        {
			if (errno ==EINTR)
			{
				cout<<"catch write error code = "<<strerror(errno)<<endl;
				continue;
			}
			
            throw StreamException_T(
                        errno, 
                        string( "Connection broken up\n"
                                "\tat SocketStream_T::writeBytes")+
                            SrcLocation_T(__FILE__, __LINE__));
        }        
        bytes_sent += this_write;

    }
    // cout<<" SocketStream_T::writeBytes() len ="<< bytes_sent <<endl;
}

//---------------------------------------------------------------------------
//  SocketStream_T::writeBytes
//---------------------------------------------------------------------------
//  Description:  Same as above except that the buf starts from off
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
void SocketStream_T::writeBytes(void* buf, int off, int len)
{
  this->writeBytes((char*)buf+off, len);
}



//---------------------------------------------------------------------------
//  SocketStream_T::ignorePipe
//---------------------------------------------------------------------------
//  Description:  Set the file descriptor working as block or non-block mode
//  Parameters:   true: block mode; false non-block mode
//  Return:       0: success; -1: failure
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
void SocketStream_T::ignorePipe()
{
    struct sigaction stSig;

    stSig.sa_handler = SIG_IGN;
    stSig.sa_flags = 0;
    sigemptyset(&stSig.sa_mask);
    sigaction(SIGPIPE,&stSig,NULL);
}


//---------------------------------------------------------------------------
//  StreamException_T()-Constructure
//---------------------------------------------------------------------------
//  Description:
//  Parameters:   what:
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
StreamException_T::StreamException_T(int code, const char* what) throw()
    : _code(code), _what(what)
{
    // No body
}

//---------------------------------------------------------------------------
//  StreamException_T()-Constructure
//---------------------------------------------------------------------------
//  Description:
//  Parameters:   what:
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
StreamException_T::StreamException_T(int code, const string& what) throw()
    : _code(code), _what(what)
{
    // No body
}

//---------------------------------------------------------------------------
//  StreamException_T()-code
//---------------------------------------------------------------------------
//  Description:
//  Parameters:   what:
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
int StreamException_T::code()
{
    return _code;
}
//---------------------------------------------------------------------------
//  StreamException_T()-Constructure
//---------------------------------------------------------------------------
//  Description:
//  Parameters:   what:
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
const char* StreamException_T::what() const throw()
{
    return _what.c_str();
}
