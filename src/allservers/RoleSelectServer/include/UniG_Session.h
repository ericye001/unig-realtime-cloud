/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_Session.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: ���Session

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
    * ��GUID����Session
    */
    UniG_Session(UNIG_GUID nGuid);

    /**
    * ��������
    */
    virtual ~UniG_Session();

    /**
    * �������Ī����������
    */
    bool OnCreate(void);

    /**
    * �������Ī����������
    */
    void OnSocketClose(void);

    /**
    * �������Ī����������
    */
    void OnTimer(void);

    /**
    * �������Ī����������
    */
    void OnReconnect(void);

    /** 
    * ��������SessionMgr����Ϣ
    * ����Ϣת��Player
    * @param pMessage SessionMgr����������Ϣ
    */ 
    void ProcessMsg(RoleMessage* pMessage);

    /**
    * UniG_Player���ã���Ҫ�����ͻ��˵���Ϣ�ݴ浽Session����
    */
    void EnqueMsg(RoleMessage* pMessage);

    /**
    * UniG_Player����Ϣ��������ת��ɴ������ʶ����Ϣ
    * �������Լ��Ķ��У�����������ڴӶ�����ȡ����
    */
    ProxyMessage* Front();

    /**
    * �Ӷ����ﵯ����Ϣ
    */
    void Pop();

    /**
    * ȡ���е�һ����Ϣ
    */
	void FrontBatch(vector<BaseMessage*>& vc, bool all = false);

    /**
    * ɾ�����׵�һ����Ϣ
    */
    void PopBatch(uint16 count);

    /**
    * ����GUID����Session
    */
    static UniG_Session* CreateSession(UNIG_GUID nGuid);

    /**
    * ȡ��GUID
    */
    UNIG_GUID GetGuid() { return m_nGuid; }

    /**
    * ȡ��ServerID
    */
    uint16 ServerID(){  return m_nServerID;}

    /**
    * ����ServerID
    */
    void ServerID(uint16 serverID){m_nServerID = serverID;}

    /**
    * ȡ�ô���ID
    */
    uint16 ProxyID(){ return m_nProxyID;}

    /**
    * ���ô���ID
    */
    void ProxyID(uint16 proxyID) {m_nProxyID = proxyID;}

    /**
    * ȡ�ÿͻ���ID
    */
    uint32 ClientID(){ return m_nClientID;}

    /**
    * ���ÿͻ���ID 
    */
    void ClientID(uint32 clientID){m_nClientID = clientID;}

    /**
    * ��ͻ��˷�����Ϣ
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
