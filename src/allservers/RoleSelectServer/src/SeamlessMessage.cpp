#include "UniG.h"

/////////////////////////////////////////////////////////////////////
LoadSubMapRequest::LoadSubMapRequest()
{
    m_nType = MsgType::LOAD_SUBMAP_REQUEST;
    m_pSubMapArr = 0;
}

LoadSubMapRequest::~LoadSubMapRequest()
{
    if(m_pSubMapArr)
    {
        delete[] m_pSubMapArr;
    }
}

void LoadSubMapRequest::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nSubMapNum, sizeof(m_nSubMapNum));
    assert(m_nSubMapNum > 0);
    m_pSubMapArr = new SimSubMap[m_nSubMapNum];
    for(int i = 0; i< m_nSubMapNum; i++)
    {
        pkt->Pop(&m_pSubMapArr[i].m_nSubMapID, sizeof(m_pSubMapArr[i].m_nSubMapID));
        pkt->Pop(&m_pSubMapArr[i].m_nSubMapNameLen, sizeof(m_pSubMapArr[i].m_nSubMapNameLen));
        m_pSubMapArr[i].m_pSubMapName = new char[m_pSubMapArr[i].m_nSubMapNameLen];
        pkt->Pop(&m_pSubMapArr[i].m_pSubMapName, m_pSubMapArr[i].m_nSubMapNameLen);
        pkt->Pop(&m_pSubMapArr[i].m_nRegionRowNum, sizeof(m_pSubMapArr[i].m_nRegionRowNum));
        pkt->Pop(&m_pSubMapArr[i].m_nRegionColNum, sizeof(m_pSubMapArr[i].m_nRegionColNum));
        pkt->Pop(&m_pSubMapArr[i].m_nRegionStartRow, sizeof(m_pSubMapArr[i].m_nRegionStartRow));
        pkt->Pop(&m_pSubMapArr[i].m_nRegionStartCol, sizeof(m_pSubMapArr[i].m_nRegionStartCol));
        pkt->Pop(&m_pSubMapArr[i].m_nMSTNum, sizeof(m_pSubMapArr[i].m_nMSTNum));
        if(m_pSubMapArr[i].m_nMSTNum > 0)
        {
            m_pSubMapArr[i].m_pMSTInfo = new uint32[m_pSubMapArr[i].m_nMSTNum];
            pkt->Pop(&m_pSubMapArr[i].m_pMSTInfo, sizeof(uint32) * m_pSubMapArr[i].m_nMSTNum);
        }
    }
}

void LoadSubMapRequest::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nSubMapNum, sizeof(m_nSubMapNum));
    for(int i=0; i<m_nSubMapNum; i++)
    {
        pkt->Push(&m_pSubMapArr[i].m_nSubMapID, sizeof(m_pSubMapArr[i].m_nSubMapID));
        pkt->Push(&m_pSubMapArr[i].m_nSubMapNameLen, sizeof(m_pSubMapArr[i].m_nSubMapNameLen));
        pkt->Push(&m_pSubMapArr[i].m_pSubMapName, m_pSubMapArr[i].m_nSubMapNameLen);
        pkt->Push(&m_pSubMapArr[i].m_nRegionRowNum, sizeof(m_pSubMapArr[i].m_nRegionRowNum));
        pkt->Push(&m_pSubMapArr[i].m_nRegionColNum, sizeof(m_pSubMapArr[i].m_nRegionColNum));
        pkt->Push(&m_pSubMapArr[i].m_nRegionStartRow, sizeof(m_pSubMapArr[i].m_nRegionStartRow));
        pkt->Push(&m_pSubMapArr[i].m_nRegionStartCol, sizeof(m_pSubMapArr[i].m_nRegionStartCol));
        pkt->Push(&m_pSubMapArr[i].m_nMSTNum, sizeof(m_pSubMapArr[i].m_nMSTNum));
        if(m_pSubMapArr[i].m_nMSTNum)
        {
            pkt->Push(m_pSubMapArr[i].m_pMSTInfo, sizeof(uint32)* m_pSubMapArr[i].m_nMSTNum);
        }
    }
}

