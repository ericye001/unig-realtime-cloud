#include "UniG.h"

ProxyListener::ProxyListener(ServerSocket* pSocket, SocketHandlerFactory* pFactory)
: ServerSocketHandler(pSocket, pFactory)
{

}

void ProxyListener::run()
{
    while(true)
    {
        m_pEpoll->Wait(1);
        Thread::sleep(1);
    }
}

ProxyObject::ProxyObject(Socket* pSocket, int nId)
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
        m_oReadBuffer.Read(&nLen, sizeof(nLen), false);
        if(m_oReadBuffer.Size() < nLen + sizeof(nLen))
        {
            // 没有完整包
            break;
        }
        iTotalReadPacket++;
        // 很没意义的new
        char* pProxyData = new char[nLen];
        m_oReadBuffer.Read(&nLen, sizeof(nLen), true);
        m_oReadBuffer.Read(pProxyData, nLen, true);

        BasePacket pkt(pProxyData, nLen, true);
        ProxyMessage* msg = new ProxyMessage;
        msg->Unpack(&pkt);
		//if( m_nProxyID == 0 )
		//{
		//	m_nProxyID = msg->m_nProxyID;
		//	ProxyObjectFactory::Instance()->Insert(m_nProxyID, this);
		//}
        UniG_MessageQueue::Instance().ProxyInQueue().Push(msg);
    }
    return iTotalReadPacket;
}

int ProxyObject::OnError()
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////

ProxyObjectFactory* ProxyObjectFactory::mProxyObjFactory = 0;

SocketHandler* ProxyObjectFactory::Create(Socket* pSocket)
{
    ProxyObject* pProxy = new ProxyObject(pSocket, 0);
    BasePacket pkt;
    pkt.Read(pProxy->Stream());
    uint16 nType = BaseMessage::Type(&pkt);
    if(nType == MsgType::PROXY_CONNECT_GAMESERVER_REQUEST)
    {
        ProxyConnRequest msg;
        msg.Unpack(&pkt);
        pProxy->SetID(msg.m_nProxyID);
        m_mapProxyObject.insert(pair<uint16, ProxyObject*>(msg.m_nProxyID, pProxy));
    }
    ProxyConnResponse response;
    response.Write(pProxy->Stream());
    pSocket->ConfigureBlocking(false);
	printf("Accept Proxy Server...\n");
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
    if(!mProxyObjFactory)
    {
        mProxyObjFactory = new ProxyObjectFactory();
    }
    return mProxyObjFactory;
}

void ProxyObjectFactory::Destroy()
{
    if(mProxyObjFactory)
    {
        delete mProxyObjFactory;
        mProxyObjFactory = 0;
    }
}


void ProxyMsgSender::run()
{
    while(true)
    {
        UniG_SessionMgr* pSessionMgr = UniG_SessionMgr::Instance();
        UniG_SessionMgr::iterator iter;
        for(iter = pSessionMgr->begin(); iter != pSessionMgr->end(); iter++)
        {
            iter->second->SendMsgToProxy();
        }
        Thread::sleep(1);
    }

}

///////////////////////////////////////////////////////////////////


ProxyConnRequest::ProxyConnRequest()
{
    m_nType = MsgType::PROXY_CONNECT_GAMESERVER_REQUEST;
}

ProxyConnRequest::~ProxyConnRequest()
{
    m_nProxyID = 0;
}

void ProxyConnRequest::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nProxyID, sizeof(m_nProxyID));
}

void ProxyConnRequest::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nProxyID, sizeof(m_nProxyID));
}

uint32 ProxyConnRequest::Size()
{
    return BaseMessage::Size() + sizeof(m_nProxyID);
}

///////////////////////////////////////////////////////////

ProxyConnResponse::ProxyConnResponse()
{
    m_nType = MsgType::PROXY_CONNECT_GAMESERVER_RESPONSE;
}

ProxyConnResponse::~ProxyConnResponse()
{
    m_nResult = 1;
}

void ProxyConnResponse::Unpack(BasePacket* pkt)
{
    BaseMessage::Unpack(pkt);
    pkt->Pop(&m_nResult, sizeof(m_nResult));
}

void ProxyConnResponse::Pack(BasePacket* pkt)
{
    BaseMessage::Pack(pkt);
    pkt->Push(&m_nResult, sizeof(m_nResult));
}

uint32 ProxyConnResponse::Size()
{
    return BaseMessage::Size() + sizeof(m_nResult);
}