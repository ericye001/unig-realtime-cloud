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

#ifndef MESSAGE_SERVER_IGO_HPP
#define MESSAGE_SERVER_IGO_HPP

#include "Message.h"
#include "Map2SrvInfo.h"
#include "MapInfo.h"
#include "SrvInfo.h"
#include "Config.h"
#include "ServerLoadToPM.h"


class ServerConnectILMRequestMsg_T : public BaseMessage_T
{
public:
    ServerConnectILMRequestMsg_T();
    virtual ~ServerConnectILMRequestMsg_T(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint16_t m_iVersion;
    SrvInfo_T _srvInfo;
};

class ServerConnectILMResponseMsg_T : public BaseMessage_T
{
public:
    ServerConnectILMResponseMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint8_t  _result;
    SrvInfo_T _srvInfo;

    uint16_t _infoBusMaxMsgNum;
    uint16_t _infoBusBatchNum;


    uint16_t m_nMaxServerNum;
    uint16_t m_nMaxMsgLength;
    // uint32_t _mapNum;
    // vector<MapUnitInfo> _mapsInfo;
};

class UpdateMSTRequestMsg_T : public BaseMessage_T
{
public:
    UpdateMSTRequestMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();



    uint32_t _MSTItemNum;
    std::vector<Map2SrvInfo_T> _MST;

    uint16_t _allSrvNum;
    std::vector<SrvInfo_T> _allSrvs;

};




class UpdateMSTResponseMsg_T : public BaseMessage_T
{
public:
    UpdateMSTResponseMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint8_t _result;
};

class ServerStatusQueryMsg_T : public BaseMessage_T
{
public:
    ServerStatusQueryMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint16_t _gsNum;
    std::vector<uint16_t> _gs;
};

class ServerStatusReplyMsg_T : public BaseMessage_T
{
public:
    ServerStatusReplyMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint16_t _playerNum;
    uint16_t _cpuLoad;
    uint16_t _mapNum;
    std::vector<MapInfo_T> _maps;
};





/////////////////////////////////////////////////////////////////////

/*************info of server'load to pm server*********************************/



class ServerLoadToPM : public BaseMessage_T
{
public:
    ServerLoadToPM();
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();


    uint16_t _mapNum;
    std::vector<SrvToPM> _srvload; 
};

class ServerLoadToPMReplyMsg : public BaseMessage_T
{
public:
  ServerLoadToPMReplyMsg();

  virtual void unpack(BasePacket_T* pkt);
  virtual void pack(BasePacket_T* pkt);
  virtual uint32_t size();

  uint8_t _result;
};



/**********************************************/










class StartPartitionRequestMsg_T : public BaseMessage_T
{
public:
    StartPartitionRequestMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    //uint16_t _serverTo;    
    uint16_t  _serverFrom;
    uint16_t _serverTo;// destination server to move the map(s)
    uint32_t _mapNum;
    std::vector<uint32_t> _maps;
};


class StartPartitionResponseMsg_T : public BaseMessage_T
{
public:
    StartPartitionResponseMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    uint8_t _result;
};





class StartZoneMoveRequestMsg_T : public BaseMessage_T
{
public:
    StartZoneMoveRequestMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    //uint16_t _serverTo;    
    SrvInfo_T  _serverFrom;
    SrvInfo_T _serverTo;// destination server to move the map(s)
    uint16_t _mapNum;
    std::vector<uint16_t> _maps;
};


class StartZoneMoveResponseMsg_T : public BaseMessage_T
{
public:
    StartZoneMoveResponseMsg_T();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint16_t _serverTo;
    uint16_t _mapMoved;

    uint8_t _result;
};





class ServerLeaveRequest_T : public BaseMessage_T
{
public:
    ServerLeaveRequest_T();    
};

class ServerLeaveResponse_T : public BaseMessage_T
{
public:
    ServerLeaveResponse_T();
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint8_t _result;
};







class ServerLockMapRequest : public BaseMessage_T
{
public:
    ServerLockMapRequest();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint16_t m_nLockMapNum;
    vector<uint32_t> m_vecMapList;
};


class ServerLockMapResponse : public BaseMessage_T
{
public:
    ServerLockMapResponse();
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint8_t m_nResult;
};






class ServerUnLockMapRequest : public BaseMessage_T
{
public:
    ServerUnLockMapRequest();

    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint16_t m_nUnLockMapNum;
    vector<uint16_t> m_vecMapList;
};


class ServerUnLockMapResponse : public BaseMessage_T
{
public:
    ServerUnLockMapResponse();
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint8_t m_nResult;
};








#endif