uint32 LoadSubMapRequest::Size()
{
    uint32 nLength = BaseMessage::Size() + sizeof(m_nSubMapNum);

    for(int i=0; i<m_nSubMapNum; i++)
    {
        nLength += sizeof(m_pSubMapArr[i].m_nSubMapID);
        nLength += sizeof(m_pSubMapArr[i].m_nSubMapNameLen);
        nLength += m_pSubMapArr[i].m_nSubMapNameLen;
        nLength += sizeof(m_pSubMapArr[i].m_nRegionRowNum);
        nLength += sizeof(m_pSubMapArr[i].m_nRegionColNum);
        nLength += sizeof(m_pSubMapArr[i].m_nRegionStartRow);
        nLength += sizeof(m_pSubMapArr[i].m_nRegionStartCol);
        nLength += sizeof(m_pSubMapArr[i].m_nMSTNum);
        nLength += m_pSubMapArr[i].m_nMSTNum * sizeof(uint32);
    }
    return nLength;
}

///////////////////////////////////////////////////////////////////
LockObjectRequest::LockObjectRequest()
{
    m_nType = MsgType::LOCK_OBJECT_REQUEST;
}

void LockObjectRequest::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nObjID, sizeof(m_nObjID));
}

void LockObjectRequest::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nObjID, sizeof(m_nObjID));
}

uint32 LockObjectRequest::Size()
{
    return BaseMessage::Size() + sizeof(m_nObjID);
}


LockObjectResponse::LockObjectResponse()
{
    m_nType = MsgType::LOCK_OBJECT_RESPONSE;     
}

void LockObjectResponse::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nObjID, sizeof(m_nObjID));
}

void LockObjectResponse::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nObjID, sizeof(m_nObjID));
}

uint32 LockObjectResponse::Size()
{
    return BaseMessage::Size() + sizeof(m_nObjID);
}


//---------------------------------------------------------------------------

ConnRequest::ConnRequest()
{

}

void ConnRequest::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_oServerInfo, sizeof(m_oServerInfo));
}

void ConnRequest::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_oServerInfo, sizeof(m_oServerInfo));
}

uint32 ConnRequest::Size()
{
    return BaseMessage::Size()+sizeof(m_oServerInfo);
}


//---------------------------------------------------------------------------

ConnResponse::ConnResponse()
{

}

void ConnResponse::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nResult, sizeof(m_nResult));
}

void ConnResponse::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nResult, sizeof(m_nResult));
}

uint32 ConnResponse::Size()
{
    return BaseMessage::Size()+sizeof(m_nResult);
}

//---------------------------------------------------------------------------
OptionRequest::OptionRequest()
{
    m_nType = Command::OPTION_REQUEST;
}

void OptionRequest::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_oOption, sizeof(m_oOption));
}

void OptionRequest::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_oOption, sizeof(m_oOption));
}

uint32 OptionRequest::Size()
{
    return BaseMessage::Size()+sizeof(m_oOption);
}




//
// ServerConnectILMRequest
//
ServerConnectILMRequestMsg::ServerConnectILMRequestMsg()
{
    m_nType = MsgType::SERVER_CONNECT_ILM_REQUEST;
}

void ServerConnectILMRequestMsg::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nMapVersion, sizeof(m_nMapVersion));
    pkt->Pop(&m_oServerInfo, sizeof(m_oServerInfo));
}

void ServerConnectILMRequestMsg::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nMapVersion, sizeof(m_nMapVersion));
    pkt->Push(&m_oServerInfo, sizeof(m_oServerInfo));
}

uint32 ServerConnectILMRequestMsg::Size()
{
    return BaseMessage::Size()+sizeof(m_oServerInfo)+sizeof(m_nMapVersion);
}


//
// ServerConnectILMResponse
//
ServerConnectILMResponseMsg::ServerConnectILMResponseMsg()
{
    m_nType = MsgType::SERVER_CONNECT_ILM_RESPONSE;
}

void ServerConnectILMResponseMsg::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nResult, sizeof(m_nResult));
    pkt->Pop(&m_oServerInfo, sizeof(m_oServerInfo));

    pkt->Pop(&m_nAsyncMaxMsgNum, sizeof(m_nAsyncMaxMsgNum));
    pkt->Pop(&m_nAsyncBatchNum, sizeof(m_nAsyncBatchNum));

    pkt->Pop(&m_nMaxServerNum, sizeof(m_nMaxServerNum));
    pkt->Pop(&m_nMaxMsgLength, sizeof(m_nMaxMsgLength));
}

