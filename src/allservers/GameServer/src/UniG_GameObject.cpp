#include "UniG.h"

void AgentInfo::SetEntity(UniG_GameObject* pEntity)
{
    m_pEntity = pEntity;
}

set<uint16> AgentInfo::GetAgentList()
{
    return m_setAgentList;
}

void AgentInfo::UpdateAgent(set<uint16> newServers)
{
    set<uint16>::iterator iterOld, iterNew;
    for(iterOld = m_setAgentList.begin(); iterOld != m_setAgentList.end(); iterOld++)
    {
        // if can't find in new server list
        if(newServers.find(*iterOld) ==newServers.end())
        {
            m_pEntity->RemoveAgent(*iterOld);
        }
    }

    for(iterNew = newServers.begin(); iterNew != newServers.end(); iterNew++)
    {
        if(m_setAgentList.find(*iterNew) == m_setAgentList.end())
        {
            m_pEntity->AddAgent(*iterNew);
        }
    }
    m_setAgentList = newServers;
}


IMPL_START_DEF_PROPERTY(UniG_GameObject, 1)
DEF_COMMON_PROPERTY(UniG_GameObject, Vector3, m_stCurPos)
IMPL_END_DEF_PROPERTY(UniG_GameObject)


UniG_GameObject::UniG_GameObject()
{
    m_nType = UNIG_TYPE_OBJECT;
    m_nGuid = 0xffffffff;
    m_pAgentInfo = new AgentInfo();
    m_pAgentInfo->SetEntity(this);
    m_bAgent = false;
    m_bLocked = false;
    Vector3 temp;
    temp.x = 150.0;
    temp.y = 0.0;
    temp.z = -150.0;
    SetupPropertyTableBuf();
    SetPosition(temp);
}

UniG_GameObject::~UniG_GameObject()
{
    Finalize();
}

uint32 UniG_GameObject::GetType() 
{
    return m_nType;
}

void UniG_GameObject::SetType(uint32 nType)
{
    m_nType = nType;
}


UNIG_GUID UniG_GameObject::GetGuid() 
{
    return m_nGuid;
}

void UniG_GameObject::SetGuid(UNIG_GUID nGuid)
{
    m_nGuid = nGuid;
}



void UniG_GameObject::Lock()
{
    m_bLocked = true;
}

void UniG_GameObject::UnLock()
{
    m_bLocked = false;
}

bool UniG_GameObject::IsLocked()
{
    return m_bLocked;
}

Vector3& UniG_GameObject::GetPosition()
{
    int nSize = sizeof(Vector3);
    return *GetPropertyValue<Vector3>("m_stCurPos", nSize);
}

void UniG_GameObject::SetPosition(const Vector3& postion)
{
    SetPropetryValue<Vector3>(&postion, "m_stCurPos");
}

void UniG_GameObject::OnAddChildObj(UniG_GameObject *pObject)
{

}

void UniG_GameObject::OnDelChildObj(UniG_GameObject *pObject)
{

}

void UniG_GameObject::AddChildObject(UniG_GameObject *pObject)
{
    if((!pObject) || (pObject->GetGuid() == 0xffffffff))
    {
        return;
    }
    m_mapChild[pObject->GetGuid()] = pObject;
    OnAddChildObj(pObject);
}

void UniG_GameObject::DelChildObject(UniG_GameObject *pObject)
{
    if((!pObject) || (pObject->GetGuid() == 0xffffffff))
    {
        return;
    }
    UNIG_GUID nId = pObject->GetGuid();
    map<UNIG_GUID, UniG_GameObject*>::iterator iter = m_mapChild.find(nId);
    if(iter == m_mapChild.end())
    {
        return;
    }
    m_mapChild.erase(iter);
    OnDelChildObj(pObject);
    if(pObject)
    {
        delete pObject;
        pObject = 0;
    }
}

// 这个函数不太好
UniG_GameObject* UniG_GameObject::FindObject(UNIG_GUID nId)
{
    if(nId == 0xffffffff || nId == 0)
    {
        return NULL;
    }
    map<UNIG_GUID, UniG_GameObject*>::iterator iter = m_mapChild.find(nId);
    if(iter == m_mapChild.end()) 
    {
        map<UNIG_GUID, UniG_GameObject*>::iterator it;
        for(it = m_mapChild.begin();it != m_mapChild.end();it++) 
        {
            UniG_GameObject *pChildObj = (*it).second;
            if(pChildObj) 
            {
                UniG_GameObject *pObject = pChildObj->FindObject(nId);
                if(pObject) 
                {
                    return pObject;
                }
            }
        }

        return NULL;
    }

    return (*iter).second;
}



void UniG_GameObject::OnUpdate()
{
    OnUpdate();
    std::map<UNIG_GUID, UniG_GameObject*>::iterator iter;
    for(iter = m_mapChild.begin(); iter != m_mapChild.end();iter++) 
    {
        UniG_GameObject *pObject = (*iter).second;
        if(pObject) 
        {
            pObject->Update();
        }
    }
}


void UniG_GameObject::OnMove(const Vector3& position, bool bStop)
{
    SetPosition(position);
}

