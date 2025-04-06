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

#ifndef CONSOLE_TO_ILM_HPP
#define CONSOLE_TO_ILM_HPP

#include "Message.h"
#include "Map2SrvInfo.h"
#include "MapInfo.h"
#include "SrvInfo.h"
#include <map>
#include <vector>
typedef uint16_t server_id_t;
typedef uint16_t map_id_t;
typedef uint16_t zone_id_t;

class StopGameServerRequest_T : public BaseMessage_T
{
public:
    StopGameServerRequest_T();
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint16_t _srvId;
};


class StopGameServerResponse_T : public BaseMessage_T
{
public:
    StopGameServerResponse_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint8_t  _result;
};


class MapunitRequest_T :  public BaseMessage_T

{

public:
    MapunitRequest_T();

};


class MapunitResponse_T : public BaseMessage_T
{
public:
  MapunitResponse_T();
  virtual void pack(BasePacket_T* pkt);
  virtual void unpack(BasePacket_T* pkt);
  virtual int type1();
  virtual uint32_t size();
  //vector<MapInfo_T> _maps;
  char _etc[100];
};









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

#endif
