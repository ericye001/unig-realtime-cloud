#include "UniG.h"

UniG_MessageQueue*  UniG_MessageQueue::m_pInstance = 0;

UniG_MessageQueue::UniG_MessageQueue(void)
{
    m_pSeamlessServiceInQueue = new MsgQueue();
    m_pSeamlessServiceOutQueue = new MsgQueue();

    m_pProxyInQueue = new MsgQueue();
    m_pProxyOutQueue = new MsgQueue();
}

UniG_MessageQueue::~UniG_MessageQueue(void)
{
    delete m_pSeamlessServiceInQueue;
    delete m_pSeamlessServiceOutQueue;
    delete m_pProxyInQueue;
    delete m_pProxyOutQueue;
}

UniG_MessageQueue& UniG_MessageQueue::Instance()
{
    if(m_pInstance == 0)
    {
        m_pInstance = new UniG_MessageQueue();
    }
    return *m_pInstance;
}

MsgQueue& UniG_MessageQueue::SeamlessInQueue()
{
    return *m_pSeamlessServiceInQueue;
}

MsgQueue& UniG_MessageQueue::SeamlessOutQueue()
{
    return *m_pSeamlessServiceOutQueue;
}

MsgQueue& UniG_MessageQueue::ProxyInQueue()
{
    return *m_pProxyInQueue;
}

MsgQueue& UniG_MessageQueue::ProxyOutQueue()
{
    return *m_pProxyOutQueue;
}