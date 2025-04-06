#include "UniG.h"

UniG_Session::UniG_Session(uint16 nServerID, uint16 nProxyID, uint16 nClientID)
{
	m_pPlayer = NULL;
    m_nServerID = nServerID;
    m_nProxyID = nProxyID;
    m_nClientID = nClientID;
}

UniG_Session::~UniG_Session()
{

}

UniG_Session* UniG_Session::CreateSession(uint16 nServerID, uint16 nProxyID, uint16 nClientID)
{
	UniG_Session *pSession = new UniG_Session(nServerID, nProxyID, nClientID);
	if(!pSession)
    {
		return 0;
    }
    if(!pSession->OnCreate())  
    {
		delete pSession;
		return 0;
	}

	return pSession;
}

bool UniG_Session::OnCreate(void)
{
	return true;
}


void UniG_Session::EnqueMsg(ObjectMessage* pMessage, bool bSystemMsg)
{

    if(GetPlayer()->IsAgent())
    {
        return ;
    }
    // 先构造消息，然后赛到队列里
    BasePacket pkt;
    pMessage->Pack(&pkt);

    ProxyMessage* pMsg = new ProxyMessage;
    if(bSystemMsg)
    {
        pMsg->m_nType = MsgType::SYSTEM_MESSAGE;
    }
    pMsg->m_nServerID = m_nServerID;
    pMsg->m_nProxyID = m_nProxyID;
    pMsg->m_nClientID = m_nClientID;
    pMsg->m_nLength =pkt.Length();

    pMsg->m_pData = new char[pMsg->m_nLength];

    PtrMemCpy(pMsg->m_pData, pkt.Data(), pkt.Length());
    
    m_oMsgQueue.Push((BaseMessage*)pMsg);
}


ProxyMessage* UniG_Session::Front()
{
    return (ProxyMessage*)(m_oMsgQueue.Front());
}

void UniG_Session::Pop()
{
    m_oMsgQueue.Pop();
}

void UniG_Session::FrontBatch(vector<BaseMessage*>& vc, bool all)
{
    m_oMsgQueue.FrontBatch(vc, all);
}

void UniG_Session::PopBatch(uint16 count)
{
    m_oMsgQueue.PopBatch(count);
}

void UniG_Session::SetPlayer(UniG_Player *pPlayer)
{
    m_pPlayer = pPlayer;
}

UniG_Player* UniG_Session::GetPlayer(void)
{
    return m_pPlayer;
}


void UniG_Session::OnSocketClose(void)
{

}

void UniG_Session::OnTimer(void)
{

}

void UniG_Session::OnReconnect(void)
{
}

void UniG_Session::ProcessMsg(ObjectMessage* pMessage)
{
    if(m_pPlayer)
    {
        m_pPlayer->EnqueMessage(pMessage);
    }
}

bool UniG_Session::SendMsgToProxy()
{
    ProxyObjectFactory* pProxyManager = ProxyObjectFactory::Instance();
    ProxyObject* pProxy;
    if(pProxyManager == NULL)
    {
        return false;
    }
    vector<BaseMessage*> msgList;
    FrontBatch(msgList, true);
    for(int i=0; i< msgList.size(); i++)
    {   
        pProxy = pProxyManager->GetProxy(((ProxyMessage*)msgList[i])->m_nProxyID);
        if(pProxy == NULL)
        {
            return false;
        }
        msgList[i]->Write(pProxy->Stream());
    }
    return true;
}
