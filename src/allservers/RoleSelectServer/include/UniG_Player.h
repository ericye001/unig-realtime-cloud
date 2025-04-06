/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_Player.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: ��Ϸ��ҵķ�װ

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _UNIG_PLAYER_H__
#define _UNIG_PLAYER_H__

class UniG_PlayerMgr
{
public:
    virtual UniG_Player* AddNewPlayer(UNIG_GUID id){return NULL;}
};

/**
* ��Ϸ��ҵķ�װ
*/
class UniG_Player
{
public:
    /**
    * ���캯��
    */
    UniG_Player(void);

    /**
    * ��������
    */
    ~UniG_Player(void);

    const UNIG_GUID& GetGuid() const { return m_ID;}
    /**
    * ������ҵĻỰ,UniG_Session���ںͿͻ��˵�ͨѶ
    * @param pSession �ỰSession
    */
    void SetSession(UniG_Session *pSession);

    /**
    * ȡ������ڿͻ��˵ĻỰ
    * @return ��һỰ
    */
    UniG_Session* GetSession();

    /**
    * ��ҵ�¼
    */
    virtual void Logon();

    /**
    * ��ʼ��������������ؽ���Ϣ
    */
    virtual bool Initialize();

    /**
    *  ���ٽ�ɫ��Ϣ
    */
    virtual void Finalize(void);

    /**
    * �ص��ĺ������û�ʵ����������֡����
    */
    virtual void OnUpdate();

    /**
    * ��Ӧ��Ϣ�ĺ������û�ʵ�����������Լ�����Ϣ
    * @param pMsg ��������Ϣ
    */
    virtual void OnMessage(ObjectMessage* pMsg);

    /**
    * ��ͻ��˷�����Ϣ
    * @param pMsg �������ͻ��˵���Ϣ
    */
    void SendMsgToClient(ObjectMessage* pMsg);

protected:

    /// ��һỰ
    UniG_Session*   m_pSession;
    UNIG_GUID       m_ID;
};

#endif
