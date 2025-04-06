/*
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 
*/

#include "message/PM2PROXY.h"


//
// SendSession2ProxyReqest
//
SendSession2ProxyReqestMsg_T::SendSession2ProxyReqestMsg_T()
{
  _type = MsgType:: SEND_SESSION_TO_PROXY_REQUEST ;
  

}

void
SendSession2ProxyReqestMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_nUserID, sizeof(m_nUserID));
    pkt->pop(&m_nSession, sizeof(m_nSession));
    pkt->pop(&m_Clien_Info, sizeof(m_Clien_Info));
    pkt->pop(&m_nLoginID, sizeof(m_nLoginID));
    pkt->pop(&m_nRoleServerID, sizeof(m_nRoleServerID));
    pkt->pop(&m_nProxyID, sizeof(m_nProxyID));
    pkt->pop(&start, sizeof(start));
 
}
;

void
SendSession2ProxyReqestMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_nUserID, sizeof(m_nUserID));
    pkt->push(&m_nSession, sizeof(m_nSession));
    pkt->push(&m_Clien_Info, sizeof(m_Clien_Info));
    pkt->push(&m_nLoginID, sizeof(m_nLoginID));
    pkt->push(&m_nRoleServerID, sizeof(m_nRoleServerID));
    pkt->push(&m_nProxyID, sizeof(m_nProxyID));
    pkt->push(&start, sizeof(start));
}


uint32_t SendSession2ProxyReqestMsg_T::size()
{
  return BaseMessage_T::size()+sizeof(m_nUserID)+sizeof(m_nSession)+sizeof(m_Clien_Info)+sizeof(m_nLoginID)+sizeof(m_nRoleServerID)+sizeof(m_nProxyID)+sizeof(start);
}



//
// SendSession2ProxyResponse
//



SendSession2ProxyResponseMsg_T::SendSession2ProxyResponseMsg_T()
{

  _type = MsgType:: SEND_SESSION_TO_PROXY_RESPONSE ;
}




void
SendSession2ProxyResponseMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_nResult, sizeof(m_nResult));
    pkt->pop(&m_nUserID, sizeof(m_nUserID));
    pkt->pop(&m_nSession, sizeof(m_nSession));
    pkt->pop(&m_nLoginID, sizeof(m_nLoginID));
    pkt->pop(&m_nRoleServerID, sizeof(m_nRoleServerID));
    pkt->pop(&m_nProxyID, sizeof(m_nProxyID));
    pkt->pop(&start, sizeof(start));
}



void
SendSession2ProxyResponseMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_nResult, sizeof(m_nResult));
    pkt->push(&m_nUserID, sizeof(m_nUserID));
    pkt->push(&m_nSession, sizeof(m_nSession));
    pkt->push(&m_nLoginID, sizeof(m_nLoginID));
    pkt->push(&m_nRoleServerID, sizeof(m_nRoleServerID));
    pkt->push(&m_nProxyID, sizeof(m_nProxyID));
    pkt->push(&start, sizeof(start));
}

uint32_t SendSession2ProxyResponseMsg_T::size()
{
  return BaseMessage_T::size()+sizeof(m_nResult)+sizeof(m_nUserID)
                              +sizeof(m_nSession)+sizeof(m_nLoginID)
                              +sizeof(m_nRoleServerID)+sizeof(m_nProxyID)+sizeof(start);
}