void UniG_GameObject::OnEnterSubMap(UniG_SubMap* pSubMap, MapRegion* pRegion, const Vector3& vPos)
{
    m_pSubMap = pSubMap;
    SetPosition(vPos);
}

void UniG_GameObject::OnSeeByPlayer(UniG_GameObject* pSaw)
{

}

void UniG_GameObject::OnLeaveSubMap(UniG_SubMap* pSubMap, MapRegion* pRegion, const Vector3& vPos)
{

}
void UniG_GameObject::OnRegionChange(MapRegion* pLast, MapRegion* pCur)
{
    uint16 nLastServer = MSTInfo::Instance().GetMSTPairByMap(pLast->RegionID()).nServerID;
    uint16 nCurServer = MSTInfo::Instance().GetMSTPairByMap(pCur->RegionID()).nServerID;
    uint16 nThisServer = SeamlessService::Instance().GetServerInfo().nServerID;

    vector<MapRegion*> regionList;
    vector<MapRegion*>::iterator iter;  

    // the last region and new region are in this server, and the object is entity
    if(!IsAgent() && nCurServer == nLastServer && nLastServer == nThisServer)
    {
        m_pSubMap->GetAroundRegion(pCur, regionList);
        std::set<uint16> srvList;
        for( iter = regionList.begin(); iter != regionList.end(); iter++)
        {
            int nCurRegionID = (*iter)->RegionID();
            uint16 nSrvID = MSTInfo::Instance().GetMSTPairByMap(nCurRegionID).nServerID;
            if(nSrvID != nThisServer)
            {
                srvList.insert(nSrvID);
            }
        }
        m_pAgentInfo->UpdateAgent(srvList);
        return;
    }

    else if(!IsAgent() && nLastServer == nThisServer && nCurServer != nThisServer)
    {
        IsAgent(true);
    }
    // agent into this server
    else if(IsAgent() && nLastServer != nThisServer && nCurServer == nThisServer)
    {
        IsAgent(false);
    }
    else
    {
        // needn't to be considered
    }



}

void UniG_GameObject::ComputeAgentList(MapRegion* pCur, vector<AgentInfo>* vecAgents)
{

}

void UniG_GameObject::EnqueMessage(ObjectMessage* pMessage)
{
    m_queObjectMsg.Push(pMessage);
}

void UniG_GameObject::ProcessMessage(ObjectMessage* pMessage)
{
    if(pMessage->m_nType == MsgType::SYN_AGENT_MESSAGE)
    {
        SynAgentMessage* pMsg = (SynAgentMessage*)pMessage;
        pMsg->m_nType = pMsg->m_nObjMsgType;
        if( pMsg->m_nInOut == MsgDir::MSG_OUT)
        {
            SendMsgToNeighbour(pMessage);
            return ;
        }
        else if(pMsg->m_nInOut == MsgDir::MSG_IN)
        {
            OnMessage(pMessage);
            return ;
        }
        else if(pMsg->m_nInOut == MsgDir::MSG_SYN)
        {
            SyncProperty(pMsg->m_pMsgData, pMsg->m_nMsgLength);
            return ;
        }

    }
    OnMessage(pMessage);
    SyncProperty();
}


bool UniG_GameObject::Initialize(void)
{
    SetupPropertyTableBuf();
    m_mapChild.clear();

    return true;
}

void UniG_GameObject::Finalize(void)
{
    ReleasePropertyTableBuf();
    m_mapChild.clear();
}


void UniG_GameObject::Update(void)
{
    vector<ObjectMessage*> vecMsg;
    m_queObjectMsg.FrontBatch(vecMsg, true);
    if(vecMsg.size())
    {
        for(int i=0; i< vecMsg.size(); i++)
            ProcessMessage(vecMsg[i]);
    }
    m_queObjectMsg.PopBatch(vecMsg.size());
    OnUpdate();
}


void UniG_GameObject::OnMessage(ObjectMessage* pMessage)
{

}

void UniG_GameObject::Serialize(AgentCreateMessage* pMessage)
{
    pMessage->m_nObjType = m_nType;
    pMessage->m_nGuid = m_nGuid;
    pMessage->m_nLength = GetPropertyBufLen();
    pMessage->m_pData = new char[pMessage->m_nLength];
    PtrMemCpy(pMessage->m_pData, m_Property, pMessage->m_nLength);   
}

void UniG_GameObject::DeSerialize(AgentCreateMessage* pMessage)
{
    m_nType = pMessage->m_nObjType;
    m_nGuid = pMessage->m_nGuid;
    PtrMemCpy(m_Property, pMessage->m_pData, pMessage->m_nLength);
}

bool UniG_GameObject::IsAgent()
{
    return m_bAgent;
}

void UniG_GameObject::IsAgent(bool bAgent)
{
    m_bAgent = bAgent;
}


/**
* 向客户端发送处理结果消息
*/
void UniG_GameObject::SendMsgToClient(ObjectMessage* pMessage)
{

}

