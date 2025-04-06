#include "UniG.h"


//  在这里定义和客户端通讯的消息有关的东西
ProxyMessage::ProxyMessage()
{
    m_pData = 0;
    m_nType = MsgType::MESSAGE_FROM_PROXY;
    m_nLength = 0;
    m_nServerID = 0;
    m_nProxyID = 0;
    m_nClientID = 0;
}
ProxyMessage::~ProxyMessage()
{
    if(m_pData)
    {
        delete[] m_pData;
        m_pData = 0;
    }
}

void ProxyMessage::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nServerID, sizeof(m_nServerID));
    pkt->Push(&m_nProxyID, sizeof(m_nProxyID));
    pkt->Push(&m_nClientID, sizeof(m_nClientID));
    pkt->Push(&m_nLength, sizeof(m_nLength));
    pkt->Push(m_pData, m_nLength);
}
void ProxyMessage::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nServerID, sizeof(m_nServerID));
    pkt->Pop(&m_nProxyID, sizeof(m_nProxyID));
    pkt->Pop(&m_nClientID, sizeof(m_nClientID));
    pkt->Pop(&m_nLength, sizeof(m_nLength));
    m_pData = new char[m_nLength];
    pkt->Pop(m_pData, m_nLength);
}

uint32 ProxyMessage::Size()
{
    return  BaseMessage::Size() +
            sizeof(m_nServerID) + 
            sizeof(m_nProxyID) + 
            sizeof(m_nClientID) + 
            sizeof(m_nLength) +
            m_nLength;
}


ObjectMessage::ObjectMessage()
{
    m_nType = MsgType::OBJECT_MESSAGE;
    m_nMsgLength = 0;
    m_pMsgData = 0;
}
ObjectMessage::~ObjectMessage()
{
    if(m_pMsgData)
    {
        delete[] m_pMsgData;
        m_pMsgData = 0;
    }
}

void ObjectMessage::Pack(BasePacket* pPacket)
{
    BaseMessage::Pack(pPacket);

    pPacket->Push(&m_nMsgLength, sizeof(m_nMsgLength));
    if(m_nMsgLength > 0)
    {
        pPacket->Push(m_pMsgData, m_nMsgLength);
    }
}

void ObjectMessage::Unpack(BasePacket* pPacket)
{
    BaseMessage::Unpack(pPacket);
    pPacket->Pop(&m_nMsgLength, sizeof(m_nMsgLength));
    if(m_nMsgLength > 0)
    {
        m_pMsgData = new char[m_nMsgLength];
        if(m_pMsgData)
        {
            pPacket->Pop(m_pMsgData, m_nMsgLength);
        }
        else
        {
            //....
        }
    }
}

uint32 ObjectMessage::Size()
{
    return BaseMessage::Size() 
               + sizeof(m_nMsgLength)
               + m_nMsgLength;
}

//----------------------------------------------------------------------------


SystemMessage::SystemMessage()
{
    m_nType = MsgType::SYSTEM_MESSAGE;
}

SystemMessage::~SystemMessage()
{

}

void SystemMessage::Pack(BasePacket* pPacket)
{
    BaseMessage::Pack(pPacket);
    pPacket->Push(&m_nWparam, sizeof(m_nWparam));
    pPacket->Push(&m_nLparam, sizeof(m_nLparam));
}

void SystemMessage::Unpack(BasePacket* pPacket)
{
    BaseMessage::Unpack(pPacket);
    pPacket->Pop(&m_nWparam, sizeof(m_nWparam));
    pPacket->Pop(&m_nLparam, sizeof(m_nLparam));
}

uint32 SystemMessage::Size()
{
    return BaseMessage::Size() + sizeof(m_nWparam) + sizeof(m_nLparam);
}


///////////////////////////////////////////////////////////////////////////////
SynAgentMessage::SynAgentMessage()
{
    m_nType = MsgType::SYN_AGENT_MESSAGE;
    m_nInOut = MsgDir::MSG_IN;
    m_nDestObject = 0;
    m_nObjType = 0;
    m_nRegionID = 0;
}
SynAgentMessage::~SynAgentMessage()
{

}

