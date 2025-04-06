/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_Player.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 游戏玩家的封装

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _UNIG_PLAYER_H__
#define _UNIG_PLAYER_H__

class AgentInfo;

class UniG_PlayerMgr
{
public:
    virtual ~UniG_PlayerMgr(){};

    virtual UniG_Player* AddNewPlayer(ObjectMessage* pMsg) = 0;

    virtual UniG_Player* AddNewPlayer(UNIG_GUID& nGuid) = 0;

    virtual void RemovePlayer(UNIG_GUID& nGuid) = 0;

    virtual UniG_Player* AddNewPlayer(char* pBuffer, int nLength) = 0;

    virtual UniG_Player* GetPlayerByID(UNIG_GUID& nGuid) = 0;

    virtual void PlayeChangeServer(UNIG_GUID nGuid, uint16 nDestSrv)
    {
        // send a cmd to client "change server"
    }

};

/**
* 游戏玩家的封装
*/
class UniG_Player : public UniG_Role
{
public:
    UniG_Player(void);

    ~UniG_Player(void);

    void SetSession(UniG_Session *pSession);

    UniG_Session* GetSession();

    virtual void Logon();
    virtual void Logout();

    virtual bool Initialize();

    virtual void Finalize(void);
    /**
    * serialize an object to buffer
    * @param pBuffer the data buffer
    * @param nLength the length of buffer
    */
    virtual void Serialize(AgentCreateMessage* pMessage);

    /**
    * deserialize a buffer to a object
    * @param pBuffer the data buffer
    * @param nLength the buffer length
    */
    virtual void DeSerialize(AgentCreateMessage* pMessage);

    virtual void OnRegionChange(MapRegion* pLast, MapRegion* pCur);

    virtual void RemoveAgent(uint16 nSrvID);

    virtual void AddAgent(uint16 nSrvID);



    /**
    * 向客户端发送消息
    * @param pMsg 待发给客户端的消息
    */
    void SendMsgToClient(ObjectMessage* pMsg);

    static void ObjectTellPlayer(UniG_GameObject* pSrc, UniG_GameObject* pDest, void* pMsg, int bToSelf);
    static void PlayerGetObjectInfo(UniG_GameObject* pSrc, UniG_GameObject* pDest, void* pMsg, int bToSelf);
protected:

    /// 玩家会话
    UniG_Session* m_pSession;
};

#endif
