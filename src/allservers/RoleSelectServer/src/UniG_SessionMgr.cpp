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



UniG_SessionMgr::UniG_SessionMgr()
{

}

UniG_SessionMgr::~UniG_SessionMgr()
{

}


UniG_Session* UniG_SessionMgr::GetSessioin(uint16 serverID, uint16 proxyID, uint32 clientID)
{
    SessionKey key(proxyID, clientID);
    map<SessionKey, UniG_Session*>::iterator iter = m_mapSession.find(key);
    if(iter != m_mapSession.end()) 
    {
        return iter->second;
    }

    return NULL;
}

UniG_Session* UniG_SessionMgr::GetSessioin(UNIG_GUID nID)
{
    map<SessionKey, UniG_Session*>::iterator iter;
    for(iter = m_mapSession.begin(); iter != m_mapSession.end(); iter++)
    {
        if( iter->second->GetGuid() == nID)
        {
            return iter->second;
        }
    }
    return NULL;
}

void UniG_SessionMgr::DestroySession(SessionKey& key)
{
    map<SessionKey, UniG_Session*>::iterator iter = m_mapSession.find(key);
    if(iter == m_mapSession.end()) 
    {
        return;
    }

    UniG_Session *pSession = (*iter).second;

    if(pSession) 
    {
        delete pSession;
        pSession = NULL;
    }
}

void UniG_SessionMgr::ProcessClientMsg(ProxyMessage* pMsg)
{
    if(pMsg)
    {
        uint16 length = pMsg->m_nLength;
        uint16 serverID = pMsg->m_nServerID;
        uint16 proxyID = pMsg->m_nProxyID;
        uint32 clientID = pMsg->m_nClientID;
        int dataLen = length - sizeof(length) - sizeof(serverID) - sizeof(proxyID) - sizeof(clientID);

        UniG_Session* pSession = GetSessioin(serverID, proxyID, clientID);

        if( pSession == NULL )                          // new session
        {
            UNIG_GUID guid ;
            memcpy( &guid, pMsg->m_pData, sizeof(guid));
            if(guid != 0xffffffff && guid !=0)
            {
                pSession = UniG_Session::CreateSession(guid);
                if(pSession == NULL)
                {
                    return ;
                }
                pSession->ServerID(serverID);
                pSession->ProxyID(proxyID);
                pSession->ClientID(clientID);

                SessionKey key(proxyID, clientID);
                m_mapSession.insert(make_pair(key, pSession));
            } 
        }

        // push msg to player
        RoleMessage Message;
        Message.m_nInOut = MsgDir::MSG_IN;
		Message.m_nType = *((uint16*)pMsg->m_pData);
		printf("Get Message From Proxy, Type Is %d\n", Message.m_nType);
        pSession->ProcessMsg(&Message);
    }

}

// temp code
uint16 UniG_SessionMgr::GetAGS()
{
	Guard<Mutex> g(m_GSMutex);

	if( m_GSList.size() == 0 )
		return 1;
	uint16 nId = m_GSList[0];
	if( nId < 0 || nId > 3000)
		return 1;
	return nId;
}

void UniG_SessionMgr::UpdateGSList(uint16 nCount, uint16* pBuf)
{
	Guard<Mutex> g(m_GSMutex);

	m_GSList.clear();
	m_GSList.insert(m_GSList.end(), pBuf, pBuf + nCount);
}

void UniG_SessionMgr::RemoveGS(uint16 nId)
{
	Guard<Mutex> g(m_GSMutex);
	
	vector<uint16>::iterator it = find(m_GSList.begin(), m_GSList.end(), nId);
	if( it != m_GSList.end())
		m_GSList.erase(it);
}

