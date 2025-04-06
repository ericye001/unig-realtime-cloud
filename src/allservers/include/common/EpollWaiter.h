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

#ifndef EPOLL_WAITER_H
#define EPOLL_WAITER_H
#include <sys/epoll.h>
#include <map>
#include <iostream>
#include <exception>
#include "Socket.h"
#include "IOStream.h"
#include "Utils.h"

using namespace std;

class EpollReactor_T;
class SocketChannel_T;
class EpollEventListener_T;


class EpollReactor_T : public ObjectLevelLockable_T<EpollReactor_T>
{
    typedef map<int, SocketChannel_T*> ChannelMap_T;
public:
    EpollReactor_T(int maxWaitFds = 5);
    virtual ~EpollReactor_T();

    void add(SocketChannel_T* channel);
    void setup(SocketChannel_T* channel);
    void remove(SocketChannel_T* channel);
    void timedWait(int timeout);
    void clear();
    SocketChannel_T* find(int id);
    int size();

private:
    EpollReactor_T(const EpollReactor_T& rhs);
    EpollReactor_T& operator=(const EpollReactor_T& rhs);  
    
  
private:
    int _epfd;
    int _maxWaitFds;
    ChannelMap_T _idMapConn;
    struct epoll_event* _pEvents;
};


class SocketChannel_T
{
public:
    SocketChannel_T();
    SocketChannel_T(
        SocketInterface_T* pSocket, 
        int event, 
        EpollEventListener_T* pListener);
    
    
    virtual ~SocketChannel_T();

    SocketInterface_T* socket();
    void socket(SocketInterface_T* pSocket);

    struct epoll_event* event();
    void event(int event);

    EpollEventListener_T* listener();
    void listener(EpollEventListener_T* pListener);
    
    void configureBlocking(bool blocking);
    
    IOStream_T* ioStream();
    
    bool getWritable();
    void setWritable(bool writable);
    
protected:
    SocketInterface_T* _socket;
    struct epoll_event _event;
    EpollEventListener_T* _listener;    
    
private:
    IOStream_T* _ioStream;
    bool _writable;
};

class EpollEventListener_T
{
public:
    virtual void handle(SocketChannel_T* channel, int events);
    virtual void onRead(SocketChannel_T* channel) {};
    virtual void onWrite(SocketChannel_T* channel) {};
    virtual void onError(SocketChannel_T* channel) {};    
};


#endif
