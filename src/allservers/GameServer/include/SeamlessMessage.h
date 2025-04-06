/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: SeamlessMessage.h
Author: Abone
Version: 2.0
Date: 2006-12-18
Description: 无缝服务相关的消息

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _SEAMLESS_MESSAGE_H__
#define _SEAMLESS_MESSAGE_H__

class SimSubMap
{
public:
    SimSubMap()
    {
        m_nSubMapID = 0;
        m_nSubMapNameLen = 0;
        m_pSubMapName = 0;
        m_nRegionRowNum = 0;
        m_nRegionColNum = 0;
        m_nRegionStartRow = 0;
        m_nRegionStartCol = 0;
        m_nMSTNum = 0;
        m_pMSTInfo = 0;
    }
    ~SimSubMap()
    {
        if(m_pSubMapName)
        {
            delete[] m_pSubMapName;
        }
        if(m_pMSTInfo)
        {
            delete[] m_pMSTInfo;
        }
    }
    uint16 m_nSubMapID;
    uint16 m_nSubMapNameLen;
    char* m_pSubMapName;
    uint16 m_nRegionRowNum;
    uint16 m_nRegionColNum;
    uint16 m_nRegionStartRow;
    uint16 m_nRegionStartCol;
    uint16 m_nMSTNum;
    uint32* m_pMSTInfo;
};

class LoadSubMapRequest : public BaseMessage
{
public:
    LoadSubMapRequest();
    ~LoadSubMapRequest();
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    uint16 m_nSubMapNum;
    SimSubMap* m_pSubMapArr;
};

class ProxyConnRequest : public BaseMessage
{
public:
    ProxyConnRequest();
    ~ProxyConnRequest();
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint16 m_nProxyID;
};

class ProxyConnResponse : public BaseMessage
{
public:
    ProxyConnResponse();
    ~ProxyConnResponse();
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint8 m_nResult;
};

class LockObjectRequest : public BaseMessage
{
public:
    LockObjectRequest();
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    UNIG_GUID m_nObjID;
};

class LockObjectResponse : public BaseMessage
{
public:
    LockObjectResponse();
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    UNIG_GUID m_nObjID;
};


class ConnRequest : public BaseMessage
{
public:    
    ConnRequest();
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    ServerInfo m_oServerInfo;
};

class ConnResponse : public BaseMessage
{
public:
    ConnResponse();
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    uint8 m_nResult;
};


class OptionRequest : public BaseMessage
{
public:
    OptionRequest ();
    
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    Option m_oOption;
};

class ServerConnectILMRequestMsg : public BaseMessage
{
public:
    ServerConnectILMRequestMsg();
    virtual ~ServerConnectILMRequestMsg(){};
    
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint16  m_nMapVersion;    
    ServerInfo m_oServerInfo;

};

class ServerConnectILMResponseMsg : public BaseMessage
{
public:
    ServerConnectILMResponseMsg();

    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint8  m_nResult;
    ServerInfo m_oServerInfo;

    uint16 m_nAsyncMaxMsgNum;
    uint16 m_nAsyncBatchNum;
    uint16 m_nMaxServerNum;
    uint16 m_nMaxMsgLength;
};


class UpdateMSTRequestMsg : public BaseMessage
{
public:
    UpdateMSTRequestMsg();

    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint32 m_nMSTItemNum;
    vector<Map2SrvInfo> m_vecMST;

    uint16 m_nAllSrvNum;
    vector<ServerInfo> m_vecAllSrvs;
};

class UpdateMSTResponseMsg : public BaseMessage
{
public:
    UpdateMSTResponseMsg();

    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint8 m_nResult;
};

class ServerStatusQueryMsg : public BaseMessage
{
public:
    ServerStatusQueryMsg();
};

class ServerStatusReplyMsg : public BaseMessage
{
public:
    ServerStatusReplyMsg();

    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint16 m_pPlayerNum;
    uint16 m_nCpuLoad;
    uint16 m_nMapNum;

    vector<MapInfo> m_vecMaps;
};

class StartPartitionRequestMsg : public BaseMessage
{
public:
    StartPartitionRequestMsg();

    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
 
    uint16  m_nServerFrom;
    uint16  m_nServerTo;      // destination server to move the map(s)
    uint32  m_nMapNum;
    vector<uint32> m_vecMaps;
};


class StartPartitionResponseMsg : public BaseMessage
{
public:
    StartPartitionResponseMsg();

    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint8 m_nResult;
};


class ServerLeaveRequest : public BaseMessage
{
public:
    ServerLeaveRequest();    
};

class ServerLeaveResponse : public BaseMessage
{
public:
    ServerLeaveResponse();
    
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint8 m_nResult;
};

class ServerLockMapRequest : public BaseMessage
{
public:
    ServerLockMapRequest();

    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    uint16 m_nLockMapNum;
    vector<uint16> m_vecMapList;
};


class ServerLockMapResponse : public BaseMessage
{
public:
    ServerLockMapResponse();
    
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint8 m_nResult;
};


class ServerUnlockMapRequest : public BaseMessage
{
public:
    ServerUnlockMapRequest();

    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    uint16 m_nUnlockMapNum;
    vector<uint16> m_vecMapList;
};

class ServerUnlockMapResponse : public BaseMessage
{
public:
    ServerUnlockMapResponse();
    
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint8 m_nResult;
};

class PackMapsRequest : public BaseMessage
{
public:
    PackMapsRequest();
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    uint16 m_nPackageMapNum;
    vector<uint32> m_vecMapList;
};

class AgentCreateMessage;

struct MapPackage
{
public:
    uint32 nMapID;
    uint32 nAgentNum;
    vector<AgentCreateMessage*> vAgentList;
};

class PackMapsResponse : public BaseMessage
{
public:
    PackMapsResponse();
    
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    uint16 m_nMapNum;
    vector<MapPackage> m_vecMapPackateList;
};


class UnpackMapsRequest : public BaseMessage
{
public:
    UnpackMapsRequest();
    
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    uint16 m_nMapNum;
    vector<MapPackage> m_vecMapPackateList;
};

class UnpackMapsResponse : public BaseMessage
{
public:
    UnpackMapsResponse();
    
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint8 m_nResult;
};


class ClearAgentRequest : public BaseMessage
{
public:
    ClearAgentRequest();
    
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    uint16 m_nMapNum;
    vector<uint16> m_vecMapList;
};


class ClearAgentResponse : public BaseMessage
{
public:
    ClearAgentResponse();
    
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();

    uint8 m_nResult;
};

class AgentCreateMessage : public BaseMessage
{
public:
    AgentCreateMessage();
    ~AgentCreateMessage();
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    uint32 m_nObjType;
    UNIG_GUID m_nGuid;
    
    uint16 m_nServerID;
    uint16 m_nProxyID;
    uint16 m_nClientID;

    uint32 m_nLength;
    char* m_pData;
};

class AgentDeleteMessage : public BaseMessage
{
public:
    AgentDeleteMessage();
    ~AgentDeleteMessage();
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    uint32 m_nObjType;
    UNIG_GUID m_nGuid;
};

class SessionCreateMessage : public BaseMessage
{
public:
    SessionCreateMessage();
    ~SessionCreateMessage();
    virtual void Unpack(BasePacket* pkt);
    virtual void Pack(BasePacket* pkt);
    virtual uint32 Size();
    
    uint32 m_nObjType;
    UNIG_GUID m_nGuid;
    uint16 m_nServerID;
    uint16 m_nProxyID;
    uint16 m_nClientID;
};

#endif
