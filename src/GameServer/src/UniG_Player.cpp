#include "UniG.h"

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

bool UniG_Player::Initialize()
{
	return true;
}


void UniG_Player::Finalize(void)
{
}


void UniG_Player::OnUpdate()
{

}


void UniG_Player::OnMessage(ObjectMessage* pMsg)
{

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
    AgentDeleteMessage* pAgentMessage = new AgentDeleteMessage;
    BasePacket pkt;
    pAgentMessage->Pack(&pkt);
    ServerContextMgr::Instance()->SendBlock(nSrvID, pkt.Data(), pkt.Length());
}

void UniG_Player::AddAgent(uint16 nSrvID)
{
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
    //ServerContextMgr::Instance()->FindSrvCtx(nSrvID)->SendMsg(sessionPkt.Data(), sessionPkt.Length());
}



void UniG_Player::OnRegionChange(MapRegion* pLast, MapRegion* pCur)
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

        // tell the client to change server.
        ObjectMessage* pMessage = new ObjectMessage;
        pMessage->m_nType = MsgType::CHANGE_SERVERID;
        pMessage->m_nMsgLength = 2;
        pMessage->m_pMsgData = new char[2];
        memcpy(pMessage->m_pMsgData, &nThisServer, 2);
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


