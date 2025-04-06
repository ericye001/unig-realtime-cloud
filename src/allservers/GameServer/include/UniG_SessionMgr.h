/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_SessionMgr.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 玩家Session管理器

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _UNIG_SESSION_MGR_H__
#define _UNIG_SESSION_MGR_H__

/**
 * Session key 从proxyID和clientID转变而成
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
 * SessinMgr是Session的容器，对所有玩家的Session进行统一管理
 */
class UniG_SessionMgr 
{
public:
    /**
     * 构造函数
     */
    UniG_SessionMgr();

    /**
     * 析构函数
     */
    ~UniG_SessionMgr();

    /**
     * 销毁Session
     * @param key 由client id和proxy id构造的session key
     */
	void DestroySession(SessionKey& key);

    /**
     * 从server id，proxy id以及client id取得Sessin
     * @param serverID 服务器ID
     * @param proxyID  代理ID
     * @param clientID 客户端ID
     * @return 玩家Session
     */
    UniG_Session* GetSessioin(uint16 serverID, uint16 proxyID, uint16 clientID);

    UniG_Session* CreateSession(uint16 serverID, uint16 proxyID, uint16 clientID);

    /**
     * 根据ID得到Session
     * @param nID 实体或影子的GUID
     */
    UniG_Session* GetSessioin(UNIG_GUID nID);
 
    /**
     * 根据ID取得Player，登陆时用到
     */
    UniG_Player* GetPlayer(UNIG_GUID nGuid);

    /**
     * 处理客户端来的消息
     * @param ProxyMessage 客户端消息
     */
    void ProcessClientMsg(ProxyMessage* pMsg);

    /**
     * 处理msgbus来的消息
     * @param ProxyMessage 客户端消息
     */
    void ProcessPeerObjectMsg(SynAgentMessage* pMsg);

    /**
     * 获得Session实例
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
