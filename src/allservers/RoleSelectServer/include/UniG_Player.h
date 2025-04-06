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

class UniG_PlayerMgr
{
public:
    virtual UniG_Player* AddNewPlayer(UNIG_GUID id){return NULL;}
};

/**
* 游戏玩家的封装
*/
class UniG_Player
{
public:
    /**
    * 构造函数
    */
    UniG_Player(void);

    /**
    * 析构函数
    */
    ~UniG_Player(void);

    const UNIG_GUID& GetGuid() const { return m_ID;}
    /**
    * 设置玩家的会话,UniG_Session用于和客户端的通讯
    * @param pSession 会话Session
    */
    void SetSession(UniG_Session *pSession);

    /**
    * 取得玩家于客户端的会话
    * @return 玩家会话
    */
    UniG_Session* GetSession();

    /**
    * 玩家登录
    */
    virtual void Logon();

    /**
    * 初始化操作，比如加载角信息
    */
    virtual bool Initialize();

    /**
    *  销毁角色信息
    */
    virtual void Finalize(void);

    /**
    * 回调的函数，用户实现它，用于帧更新
    */
    virtual void OnUpdate();

    /**
    * 响应消息的函数，用户实现它来处理自己的消息
    * @param pMsg 待处理消息
    */
    virtual void OnMessage(ObjectMessage* pMsg);

    /**
    * 向客户端发送消息
    * @param pMsg 待发给客户端的消息
    */
    void SendMsgToClient(ObjectMessage* pMsg);

protected:

    /// 玩家会话
    UniG_Session*   m_pSession;
    UNIG_GUID       m_ID;
};

#endif
