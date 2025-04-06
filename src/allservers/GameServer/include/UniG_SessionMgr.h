/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_SessionMgr.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: ���Session������

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _UNIG_SESSION_MGR_H__
#define _UNIG_SESSION_MGR_H__

/**
 * Session key ��proxyID��clientIDת�����
 */
class SessionKey
{
public:
    SessionKey(uint16 proxyID, uint16 clientID)
        : m_nProxyID(proxyID), m_nClientID(clientID){}
    uint16 m_nProxyID;
    uint16 m_nClientID;
};
inline bool operator < (const SessionKey& left, const SessionKey& right)
{
    return ((left.m_nClientID << 16) + left.m_nProxyID) < ((right.m_nClientID << 16) + right.m_nProxyID);
}

/**
 * SessinMgr��Session����������������ҵ�Session����ͳһ����
 */
class UniG_SessionMgr 
{
public:
    /**
     * ���캯��
     */
    UniG_SessionMgr();

    /**
     * ��������
     */
    ~UniG_SessionMgr();

    /**
     * ����Session
     * @param key ��client id��proxy id�����session key
     */
	void DestroySession(SessionKey& key);

    /**
     * ��server id��proxy id�Լ�client idȡ��Sessin
     * @param serverID ������ID
     * @param proxyID  ����ID
     * @param clientID �ͻ���ID
     * @return ���Session
     */
    UniG_Session* GetSessioin(uint16 serverID, uint16 proxyID, uint16 clientID);

    UniG_Session* CreateSession(uint16 serverID, uint16 proxyID, uint16 clientID);

    /**
     * ����ID�õ�Session
     * @param nID ʵ���Ӱ�ӵ�GUID
     */
    UniG_Session* GetSessioin(UNIG_GUID nID);
 
    /**
     * ����IDȡ��Player����½ʱ�õ�
     */
    UniG_Player* GetPlayer(UNIG_GUID nGuid);

    /**
     * ����ͻ���������Ϣ
     * @param ProxyMessage �ͻ�����Ϣ
     */
    void ProcessClientMsg(ProxyMessage* pMsg);

    /**
     * ����msgbus������Ϣ
     * @param ProxyMessage �ͻ�����Ϣ
     */
    void ProcessPeerObjectMsg(SynAgentMessage* pMsg);

    /**
     * ���Sessionʵ��
     */
	static UniG_SessionMgr* Instance(void);

    typedef std::map<SessionKey, UniG_Session*>::iterator iterator;
    typedef std::map<SessionKey, UniG_Session*>::const_iterator const_iterator;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    UniG_Session* GetFirst();
    UniG_Session* GetNext();

private:
    UniG_Session* CreateSession(UNIG_GUID nGuid);
	static UniG_SessionMgr* m_pInstance;
	map<SessionKey, UniG_Session*> m_mapSession;
    Mutex m_oSessionMutex;
    iterator m_pCurSession;
};

#endif 
