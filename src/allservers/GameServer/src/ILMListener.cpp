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
    printf("---Server ID:%d ----------\n", response.m_oServerInfo.nServerID);
    
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
    int nTotalHandled = 0;
    while(true)
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
        BasePacket pkt(pMsgData, nLength, true);
        Handle(pkt);
        nTotalHandled++;
    }
    return nTotalHandled;
}

int ILMObject::OnError()
{
    return 0;
}

int ILMObject::Handle(BasePacket& pkt)
{
    switch (BaseMessage::Type(&pkt))
    {
        case MsgType::LOCK_MAP_REQUEST:
        {
            cout << "----Lock map message" << endl;
            ServerLockMapRequest* pMsg = new ServerLockMapRequest;
            pMsg->Unpack(&pkt);
            Handle(pMsg);
            break;
        }
        case MsgType::UNLOCK_MAP_REQUEST:
        {
            cout << "----UnLock map message" << endl;
            ServerUnlockMapRequest *pMsg = new ServerUnlockMapRequest;
            pMsg->Unpack(&pkt);
            Handle(pMsg);
            break;
        }
        case MsgType::UPDATE_MST_REQUEST:
        {
            cout << "----Update MST message" << endl;
            UpdateMSTRequestMsg* pMsg = new UpdateMSTRequestMsg;
            pMsg->Unpack(&pkt);
            Handle(pMsg);
            break;
        }
        case MsgType::SERVER_STATUS_QUERY:
        {
            //cout << "----Query server status" << endl;
            ServerStatusQueryMsg* pMsg = new ServerStatusQueryMsg;
            pMsg->Unpack(&pkt);
            Handle(pMsg);
            break;
        }
        case MsgType::START_PARTITION_REQUEST:
        {
            cout << "----Start Partition request  message" << endl;
            StartPartitionRequestMsg* pMsg = new StartPartitionRequestMsg;
            pMsg->Unpack(&pkt);
            Handle(pMsg);
            break;
        }  
        case MsgType::UNDO_START_PARTITION_REQUEST:
        {
            cout << "----Undo partition request" << endl;
            StartPartitionRequestMsg* pMsg = new StartPartitionRequestMsg;
            pMsg->Unpack(&pkt);
            Handle(pMsg);
            break;
        }                          
        case MsgType::SERVER_LEAVE_REQUEST:
        {
            cout << "----Server leave message" << endl;
            ServerLeaveRequest *pMsg = new ServerLeaveRequest;
            pMsg->Unpack(&pkt);
            Handle(pMsg);   
            break;
        }          
        default:
        {
            cout << "----ILMObject::execute\n"
                "Unkown message type: " << BaseMessage::Type(&pkt) << endl;
            break;
        }
    }
    return 1;
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
    UpdateMSTResponseMsg updateResponse;
    updateResponse.m_nResult = 1;
    BasePacket pkt;
    updateResponse.Pack(&pkt);
    MsgForward(pkt);
    delete pMsg;
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
    SeamlessService& ss = SeamlessService::Instance();

    ServerStatusReplyMsg* pQueryStatusReplyMsg = new  ServerStatusReplyMsg;

    vector<Map2SrvInfo>& possessMST = ss.MST();
    pQueryStatusReplyMsg->m_nMapNum = possessMST.size();
    MapInfo mapInfo;
    for(int i=0; i< pQueryStatusReplyMsg->m_nMapNum; i++)
    {
        mapInfo.nMapID = possessMST[i].nMapID;
        pQueryStatusReplyMsg->m_vecMaps.push_back(mapInfo);
    }
    pQueryStatusReplyMsg->m_pPlayerNum = 0;
    pQueryStatusReplyMsg->m_nCpuLoad = 0;
    //printf("**************query server status reply\n");
    if (pQueryStatusReplyMsg->m_nMapNum)
    {
        // transfer this message to main thread , and wait return
        UniG_MessageQueue::Instance().ILMInQueue().Push(pQueryStatusReplyMsg);
        if(!UniG_MessageQueue::Instance().ILMOutQueue().Size())
        {
            UniG_MessageQueue::Instance().ILMOutQueue().RequestIncoming();
        }
        pQueryStatusReplyMsg = (ServerStatusReplyMsg*)UniG_MessageQueue::Instance().ILMOutQueue().Front();

    
        // 这里根据上边的结果把resp的m_nCpuLoad和m_nPlayerNum添好
        for(int i=0; i< pQueryStatusReplyMsg->m_nMapNum; i++)
        {
            pQueryStatusReplyMsg->m_pPlayerNum += pQueryStatusReplyMsg->m_vecMaps[i].nPlayerNum;
        }
        BasePacket pkt;
        pQueryStatusReplyMsg->Pack(&pkt);
        //printf("**************query server status reply success\n");
        MsgForward(pkt);
        UniG_MessageQueue::Instance().ILMOutQueue().Pop();
       
    }
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
    if (pMsg->m_nType == MsgType::UNDO_START_PARTITION_REQUEST)
    {
        return UndoMigration(pMsg);
    }

    SeamlessService& ss = SeamlessService::Instance();

    // judge whether accept or transfer
    if (pMsg->m_nServerFrom == ss.GetServerInfo().nServerID)
    {
        SeamlessService::m_nIsThisSrvDest = 0;
        return TransferMigrators(pMsg);
    }
    else if (pMsg->m_nServerTo == ss.GetServerInfo().nServerID)
    {
        SeamlessService::m_nIsThisSrvDest = 1;
        return AcceptMigrators(pMsg);
    }
    return 0;
}

