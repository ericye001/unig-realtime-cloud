#include "UniG.h"


ILMListener::ILMListener(ILMArgs &args)
{
    Socket* pSocket = new Socket(args.nILMIp, args.nILMPort);
    m_pEpoll = new Epoll();
    ILMObject* pILMObj = new ILMObject(pSocket);
    pILMObj->Init(args);
    m_pEpoll->Add(pILMObj);
    m_bStopFlag = false;
}

ILMListener::~ILMListener()
{

}


void ILMListener::Stop()
{
    m_bStopFlag = true;
}

void ILMListener::run()
{
    while (true)
    {
        m_pEpoll->Wait(100);
        if (m_bStopFlag)
        {
            return;
        }
        Thread::sleep(1);
    }
}



//---------------------------------------------------------------------------
//  ILMObject::ILMObject
//---------------------------------------------------------------------------
//  Description:  Constructure, initialize some fields
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
ILMObject::ILMObject(Socket* pSocket)
: SocketHandler(pSocket)
{

}

ILMObject::~ILMObject()
{

}

void ILMObject::Init(ILMArgs& args)
{
    if (!m_pSocket)
    {
        throw StrException(
            string("Fail to new Socket\n\n"
            "\tat ILMObject::init")
            +SrcLocation(__FILE__, __LINE__));
    }
    // 填充向ILM请求下信息，转发给ILM

    ServerConnectILMRequestMsg request;

    request.m_oServerInfo.nIP = args.nSrvIp;
    request.m_oServerInfo.nPort = args.nSrvPort;

	request.Write(Stream());

    ServerConnectILMResponseMsg response;
    response.Read(Stream());

    if (!response.m_nResult)
    {
        throw StrException(
            string("Fail to connect with ILM\n"
            "\tat ILMObject::Init")+
            SrcLocation(__FILE__, __LINE__));
    }

    // 解析从ILM Server返回的结果
    SeamlessService& service = SeamlessService::Instance();

    service.MaxAsyncMsg(response.m_nAsyncMaxMsgNum);
    service.AsyncBatchNum(response.m_nAsyncBatchNum);
    service.MaxMsgLength(response.m_nMaxMsgLength);
    service.MaxServerNum(response.m_nMaxServerNum);
    service.SetServerInfo(response.m_oServerInfo);

    // TODO 通知ILM加载地图信息
}


//---------------------------------------------------------------------------
//  ILMObject::OnRead
//---------------------------------------------------------------------------
//  Description:  response the command from ILM, called by Epoll
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
int ILMObject::OnRead()
{
    while(true)
    {   
        try
        {

            // is there a complete packet?
            // BasePacket的长度用4个字节表示
            uint32 nLength;
            if(m_oReadBuffer.Size() <= sizeof(nLength))
            {
                // there's no a complete packte
                return 0;
            }
            m_oReadBuffer.Read(&nLength, sizeof(nLength), false);
            if(m_oReadBuffer.Size() < (int)nLength+sizeof(nLength))
            {
                // there's no a complete packte
                return 0;
            }
            // 把长度数据跳过
			m_oReadBuffer.Read(&nLength, sizeof(nLength), true);
            char* pMsgData = new char[nLength];
            memset(pMsgData, 0, nLength);
            m_oReadBuffer.Read(pMsgData, nLength, true);
            BasePacket pkg(pMsgData, nLength, true);

            switch (BaseMessage::Type(&pkg))
            {
            case MsgType::SERVER_STATUS_QUERY:
                {
                    cout << "----Query server status" << endl;
                    ServerStatusQueryMsg msg;
                    msg.Unpack(&pkg);
                    Handle(&msg);
                    break;
                }
			case MsgType::REMOVE_SERVER_REQUEST:			// temp code(gs remove)
				{
					ServerLeaveRequest msg;
					msg.Unpack(&pkg);
					UniG_SessionMgr::Instance()->RemoveGS(msg.m_nServerId);					
				}
				break;
            default:
                {
                    cout << "----ILMObject::execute\n"
                        "Unkown message type: " << BaseMessage::Type(&pkg) << endl;
                    break;
                }
            }
        }
        catch (exception& e)
        {
            cout << "exception: " << e.what() << endl
                << "\tat ILMObject::execute" 
                << SrcLocation(__FILE__, __LINE__) << endl;
            throw e;
        }
        return 1;
    }

}

int ILMObject::OnError()
{
    return 0;
}


//---------------------------------------------------------------------------
//  ILMObject::Handle
//---------------------------------------------------------------------------
//  Description:  handle the ServerStatusQueryMsg from ILM
//  Parameters:   pMsg: the command from ILM
//                pConn: the connection with ILM
//  Return:       None
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
int ILMObject::Handle(ServerStatusQueryMsg* pMsg)
{
    ////ServerStatusQueryMsg *pReq = pMsg;
    ////cout << "----ServerEngine\treceive status query from ILM" << endl;
    //ServerStatusReplyMsg resp;
    //ServerStatusReplyMsg *pResp = &resp;
    //SeamlessService& ss = SeamlessService::Instance();
    //vector<Map2SrvInfo>& possessMST = ss.MST();
    //resp.m_nMapNum = possessMST.size();
    //resp.m_pPlayerNum = 0;
    //resp.m_nCpuLoad = 0;
    //resp.m_nSyn = 0;
    //resp.m_nExt = 0;
    //if (resp.m_nMapNum)
    //{
    //    // 得到这个Server的负载信息
    //    // 把消息传递给游戏主线程模块，让它填充数据
    //    UniG_MessageQueue::Instance().SeamlessInQueue().Push(pResp);
    //    if(!UniG_MessageQueue::Instance().SeamlessOutQueue().Size())
    //    {
    //        UniG_MessageQueue::Instance().SeamlessOutQueue().RequestIncoming();
    //    }
    //    pResp = (ServerStatusReplyMsg*)UniG_MessageQueue::Instance().SeamlessOutQueue().Front();
    //    UniG_MessageQueue::Instance().SeamlessOutQueue().Pop();
    //}
    //
    //// 这里根据上边的结果把resp的m_nCpuLoad和m_nPlayerNum添好
    //for(int i=0; i< resp.m_nMapNum; i++)
    //{
    //    resp.m_pPlayerNum += resp.m_vecMaps[i].nPlayerNum;
    //}

    //MsgForward(resp);
    return 0;
}

void ILMObject::MsgForward(BaseMessage& msg)
{
    //Lock lock(*this);
    msg.Write(Stream());
}



