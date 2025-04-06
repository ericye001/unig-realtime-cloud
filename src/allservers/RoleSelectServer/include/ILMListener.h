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
    * 搬家后给ILM发送消息的函数(暂时未用到)
    */
    void MigrationResponse(StartPartitionResponseMsg& msg);
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
    * 处理查询Server状态消息
    * @param pMsg 查询消息
    */
    int Handle(ServerStatusQueryMsg* pMsg);

    /**
    * 向ILM发消息
    */
    void MsgForward(BaseMessage& msg);
};

#endif
