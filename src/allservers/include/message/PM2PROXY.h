/*
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 

*/
#ifndef MESSAGE_PM_PROXY_HPP
#define MESSAGE_PM_PROXY_HPP
#include "Message.h"
#include "ClientInfo.h"
#include "ProxySrvInfo.h"


class SendSession2ProxyReqestMsg_T : public BaseMessage_T
{
public:
    SendSession2ProxyReqestMsg_T();
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
  
    uint32_t m_nUserID;
    uint32_t m_nSession;
    ClientInfo m_Clien_Info;
    uint16_t m_nLoginID;
    uint16_t m_nRoleServerID;
    uint16_t m_nProxyID;
    struct timeval start;
};

class SendSession2ProxyResponseMsg_T : public BaseMessage_T
{
public:
    SendSession2ProxyResponseMsg_T();
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint8_t m_nResult;
    uint32_t m_nUserID;
    uint32_t m_nSession;
    uint16_t m_nLoginID;
    uint16_t m_nRoleServerID;
    uint16_t m_nProxyID;
    struct timeval start;
};


#endif

