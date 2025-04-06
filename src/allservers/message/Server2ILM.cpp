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

#include "message/Server2ILM.h"

//
// ServerConnectIGORequest
//
ServerConnectILMRequestMsg_T::ServerConnectILMRequestMsg_T()
{
    _type = MsgType::SERVER_CONNECT_ILM_REQUEST;
}

void
ServerConnectILMRequestMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_iVersion, sizeof(m_iVersion));
    pkt->pop(&_srvInfo, sizeof(_srvInfo));
 
}

void
ServerConnectILMRequestMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->pop(&m_iVersion, sizeof(m_iVersion));
    pkt->push(&_srvInfo, sizeof(_srvInfo));
}

uint32_t ServerConnectILMRequestMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_srvInfo)+sizeof(m_iVersion);
}


//
// ServerConnectIGOResponse
//
ServerConnectILMResponseMsg_T::ServerConnectILMResponseMsg_T()
{
    _type = MsgType::SERVER_CONNECT_ILM_RESPONSE;
}

void
ServerConnectILMResponseMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));
    pkt->pop(&_srvInfo, sizeof(_srvInfo));

    pkt->pop(&_infoBusMaxMsgNum, sizeof(_infoBusMaxMsgNum));
    pkt->pop(&_infoBusBatchNum, sizeof(_infoBusBatchNum));
    pkt->pop(&m_nMaxServerNum, sizeof(m_nMaxServerNum));
    pkt->pop(&m_nMaxMsgLength, sizeof(m_nMaxMsgLength));


}

void
ServerConnectILMResponseMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
    pkt->push(&_srvInfo, sizeof(_srvInfo));

    pkt->push(&_infoBusMaxMsgNum, sizeof(_infoBusMaxMsgNum));
    pkt->push(&_infoBusBatchNum, sizeof(_infoBusBatchNum));
    pkt->push(&m_nMaxServerNum, sizeof(m_nMaxServerNum));
    pkt->push(&m_nMaxMsgLength, sizeof(m_nMaxMsgLength));
    
}

uint32_t ServerConnectILMResponseMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_result)
           + sizeof(_srvInfo)+ sizeof(_infoBusMaxMsgNum)+ sizeof(_infoBusBatchNum)
           + sizeof(m_nMaxServerNum) + sizeof(m_nMaxMsgLength);
}
    

//
// UpdateMSTRequest
//
UpdateMSTRequestMsg_T::UpdateMSTRequestMsg_T()
{
    _type = MsgType::UPDATE_MST_REQUEST;
}


void
UpdateMSTRequestMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_MSTItemNum, sizeof(_MSTItemNum));
   
    if(_MSTItemNum > 0)
    {
        _MST.reserve(_MSTItemNum);
        Map2SrvInfo_T mstItem;
        for(uint16_t i = 0; i < _MSTItemNum; ++i)
        {
            pkt->pop(&mstItem, sizeof(Map2SrvInfo_T));
            _MST.push_back(mstItem);
        }
    }
    pkt->pop(&_allSrvNum, sizeof(_allSrvNum));
    if (_allSrvNum > 0)
    {
        _allSrvs.reserve(_allSrvNum);
        SrvInfo_T srvInfo;
        for(uint16_t i = 0; i < _allSrvNum; ++i)
        {
            pkt->pop(&srvInfo, sizeof(srvInfo));
            _allSrvs.push_back(srvInfo);
        }
    }
}

void
UpdateMSTRequestMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    _MSTItemNum = _MST.size();
    pkt->push(&_MSTItemNum, sizeof(_MSTItemNum));
    for (uint16_t i = 0; i < _MSTItemNum; ++i)
    {
     
      pkt->push(&_MST[i], sizeof(Map2SrvInfo_T));
     
    }

    _allSrvNum = _allSrvs.size();
    //cout<<" UpdateMSTRequestMsg_T::pack() Srv Num "<< _allSrvNum  <<endl;
    pkt->push(&_allSrvNum, sizeof(_allSrvNum));
    for (uint16_t i = 0; i < _allSrvNum; ++i)
    {
        pkt->push(&_allSrvs[i], sizeof(SrvInfo_T));
    }


}

