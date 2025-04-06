#include "UniG.h"


ILMListener::ILMListener(ILMArgs &args)
{
    Socket* pSocket = new Socket(args.nILMIp, args.nILMPort);
    pSocket->ConfigureBlocking(false);
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
            m_oReadBuffer.Skip(sizeof(nLength));
            char* pMsgData = new char[nLength];
            memset(pMsgData, 0, nLength);
            m_oReadBuffer.Read(pMsgData, nLength, true);
            BasePacket pkg(pMsgData, nLength, true);

            switch (BaseMessage::Type(&pkg))
            {
            case MsgType::LOCK_MAP_REQUEST:
            {
                cout << "----Lock map message" << endl;
                ServerLockMapRequest msg;
                msg.Unpack(&pkg);
                Handle(&msg);
                break;
            }
            case MsgType::UNLOCK_MAP_REQUEST:
            {
                cout << "----Lock map message" << endl;
                ServerUnlockMapRequest msg;
                msg.Unpack(&pkg);
                Handle(&msg);
                break;
            }
            case MsgType::UPDATE_MST_REQUEST:
            {
                cout << "----Update MST message" << endl;
                UpdateMSTRequestMsg msg;
                msg.Unpack(&pkg);
                Handle(&msg);
                break;
            }
            case MsgType::SERVER_STATUS_QUERY:
            {
                cout << "----Query server status" << endl;
                ServerStatusQueryMsg msg;
                msg.Unpack(&pkg);
                //Handle(&msg);
                break;
            }
            case MsgType::START_PARTITION_REQUEST:
            {
                cout << "----Start Partition request  message" << endl;
                StartPartitionRequestMsg msg;
                msg.Unpack(&pkg);
                Handle(&msg);
                break;
            }  
            case MsgType::UNDO_START_PARTITION_REQUEST:
            {
                cout << "----Undo partition request" << endl;
                StartPartitionRequestMsg msg;
                msg.Unpack(&pkg);
                Handle(&msg);
                break;
            }                          
            case MsgType::SERVER_LEAVE_REQUEST:
            {
                cout << "----Server leave message" << endl;
                ServerLeaveRequest msg;
                msg.Unpack(&pkg);
                Handle(&msg);   
                break;
            }          
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
//  Description:  handle the UpdateMSTRequestMsg frsetAvatarsRegeneratedom ILM
//  Parameters:   pMsg: the command from ILM
//                pConn: the connection with ILM
//  Return:       None
//  Exception:    None----ServerEngine
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
int ILMObject::Handle(UpdateMSTRequestMsg* pMsg)
{
    SeamlessService& ss = SeamlessService::Instance();
    MSTInfo& mst = MSTInfo::Instance();
    // setup global MST
    mst.AllMST(pMsg->m_vecMST);
    ss.MST(pMsg->m_vecMST);

    // setup all servers' infomation
    mst.AllSrvs(pMsg->m_vecAllSrvs);

    // 向ILM 发送收到确认消息
    UpdateMSTResponseMsg resp;
    resp.m_nResult = 1;
    MsgForward(resp);
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
    //ServerStatusQueryMsg *pReq = pMsg;
    //cout << "----ServerEngine\treceive status query from ILM" << endl;
    ServerStatusReplyMsg resp;
    ServerStatusReplyMsg *pResp = &resp;
    SeamlessService& ss = SeamlessService::Instance();
    vector<Map2SrvInfo>& possessMST = ss.MST();
    resp.m_nMapNum = possessMST.size();
    resp.m_pPlayerNum = 0;
    resp.m_nCpuLoad = 0;
    if (resp.m_nMapNum)
    {
        // 得到这个Server的负载信息
        // 把消息传递给游戏主线程模块，让它填充数据
        UniG_MessageQueue::Instance().SeamlessInQueue().Push(pResp);
        if(!UniG_MessageQueue::Instance().SeamlessOutQueue().Size())
        {
            UniG_MessageQueue::Instance().SeamlessOutQueue().RequestIncoming();
        }
        pResp = (ServerStatusReplyMsg*)UniG_MessageQueue::Instance().SeamlessOutQueue().Front();
        UniG_MessageQueue::Instance().SeamlessOutQueue().Pop();
    }
    
    // 这里根据上边的结果把resp的m_nCpuLoad和m_nPlayerNum添好
    for(int i=0; i< resp.m_nMapNum; i++)
    {
        resp.m_pPlayerNum += resp.m_vecMaps[i].nPlayerNum;
    }

    MsgForward(resp);
    return 0;
}
//---------------------------------------------------------------------------
//  ILMObject::Handle
//---------------------------------------------------------------------------
//  Description:  handle the StartPartitionRequestMsg from ILM
//  Parameters:   pMsg: the command from ILM
//                pConn: the connection with ILM
//  Return:       None
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
int ILMObject::Handle(StartPartitionRequestMsg* pMsg)
{
    // 搬家命令取消,这块应该可以不作了
    // TODO 考虑一下
    if ((int)(pMsg->m_nType) == (int)(MsgType::UNDO_START_PARTITION_REQUEST))
    {
        return UndoMigration(pMsg);
    }

    StartPartitionRequestMsg *pReq = pMsg;
    SeamlessService& ss = SeamlessService::Instance();

    // judge whether accept or transfer
    if (pReq->m_nServerFrom == ss.GetServerInfo().nServerID)
    {
        return TransferMigrators(pMsg);
    }
    else if (pReq->m_nServerTo == ss.GetServerInfo().nServerID)
    {
        return AcceptMigrators(pMsg);
    }
    return 0;
}

int ILMObject::TransferMigrators(StartPartitionRequestMsg* pReq)
{
    SeamlessService& ss = SeamlessService::Instance();
    MSTInfo& mst = MSTInfo::Instance();

    // Make partition data and  send it to destination server
    StartPartitionResponseMsg resp;
    resp.m_nResult = 0; 

    // 打包消息，传递给主线程处理
    PackMapsRequest packRequest;
    packRequest.m_nPackageMapNum = pReq->m_nMapNum;

    packRequest.m_vecMapList = pReq->m_vecMaps;
    // 主线程的处理结果,内存由主线程申请，这边释放
    PackMapsResponse* pPackMapResp;
    UniG_MessageQueue::Instance().SeamlessInQueue().Push(&packRequest);
    if(!UniG_MessageQueue::Instance().SeamlessOutQueue().Size())
    {
        UniG_MessageQueue::Instance().SeamlessOutQueue().RequestIncoming();
    }
    pPackMapResp = (PackMapsResponse*)UniG_MessageQueue::Instance().SeamlessOutQueue().Front();
    UniG_MessageQueue::Instance().SeamlessOutQueue().Pop();

    // 把数据打成一个包
    BasePacket pkt;
    pPackMapResp->Pack(&pkt);
    try
    {
        ServerContextMgr::Instance()->SendBlock(
            pReq->m_nServerTo, 
            pkt.Data(),
            pkt.Length());
    }
    catch (exception& e)
    {
        cout << "exception: e" << e.what() << endl
            << "\tat ILMObject::transferMigrators"
            << SrcLocation(__FILE__, __LINE__) << endl;        
        MsgForward(resp);
        return -1;
    }
    // 读从相邻server的处理结果
    void* result;
    uint32 length;
    try
    {        
        ServerContextMgr::Instance()->RecvBlock(pReq->m_nServerTo, result, length);
    }
    catch (exception& e)
    {
        cout << "exception: e" << e.what() << endl
            << "\tat ILMObject::transferMigrators"
            << SrcLocation(__FILE__, __LINE__) << endl;
        MsgForward(resp);
        return -1;
    }
    
    // 如果正确则告诉ILM 搬家成功了
    if (*(int*)result)
    {
        resp.m_nResult = 1;   // Success
        MsgForward(resp);
    }
    else
    {
        MsgForward(resp);
    }
    free(result);  
    return 0;
}


int ILMObject::AcceptMigrators(StartPartitionRequestMsg* pReq)
{
    uint32 length;
    void* block;
    // 阻塞式收一个包
    // 包式4字节的长度开始，然后紧跟着始对应长度的数据
    ServerContextMgr::Instance()->RecvBlock( pReq->m_nServerFrom, 
        block, 
        length);
    
    // 构成一个包，然后构成消息，发给主线程
    BasePacket pkt(block, length);
    UnpackMapsRequest unpackRequest;
    unpackRequest.Unpack(&pkt);

    UniG_MessageQueue::Instance().SeamlessInQueue().Push(&unpackRequest);
    if(!UniG_MessageQueue::Instance().SeamlessOutQueue().Size())
    {
        UniG_MessageQueue::Instance().SeamlessOutQueue().RequestIncoming();
    }
    UnpackMapsResponse* pUnpackResponse = (UnpackMapsResponse*)UniG_MessageQueue::Instance().SeamlessOutQueue().Front();
    UniG_MessageQueue::Instance().SeamlessOutQueue().Pop();
    // 如果成功接收这片地图，告诉搬出Server

    SeamlessService& ss = SeamlessService::Instance();
    int result = pUnpackResponse->m_nResult;
   
    ServerContextMgr::Instance()->SendBlock(pReq->m_nServerFrom,
            &result,
            sizeof(result));
    return 0;
}

void ILMObject::MigrationResponse(StartPartitionResponseMsg& msg)
{
    //cout << "Engine\tgot response: " << *(int*)result << endl;
    if (msg.m_nResult)
    {
        // change server id in all clients
        //cout << "Engine\tchange server: " << endl;
        //cout << "Engine\tdelete players" << endl;
        //char* mapName = MSTInfo::Instance().GetMapInfo(msg.m_nMapMoved).szName;
        //ServerEngine::instance().logicIntf().
        //    releaseMapTo_FP(mapName, msg._serverTo);
        //logIt();
    }

    MsgForward(msg);
    //cout << "Engine\tpartition succeeded." << endl;    
}

int ILMObject::UndoMigration(StartPartitionRequestMsg* pReq)
{
    //return;
    //ServerEngine& engine = ServerEngine::instance();

    //char* mapName = MSTInfo::instance().mapInfo(pReq->_maps[0])._name;
    //engine.logicIntf().undoMigration_FP(mapName);
    return 0;
}


//---------------------------------------------------------------------------
//  ILMObject::Handle
//---------------------------------------------------------------------------
//  Description:  handle the ServerLeaveRequest from ILM
//  Parameters:   pMsg: the command from ILM
//                pConn: the connection with ILM
//  Return:       None
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
int ILMObject::Handle(ServerLeaveRequest* pReq)
{
    //cout << "----ServerEngine\tILM requires to stop" << endl;
    ServerContextMgr::Instance()->Deinit();    
    //cout << "----ServerEngine\tServerContextMgr deinited" << endl;

    //ILMObject::instance().stop();    
    //kill(getpid(), SIGQUIT); 
    return 0;
}

int ILMObject::Handle(ServerLockMapRequest* pMsg)
{
    UniG_MessageQueue::Instance().SeamlessInQueue().Push(pMsg);
    if(UniG_MessageQueue::Instance().SeamlessOutQueue().Size() == 0)
    {
        UniG_MessageQueue::Instance().SeamlessOutQueue().RequestIncoming();
    }
    ServerLockMapResponse* pResponse = (ServerLockMapResponse*)(UniG_MessageQueue::Instance().SeamlessOutQueue().Front());
    UniG_MessageQueue::Instance().SeamlessOutQueue().Pop();
    MsgForward(*pResponse);
    return 0;
}

int ILMObject::Handle(ServerUnlockMapRequest* pMsg)
{
    UniG_MessageQueue::Instance().SeamlessInQueue().Push(pMsg);
    if(UniG_MessageQueue::Instance().SeamlessOutQueue().Size() == 0)
    {
        UniG_MessageQueue::Instance().SeamlessOutQueue().RequestIncoming();
    }
    ServerUnlockMapResponse* pResponse = (ServerUnlockMapResponse*)(UniG_MessageQueue::Instance().SeamlessOutQueue().Front());
    UniG_MessageQueue::Instance().SeamlessOutQueue().Pop();
    MsgForward(*pResponse);
    return 0;
}

void ILMObject::MsgForward(BaseMessage& msg)
{
    //Lock lock(*this);
    msg.Write(Stream());
}

