/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: ILMListener.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 负责和ILMServe进行通讯，响应ILMServer的命令

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _ILM_LISTENER_H__
#define _ILM_LISTENER_H__

class ILMListener : public Runnable
{
public:
    ILMListener(ILMArgs& args);
    ~ILMListener();
    void run();
    void Stop();
private:
    Epoll* m_pEpoll;
    bool m_bStopFlag;
};

/**
 * 处理来自ILM消息的类
 */
class ILMObject : public SocketHandler
{
public:
    /**
     * 构造函数，初始化和ILM的连接工作
     */
    ILMObject(Socket* pSocket);

    /**
     * 析构函数
     */
    
    virtual ~ILMObject();
    
    /**
     * 当ILM端有数据发送过来
     */
    virtual int OnRead ();

    /**
     * 当于ILM通讯发生错误
     */
    virtual int OnError();

    /**
     * 初始化和ILM的连接
     * @param args 参数
     * @see ILMArgs
     */
    void Init(ILMArgs& args);   

private:
    
    ILMObject(const ILMObject& rhs);
    ILMObject& operator=(const ILMObject& rhs);
        
    /**
     * handler the packet from ILM
     */

    int Handle(BasePacket& pkt);

    /**
     * 处理更新MST消息
     * @param pMsg MST消息
     */
    int Handle(UpdateMSTRequestMsg* pMsg);

    /**
     * 处理查询Server状态消息
     * @param pMsg 查询消息
     */
    int Handle(ServerStatusQueryMsg* pMsg);

    /**
     * 处理搬家请求消息(包含接收和发送俩个)
     * @param pMsg 搬家消息
     */
    int Handle(StartPartitionRequestMsg* pMsg);

    /**
     * 处理server请求离开消息
     * @param pMsg 请求离开消息
     */
    int Handle(ServerLeaveRequest* pMsg);

    /**
     * 处理锁定地图消息
     * @param pMsg 锁定地图消息
     */
    int Handle(ServerLockMapRequest* pMsg);

    /**
     * 处理释放锁定地图消息
     * @param pMsg 释放对地图的锁定
     */
    int Handle(ServerUnlockMapRequest* pMsg);
    
    /** 
     * 打包并且发送地图消息
     */
    int TransferMigrators(StartPartitionRequestMsg* pMsg);

    /**
     * 接收搬家消息
     */
    int AcceptMigrators(StartPartitionRequestMsg* pMsg);
    
    /** 
     * 取消搬家(不容易实现吧)
     */
    int UndoMigration(StartPartitionRequestMsg* pMsg);
    
    /**
     * 向ILM发消息
     */
    void MsgForward(BasePacket& pkt);
};



#endif

