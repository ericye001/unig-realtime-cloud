#include "UniG.h"

UniG_SessionMgr* UniG_SessionMgr::m_pInstance = 0;

UniG_SessionMgr* UniG_SessionMgr::Instance(void)
{
	if(!m_pInstance)
    {
        m_pInstance = new UniG_SessionMgr();
    }
    return m_pInstance;
}

UniG_SessionMgr::iterator UniG_SessionMgr::begin()
{
    return m_mapSession.begin();
}

UniG_SessionMgr::iterator UniG_SessionMgr::end()
{
    return m_mapSession.end();
}

UniG_SessionMgr::const_iterator UniG_SessionMgr::begin() const
{
    return m_mapSession.begin();
}

UniG_SessionMgr::const_iterator UniG_SessionMgr::end() const
{
    return m_mapSession.end();
}

UniG_Session* UniG_SessionMgr::GetFirst()
{
    Guard<Mutex> g(m_oSessionMutex);
    if(m_mapSession.empty())
    {
        return NULL;
    }
    m_pCurSession = m_mapSession.begin();
    return m_pCurSession->second;
}

UniG_Session* UniG_SessionMgr::GetNext()
{
    Guard<Mutex> g(m_oSessionMutex);
    m_pCurSession++;
    if(m_pCurSession == m_mapSession.end())
    {
        return NULL;
    }
    return m_pCurSession->second;
}


UniG_SessionMgr::UniG_SessionMgr()
{
    m_pCurSession = NULL;
}

UniG_SessionMgr::~UniG_SessionMgr()
{

}


UniG_Session* UniG_SessionMgr::GetSessioin(uint16 serverID, uint16 proxyID, uint16 clientID)
{
    Guard<Mutex> g(m_oSessionMutex);
    SessionKey key(proxyID, clientID);
    map<SessionKey, UniG_Session*>::iterator iter = m_mapSession.find(key);
	if(iter != m_mapSession.end()) 
    {
		return iter->second;
	}

    return NULL;
}


UniG_Session* UniG_SessionMgr::CreateSession(uint16 serverID, uint16 proxyID, uint16 clientID)
{
    Guard<Mutex> g(m_oSessionMutex);
    SessionKey key(proxyID, clientID);
    map<SessionKey, UniG_Session*>::iterator iter = m_mapSession.find(key);
	if(iter == m_mapSession.end()) 
    {
        SessionKey key(proxyID, clientID);
        UniG_Session* pSession = UniG_Session::CreateSession(serverID, proxyID, clientID);
        m_mapSession.insert(make_pair(key, pSession));
        return pSession;
	}
    return iter->second;
}

UniG_Session* UniG_SessionMgr::GetSessioin(UNIG_GUID nID)
{
    Guard<Mutex> g(m_oSessionMutex);
    map<SessionKey, UniG_Session*>::iterator iter;
    for(iter = m_mapSession.begin(); iter != m_mapSession.end(); iter++)
    {
        if( iter->second->GetPlayer()->GetGuid() == nID)
        {
            return iter->second;
        }
    }
    return NULL;
}

void UniG_SessionMgr::DestroySession(SessionKey& key)
{
    Guard<Mutex> g(m_oSessionMutex);
	map<SessionKey, UniG_Session*>::iterator iter = m_mapSession.find(key);
	if(iter == m_mapSession.end()) 
    {
		return;
	}

	UniG_Session *pSession = (*iter).second;

	if(pSession) 
    {
        UniG_Player* pPlayer = pSession->GetPlayer();
		delete pSession;
		pSession = NULL;
	}
}

UniG_Player* UniG_SessionMgr::GetPlayer(UNIG_GUID nGuid)
{
    //todo 从数据库里加载玩家信息
    return NULL;
}

void UniG_SessionMgr::ProcessClientMsg(ProxyMessage* pMsg)
{
    // 在这里需要解包，觉得转发给哪个玩家的session
    if(pMsg)
    {
        uint16 serverID = pMsg->m_nServerID;
        uint16 proxyID = pMsg->m_nProxyID;
        uint16 clientID = pMsg->m_nClientID;
        uint16 length = pMsg->m_nLength;
        BasePacket pkt(pMsg->m_pData, pMsg->m_nLength);

        if(pMsg->m_nType == MsgType::SYSTEM_MESSAGE)
        {
            SystemMessage *pSysMsg = new SystemMessage();
            pSysMsg->Unpack(&pkt);

            switch(pSysMsg->m_nType)
            {
                case MsgType::NEW_USER_LOGIN:
                {
                    UniG_Session* pSession = CreateSession(serverID, proxyID, clientID);

                    UniG_PlayerMgr *pPlayerMgr = UniG_GameServer::Instance().GetGameLogic()->GetPlayerMgr();
                    UNIG_GUID nGuid = pSysMsg->m_nWparam;
                    UniG_Player * pNewPlayer = pPlayerMgr->AddNewPlayer(nGuid);

                    if(pSession == NULL || pNewPlayer == NULL)
                    {
                        return ;
                    }

                    pSession->ServerID(serverID);
                    pSession->ProxyID(proxyID);
                    pSession->ClientID(clientID);

                    pSession->SetPlayer(pNewPlayer);
                    pNewPlayer->SetSession(pSession);

                    pNewPlayer->Logon();
                    return ;
                }
                case MsgType::USER_LOGOUT:
                {
                    UniG_Session* pSession = GetSessioin(serverID, proxyID, clientID);
                    UniG_PlayerMgr *pPlayerMgr = UniG_GameServer::Instance().GetGameLogic()->GetPlayerMgr();
                    UNIG_GUID nGuid = pSysMsg->m_nWparam;
                    UniG_Player * pPlayer = pPlayerMgr->GetPlayerByID(nGuid);

                    if( pPlayer != NULL )
                        pPlayer->Logout();
                    
                    //if(pSession != NULL )
                    //{
                    //    DestroySession(SessionKey(proxyID, clientID));
                    //    return ;
                    //}
                    return;
                }
            }
            
        }
        else if(pMsg->m_nType == MsgType::OBJECT_MESSAGE)
        {
            ObjectMessage *pObjMsg = new ObjectMessage();
            pObjMsg->Unpack(&pkt);

            UniG_Session* pSession = GetSessioin(serverID, proxyID, clientID);
            if(pSession == NULL)
            {
                // TODO  error message
                return;
            }
            pSession->ProcessMsg(pObjMsg);
        }
    }

}

void UniG_SessionMgr::ProcessPeerObjectMsg(SynAgentMessage *pMsg)
{
    GetSessioin(pMsg->m_nDestObject)->GetPlayer()->SendMsgToNeighbour(pMsg);
}

