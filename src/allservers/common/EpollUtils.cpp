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

#include "common/EpollUtils.h"

Epoll_T::Epoll_T(uint32_t maxWaitFds) 
    : ObjectLevelLockable_T<Epoll_T>(),
    _maxWaitFds(maxWaitFds)
{
    if ((_epfd = epoll_create(maxWaitFds)) < 0)
    {
        throw EpollException_T(
                string("epoll_create error(")+strerror(errno)+"\n"
                "\tat Epoll_T::Epoll_T"+
                SrcLocation_T(__FILE__, __LINE__));
    }
    _pEvents = new struct epoll_event [_maxWaitFds];
    if (!_pEvents)
    {
        throw EpollException_T(
                string( "Fail to create struct epoll_event\n"
                        "\tat Epoll_T::Epoll_T")+
                SrcLocation_T(__FILE__, __LINE__));
    }
}
Epoll_T::~Epoll_T()
{
    clear();
    close(_epfd);
    delete [] _pEvents;    
}
void Epoll_T::add(Connection_T* pConn)
{
 
    Lock_T lock(*this);

    if (_idMapConn.size() >= _maxWaitFds)
    {

        throw EpollException_T(
                string( "Too many Connection_T added into epoll\n"
                "\tat Epoll_T::add")+
                SrcLocation_T(__FILE__, __LINE__));
    }
    
    if (epoll_ctl(_epfd, EPOLL_CTL_ADD,
            pConn->socket()->sfd(), pConn->event()) < 0)
    {
        throw EpollException_T(
                string( "epoll_ctl error(")+strerror(errno)+")\n"
                "\tat Epoll_T::add"+
                SrcLocation_T(__FILE__, __LINE__));
    }
    //cout<<pConn->socket()->sfd()<<endl;
    _idMapConn.insert(make_pair(pConn->socket()->sfd(), pConn));


}   
void Epoll_T::setup(Connection_T* pConn)
{
    Lock_T lock(*this);
    if (epoll_ctl(_epfd, EPOLL_CTL_MOD,
            pConn->socket()->sfd(), pConn->event()) < 0)
    {
        throw EpollException_T(
                string( "epoll_ctl error(")+strerror(errno)+")\n"
                "\tat Epoll_T::setup"+
                SrcLocation_T(__FILE__, __LINE__));
    }
}

void Epoll_T::remove(Connection_T* pConn)
{
    Lock_T lock(*this);
 
    map<uint32_t, Connection_T*>::iterator gotIt;
    gotIt = _idMapConn.find(pConn->socket()->sfd());
    if (gotIt == _idMapConn.end())
    {
        return;
    }
    if(pConn)
      {
        if (epoll_ctl(_epfd, EPOLL_CTL_DEL,
                      pConn->socket()->sfd(), pConn->event()) < 0)
          {
            throw EpollException_T(
                                   string( "epoll_ctl error(")+strerror(errno)+")\n"
                                   "\tat Epoll_T::remove"+
                                   SrcLocation_T(__FILE__, __LINE__));
          }
      }
    _idMapConn.erase(gotIt);

}

void Epoll_T::wait(int timeout)
{


    int i, nfds;
    Connection_T* pConn;
    struct epoll_event *cevents;
    bool loop = (timeout == -1) ? true : false;
    struct timeval tv_begin;
    struct timeval tv_now;
    gettimeofday(&tv_begin, NULL);
    do 
    {
        {   // Use bracket to assume lock the right scope --- only epoll_wait
            // no used
            //Lock_T lock(*this);
            nfds = epoll_wait(_epfd, _pEvents, _maxWaitFds, timeout);
            if (nfds < 0)
            {
				if (errno ==EINTR)
				{
					cout<<"catch epoll_wait error code = "<<strerror(errno)<<endl;
					continue;
				}
			  
				throw EpollException_T( string("epoll_wait failed")+
										strerror(errno)+
										SrcLocation_T(__FILE__, __LINE__)); 


            }
        }
        /**
            Ok, events occured and held in events buffer
        */
        
        for (i = 0, cevents = _pEvents; i < nfds; i++, cevents++) {
         
            pConn = (Connection_T*)cevents->data.ptr;
            if (pConn->socket()->sfd() != -1) {
             
                if (cevents->events & pConn->event()->events)
                {
                 
                    /* The events occured match that we are waiting for,
                        so call the callback function.
                    */         
                  // cout<<"Ok, events occured and held in events buffer ::"<<pConn->length()<<" \n";
                 
                  pConn->seteventType(cevents->events);
                  pConn->action()->execute(pConn);
                }
            }
        }
        if (!loop)
        {
            gettimeofday(&tv_now, NULL);
            timeout -= (tv_now.tv_sec-tv_begin.tv_sec)*1000+(tv_now.tv_usec
                                                             - tv_begin.tv_usec)/1000;
            if (timeout <= 0)
            {
                return;
            }
        }
    } while (true);
}