void
ServerConnectILMResponseMsg::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nResult, sizeof(m_nResult));
    pkt->Push(&m_oServerInfo, sizeof(m_oServerInfo));

    pkt->Push(&m_nAsyncMaxMsgNum, sizeof(m_nAsyncMaxMsgNum));
    pkt->Push(&m_nAsyncBatchNum, sizeof(m_nAsyncBatchNum));

    pkt->Push(&m_nMaxServerNum, sizeof(m_nMaxServerNum));
    pkt->Push(&m_nMaxMsgLength, sizeof(m_nMaxMsgLength));
}

uint32 ServerConnectILMResponseMsg::Size()
{
    return BaseMessage::Size()+sizeof(m_nResult)+
        sizeof(m_oServerInfo)+
        sizeof(m_nAsyncMaxMsgNum)+
        sizeof(m_nAsyncBatchNum)+
        sizeof(m_nMaxServerNum)+
        sizeof(m_nMaxMsgLength);
}


//
// UpdateMSTRequest
//
UpdateMSTRequestMsg::UpdateMSTRequestMsg()
{
    m_nType = MsgType::UPDATE_MST_REQUEST;
}


void UpdateMSTRequestMsg::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nMSTItemNum, sizeof(m_nMSTItemNum));

    if(m_nMSTItemNum > 0)
    {
        m_vecMST.reserve(m_nMSTItemNum);
        Map2SrvInfo mstItem;

        for(int i = 0; i < m_nMSTItemNum; ++i)
        {
            pkt->Pop(&mstItem, sizeof(Map2SrvInfo));
            m_vecMST.push_back(mstItem);
        }
    }


    pkt->Pop(&m_nAllSrvNum, sizeof(m_nAllSrvNum));
    if (m_nAllSrvNum > 0)
    {
        m_vecAllSrvs.reserve(m_nAllSrvNum);
        ServerInfo srvInfo;
        for(int i = 0; i < m_nAllSrvNum; ++i)
        {
            pkt->Pop(&srvInfo, sizeof(srvInfo));
            m_vecAllSrvs.push_back(srvInfo);
        }
    }
}

void UpdateMSTRequestMsg::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    m_nMSTItemNum = m_vecMST.size();
    pkt->Push(&m_nMSTItemNum, sizeof(m_nMSTItemNum));
    for (int i = 0; i < m_nMSTItemNum; ++i)
    {
        pkt->Push(&m_vecMST[i], sizeof(Map2SrvInfo));
    }

    m_nAllSrvNum = m_vecAllSrvs.size();
    pkt->Push(&m_nAllSrvNum, sizeof(m_nAllSrvNum));
    for (int i = 0; i < m_nAllSrvNum; ++i)
    {
        pkt->Push(&m_vecAllSrvs[i], sizeof(ServerInfo));
    }
}

uint32 UpdateMSTRequestMsg::Size()
{
    return BaseMessage::Size()+sizeof(m_nMSTItemNum)+
        sizeof(Map2SrvInfo)*m_vecMST.size()+
        sizeof(m_nAllSrvNum)+
        sizeof(ServerInfo)*m_vecAllSrvs.size();
}

//
// UpdateMSTResponse
//
UpdateMSTResponseMsg::UpdateMSTResponseMsg()
{
    m_nType = MsgType::UPDATE_MST_RESPONSE;
}

void UpdateMSTResponseMsg::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nResult, sizeof(m_nResult));
}

void
UpdateMSTResponseMsg::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nResult, sizeof(m_nResult));
}

uint32 UpdateMSTResponseMsg::Size()
{
    return BaseMessage::Size()+sizeof(m_nResult);
}


//
// ServerStatusQuery
//
ServerStatusQueryMsg::ServerStatusQueryMsg()
{
    m_nType = MsgType::SERVER_STATUS_QUERY;
}

void ServerStatusQueryMsg::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
	uint16 count;
	pkt->Pop(&count, sizeof(count));
	if( count > 0 && count < 3000 )
	{
		uint16* pgs = new uint16[count];
		pkt->Pop(pgs, sizeof(uint16)*count);
		UniG_SessionMgr::Instance()->UpdateGSList(count, pgs);
		delete []pgs;
	}
}

//
// ServerStatusReply
//

ServerStatusReplyMsg::ServerStatusReplyMsg()
{
    m_nType = MsgType::SERVER_STATUS_REPLY;
    m_nMapNum = 0;
}

void ServerStatusReplyMsg::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_pPlayerNum, sizeof(m_pPlayerNum));
    pkt->Pop(&m_nCpuLoad, sizeof(m_nCpuLoad));
    pkt->Pop(&m_nMapNum, sizeof(m_nMapNum));
    if(m_nMapNum > 0)
    {
        m_vecMaps.reserve(m_nMapNum);
        MapInfo theMap;
        for(int i=0; i<m_nMapNum; i++)
        {
            pkt->Pop(&theMap, sizeof(theMap));
            m_vecMaps.push_back(theMap);
        }
    }
}