int ILMObject::TransferMigrators(StartPartitionRequestMsg* pMsg)
{
    int nDestServer = pMsg->m_nServerTo;
    SeamlessService& ss = SeamlessService::Instance();
    MSTInfo& mst = MSTInfo::Instance();

    // Make partition data and  send it to destination server
    StartPartitionResponseMsg resp;
    resp.m_nResult = 0; 

    // tell the main thread to packet the region msg
    PackMapsRequest* pPackMapRequest = new PackMapsRequest;
    pPackMapRequest->m_nPackageMapNum = pMsg->m_nMapNum;
    pPackMapRequest->m_vecMapList = pMsg->m_vecMaps;
    delete pMsg;

    // transfer the msg to the main thread
    UniG_MessageQueue::Instance().ILMInQueue().Push(pPackMapRequest);

    PackMapsResponse* pPackMapResp;
    
    if(!UniG_MessageQueue::Instance().ILMOutQueue().Size())
    {
        UniG_MessageQueue::Instance().ILMOutQueue().RequestIncoming();
    }
    pPackMapResp = (PackMapsResponse*)UniG_MessageQueue::Instance().ILMOutQueue().Front();

    BasePacket pkt;
    pPackMapResp->Pack(&pkt);

    UniG_MessageQueue::Instance().ILMOutQueue().Pop();
    try
    {
        ServerContextMgr::Instance()->SendBlock(nDestServer, pkt.Data(), pkt.Length());
    }
    catch (exception& e)
    {
        cout << "exception: e" << e.what() << endl
            << "\tat ILMObject::transferMigrators"
            << SrcLocation(__FILE__, __LINE__) << endl;        
        BasePacket pkt;
        resp.Pack(&pkt);
        MsgForward(pkt);
        return -1;
    }
    // read the result from peer server.

    void* result = 0;
    uint32 length;
    try
    {        
        ServerContextMgr::Instance()->RecvBlock(nDestServer, result, length);
    }
    catch (exception& e)
    {
        cout << "exception: e" << e.what() << endl
            << "\tat ILMObject::transferMigrators"
            << SrcLocation(__FILE__, __LINE__) << endl;

        BasePacket pkt;
        resp.Pack(&pkt);
        MsgForward(pkt);
        
        if(result)
        {
            delete result;  
        }
        return -1;
    }
    
    // tell the ILM server successful
    if (*(int*)result)
    {
        resp.m_nResult = 1;
        BasePacket pkt;
        resp.Pack(&pkt);
        MsgForward(pkt);
        delete[] result;  

    }
    else
    {
        resp.m_nResult = 0;
        BasePacket pkt;
        resp.Pack(&pkt);
        MsgForward(pkt);
        delete result;  
    }
    return 0;
}


int ILMObject::AcceptMigrators(StartPartitionRequestMsg* pMsg)
{
    uint32 length;
    void* block;
    ServerContextMgr::Instance()->RecvBlock(pMsg->m_nServerFrom, block, length);
    
    BasePacket pkt(block, length);
    UnpackMapsRequest* pUnpackMap = new UnpackMapsRequest;
    pUnpackMap->Unpack(&pkt);
    pUnpackMap->m_nType = MsgType::UNPACK_MAP_REQUEST;
    UniG_MessageQueue::Instance().ILMInQueue().Push(pUnpackMap);
    if(!UniG_MessageQueue::Instance().ILMOutQueue().Size())
    {
        UniG_MessageQueue::Instance().ILMOutQueue().RequestIncoming();
    }
    UnpackMapsResponse* pUnpackResponse = (UnpackMapsResponse*)UniG_MessageQueue::Instance().ILMOutQueue().Front();

    int result = pUnpackResponse->m_nResult;
   
    ServerContextMgr::Instance()->SendBlock(pMsg->m_nServerFrom, &result, sizeof(result));
    UniG_MessageQueue::Instance().ILMOutQueue().Pop();
    delete pMsg;
    return 0;
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
int ILMObject::Handle(ServerLeaveRequest* pMsg)
{
    //cout << "----ServerEngine\tILM requires to stop" << endl;
    //ServerContextMgr::Instance()->Deinit();    
    //cout << "----ServerEngine\tServerContextMgr deinited" << endl;

    //ILMObject::instance().stop();    
    //kill(getpid(), SIGQUIT); 
    delete pMsg;
    return 0;
}

int ILMObject::Handle(ServerLockMapRequest* pMsg)
{
    UniG_MessageQueue::Instance().ILMInQueue().Push(pMsg);
    if(UniG_MessageQueue::Instance().ILMOutQueue().Size() == 0)
    {
        UniG_MessageQueue::Instance().ILMOutQueue().RequestIncoming();
    }
    ServerLockMapResponse* pResponse = (ServerLockMapResponse*)(UniG_MessageQueue::Instance().ILMOutQueue().Front());
    BasePacket pkt;
    pResponse->Pack(&pkt);
    UniG_MessageQueue::Instance().ILMOutQueue().Pop();
    MsgForward(pkt);
    return 0;
}

int ILMObject::Handle(ServerUnlockMapRequest* pMsg)
{
    UniG_MessageQueue::Instance().ILMInQueue().Push(pMsg);
    if(UniG_MessageQueue::Instance().ILMOutQueue().Size() == 0)
    {
        UniG_MessageQueue::Instance().ILMOutQueue().RequestIncoming();
    }
    ServerUnlockMapResponse* pResponse = (ServerUnlockMapResponse*)(UniG_MessageQueue::Instance().ILMOutQueue().Front());
    BasePacket pkt;
    pResponse->Pack(&pkt);
    UniG_MessageQueue::Instance().ILMOutQueue().Pop();
    MsgForward(pkt);
    return 0;
}

void ILMObject::MsgForward(BasePacket& pkt)
{
    int nLength = pkt.Length();
    Write(&nLength, sizeof(nLength));
    Write(pkt.Data(), nLength);
}

