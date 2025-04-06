#include "UniG.h"

UniG_Session::UniG_Session(UNIG_GUID nGuid)
{
    m_nGuid = nGuid;
}

UniG_Session::~UniG_Session()
{
}

UniG_Session* UniG_Session::CreateSession(UNIG_GUID nGuid)
{

    UniG_Session *pSession = new UniG_Session(nGuid);
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


void UniG_Session::EnqueMsg(RoleMessage* pMessage)
{
    // 先构造消息，然后赛到队列里
    ProxyMessage* pMsg = new ProxyMessage;
	pMsg->m_nLength = pMessage->Size() + pMsg->GetHeaderLength();
    pMsg->m_nServerID = m_nServerID;
    pMsg->m_nProxyID = m_nProxyID;
    pMsg->m_nClientID = m_nClientID;

	BasePacket pkt;
	pMessage->Pack(&pkt);
	
	pMsg->m_pData = new char[pkt.Length()];
	memcpy(pMsg->m_pData, pkt.Data(), pkt.Length());

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

void UniG_Session::OnSocketClose(void)
{

}

void UniG_Session::OnTimer(void)
{

}

void UniG_Session::OnReconnect(void)
{
}

void UniG_Session::ProcessMsg(RoleMessage* pMessage)
{
    RoleMessage rmsg;
	
    switch(pMessage->m_nType)
    {
	case MsgType::SYSTEM_HEART_TICK:
		{
			rmsg.m_nType = MsgType::SYSTEM_HEART_TICK;

            rmsg.m_nSrcObject = m_nGuid;
			rmsg.m_nMsgLength = 0;
			rmsg.m_pMsgData = 0;
            EnqueMsg(&rmsg);
		}
		break;
    case MsgType::USER_SEARCH_ALL_ROLE:          // 查询所有角色
        {
            rmsg.m_nType = MsgType::USER_SEARCH_ALL_ROLE;
        }
        break;
    case MsgType::USER_ADD_ROLE:                 // 新添角色
        {
            rmsg.m_nType = MsgType::USER_ADD_ROLE;
        }
        break;
    case MsgType::USER_DELETE_ROLE:              // 删除角色
        {
            rmsg.m_nType = MsgType::USER_DELETE_ROLE;
        }
        break;
    case MsgType::USER_ENTER_GAME:               // 选定角色进入游戏
        {
            rmsg.m_nSrcObject = m_nGuid;
			rmsg.m_nMsgLength = sizeof(int);
            rmsg.m_pMsgData = new char[rmsg.m_nMsgLength];
			int nServerID = UniG_SessionMgr::Instance()->GetAGS();
            memcpy(rmsg.m_pMsgData, &nServerID, sizeof(int));
            rmsg.m_nType = MsgType::USER_ENTER_GAME;

            EnqueMsg(&rmsg);
        }
        break;
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
		ProxyMessage* pPM = (ProxyMessage*)msgList[i];

        pProxy = pProxyManager->GetProxy(pPM->m_nProxyID);
        if(pProxy == NULL)
        {
            return false;
        }
        msgList[i]->Write(pProxy->Stream());
		printf("Send to Proxy\n");
    }
	PopBatch(msgList.size());
    return true;
}
