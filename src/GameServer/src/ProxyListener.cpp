#include "UniG.h"

ProxyListener::ProxyListener(ServerSocket* pSocket, SocketHandlerFactory* pFactory)
: ServerSocketHandler(pSocket, pFactory)
{
    
}

void ProxyListener::run()
{
    while(true)
    {
        m_pEpoll->Wait(10);
        Thread::sleep(1);
    }
}

ProxyObject::ProxyObject(Socket* pSocket, uint16 nId)
: SocketHandler(pSocket), m_nProxyID(nId)
{
    
}

ProxyObject::~ProxyObject(void)
{

}

int ProxyObject::OnRead()
{
    uint32 nLen;
    int iTotalReadPacket = 0;
    while(true)
    {
        nLen = 0;
        if(m_oReadBuffer.Read(&nLen, sizeof(nLen), false) < 0)
        {
            return -1;
        }

        if(m_oReadBuffer.Size() < nLen + sizeof(nLen))
        {
            // 没有完整包
            break;
        }
        iTotalReadPacket++;
        // 很没意义的new
        char* pProxyData = new char[nLen];
        m_oReadBuffer.Skip(sizeof(nLen));

        m_oReadBuffer.Read(pProxyData, nLen, true);
   
        BasePacket pkt(pProxyData, nLen, true);
        ProxyMessage* msg = new ProxyMessage;
        msg->Unpack(&pkt);

        UniG_MessageQueue::Instance().ProxyInQueue().Push(msg);
    }
    return iTotalReadPacket;
}

int ProxyObject::OnError()
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////

ProxyObjectFactory* ProxyObjectFactory::m_pInstance = 0;

SocketHandler* ProxyObjectFactory::Create(Socket* pSocket)
{
    ProxyObject* pProxy = new ProxyObject(pSocket, 0);
    pSocket->ConfigureBlocking(false);
    BasePacket pkt;
    pkt.Read(pProxy->Stream());
    uint16 nType = BaseMessage::Type(&pkt);
    ProxyConnResponse response;
    if(nType == MsgType::PROXY_CONNECT_GAMESERVER_REQUEST)
    {
        ProxyConnRequest msg;
        msg.Unpack(&pkt);
        pProxy->SetID(msg.m_nProxyID);
        m_mapProxyObject.insert(pair<uint16, ProxyObject*>(msg.m_nProxyID, pProxy));
        response.m_nResult = 1;
    }
    else
    {
        response.m_nResult = 0;
    }
    
    response.Write(pProxy->Stream());
    return pProxy;  
}

void ProxyObjectFactory::Insert(uint16 nProxyID, ProxyObject* pProxy)
{
    if(m_mapProxyObject.find(nProxyID) == m_mapProxyObject.end())
    {
        m_mapProxyObject.insert(pair<uint16, ProxyObject*>(nProxyID, pProxy));
    }
}

ProxyObject* ProxyObjectFactory::GetProxy(uint16 nProxyID)
{
    map<uint16, ProxyObject*>::iterator iterFind;
    iterFind = m_mapProxyObject.find(nProxyID);
    if(iterFind != m_mapProxyObject.end())
    {
        return iterFind->second;
    }
    return NULL;
}

ProxyObjectFactory* ProxyObjectFactory::Instance()
{
    if(!m_pInstance)
    {
        m_pInstance = new ProxyObjectFactory();
    }
    return m_pInstance;
}

void ProxyObjectFactory::Destroy()
{
    if(m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = 0;
    }
}


void ProxyMsgSender::run()
{
    UniG_SessionMgr* pSessionMgr = UniG_SessionMgr::Instance();
    UniG_SessionMgr::iterator iter;
    while(1)
    {
        for(iter = pSessionMgr->begin(); iter != pSessionMgr->end(); iter++)
        {
            vector<BaseMessage*> msgList;
            iter->second->FrontBatch(msgList,true);
            for(int i=0; i< msgList.size(); i++)
            {
                ProxyObject* pProxy = ProxyObjectFactory::Instance()->GetProxy(((ProxyMessage*)msgList[i])->m_nProxyID);
                if(pProxy)
                {
                    msgList[i]->Write(pProxy->Stream());
                }
            }
            iter->second->PopBatch(msgList.size());
            
        }
        Thread::sleep(1);
    }
}

