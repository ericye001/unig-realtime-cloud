/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_Session.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 玩家Session

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _UNIG_SESSION_H__
#define _UNIG_SESSION_H__

class UniG_Session 
{
public:
    /**
    * 由GUID构建Session
    */
    UniG_Session(UNIG_GUID nGuid);

    /**
    * 析构函数
    */
    virtual ~UniG_Session();

    /**
    * 处理各种莫名其妙的情况
    */
    bool OnCreate(void);

    /**
    * 处理各种莫名其妙的情况
    */
    void OnSocketClose(void);

    /**
    * 处理各种莫名其妙的情况
    */
    void OnTimer(void);

    /**
    * 处理各种莫名其妙的情况
    */
    void OnReconnect(void);

    /** 
    * 处理来自SessionMgr的消息
    * 把消息转给Player
    * @param pMessage SessionMgr传过来的消息
    */ 
    void ProcessMsg(RoleMessage* pMessage);

    /**
    * UniG_Player调用，把要发给客户端的消息暂存到Session队列
    */
    void EnqueMsg(RoleMessage* pMessage);

    /**
    * UniG_Player的消息经过处理转变成代理可认识的消息
    * 并放入自己的队列，这个函数用于从队列中取内容
    */
    ProxyMessage* Front();

    /**
    * 从队列里弹出消息
    */
    void Pop();

    /**
    * 取队列的一组消息
    */
	void FrontBatch(vector<BaseMessage*>& vc, bool all = false);

    /**
    * 删除队首的一组消息
    */
    void PopBatch(uint16 count);

    /**
    * 根据GUID创建Session
    */
    static UniG_Session* CreateSession(UNIG_GUID nGuid);

    /**
    * 取得GUID
    */
    UNIG_GUID GetGuid() { return m_nGuid; }

    /**
    * 取得ServerID
    */
    uint16 ServerID(){  return m_nServerID;}

    /**
    * 设置ServerID
    */
    void ServerID(uint16 serverID){m_nServerID = serverID;}

    /**
    * 取得代理ID
    */
    uint16 ProxyID(){ return m_nProxyID;}

    /**
    * 设置代理ID
    */
    void ProxyID(uint16 proxyID) {m_nProxyID = proxyID;}

    /**
    * 取得客户但ID
    */
    uint32 ClientID(){ return m_nClientID;}

    /**
    * 设置客户端ID 
    */
    void ClientID(uint32 clientID){m_nClientID = clientID;}

    /**
    * 向客户端发送消息
    * 
    */
    bool SendMsgToProxy();

private:
    UNIG_GUID m_nGuid;
    MsgQueue m_oMsgQueue;
    uint16 m_nProxyID;
    uint32 m_nClientID;
    uint16 m_nServerID;
};

#endif 
