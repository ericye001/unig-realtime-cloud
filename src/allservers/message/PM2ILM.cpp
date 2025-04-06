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

#include "message/PM2ILM.h"

//
// LoginConnectIGO
//

PMConnectILMRequestMsg_T::PMConnectILMRequestMsg_T()
{
    _type = MsgType::PM_CONNECT_ILM_REQUEST;
}


void
PMConnectILMRequestMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_version, sizeof(m_version));
}

void
PMConnectILMRequestMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_version, sizeof(m_version));
}

uint32_t PMConnectILMRequestMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(m_version);
}



//
// LoginConnectIGOResponse
//
PMConnectILMResponseMsg_T::PMConnectILMResponseMsg_T()
{
    _type = MsgType::PM_CONNECT_ILM_RESPONSE;
}


void
PMConnectILMResponseMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_version, sizeof(m_version));
}

void
PMConnectILMResponseMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_version, sizeof(m_version));
}

uint32_t PMConnectILMResponseMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(m_version);
}

//
// AddServerRequest
//
AddServerRequestMsg_T::AddServerRequestMsg_T()
{
    _type = MsgType::ADD_SERVER_REQUEST;
}

void
AddServerRequestMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_newServer, sizeof(_newServer));
    pkt->pop(&m_version, sizeof(m_version));
}

void
AddServerRequestMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_newServer, sizeof(_newServer));
    pkt->push(&m_version, sizeof(m_version));
}

uint32_t AddServerRequestMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_newServer)+sizeof(m_version);
}

//
// AddServerResponse
//
AddServerResponseMsg_T::AddServerResponseMsg_T()
{
    _type = MsgType::ADD_SERVER_RESPONSE;
}

void
AddServerResponseMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_addedServer, sizeof(_addedServer));
    pkt->pop(&m_version, sizeof(m_version));
}

void
AddServerResponseMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_addedServer, sizeof(_addedServer));
    pkt->push(&m_version, sizeof(m_version));
}

uint32_t AddServerResponseMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_addedServer)+sizeof(m_version);
}






ServerLeaveRequestMsg_T::ServerLeaveRequestMsg_T()
{
    _type = MsgType::REMOVE_SERVER_REQUEST;
}


void
ServerLeaveRequestMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_nServerID, sizeof(m_nServerID));
}

void
ServerLeaveRequestMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_nServerID, sizeof(m_nServerID));
}

uint32_t ServerLeaveRequestMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(m_nServerID);
}




ServerLeaveResponseMsg_T::ServerLeaveResponseMsg_T()
{
    _type = MsgType::REMOVE_SERVER_RESPONSE;
}


void
ServerLeaveResponseMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_nResult, sizeof(m_nResult));
    pkt->pop(&m_nServerID, sizeof(m_nServerID));
}

void
ServerLeaveResponseMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_nResult, sizeof(m_nResult));
    pkt->push(&m_nServerID, sizeof(m_nServerID));
}

uint32_t ServerLeaveResponseMsg_T::size()
{
    return BaseMessage_T::size()+ sizeof(m_nResult) + sizeof(m_nServerID);
}