uint32_t UpdateMSTRequestMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_allSrvNum)+ sizeof(SrvInfo_T)*_allSrvs.size()
                                +sizeof(_MSTItemNum)+sizeof(Map2SrvInfo_T)*_MST.size();
}

//
// UpdateMSTResponse
//
UpdateMSTResponseMsg_T::UpdateMSTResponseMsg_T()
{
    _type = MsgType::UPDATE_MST_RESPONSE;
}

void
UpdateMSTResponseMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));
}

void
UpdateMSTResponseMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
}

uint32_t UpdateMSTResponseMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_result);
}


//
// ServerStatusQuery
//
ServerStatusQueryMsg_T::ServerStatusQueryMsg_T()
{
    _type = MsgType::SERVER_STATUS_QUERY;
    _gsNum = 0;
    _gs.clear();
}

void
ServerStatusQueryMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_gsNum, sizeof(_gsNum));
    if(_gsNum > 0)
    {
        _gs.reserve(_gsNum);
        uint16_t _gsid;
        for(uint16_t i=0; i<_gsNum; i++)
        {
            pkt->pop(&_gsid, sizeof(_gsid));
            _gs.push_back(_gsid);
        }
    }
}

void
ServerStatusQueryMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    _gsNum = _gs.size();
    pkt->push(&_gsNum, sizeof(_gsNum));
    for(uint16_t i=0; i<_gsNum; i++)
    {
        pkt->push(&_gs[i], sizeof(uint16_t));
    }
}





uint32_t ServerStatusQueryMsg_T::size()
{
    return BaseMessage_T::size()+ sizeof(_gsNum)+ sizeof(uint16_t)*_gs.size();
}

  

//
////ServerLoadToPM
//
/*
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

**/



ServerLoadToPM::ServerLoadToPM()
{
    _type = MsgType::SERVER_LOAD_PM;
    _mapNum = 0;
    _srvload.clear();
}



void
ServerLoadToPM::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_mapNum, sizeof(_mapNum));
    if(_mapNum > 0)
    {
        _srvload.reserve(_mapNum);
        SrvToPM theMapLoad;
        for(uint16_t i=0; i<_mapNum; i++)
        {
            pkt->pop(&theMapLoad, sizeof(theMapLoad));
            _srvload.push_back(theMapLoad);
        }
    }
}

void
ServerLoadToPM::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    _mapNum = _srvload.size();
    pkt->push(&_mapNum, sizeof(_mapNum));
    for(uint16_t i=0; i<_mapNum; i++)
    {
        pkt->push(&_srvload[i], sizeof(SrvToPM));
    }
}





uint32_t ServerLoadToPM::size()
{
    return BaseMessage_T::size()+ sizeof(_mapNum)+ sizeof(SrvToPM)*_srvload.size();
}




//
////
//
/*
class ServerLoadToPMReplyMsg : public BaseMessage_T
{
public:
  ServerLoadToPMReplyMsg();

  virtual void unpack(BasePacket_T* pkt);
  virtual void pack(BasePacket_T* pkt);
  virtual uint32_t size();

  uint8_t _result;
};


**/

ServerLoadToPMReplyMsg::ServerLoadToPMReplyMsg()
{
    _type = MsgType::SERVER_LOAD_PM_REPLAY;
}

void
ServerLoadToPMReplyMsg::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));
}

void
ServerLoadToPMReplyMsg::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
}

uint32_t ServerLoadToPMReplyMsg::size()
{
    return BaseMessage_T::size()+sizeof(_result);
}




//
// ServerStatusReply
//

ServerStatusReplyMsg_T::ServerStatusReplyMsg_T()
{
    _type = MsgType::SERVER_STATUS_REPLY;
    _mapNum = 0;
}

void
ServerStatusReplyMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_playerNum, sizeof(_playerNum));
    // cout<<" _playerNum = "<<_playerNum <<endl;
    pkt->pop(&_cpuLoad, sizeof(_cpuLoad));
    pkt->pop(&_mapNum, sizeof(_mapNum));
    if(_mapNum > 0)
    {
        _maps.reserve(_mapNum);
        MapInfo_T theMap;
        for(int i=0; i<_mapNum; i++)
        {
            pkt->pop(&theMap, sizeof(theMap));
            _maps.push_back(theMap);
        }
    }
}

