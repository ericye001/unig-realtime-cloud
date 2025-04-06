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

#include "common/EpollWaiter.h"

EpollReactor_T::EpollReactor_T(int maxWaitFds) 
    : ObjectLevelLockable_T<EpollReactor_T>(),
    _maxWaitFds(maxWaitFds)
{
    if ((_epfd = epoll_create(maxWaitFds)) < 0)
    {
        throw StrException_T(
                string("epoll_create error(")+strerror(errno)+"\n"
                "\tat EpollReactor_T::EpollReactor_T"+
                SrcLocation_T(__FILE__, __LINE__));
    }
    _pEvents = new struct epoll_event [_maxWaitFds];
    if (!_pEvents)
    {
        throw StrException_T(
                string( "Fail to create struct epoll_event\n"
                        "\tat EpollReactor_T::EpollReactor_T")+
                SrcLocation_T(__FILE__, __LINE__));
    }
}
EpollReactor_T::~EpollReactor_T()
{
    clear();
    close(_epfd);
    delete [] _pEvents;    
}
void EpollReactor_T::add(SocketChannel_T* channel)
{
    Lock_T lock(*this);
    if (_idMapConn.size() >= (uint)_maxWaitFds)
    {
        throw StrException_T(
                string( "Too many SocketChannel_T added into epoll\n"
                "\tat EpollReactor_T::add")+
                SrcLocation_T(__FILE__, __LINE__));
    }
    
    if (epoll_ctl(_epfd, EPOLL_CTL_ADD,
            channel->socket()->sfd(), channel->event()) < 0)
    {
        throw StrException_T(
                string( "epoll_ctl error(")+strerror(errno)+")\n"
                "\tat EpollReactor_T::add"+
                SrcLocation_T(__FILE__, __LINE__));
    }

    _idMapConn.insert(make_pair(channel->socket()->sfd(), channel));
}
void EpollReactor_T::setup(SocketChannel_T* channel)
{
    Lock_T lock(*this);
    if (epoll_ctl(_epfd, EPOLL_CTL_MOD,
            channel->socket()->sfd(), channel->event()) < 0)
    {
        throw StrException_T(
                string( "epoll_ctl error(")+strerror(errno)+")\n"
                "\tat EpollReactor_T::setup"+
                SrcLocation_T(__FILE__, __LINE__));
    }
}

void EpollReactor_T::remove(SocketChannel_T* channel)
{
    Lock_T lock(*this);
    ChannelMap_T::iterator gotIt;
    gotIt = _idMapConn.find(channel->socket()->sfd());
    if (gotIt == _idMapConn.end())
    {
        return;
    }

    if (epoll_ctl(_epfd, EPOLL_CTL_DEL,
            channel->socket()->sfd(), channel->event()) < 0)
    {
        throw StrException_T(
                string( "epoll_ctl error(")+strerror(errno)+")\n"
                        "\tat EpollReactor_T::remove"+
                SrcLocation_T(__FILE__, __LINE__));
    }
    _idMapConn.erase(gotIt);
    // Let client delete channel itself
    //delete channel;
}

void EpollReactor_T::timedWait(int timeout)
{
    int i, nfds = 0;
    struct epoll_event* cevents;
    {   // Use bracket to assume lock the right scope --- only epoll_wait
        // no used
        //Lock_T lock(*this);
        nfds = epoll_wait(_epfd, _pEvents, _maxWaitFds, timeout);
        if (nfds < 0)
        {
            throw StrException_T(
                string("epoll_wait failed")+
                strerror(errno)+
                SrcLocation_T(__FILE__, __LINE__));
        }
    }
    /**
        Ok, events occured and held in events buffer
    */
    for (i = 0, cevents = _pEvents; 
            i < nfds; 
            i++, cevents++) 
    {
        SocketChannel_T* channel = (SocketChannel_T*)cevents->data.ptr;
        channel->listener()->handle(channel, cevents->events);
	//cout << "events : " << cevents->events << "\n"; 
        //	cout << "fd : " << channel->socket()->sfd() << "\n"; 
    }        
}

void EpollReactor_T::clear()
{
    Lock_T lock(*this);
    ChannelMap_T::iterator curIt;
    for (curIt = _idMapConn.begin(); curIt != _idMapConn.end(); )
    {
        SocketChannel_T* channel = curIt->second;        
        if (epoll_ctl(_epfd, EPOLL_CTL_DEL,
            channel->socket()->sfd(), channel->event()) < 0)
        {
            throw StrException_T(
                    string( "epoll_ctl error(")+strerror(errno)+")\n"
                            "\tat EpollReactor_T::clear"+
                    SrcLocation_T(__FILE__, __LINE__));
        }
        _idMapConn.erase(curIt++);
    }
}

SocketChannel_T* EpollReactor_T::find(int id)
{
    Lock_T lock(*this);
    ChannelMap_T::iterator gotIt = _idMapConn.find(id);

    if (gotIt != _idMapConn.end())
    {
        return gotIt->second;
    }
    return NULL;
}

int EpollReactor_T::size()
{
    Lock_T lock(*this);
    return _idMapConn.size();
}
    

SocketChannel_T::SocketChannel_T()
    :
    _socket(0),
    _listener(0),
    _ioStream(0)
{
    
}

SocketChannel_T::SocketChannel_T(
    SocketInterface_T* pSocket, 
    int event, 
    EpollEventListener_T* pListener
)
    :   
    _socket(pSocket), 
    _listener(pListener),
    _ioStream(0),
    _writable(true)
{
    _event.events = event;
    _event.data.ptr = this;
}

SocketChannel_T::~SocketChannel_T()
{
    if (_socket)
    {
        delete _socket;
    }
    if (_listener)
    {
       // delete _listener;
    }
}


SocketInterface_T* SocketChannel_T::socket()
{
    return _socket;
}
void SocketChannel_T::socket(SocketInterface_T* pSocket)
{
    // We don't care about whether _socket is already assigned with 
    // an object(usualy, the existing object should be deleted),
    // we just replace the _socket
    _socket = pSocket;
}

struct epoll_event* SocketChannel_T::event()
{
    return &_event;
}

void SocketChannel_T::event(int event)
{
    _event.events = event;
    _event.data.ptr = this;
}


EpollEventListener_T* SocketChannel_T::listener()
{
    return _listener;
}

void SocketChannel_T::listener(EpollEventListener_T* pListener)
{
    // We don't care about whether _listener is already assigned with 
    // an object(usualy, the existing object should be deleted),
    // we just replace the _listener
    _listener = pListener;
}
void SocketChannel_T::configureBlocking(bool blocking)
{
    ioStream()->configureBlocking(blocking);
}

IOStream_T* SocketChannel_T::ioStream()
{
    if (!_ioStream)
    {
        _ioStream = new IOStream_T(_socket->sfd());
    }
    
    return _ioStream;
}

bool SocketChannel_T::getWritable()
{
    return _writable;
}

void SocketChannel_T::setWritable(bool writable)
{
    _writable = writable;
}

void EpollEventListener_T::handle(SocketChannel_T* channel, int events)
{
    if (events & EPOLLOUT) {
        channel->setWritable(true);
        onWrite(channel);
    } else {
        channel->setWritable(false);
    }
    if ((events & EPOLLERR) || (events & EPOLLHUP)) {
        onError(channel);
    } else if (events & EPOLLIN) {
        onRead(channel);
    }
    
}
