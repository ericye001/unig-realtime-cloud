#include "UniG.h"

UniG_MessageQueue*  UniG_MessageQueue::m_pInstance = 0;

UniG_MessageQueue::UniG_MessageQueue(void)
{
    m_pILMInQueue = new MsgQueue();
    m_pILMOutQueue = new MsgQueue();

    m_pProxyInQueue = new MsgQueue();
    m_pProxyOutQueue = new MsgQueue();

    m_pMsgBusInQueue = new MsgQueue();
    m_pMsgBusOutQueue = new MsgQueue();
}

UniG_MessageQueue::~UniG_MessageQueue(void)
{
    delete m_pILMInQueue;
    delete m_pILMOutQueue;
    delete m_pProxyInQueue;
    delete m_pProxyOutQueue;
    delete m_pMsgBusInQueue;
    delete m_pMsgBusOutQueue;
}


UniG_MessageQueue& UniG_MessageQueue::Instance()
{
    if(m_pInstance == 0)
    {
        m_pInstance = new UniG_MessageQueue();
    }
    return *m_pInstance;
}

    MsgQueue& UniG_MessageQueue::ILMInQueue()
{
    return *m_pILMInQueue;
}

MsgQueue& UniG_MessageQueue::ILMOutQueue()
{
    return *m_pILMOutQueue;
}

MsgQueue& UniG_MessageQueue::MsgBusInQueue()
{
    return *m_pMsgBusInQueue;
}

MsgQueue& UniG_MessageQueue::MsgBusOutQueue()
{
    return *m_pMsgBusOutQueue;
}

MsgQueue& UniG_MessageQueue::ProxyInQueue()
{
    return *m_pProxyInQueue;
}

MsgQueue& UniG_MessageQueue::ProxyOutQueue()
{
    return *m_pProxyOutQueue;
}