void
ServerStatusReplyMsg::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_pPlayerNum, sizeof(m_pPlayerNum));
    pkt->Push(&m_nCpuLoad, sizeof(m_nCpuLoad));
    m_nMapNum = m_vecMaps.size();
    pkt->Push(&m_nMapNum, sizeof(m_nMapNum));
    for(int i=0; i<m_nMapNum; i++)
    {
        pkt->Push(&m_vecMaps[i], sizeof(MapInfo));
    }
}

uint32 ServerStatusReplyMsg::Size()
{
    return BaseMessage::Size()+sizeof(m_pPlayerNum)+
        sizeof(m_nCpuLoad)+
        sizeof(m_nMapNum)+
        sizeof(MapInfo)*m_vecMaps.size();
}


//
// StartPartitionRequest
//
StartPartitionRequestMsg::StartPartitionRequestMsg()
{
    m_nType = MsgType::START_PARTITION_REQUEST;
    m_nMapNum = 0;
}

void
StartPartitionRequestMsg::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nServerFrom, sizeof(m_nServerFrom));
    pkt->Pop(&m_nServerTo, sizeof(m_nServerTo));
    pkt->Pop(&m_nMapNum, sizeof(m_nMapNum));

    if (m_nMapNum > 0)
    {
        m_vecMaps.reserve(m_nMapNum);
        uint16 nMapID;
        for (int i = 0; i < m_nMapNum; ++i)
        {
            pkt->Pop(&nMapID, sizeof(nMapID));
            m_vecMaps.push_back(nMapID);
        }
    }
}

void
StartPartitionRequestMsg::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    m_nMapNum = m_vecMaps.size();
    pkt->Push(&m_nServerFrom, sizeof(m_nServerFrom));
    pkt->Push(&m_nServerTo, sizeof(m_nServerTo));
    m_nMapNum = m_vecMaps.size();
    pkt->Push(&m_nMapNum, sizeof(m_nMapNum));
    for (int i = 0; i < m_nMapNum; ++i)
    {
        pkt->Push(&m_vecMaps[i], sizeof(uint16));
    }
}

uint32 StartPartitionRequestMsg::Size()
{
    return BaseMessage::Size()+
        sizeof(m_nServerFrom)+
        sizeof(m_nServerTo)+
        sizeof(m_nMapNum)+
        sizeof(uint16)*m_vecMaps.size();
}

//
// StartPartitionResponse
//
StartPartitionResponseMsg::StartPartitionResponseMsg()
{
    m_nType = MsgType::START_PARTITION_RESPONSE;
}

void
StartPartitionResponseMsg::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nResult, sizeof(m_nResult));
}

void
StartPartitionResponseMsg::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nResult, sizeof(m_nResult));
}

uint32 StartPartitionResponseMsg::Size()
{
    return BaseMessage::Size() + sizeof(m_nResult);
}

//
// Server leave request
//
ServerLeaveRequest::ServerLeaveRequest()
{
    m_nType = MsgType::SERVER_LEAVE_REQUEST;
}

void ServerLeaveRequest::Unpack(BasePacket* pkt)
{ 
	BaseMessage::Unpack(pkt);
	pkt->Pop(&m_nServerId, sizeof(m_nServerId));
}

void ServerLeaveRequest::Pack(BasePacket* pkt)
{
	BaseMessage::Pack(pkt);
	pkt->Push(&m_nServerId, sizeof(m_nServerId));
}

uint32 ServerLeaveRequest::Size()
{
    return BaseMessage::Size() + sizeof(m_nServerId);
}
//
// Server leave response
//
ServerLeaveResponse::ServerLeaveResponse()
{
    m_nType = MsgType::SERVER_LEAVE_RESPONSE;
}


void
ServerLeaveResponse::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nResult, sizeof(m_nResult));
}

void
ServerLeaveResponse::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nResult, sizeof(m_nResult));
}

uint32 ServerLeaveResponse::Size()
{
    return BaseMessage::Size()+sizeof(m_nResult);
}

ServerLockMapRequest::ServerLockMapRequest()
{
    m_nType = MsgType::LOCK_MAP_REQUEST;
}