/**
* 向周边玩家广播处理结果消息(9 regions)
*/
void UniG_GameObject::SendMsgToNeighbour(ObjectMessage* pMessage, bool bIncludeSelf)
{
    Assitant(pMessage);
    // construct a result message to send to agent
    m_pSubMap->BroadcastMsg(m_nType, GetPosition(), this, pMessage, bIncludeSelf);

}

/**
* 向周边玩家广播处理结果消息(objects in a circle)
*/
void UniG_GameObject::SendMsgToNeighbour(ObjectMessage* pMessage, Vector3& vCenter, float fRadius)
{
    Assitant(pMessage);
    // construct a result message to send to agent    
    m_pSubMap->BroadcastMsg(m_nType, this, pMessage, true);
}


void UniG_GameObject::Assitant(ObjectMessage* pMessage)
{
    if(IsAgent())
    {
        return ;
    } 

    MapRegion* pCurRegion = m_pSubMap->GetRegion(GetPosition());
    set<uint16> setNewSrvs;
    set<uint16>::iterator iter;
    if(pCurRegion)
    {
        vector<MapRegion*> aroundRegions;
        m_pSubMap->GetAroundRegion(pCurRegion, aroundRegions);
        if(!aroundRegions.empty())
        {
            for(int i=0; i<aroundRegions.size(); i++)
            {
                uint16 tempSrvID = MSTInfo::Instance().GetMSTPairByMap(aroundRegions[i]->RegionID()).nServerID;
                if(tempSrvID !=0 && tempSrvID != SeamlessService::Instance().GetServerInfo().nServerID)
                {
                    setNewSrvs.insert(tempSrvID);
                }
            }
        }
    }
    m_pAgentInfo->UpdateAgent(setNewSrvs);

    if(setNewSrvs.empty())
    {
        return ;
    }

    SynAgentMessage synMsg;
    synMsg.m_nInOut = MsgDir::MSG_OUT;
    synMsg.m_nDestObject = m_nGuid;
    synMsg.m_nObjType = m_nType;
    synMsg.m_nRegionID = m_pSubMap->GetRegion(GetPosition())->RegionID();
    synMsg.m_nMsgLength = pMessage->m_nMsgLength;
    synMsg.m_nObjMsgType = pMessage->m_nType;
    synMsg.m_pMsgData = new char[synMsg.m_nMsgLength];

    PtrMemCpy(synMsg.m_pMsgData, pMessage->m_pMsgData, synMsg.m_nMsgLength);

    BasePacket pkt;
    synMsg.Pack(&pkt);
    int nLength = pkt.Length();
    set<uint16> srvList = m_pAgentInfo->GetAgentList();
    for(iter = srvList.begin(); iter != srvList.end(); iter++)
    {
        ServerContextMgr::Instance()->SendMsg(*iter, pkt.Data(), pkt.Length());
    }
}

void UniG_GameObject::RemoveAgent(uint16 nSrvID)
{
    printf("玩家 %d 删除Server %d 上的影子\n", m_nGuid, nSrvID);
    AgentDeleteMessage* pAgentMessage = new AgentDeleteMessage;
    BasePacket pkt;
    pAgentMessage->Pack(&pkt);
    ServerContextMgr::Instance()->SendBlock(nSrvID, pkt.Data(), pkt.Length());
}

void UniG_GameObject::AddAgent(uint16 nSrvID)
{
    printf("玩家 %d 在Server %d 上产生影子\n", m_nGuid, nSrvID);
    AgentCreateMessage *pAgentMessage = new AgentCreateMessage;

    Serialize(pAgentMessage);
    BasePacket pkt;
    pAgentMessage->Pack(&pkt);
    ServerContextMgr::Instance()->SendBlock(nSrvID, pkt.Data(), pkt.Length());
    delete pAgentMessage;
    // need or not reply???
}

void UniG_GameObject::SyncProperty()
{
    printf("打包属性要同步， 本玩家Agent?%d\n", m_bAgent);
    SynAgentMessage synMsg;
    synMsg.m_nInOut = MsgDir::MSG_SYN;
    synMsg.m_nDestObject = m_nGuid;
    synMsg.m_nObjMsgType = synMsg.m_nType;
    synMsg.m_nObjType = m_nType;
    synMsg.m_nRegionID = m_pSubMap->GetRegion(GetPosition())->RegionID();
    synMsg.m_pMsgData = CPropertyTable::GetDirtyData(synMsg.m_nMsgLength, true);
    assert(synMsg.m_nMsgLength < 512);


    BasePacket pkt;
    synMsg.Pack(&pkt);
    int nLength = pkt.Length();
    set<uint16> srvList = m_pAgentInfo->GetAgentList();
    set<uint16>::iterator iter;
    for(iter = srvList.begin(); iter != srvList.end(); iter++)
    {
        ServerContextMgr::Instance()->SendMsg(*iter, pkt.Data(), pkt.Length());
    }
}

void UniG_GameObject::SyncProperty(char* pBuffer, int nLength)
{
    printf("收到同步属性包，length:%d\n", nLength);
    CPropertyTable::SetDirtyData(pBuffer);
}

UniG_SubMap* UniG_GameObject::GetSubMap()
{
    return m_pSubMap;
}