void Epoll_T::clear()
{
    Lock_T lock(*this);
    map<uint32_t, Connection_T*>::iterator curIt;
    for (curIt = _idMapConn.begin(); curIt != _idMapConn.end(); )
    {
        Connection_T* pConn = curIt->second;   
		pConn->socket()->sfd();

        if (epoll_ctl(_epfd, EPOLL_CTL_DEL,
            pConn->socket()->sfd(), pConn->event()) < 0)
        {
            throw EpollException_T(
                    string( "epoll_ctl error(")+strerror(errno)+")\n"
                            "\tat Epoll_T::clear"+
                    SrcLocation_T(__FILE__, __LINE__));
        }
        _idMapConn.erase(curIt++);
    }
}

Connection_T* Epoll_T::find(uint32_t id)
{
    Lock_T lock(*this);
    map<uint32_t, Connection_T*>::iterator gotIt = _idMapConn.find(id);

    if (gotIt != _idMapConn.end())
    {
        return gotIt->second;
    }
    return NULL;
}

uint32_t Epoll_T::size()
{
    Lock_T lock(*this);
    return _idMapConn.size();
}
    
EpollException_T::EpollException_T(const char* what) throw()
    : _what(what)
{
    // No body
}

EpollException_T::EpollException_T(const string& what) throw()
    : _what(what)
{
    // No body
}

const char* EpollException_T::what() const throw()
{
    return _what.c_str();
}

Connection_T::Connection_T(uint32_t bufferSize)
    : _pSocket(0), 
      _pAction(0), 
      _bufferSize(bufferSize),
      _buffer(0),
      _length(0),
      _cursor(0)
{
    memset(&_event, 0, sizeof(_event));
    if (_bufferSize > 0)
    {
        _buffer = new char[_bufferSize];
        if (!_buffer)
        {
            throw EpollException_T(
                string( "Fail to create Connection_T\n"
                        "\tat Connection_T::Connection_T")+
                SrcLocation_T(__FILE__, __LINE__));
        }
        _cursor = _buffer;
    }
}

Connection_T::Connection_T(
                           SocketInterface_T* pSocket, 
                           uint32_t event, 
                           Action_T* pAction,
                           uint32_t bufferSize)
    :   _pSocket(pSocket), 
        _pAction(pAction),
        _bufferSize(bufferSize),
        _buffer(0),
        _length(0),
        _cursor(0)
{
  _event.events = event;
  _event.data.ptr = this;
  if (_bufferSize > 0)
    {
      _buffer = new char[_bufferSize];
      if (!_buffer)
        {
          throw EpollException_T(
                                 string( "Fail to create Connection_T\n"
                                         "\tat Connection_T::Connection_T")+
                                 SrcLocation_T(__FILE__, __LINE__));
        }
      _cursor = _buffer;
    }
}

Connection_T::~Connection_T()
{
  
  //cout<<" Connection_T::~Connection_T() "<<endl;
  
  if (_pSocket)
    {
      delete _pSocket;
    }
  
  /*  if (_pAction)
      {
      delete _pAction;
      }
  **/
  
  if (_buffer&&_bufferSize)
    {
      delete [] _buffer;
    }




}


SocketInterface_T* Connection_T::socket()
{
    return _pSocket;
}
void Connection_T::socket(SocketInterface_T* pSocket)
{
    // We don't care about whether _pSocket is already assigned with 
    // an object(usualy, the existing object should be deleted),
    // we just replace the _pSocket
    _pSocket = pSocket;
}

struct epoll_event* Connection_T::event()
{
    return &_event;
}

void Connection_T::event(uint32_t event)
{
    _event.events = event;
    _event.data.ptr = this;
}


void Connection_T::seteventType(uint32_t event)
{
  _eventType=event;

}


uint32_t Connection_T::geteventType()
{
 return  _eventType;
}


Action_T* Connection_T::action()
{
    return _pAction;
}

void Connection_T::action(Action_T* pAction)
{
    // We don't care about whether _pAction is already assigned with 
    // an object(usualy, the existing object should be deleted),
    // we just replace the _pAction
    _pAction = pAction;
}

int Connection_T::fd()
{
    if (_pSocket)
    {
        return _pSocket->sfd();
    }
    return 0;
}

SocketStream_T Connection_T::stream()
{
    return SocketStream_T(fd());
}