void ServerLockMapRequest::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nLockMapNum, sizeof(m_nLockMapNum));
    for(int i=0; i<m_vecMapList.size(); i++)
    {
        pkt->Pop(&m_vecMapList[i], sizeof(uint16));
    }
}

void ServerLockMapRequest::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nLockMapNum, sizeof(m_nLockMapNum));
    for(int i=0; i<m_vecMapList.size(); i++)
    {
        pkt->Push(&m_vecMapList[i], sizeof(uint16));
    }
}

uint32 ServerLockMapRequest::Size()
{
    return BaseMessage::Size()+sizeof(m_nLockMapNum)+sizeof(uint16)*m_nLockMapNum;
}

//
// ServerLockMapResponse
//
ServerLockMapResponse::ServerLockMapResponse()
{
    m_nType = MsgType::LOCK_MAP_RESPONSE;
}

void ServerLockMapResponse::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nResult, sizeof(m_nResult));
}

void ServerLockMapResponse::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nResult, sizeof(m_nResult));
}

uint32 ServerLockMapResponse::Size()
{
    return BaseMessage::Size()+sizeof(m_nResult);
}

//*************************************************
ServerUnlockMapRequest::ServerUnlockMapRequest()
{
    m_nType = MsgType::UNLOCK_MAP_REQUEST;
}

void ServerUnlockMapRequest::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nUnlockMapNum, sizeof(m_nUnlockMapNum));
    for(int i=0; i<m_vecMapList.size(); i++)
    {
        pkt->Pop(&m_vecMapList[i], sizeof(uint16));
    }
}

void ServerUnlockMapRequest::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nUnlockMapNum, sizeof(m_nUnlockMapNum));
    for(int i=0; i<m_vecMapList.size(); i++)
    {
        pkt->Push(&m_vecMapList[i], sizeof(uint16));
    }
}

uint32 ServerUnlockMapRequest::Size()
{
    return BaseMessage::Size()+sizeof(m_nUnlockMapNum)+sizeof(uint16)*m_nUnlockMapNum;
}


//
// ServerUnlockMapResponse
//
ServerUnlockMapResponse::ServerUnlockMapResponse()
{
    m_nType = MsgType::UNLOCK_MAP_RESPONSE;
}

void ServerUnlockMapResponse::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nResult, sizeof(m_nResult));
}

void ServerUnlockMapResponse::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nResult, sizeof(m_nResult));
}

uint32 ServerUnlockMapResponse::Size()
{
    return BaseMessage::Size()+sizeof(m_nResult);
}

//**************************************************
PackMapsRequest::PackMapsRequest()
{
    m_nType = MsgType::PACK_MAP_REQUEST;
}

void PackMapsRequest::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nPackageMapNum, sizeof(m_nPackageMapNum));
    for(int i=0; i<m_vecMapList.size(); i++)
    {
        pkt->Pop(&m_vecMapList[i], sizeof(uint16));
    }
}

void PackMapsRequest::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nPackageMapNum, sizeof(m_nPackageMapNum));
    for(int i=0; i<m_vecMapList.size(); i++)
    {
        pkt->Push(&m_vecMapList[i], sizeof(uint16));
    }
}

uint32 PackMapsRequest::Size()
{
    return BaseMessage::Size()+sizeof(m_nPackageMapNum)+sizeof(uint16)*m_nPackageMapNum;
}


//
// PackMapsResponse
//
PackMapsResponse::PackMapsResponse()
{
    m_nType = MsgType::PACK_MAP_RSPONSE;
}

void PackMapsResponse::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nMapNum, sizeof(m_nMapNum));
    MapPackage tmpPackage;
    memset(&tmpPackage, 0, sizeof(tmpPackage));
    for(uint16 i = 0; i< m_nMapNum; i++)
    {
        pkt->Pop(&(tmpPackage.nMapID), sizeof(tmpPackage.nMapID));
        pkt->Pop(&(tmpPackage.nLength), sizeof(tmpPackage.nLength));
        tmpPackage.pData = new char[tmpPackage.nLength];
        pkt->Pop(tmpPackage.pData, tmpPackage.nLength);
        m_vecMapPackateList.push_back(tmpPackage);
        memset(&tmpPackage, 0, sizeof(tmpPackage));
    }
}