void SynAgentMessage::Pack(BasePacket* pPacket)
{
    ObjectMessage::Pack(pPacket);
    pPacket->Push(&m_nObjType, sizeof(m_nObjType));
    pPacket->Push(&m_nRegionID, sizeof(m_nRegionID));
    pPacket->Push(&m_nDestObject, sizeof(m_nDestObject));
    pPacket->Push(&m_nInOut, sizeof(m_nInOut));
    pPacket->Push(&m_nObjMsgType, sizeof(m_nObjMsgType));
    
}
void SynAgentMessage::Unpack(BasePacket* pPacket)
{
    ObjectMessage::Unpack(pPacket);
    pPacket->Pop(&m_nObjType, sizeof(m_nObjType));
    pPacket->Pop(&m_nRegionID, sizeof(m_nRegionID));
    pPacket->Pop(&m_nDestObject, sizeof(m_nDestObject));
    pPacket->Pop(&m_nInOut, sizeof(m_nInOut));
    pPacket->Pop(&m_nObjMsgType, sizeof(m_nObjMsgType));
}

uint32 SynAgentMessage::Size()
{
    return ObjectMessage::Size() 
        + sizeof(m_nObjType)
        + sizeof(m_nRegionID) 
        + sizeof(m_nDestObject) 
        + sizeof(m_nInOut)
        + sizeof(m_nObjMsgType);
}

//---------------------------------------------------------------------------
ObjectMsgQueue::ObjectMsgQueue()
    : m_nMaxAsyncMsg(1000), m_nBatchNum(20), m_oCondition(m_oMutex)
{
    
}


ObjectMsgQueue::~ObjectMsgQueue()
{
    if (m_lstQueue.size() > 0)
    {
        list<ObjectMessage*>::iterator curIt = m_lstQueue.begin();
        list<ObjectMessage*>::iterator endIt = m_lstQueue.end();
        for ( ; curIt != endIt; ++curIt)
        {
            delete *curIt;
        }
    }
    m_lstQueue.clear();
}

uint16 ObjectMsgQueue::MaxAsynMsg()
{
    return m_nMaxAsyncMsg;
}

void ObjectMsgQueue::MaxAsynMsg(uint16 maxMsgNum)
{
    m_nMaxAsyncMsg = maxMsgNum;
}

uint16 ObjectMsgQueue::BatchNum()
{
    return m_nBatchNum;
}
void ObjectMsgQueue::Push(ObjectMessage* pMsg)
{
    Guard<Mutex> g(m_oMutex);
    m_lstQueue.push_back(pMsg);
}

uint32 ObjectMsgQueue::Size()
{
    Guard<Mutex> g(m_oMutex);
    return (uint32)m_lstQueue.size();
}
ObjectMessage* ObjectMsgQueue::Front()
{
    Guard<Mutex> g(m_oMutex);
    if (m_lstQueue.size())
    {
        return m_lstQueue.front();
    }
    return NULL;
}
void ObjectMsgQueue::Pop()
{
    Guard<Mutex> g(m_oMutex);
    m_lstQueue.pop_front();
}

void ObjectMsgQueue::FrontBatch(vector<ObjectMessage*>& vc, bool all)
{
    Guard<Mutex> g(m_oMutex);
    size_t num;
    (all) ? (num = m_lstQueue.size()) : num = m_nBatchNum;
    if (m_lstQueue.size() < num)
    {
        return;
    }
    
    list<ObjectMessage*>::iterator curIt = m_lstQueue.begin();
    list<ObjectMessage*>::iterator endIt = m_lstQueue.end();
    for (uint16 i = 0; i < num; ++i)
    {
        vc.push_back(*curIt);
        ++curIt;
    }
}

void ObjectMsgQueue::PopBatch(uint16 count)
{
    Guard<Mutex> g(m_oMutex);
    for (uint16 i = 0; i < count; ++i)
    {
        delete m_lstQueue.front();
        m_lstQueue.pop_front();
    }
}
void ObjectMsgQueue::RequestIncoming()
{
    // ????
    Guard<Mutex> g(m_oMutex);
    m_oCondition.wait();
}
void ObjectMsgQueue::NotifyIncoming()
{
    // ????
    Guard<Mutex> g(m_oMutex);
    m_oCondition.signal();
}

