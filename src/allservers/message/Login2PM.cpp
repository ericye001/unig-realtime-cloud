/*
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 
*/

#include "message/Login2PM.h"




//
// AddClientToPMRequest
//
AddClientToPMRequestMsg_T::AddClientToPMRequestMsg_T()
{
  _type = MsgType::ADD_CLIENT_TO_PM_REQUEST ;
  

}

void
AddClientToPMRequestMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_nUserID, sizeof(m_nUserID));
    //cout<<" nUserID "<<m_nUserID<<endl;
    pkt->pop(&m_nSession, sizeof(m_nSession));
    //cout<<" nSession "<<m_nSession<<endl;
    pkt->pop(&m_Clien_Info, sizeof(m_Clien_Info));
}


void
AddClientToPMRequestMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_nUserID, sizeof(m_nUserID));
    pkt->push(&m_nSession, sizeof(m_nSession));
    pkt->push(&m_Clien_Info, sizeof(m_Clien_Info));
}


uint32_t AddClientToPMRequestMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(m_nUserID)+sizeof(m_nSession)+sizeof(m_Clien_Info);
}



//
// AddClientToPMResponse
//



AddClientToPMResponseMsg_T::AddClientToPMResponseMsg_T()
{

  _type = MsgType::ADD_CLIENT_TO_PM_RESPONSE ;
}




void
AddClientToPMResponseMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_nUserID, sizeof(m_nUserID));
    pkt->pop(&m_nSession, sizeof(m_nSession));
    pkt->pop(&m_ProxySrv_Info, sizeof(m_ProxySrv_Info));
    pkt->pop(&m_nRoleServerID, sizeof(m_nRoleServerID));
}



void
AddClientToPMResponseMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_nUserID, sizeof(m_nUserID));
    pkt->push(&m_nSession, sizeof(m_nSession));
    pkt->push(&m_ProxySrv_Info, sizeof(m_ProxySrv_Info));
    pkt->push(&m_nRoleServerID, sizeof(m_nRoleServerID));
}

uint32_t AddClientToPMResponseMsg_T::size()
{
  return BaseMessage_T::size()+sizeof(m_nUserID)+sizeof(m_nSession)+sizeof(m_ProxySrv_Info)+sizeof(m_nRoleServerID);
}


