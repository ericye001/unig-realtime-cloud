/*
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 

*/
#ifndef MESSAGE_LOGIN_PM_HPP
#define MESSAGE_LOGIN_PM_HPP
#include "Message.h"
#include "ClientInfo.h"
#include "ProxySrvInfo.h"
#include "message/Login2PM.h"

class AddClientToPMRequestMsg_T : public BaseMessage_T
{
public:
    AddClientToPMRequestMsg_T();
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
  
    uint32_t m_nUserID;
    uint32_t m_nSession;
    ClientInfo m_Clien_Info;
};

class AddClientToPMResponseMsg_T : public BaseMessage_T
{
public:
    AddClientToPMResponseMsg_T();
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint32_t m_nUserID;
    uint32_t m_nSession;
    ProxySrvInfo  m_ProxySrv_Info;
    uint32_t m_nRoleServerID;
 
};






#endif

