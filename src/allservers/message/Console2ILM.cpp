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

#include "message/Console2ILM.h"

//
// ServerConnectIGORequest
//
StopGameServerRequest_T::StopGameServerRequest_T()
{
    _type = MsgType::SERVER_LEAVE_REQUEST;
}

void
StopGameServerRequest_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_srvId, sizeof(_srvId));
}

void
StopGameServerRequest_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_srvId, sizeof(_srvId));
}

uint32_t StopGameServerRequest_T::size()
{
    return BaseMessage_T::size()+sizeof(_srvId);
}


//
// ServerConnectIGOResponse
//
StopGameServerResponse_T::StopGameServerResponse_T()
{
    _type = MsgType::SERVER_LEAVE_RESPONSE;
}

void
StopGameServerResponse_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));
}

void
StopGameServerResponse_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
}

uint32_t StopGameServerResponse_T::size()
{
    return BaseMessage_T::size()+sizeof(_result);
}
  



//
// MapunitRequest
//
MapunitRequest_T::MapunitRequest_T()
{
  _type = MsgType:: MAP_UNIT_RESPONSE;
}









MapunitResponse_T::MapunitResponse_T()
{
  _type = MsgType:: MAP_UNIT_RESPONSE;
}


void
MapunitResponse_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    
    pkt->push(&_etc, sizeof(_etc));
}

void
MapunitResponse_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_etc, sizeof(_etc));
}

int
MapunitResponse_T::type1()
{
    return _type;

}

uint32_t MapunitResponse_T::size()
{
    return BaseMessage_T::size()+sizeof(_etc);
}
  



/*

class GetPlanMsgRequest_T :  public BaseMessage_T

{

public:
    GetPlanMsgRequest_T();
    virtual void pack(BasePacket_T* pkt);
    virtual void unpack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint16_t _fromID;
    uint16_t _toID;
    uint16_t _zoneID;


};


class GetPlanMsgResponse_T : public BaseMessage_T
{
public:
  GetPlanMsgResponse_T();

};


**/


GetPlanMsgRequest_T::GetPlanMsgRequest_T()
{
    _type = MsgType::SERVER_STATUS_QUERY;
}

void
GetPlanMsgRequest_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_fromID, sizeof(_fromID));
    pkt->pop(&_toID, sizeof(_toID));
    pkt->pop(&_zoneID, sizeof(_zoneID));
}

void
GetPlanMsgRequest_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_fromID, sizeof(_fromID));
    pkt->push(&_toID, sizeof(_toID));
    pkt->push(&_zoneID, sizeof(_zoneID));
   
}

uint32_t GetPlanMsgRequest_T::size()
{
  return BaseMessage_T::size() + sizeof(_fromID) +  sizeof(_toID) + sizeof(_zoneID);
}


GetPlanMsgResponse_T::GetPlanMsgResponse_T()
{
    _type = MsgType::SERVER_STATUS_REPLY;
}
