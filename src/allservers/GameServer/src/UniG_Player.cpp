#include "UniG.h"

void UniG_Player::ObjectTellPlayer(UniG_GameObject* pSrc, UniG_GameObject* pDest, void* pMsg, int bToSelf)
{
    if( !bToSelf && pSrc == pDest)
    {
        return;
    }

    UniG_Player* pPlayer = (UniG_Player*)pDest;
    if( pPlayer && !pPlayer->IsAgent() )
        pPlayer->GetSession()->EnqueMsg((ObjectMessage*)pMsg);
}

void UniG_Player::PlayerGetObjectInfo(UniG_GameObject* pSrc, UniG_GameObject* pDest, void* pMsg, int bToSelf)
{
    if( !bToSelf && pSrc == pDest)
    {
        return;
    }
    pDest->OnSeeByPlayer(pSrc);
}

UniG_Player::UniG_Player(void)
{
    m_nType = UNIG_TYPE_PLAYER;
    m_pSession = 0;
}

UniG_Player::~UniG_Player(void)
{
    ReleasePropertyTableBuf();
    Finalize();
}

void UniG_Player::SetSession(UniG_Session* session) 
{
    m_pSession = session;
}

UniG_Session* UniG_Player::GetSession()
{
    return m_pSession;
}


void UniG_Player::Logon()
{

}

void UniG_Player::Logout()
{

}

bool UniG_Player::Initialize()
{
    return UniG_Role::Initialize();
}

void UniG_Player::Finalize(void)
{
    UniG_Role::Finalize();
}

void UniG_Player::Serialize(AgentCreateMessage* pMessage)
{
    pMessage->m_nObjType = m_nType;
    pMessage->m_nGuid = m_nGuid;
    pMessage->m_nLength = GetPropertyBufLen();
    pMessage->m_pData = new char[pMessage->m_nLength];
    PtrMemCpy(pMessage->m_pData, m_Property, pMessage->m_nLength);
    UniG_Session* pSession = GetSession();
    if(pSession)
    {
        pMessage->m_nServerID = 0;
        pMessage->m_nProxyID = pSession->ProxyID();
        pMessage->m_nClientID = pSession->ClientID();
    }
    else
    {
        pMessage->m_nServerID = 0;
        pMessage->m_nProxyID = 0;
        pMessage->m_nClientID = 0;
    }


}

void UniG_Player::DeSerialize(AgentCreateMessage* pMessage)
{
    m_nType = pMessage->m_nObjType;
    m_nGuid = pMessage->m_nGuid;
    PtrMemCpy(m_Property, pMessage->m_pData, pMessage->m_nLength);
    UniG_Session* pSession = UniG_SessionMgr::Instance()->CreateSession(
        SeamlessService::Instance().GetServerInfo().nServerID//pMessage->m_nServerID
        , pMessage->m_nProxyID, pMessage->m_nClientID);
    SetSession(pSession);
    pSession->SetPlayer(this);
}


void UniG_Player::SendMsgToClient(ObjectMessage* pMessage)
{
    if(m_pSession)
    {
        m_pSession->EnqueMsg(pMessage);
    }
}


void UniG_Player::RemoveAgent(uint16 nSrvID)
{
    printf("玩家 %d 删除Server %d 上的影子\n", m_nGuid, nSrvID);
    AgentDeleteMessage* pAgentMessage = new AgentDeleteMessage;
    pAgentMessage->m_nGuid = m_nGuid;
    pAgentMessage->m_nObjType = GetType();

    BasePacket pkt;
    pAgentMessage->Pack(&pkt);
    ServerContextMgr::Instance()->SendMsg(nSrvID, pkt.Data(), pkt.Length());
}

void UniG_Player::AddAgent(uint16 nSrvID)
{
    printf("玩家 %d 在Server %d 上产生影子\n", m_nGuid, nSrvID);
    AgentCreateMessage *pAgentMessage = new AgentCreateMessage;

    Serialize(pAgentMessage);
    BasePacket pkt;
    pAgentMessage->Pack(&pkt);
    ServerContextMgr::Instance()->SendMsg(nSrvID, pkt.Data(), pkt.Length());
    delete pAgentMessage;

    SessionCreateMessage message;
    message.m_nObjType = m_nType;
    message.m_nGuid = m_nGuid;
    message.m_nServerID = nSrvID;
    message.m_nProxyID = GetSession()->ProxyID();
    message.m_nClientID = GetSession()->ClientID();

    BasePacket sessionPkt;
    message.Pack(&sessionPkt);
    ServerContextMgr::Instance()->SendMsg(nSrvID, sessionPkt.Data(), sessionPkt.Length());
}



void UniG_Player::OnRegionChange(MapRegion* pLast, MapRegion* pCur)
{
    uint16 nLastServer = MSTInfo::Instance().GetMSTPairByMap(pLast->RegionID()).nServerID;
    uint16 nCurServer = MSTInfo::Instance().GetMSTPairByMap(pCur->RegionID()).nServerID;
    uint16 nThisServer = SeamlessService::Instance().GetServerInfo().nServerID;

    printf("跨Region\n, from %d to %d\n", pLast->RegionID(), pCur->RegionID());
    printf("last server:%d\n",nLastServer);
    printf("next server:%d\n",nCurServer);
    printf("this server:%d\n",nThisServer);
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

        // tell the client to change server.
        ObjectMessage* pMessage = new ObjectMessage;
        pMessage->m_nType = MsgType::CHANGE_SERVERID;
        pMessage->m_nMsgLength = 2;
        pMessage->m_pMsgData = new char[2];
        PtrMemCpy(pMessage->m_pMsgData, &nThisServer, 2);
        GetSession()->EnqueMsg(pMessage, true);
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