void
ServerStatusReplyMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_playerNum, sizeof(_playerNum));
    pkt->push(&_cpuLoad, sizeof(_cpuLoad));
    _mapNum = _maps.size();
    pkt->push(&_mapNum, sizeof(_mapNum));
    for(int i=0; i<_mapNum; i++)
    {
        pkt->push(&_maps[i], sizeof(MapInfo_T));
    }
}

uint32_t ServerStatusReplyMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_playerNum)+
                        sizeof(_cpuLoad)+
                        sizeof(_mapNum)+
                        sizeof(MapInfo_T)*_maps.size();
}


//
// StartPartitionRequest
//
StartPartitionRequestMsg_T::StartPartitionRequestMsg_T()
{
    _type = MsgType::START_PARTITION_REQUEST;
    _mapNum = 0;
}






StartZoneMoveRequestMsg_T::StartZoneMoveRequestMsg_T()
{
    _type = MsgType::START_ZONE_MOVE_REQUEST;
    _mapNum = 0;
}






void
StartPartitionRequestMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_serverFrom, sizeof(_serverFrom));
    pkt->pop(&_serverTo, sizeof(_serverTo));
    pkt->pop(&_mapNum, sizeof(_mapNum));

    if (_mapNum > 0)
    {
        _maps.reserve(_mapNum);
        uint32_t mapID;
        for (uint32_t i = 0; i < _mapNum; ++i)
        {
            pkt->pop(&mapID, sizeof(mapID));
            _maps.push_back(mapID);
        }
    }
}



void
StartZoneMoveRequestMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_serverFrom, sizeof(_serverFrom));
    pkt->pop(&_serverTo, sizeof(_serverTo));
    pkt->pop(&_mapNum, sizeof(_mapNum));

    if (_mapNum > 0)
    {
        _maps.reserve(_mapNum);
        uint16_t mapID;
        for (uint32_t i = 0; i < _mapNum; ++i)
        {
            pkt->pop(&mapID, sizeof(mapID));
            _maps.push_back(mapID);
        }
    }
}












void
StartPartitionRequestMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    _mapNum = _maps.size();
    pkt->push(&_serverFrom, sizeof(_serverFrom));
    pkt->push(&_serverTo, sizeof(_serverTo));
    _mapNum = _maps.size();
    pkt->push(&_mapNum, sizeof(_mapNum));
    for (uint32_t i = 0; i < _mapNum; ++i)
    {
        pkt->push(&_maps[i], sizeof(uint32_t));
    }
}






void
StartZoneMoveRequestMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    _mapNum = _maps.size();
    pkt->push(&_serverFrom, sizeof(_serverFrom));
    pkt->push(&_serverTo, sizeof(_serverTo));
    _mapNum = _maps.size();
    pkt->push(&_mapNum, sizeof(_mapNum));
    for (uint32_t i = 0; i < _mapNum; ++i)
    {
        pkt->push(&_maps[i], sizeof(uint16_t));
    }
}











uint32_t StartPartitionRequestMsg_T::size()
{
    return BaseMessage_T::size()+
        sizeof(_serverFrom)+
        sizeof(_serverTo)+
        sizeof(_mapNum)+
        sizeof(uint32_t)*_maps.size();
}




uint32_t StartZoneMoveRequestMsg_T::size()
{
    return BaseMessage_T::size()+
        sizeof(_serverFrom)+
        sizeof(_serverTo)+
        sizeof(_mapNum)+
        sizeof(uint16_t)*_maps.size();
}









//
// StartPartitionResponse
//
StartPartitionResponseMsg_T::StartPartitionResponseMsg_T()
{
    _type = MsgType::START_PARTITION_RESPONSE;
}



StartZoneMoveResponseMsg_T::StartZoneMoveResponseMsg_T()
{
    _type = MsgType::START_ZONE_MOVE_RESPONSE;
}




void
StartPartitionResponseMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));
}




