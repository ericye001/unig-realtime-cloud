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

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <loki/Factory.h>
#include <loki/Functor.h>
#include <stdint.h>
#include "common/Utils.h"
#include "common/SocketStream.h"
#include "common/EpollUtils.h"
#include "common/IOStream.h"

using namespace Loki;

#include "MsgCode.h"

namespace MsgResult
{
  const int
    SUCCESS = 1,
    FAILURE = 0;
};

class BasePacket_T;
class BaseMessage_T;
class MsgHandler_T;

class MsgHandler_T
{
public:
    virtual void handle(BaseMessage_T* pMsg, void* arg){};
};

class BasePacket_T
{
public:
    BasePacket_T(bool bindOn = true);
    BasePacket_T(void* data, uint32_t length, bool bindOn = false); 
    BasePacket_T(Connection_T* pConn, bool bindOn = false);   
    
    virtual ~BasePacket_T();
    
    static bool containsOne(Connection_T* pConn);
    
    bool getInputPacket(NonBlockConnection_T* pConn, bool bindOn);
    bool getOutputPacket(NonBlockConnection_T* pConn, bool bindOn);
    bool putOutputBuffer(NonBlockConnection_T* pConn);
    
    void push(void* pData, uint32_t length);
    void pop(void* pData, uint32_t length);

    virtual void write(Connection_T* pConn);
    virtual void read(Connection_T* pConn);
    virtual void write(SocketStream_T* pSrteam);
    virtual void read(SocketStream_T* pStream);
    
    virtual void read(IOStream_T* ioStream);
    virtual void write(IOStream_T* ioStream);
    
    void reserve(uint32_t length);
    
    uint32_t length();
        
    char* data();
    char* data(uint32_t& length);
    
    void data(void* data, uint32_t length);
    
    void bind(bool bindOn);
    void reset();    
    
protected:
    uint32_t _length;
    char* _data;
    uint32_t _rindexIn;
    uint32_t _rindexOut;
    bool _bindOn;
};

class BaseMessage_T
{
public:
    BaseMessage_T();
    virtual ~BaseMessage_T();
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    
    virtual uint32_t size();
        
    static uint16_t type(BasePacket_T* pkt);
    //static uint8_t ext(BasePacket_T* pkt);
    
    virtual void handleMe(MsgHandler_T* handler, void* arg){};

    virtual void read(Connection_T* pConn);
    virtual void write(Connection_T* pConn);
    virtual void write(SocketStream_T* pSrteam);
    virtual void read(SocketStream_T* pStream);


    virtual void write(NonBlockConnection_T* pConn);


    virtual void read(IOStream_T* ioStream);
    virtual void write(IOStream_T* ioStream);
    
    uint16_t _type;        
    // uint8_t _ext;
    // uint8_t _syn;
};


#endif

