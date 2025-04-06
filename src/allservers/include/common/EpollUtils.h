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

#ifndef EPOLL_UTILS_HPP
#define EPOLL_UTILS_HPP
#include <sys/epoll.h>
#include <map>
#include <iostream>
#include <exception>
#include "Socket.h"
#include "SocketStream.h"
#include "Utils.h"
#include "Buffer.h"
#include "IOStream.h"
using namespace std;

template <class T> class EpollSingleton_T;
class Epoll_T;
class Connection_T;
class Action_T;
class EpollException_T;




typedef struct Package
{
    int _id;
    char _data[1024];
}Package_T;

template <class T> class EpollSingleton_T
{
public:
    static void init(int maxWaitFds)
    {
        instance(maxWaitFds);
    }
    static void deinit()
    {
        if (_pInstance)
        {
            delete _pInstance;
        }
    }
    
    static Epoll_T& instance(int maxWaitFds = 2000)
    {
        if (_pInstance)
        {
            return *_pInstance;
        }
        _pInstance = new Epoll_T(maxWaitFds);
        if (!_pInstance)
        {
            throw StrException_T(
                string("Fail to create Epoll_T: lack of memory\n"
                        "\tat EpollSingleton_T<T>::initialize")+
                SrcLocation_T(__FILE__, __LINE__));
        }        
        return *_pInstance;
    }
private:
    static Epoll_T* _pInstance;
};
    
class Epoll_T : public ObjectLevelLockable_T<Epoll_T>
{
public:
    Epoll_T(uint32_t maxWaitFds = 5);
    virtual ~Epoll_T();

    void add(Connection_T* pConn);
    void setup(Connection_T* pConn);
    void remove(Connection_T* pConn);
    void wait(int timeout);
    void clear();
    Connection_T* find(uint32_t id);
    uint32_t size();

private:
    Epoll_T(const Epoll_T& rhs);
    Epoll_T& operator=(const Epoll_T& rhs);
    
    
  
    
    int _epfd;
    uint32_t _maxWaitFds;
    map<uint32_t, Connection_T*> _idMapConn;
    struct epoll_event* _pEvents;
};


class EpollException_T : public exception
{
public:
    EpollException_T(const char* what) throw();
    EpollException_T(const string& what) throw();
    ~EpollException_T()throw(){};

    virtual const char* what() const throw();

private:
    std::string _what;
};

class Connection_T
{
public:
    Connection_T(uint32_t bufferSize = 0);
    
    Connection_T(
        SocketInterface_T* pSocket, 
        uint32_t event, 
        Action_T* pAction,
        uint32_t bufferSize = 0);
    
    
    virtual ~Connection_T();

    SocketInterface_T* socket();
    void socket(SocketInterface_T* pSocket);

    struct epoll_event* event();
    void event(uint32_t event);
    void seteventType(uint32_t event);
    uint32_t geteventType();
    Action_T* action();
    void action(Action_T* pAction);
    
    int fd();
    SocketStream_T stream();
    
    
    int read();
    uint32_t length();
    char* cursor();
    void skip(uint32_t bytes);
    
protected:
    SocketInterface_T* _pSocket;
    struct epoll_event _event;
    Action_T* _pAction;
    uint32_t _bufferSize;
    uint32_t _eventType;
    char* _buffer;
    uint32_t _length;
    char* _cursor;
};





class NonBlockConnection_T : public  Connection_T
{
public:
  NonBlockConnection_T(int intputBufferSize=4000, 
                 int outputBufferSize=4000); 
  NonBlockConnection_T(
        SocketInterface_T* pSocket, 
        uint32_t event, 
        Action_T* pAction,
        int intputBufferSize=4000, 
        int outputBufferSize=4000); 
  
    virtual ~NonBlockConnection_T();


    void configureBlocking(bool blocking);
    IOStream_T* ioStream();



    CharBuffer_T* getInputBuffer();
    CharBuffer_T* getOutputBuffer();

    virtual void read();
    virtual void write();
    int _readnum;   
protected:
    IOStream_T* _ioStream;
    CharBuffer_T* _inputBuffer;
    CharBuffer_T* _outputBuffer;
    // int _readnum;
   
};




class Action_T
{
public:
    virtual void execute(Connection_T* pConn) = 0;
   
};

template <class T> Epoll_T* EpollSingleton_T<T>::_pInstance = 0;
typedef EpollSingleton_T<Type2Type_T<Epoll_T> > EpollUtil_T;



#endif