int Connection_T::read()
{
    if (_cursor != _buffer)
    {
        memcpy(_buffer, _cursor, _length);
        cout<<_buffer;
        _cursor = _buffer;
    }
    
    int bytes = stream().read(_buffer+_length, _bufferSize-_length);
    if (bytes <= 0)
    {
        throw StreamException_T(0,
            string( "Fail to read from socket:")+strerror(errno)+"\n"
            "\tat Connection_T::read"+
            SrcLocation_T(__FILE__, __LINE__));
    }
    
    _length += bytes;
    return bytes;
}

uint32_t Connection_T::length()
{
    return _length;
}

char* Connection_T::cursor()
{
    return _cursor;
}

void Connection_T::skip(uint32_t bytes)
{
    if (bytes > _length)
    {
        throw EpollException_T(
            string( "Offset overflowed\n"
                    "\tat Connection_T::skip\n")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    _cursor += bytes;
    _length -= bytes;
}



/******************NonBlockConnection_T*************************/




NonBlockConnection_T::NonBlockConnection_T(int inputBufferSize,int outputBufferSize)
  : _readnum(0),
    _ioStream(0)  
{
  memset(&_event, 0, sizeof(_event));
  _inputBuffer = CharBuffer_T::allocate(inputBufferSize);
  _outputBuffer = CharBuffer_T::allocate(outputBufferSize);

}



NonBlockConnection_T::NonBlockConnection_T(SocketInterface_T* pSocket, 
                                           uint32_t event1, 
                                           Action_T* pAction,
                                           int intputBufferSize, 
                                           int outputBufferSize)
  :_ioStream(0)  
{
  

  //cout<<endl<<endl<<endl<<endl<<endl<<"NonBlockConnection_T::NonBlockConnection_T()"<<endl;
  socket(pSocket);
  event(event1);
  action(pAction);
  //configureBlocking(false);
  _inputBuffer = CharBuffer_T::allocate(intputBufferSize);
  _outputBuffer = CharBuffer_T::allocate(outputBufferSize);

}


NonBlockConnection_T::~NonBlockConnection_T()
{



  // cout<<"NonBlockConnection_T::~NonBlockConnection_T()"<<endl<<endl<<endl<<endl<<endl<<endl;


    if (_inputBuffer) {

        delete _inputBuffer;
    }

    if (_outputBuffer) {

        delete _outputBuffer;
    }

    if(_ioStream)  {

      delete _ioStream;
    }
    
}





void NonBlockConnection_T::configureBlocking(bool blocking)
{
    ioStream()->configureBlocking(blocking);
}

IOStream_T* NonBlockConnection_T::ioStream()
{
    if (!_ioStream)
    {
     
      _ioStream = new IOStream_T(fd(),false);
    }
    
    return _ioStream;
}


void NonBlockConnection_T::read()
{
  
  getInputBuffer()->alignLeft();
  char buffer[64];
  int length;
  int readnum=0;	
  
  do {
    
    length = getInputBuffer()->remaining() > (int)sizeof(buffer) ? sizeof(buffer) : getInputBuffer()->remaining();
    length = ioStream()->read(buffer, length);
    //cout<<" ioStream()->read(buffer, length) "<<length <<endl;
    readnum++;
    if (length < 0) {
      //cout<<string("SocketReadingFault at NonBlockConnection_T::read")<< SrcLocation_T(__FILE__, __LINE__)<<endl;;
      throw StrException_T( string("SocketReadingFault at NonBlockConnection_T::read") +
                            SrcLocation_T(__FILE__, __LINE__));     
    }
    
    if(length>0)
      {
        getInputBuffer()->put(buffer, length); 
        //cout<<" getInputBuffer()->put(buffer, length) "<< length <<endl;
      }
  } while (length && getInputBuffer()->remaining());  
  
  if(!length&&(readnum==1))
    {
      _readnum++;
    }   
  
}

void NonBlockConnection_T::write()
{

  char buffer[64];
  int length;
  do {
    
    length = getOutputBuffer()->clength() > (int)sizeof(buffer) ? sizeof(buffer) : getOutputBuffer()->clength();
    if (length)
      {
        getOutputBuffer()->get(buffer, length);
        ioStream()->writeBytes(buffer, length);
      }
  }while (length && getOutputBuffer()->clength());

  
  getOutputBuffer()->reset();

}


CharBuffer_T* NonBlockConnection_T::getInputBuffer()
{
  return _inputBuffer;
}

CharBuffer_T* NonBlockConnection_T::getOutputBuffer()
{
  return _outputBuffer;
}
