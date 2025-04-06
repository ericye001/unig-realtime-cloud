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

#ifndef MESSAGE_PM_ILM_HPP
#define MESSAGE_PM_ILM_HPP

#include "Message.h"
#include "SrvInfo.h"
#include "MSTVersion.h"



class PMConnectILMRequestMsg_T : public BaseMessage_T
{
public:
    PMConnectILMRequestMsg_T();
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    MSTVersion  m_version;
};

class PMConnectILMResponseMsg_T : public BaseMessage_T
{
public:
    PMConnectILMResponseMsg_T();
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    MSTVersion  m_version;
 
};

class AddServerRequestMsg_T : public BaseMessage_T
{
public:
    AddServerRequestMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    SrvInfo_T _newServer;
    MSTVersion  m_version;

};

class AddServerResponseMsg_T : public BaseMessage_T
{
public:
    AddServerResponseMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    SrvInfo_T _addedServer;    // login set id to 0 to indicate an error
    MSTVersion  m_version;
};


class ServerLeaveRequestMsg_T : public BaseMessage_T
{
public:
    ServerLeaveRequestMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint16_t m_nServerID;

};

class ServerLeaveResponseMsg_T : public BaseMessage_T
{
public:
    ServerLeaveResponseMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    uint8_t m_nResult;
    uint16_t m_nServerID;
};











#endif