void
StartZoneMoveResponseMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_serverTo, sizeof(_serverTo));
    pkt->pop(&_mapMoved, sizeof(_mapMoved));
    pkt->pop(&_result, sizeof(_result));
}







void
StartPartitionResponseMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
}







void
StartZoneMoveResponseMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_serverTo, sizeof(_serverTo));
    pkt->push(&_mapMoved, sizeof(_mapMoved));
    pkt->push(&_result, sizeof(_result));
}





uint32_t StartPartitionResponseMsg_T::size()
{
    return BaseMessage_T::size()
             +  sizeof(_result);
}
    





uint32_t StartZoneMoveResponseMsg_T::size()
{
    return BaseMessage_T::size()+
        sizeof(_serverTo)+
        sizeof(_mapMoved)+
        sizeof(_result);
}
   










//
// Server leave request
//
ServerLeaveRequest_T::ServerLeaveRequest_T()
{
    _type = MsgType::SERVER_LEAVE_REQUEST;
}


//
// Server leave response
//
ServerLeaveResponse_T::ServerLeaveResponse_T()
{
    _type = MsgType::SERVER_LEAVE_RESPONSE;
}


void
ServerLeaveResponse_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));
}

void
ServerLeaveResponse_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
}

uint32_t ServerLeaveResponse_T::size()
{
    return BaseMessage_T::size()+sizeof(_result);
}


//
/// ServerLockMapRequest
//

ServerLockMapRequest::ServerLockMapRequest()
{
    _type = MsgType::LOCK_MAP_REQUEST;
}

void ServerLockMapRequest::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_nLockMapNum, sizeof(m_nLockMapNum));
    for(uint32_t i=0; i<m_vecMapList.size(); i++)
    {
        pkt->pop(&m_vecMapList[i], sizeof(uint32_t));
    }
}

void ServerLockMapRequest::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_nLockMapNum, sizeof(m_nLockMapNum));
    for(uint32_t i=0; i<m_vecMapList.size(); i++)
    {
        pkt->push(&m_vecMapList[i], sizeof(uint32_t));
    }
}

uint32_t ServerLockMapRequest::size()
{
    return BaseMessage_T::size()+sizeof(m_nLockMapNum)+sizeof(uint32_t)*m_nLockMapNum;
}

//
// ServerLockMapResponse
//
ServerLockMapResponse::ServerLockMapResponse()
{
    _type = MsgType::UNLOCK_MAP_RESPONSE;
}

void ServerLockMapResponse::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_nResult, sizeof(m_nResult));
}

void ServerLockMapResponse::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_nResult, sizeof(m_nResult));
}

uint32_t ServerLockMapResponse::size()
{
    return BaseMessage_T::size()+sizeof(m_nResult);
}

//*************************************************
//
/// ServerUnLockMapRequest
//

ServerUnLockMapRequest::ServerUnLockMapRequest()
{
    _type = MsgType::UNLOCK_MAP_REQUEST;
}

void ServerUnLockMapRequest::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_nUnLockMapNum, sizeof(m_nUnLockMapNum));
    for(uint32_t i=0; i<m_vecMapList.size(); i++)
    {
        pkt->pop(&m_vecMapList[i], sizeof(uint16_t));
    }
}

void ServerUnLockMapRequest::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_nUnLockMapNum, sizeof(m_nUnLockMapNum));
    for(uint32_t i=0; i<m_vecMapList.size(); i++)
    {
        pkt->push(&m_vecMapList[i], sizeof(uint16_t));
    }
}

uint32_t ServerUnLockMapRequest::size()
{
    return BaseMessage_T::size()+sizeof(m_nUnLockMapNum)+sizeof(uint16_t)*m_nUnLockMapNum;
}


//
// ServerLockMapResponse
//
ServerUnLockMapResponse::ServerUnLockMapResponse()
{
    _type = MsgType::LOCK_MAP_RESPONSE;
}

void ServerUnLockMapResponse::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&m_nResult, sizeof(m_nResult));
}

void ServerUnLockMapResponse::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&m_nResult, sizeof(m_nResult));
}

uint32_t ServerUnLockMapResponse::size()
{
    return BaseMessage_T::size()+sizeof(m_nResult);
}

//*************************************************