void PackMapsResponse::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nMapNum, sizeof(m_nMapNum));
    MapPackage tmpPackage;
    for(uint16 i = 0; i< m_nMapNum; i++)
    {
        tmpPackage = m_vecMapPackateList[i];
        pkt->Push(&(tmpPackage.nMapID), sizeof(tmpPackage.nMapID));
        pkt->Push(&(tmpPackage.nLength), sizeof(tmpPackage.nLength));
        pkt->Push(tmpPackage.pData, tmpPackage.nLength);
    }
}

uint32 PackMapsResponse::Size()
{
    int iMapPackageLength = 0;
    iMapPackageLength += sizeof(m_nMapNum);
    for(uint16 i=0; i< m_nMapNum; i++)
    {
        iMapPackageLength += sizeof(m_vecMapPackateList[i].nMapID);
        iMapPackageLength += m_vecMapPackateList[i].nLength;
    }
    return BaseMessage::Size()+iMapPackageLength;
}


//
// UnpackMapsRequest
//
UnpackMapsRequest::UnpackMapsRequest()
{
    m_nType = MsgType::UNPACK_MAP_REQUEST;
}

void UnpackMapsRequest::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nMapNum, sizeof(m_nMapNum));
    MapPackage tmpPackage;
    memset(&tmpPackage, 0, sizeof(tmpPackage));
    for(uint16 i = 0; i< m_nMapNum; i++)
    {
        pkt->Pop(&(tmpPackage.nMapID), sizeof(tmpPackage.nMapID));
        pkt->Pop(&(tmpPackage.nLength), sizeof(tmpPackage.nLength));
        tmpPackage.pData = new char[tmpPackage.nLength];
        pkt->Pop(tmpPackage.pData, tmpPackage.nLength);
        m_vecMapPackateList.push_back(tmpPackage);
        memset(&tmpPackage, 0, sizeof(tmpPackage));
    }
}

void UnpackMapsRequest::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nMapNum, sizeof(m_nMapNum));
    MapPackage tmpPackage;
    for(uint16 i = 0; i< m_nMapNum; i++)
    {
        tmpPackage = m_vecMapPackateList[i];
        pkt->Push(&(tmpPackage.nMapID), sizeof(tmpPackage.nMapID));
        pkt->Push(&(tmpPackage.nLength), sizeof(tmpPackage.nLength));
        pkt->Push(tmpPackage.pData, tmpPackage.nLength);
    }
}

uint32 UnpackMapsRequest::Size()
{
    int iMapPackageLength = 0;
    iMapPackageLength += sizeof(m_nMapNum);
    for(uint16 i=0; i< m_nMapNum; i++)
    {
        iMapPackageLength += sizeof(m_vecMapPackateList[i].nMapID);
        iMapPackageLength += m_vecMapPackateList[i].nLength;
    }
    return BaseMessage::Size()+iMapPackageLength;
}

UnpackMapsResponse::UnpackMapsResponse()
{
    m_nType = MsgType::UNPACK_MAP_RSPONSE;
}
void UnpackMapsResponse::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nResult, sizeof(m_nResult));
}

void UnpackMapsResponse::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nResult, sizeof(m_nResult));
}

uint32 UnpackMapsResponse::Size()
{
    return BaseMessage::Size()+sizeof(m_nResult);
}

ClearAgentRequest::ClearAgentRequest()
{
    m_nType = MsgType::REALEASE_OBJECT_REQUEST;
}

void ClearAgentRequest::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nMapNum, sizeof(m_nMapNum));
    for(uint16 i = 0; i< m_nMapNum; i++)
    {
        pkt->Pop(&(m_vecMapList[i]), sizeof(uint16));
    }
}

void ClearAgentRequest::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nMapNum, sizeof(m_nMapNum));
    for(uint16 i = 0; i< m_nMapNum; i++)
    {
        pkt->Push(&(m_vecMapList[i]), sizeof(uint16));
    }
}

uint32 ClearAgentRequest::Size()
{
    int iMapPackageLength = 0;
    iMapPackageLength += sizeof(m_nMapNum);
    for(uint16 i=0; i< m_nMapNum; i++)
    {
        iMapPackageLength += sizeof(m_vecMapList[i]);
    }
    return BaseMessage::Size()+iMapPackageLength;
}

//***********************************************************
ClearAgentResponse::ClearAgentResponse()
{
    m_nType = MsgType::REALEASE_OBJECT_RESPONSE;
}

void ClearAgentResponse::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nResult, sizeof(m_nResult));
}

void ClearAgentResponse::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nResult, sizeof(m_nResult));
}

uint32 ClearAgentResponse::Size()
{
    return BaseMessage::Size()+sizeof(m_nResult);
}

