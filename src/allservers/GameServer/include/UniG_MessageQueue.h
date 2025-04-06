/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_MessageQueue.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 几个全局的消息队列

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _UNIG_MESSAGE_QUEUE_H__
#define _UNIG_MESSAGE_QUEUE_H__


class UniG_MessageQueue
{
private:
    UniG_MessageQueue();
    UniG_MessageQueue(UniG_MessageQueue& that);
    void operator = (UniG_MessageQueue& that);
    ~UniG_MessageQueue();
private:
    MsgQueue* m_pILMInQueue;
    MsgQueue* m_pILMOutQueue;

    MsgQueue* m_pMsgBusInQueue;
    MsgQueue* m_pMsgBusOutQueue;

    MsgQueue* m_pProxyInQueue;
    MsgQueue* m_pProxyOutQueue;

    static UniG_MessageQueue* m_pInstance;
public:
    static UniG_MessageQueue& Instance();
    MsgQueue& ILMInQueue();
    MsgQueue& ILMOutQueue();

    MsgQueue& MsgBusInQueue();
    MsgQueue& MsgBusOutQueue();

    MsgQueue& ProxyInQueue();
    MsgQueue& ProxyOutQueue();
};

#endif

