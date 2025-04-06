#include "UniG.h"

ServerContextMgr* ServerContextMgr::m_pInstance = 0;
bool ServerContextMgr::m_bHasInit = false;
Mutex ServerContextMgr::m_oClassMutex;
Condition ServerContextMgr::m_oClassCondition(m_oClassMutex);

void ServerContextMgr::Init(ServerContextArgs& args)
{
    if (!m_pInstance)
    { 
        Guard<Mutex> g(m_oClassMutex);
        if (m_pInstance)
        {
            return;
        }
        try
        {
            m_pInstance = new ServerContextMgr(args);
            Thread* pMsgThread = new Thread(m_pInstance->m_pMsgThread);
            Thread* pBlockThread = new Thread(m_pInstance->m_pBlockThread);
        }
        catch (exception& e)
        {
            throw SocketException(
                string(e.what())+"\n"+
                "\tat ServerContextMgr::init"+
                SrcLocation(__FILE__, __LINE__));
        }
        UNIG_ASSERT(m_pInstance, "create a ServerContextMgr instance");         
    }
}
        
void ServerContextMgr::Deinit()
{
    if (m_pInstance)
    {
        delete m_pInstance;
    }    
}

ServerContextMgr* ServerContextMgr::Instance()
{
    if (m_pInstance)
    {
        return m_pInstance;
    }
   // UNIG_ASSERT(m_pInstance, "the ServerContextMgr instance is null");
    return NULL;
}

//---------------------------------------------------------------------------
ServerContextMgr::ServerContextMgr(ServerContextArgs& args)
    : m_oArgs(args)
{

    m_pMsgEpoll = new Epoll();
    m_pBlockEpoll = new Epoll();
    if(!m_pMsgEpoll || !m_pBlockEpoll)
    {
        throw EpollException("Create epoll in ServerContextMgr::ServerContextMgr\n");
    }

    // listen to the adjoining server to connect
    ServerSocket* pMsgSocket = new ServerSocket(args.serverInfo.nIP, args.serverInfo.nMsgBusMsgPort);
    ServerSocket* pBlockSocket = new ServerSocket(args.serverInfo.nIP, args.serverInfo.nMsgBusMsgPort+1);
    assert(pMsgSocket);
    assert(pBlockSocket);

    MsgDataListener* pMsgAcceptor = new MsgDataListener(pMsgSocket);
    BlockDataListener* pBlockAcceptor = new BlockDataListener(pBlockSocket);
    if (!pMsgAcceptor || !pBlockAcceptor)
    {
        delete pMsgAcceptor;
        delete pBlockAcceptor;
        throw SocketException(
            string(   "Fail to create ServerContextAcceptor\n"
                        "\tat ServerContextMgr::ServerContextMgr")+
            SrcLocation(__FILE__, __LINE__));
    }

    // it doesn't matter anything whether add pAcceptor to m_pMsgEpoll or m_pBlockEpoll
    m_pMsgEpoll->Add(pMsgAcceptor);

    m_pMsgThread = new MsgThread;
    if (!m_pMsgThread)
    {
        m_pMsgEpoll->Remove(pMsgAcceptor);
        delete pMsgAcceptor;
        UNIG_ASSERT(m_pMsgThread, "error in generate a msg thread");
    }
    
    m_pBlockEpoll->Add(pBlockAcceptor);
    m_pBlockThread = new BlockThread;
    
    if (!m_pBlockThread)
    {
        m_pMsgEpoll->Remove(pBlockAcceptor);
        delete pBlockAcceptor;
        UNIG_ASSERT(m_pBlockThread, "error in generate a msg thread");
    }
    m_oClassCondition.signal();
    m_bHasInit = true;
}

ServerContextMgr::~ServerContextMgr()
{
    // Release resource
    map<int, ServerContext*>::iterator curIt = m_mapServers.begin();
    map<int, ServerContext*>::iterator endIt = m_mapServers.end();
    for ( ; curIt != endIt; ++curIt)
    {
        delete curIt->second;
    }
    m_pMsgThread->Stop(); 
    m_pBlockThread->Stop();
    m_pMsgEpoll->Clear();
    m_pBlockEpoll->Clear();
    delete m_pMsgEpoll;
    delete m_pBlockEpoll;
}


void ServerContextMgr::SetupConnection(ServerInfo& server)
{       
    if(!ServerContextMgr::m_bHasInit)
    {
        m_oClassCondition.wait();
    }
    // If the peer server which will be connected with is higher priority
    // than this server, then return(lower server id means higher priority).
    // We only try to connect with the server with lower priority to assume
    // each connection is from higher priorty to lower priority, for example,
    // from id(1) to id(2) is allowed, but from id(2) to (1) is prohibitted.
    if (server.nServerID <= m_oArgs.serverInfo.nServerID)
    {
        // From lower priority to higher priority or tring to connect with
        // itself
    	return;
	}
	
    // Get the server context(find existing or create new one).
    ServerContext* pSrvCtx = GetSrvCtx(server);
    pSrvCtx->SetupConnection();
}

void ServerContextMgr::DestroyConnection(ServerInfo& server)
{
    RemoveSrvCtx(server);    
}

int ServerContextMgr::SendBlock(uint16 srvId, void* pData, uint32 length)
{
    ServerContext* pSrvCtx = FindSrvCtx(srvId);
    UNIG_ASSERT(pSrvCtx, "Can't find peer server by id");
    pSrvCtx->SendBlock(pData, length);
    return 0;
}

void ServerContextMgr::RecvBlock(uint16 srvId, void*& pData, uint32& length)
{
    ServerContext* pSrvCtx = FindSrvCtx(srvId);
    UNIG_ASSERT(pSrvCtx, "Can't find peer server by id");
    pSrvCtx->RecvBlock(pData, length);    
}

int ServerContextMgr::SendMsg(uint16 srvId, void* pData, uint32 length)
{
    ServerContext* pSrvCtx = FindSrvCtx(srvId);
    UNIG_ASSERT(pSrvCtx, "Can't find peer server by id");
    pSrvCtx->SendMsg(pData, length);
    return 0;
}
    
ServerContext* ServerContextMgr::GetSrvCtx(ServerInfo& item)
{   
    Guard<Mutex> g(m_oClassMutex);
    map<int, ServerContext*>::iterator gotIt;
    gotIt = m_mapServers.find(item.nServerID);
    if (gotIt != m_mapServers.end())
    {
        return gotIt->second;
    }
     
    ServerContext* pSrvCtx = CreateSrvCtx(item);
    m_mapServers.insert(make_pair(item.nServerID, pSrvCtx));
    return pSrvCtx;        
}
ServerContext* ServerContextMgr::FindSrvCtx(uint16 srvId)
{
    Guard<Mutex> g(m_oClassMutex);
    map<int, ServerContext*>::iterator gotIt;
    gotIt = m_mapServers.find(srvId);
    if (gotIt != m_mapServers.end())
    {
        return gotIt->second;
    }
    return NULL;
}

ServerContext* ServerContextMgr::CreateSrvCtx(ServerInfo& item)
{
    ServerContext* pSrvCtx = new ServerContext(item);
    UNIG_ASSERT(pSrvCtx, "Can't create connection to peer server");
    
    return pSrvCtx;
}

void ServerContextMgr::RemoveSrvCtx(ServerInfo& item)
{
    Guard<Mutex> g(m_oClassMutex);
    map<int, ServerContext*>::iterator gotIt;
    gotIt = m_mapServers.find(item.nServerID);
    if (gotIt != m_mapServers.end())
    {
        delete gotIt->second;
        m_mapServers.erase(gotIt);
    }
}


Epoll* ServerContextMgr::GetMsgEpoll()
{
    Guard<Mutex> g(m_pMsgEpollMutex);
    return m_pMsgEpoll;
}

Epoll* ServerContextMgr::GetBlockEpoll()
{
    Guard<Mutex> g(m_pBlockEpollMutex);
    return m_pBlockEpoll;
}

ServerContextArgs& ServerContextMgr::Args()
{
    return m_oArgs;
}



//*************************************************************************
ServerContext::ServerContext(ServerInfo& item)
: m_oServerInfo(item), m_pMsgConn(0), m_pBlockConn(0),m_oBlockDataCondition(m_oBlockDataMutex)    
{
    
}

ServerContext::~ServerContext()
{
    DestroyConnection();
}

void ServerContext::SetupConnection()
{

    // If the message connection already exists, ignore the
    // connecting attempt.
    if (!GetMsgConn())
    {
        InitMsgConnection();
    }

    // If the block connection already exists, ignore the
    // connecting attempt.
    if (!GetBlockConn())
    {
        InitBlockConnection();
    }
	printf("msg bus set up success\n");
}

void ServerContext::DestroyConnection()
{
    Guard<Mutex> g(m_oMutex);
    ResetBlockConn();
    ServerContextMgr::Instance()->GetBlockEpoll()->Remove(m_pBlockConn);
    ResetMsgConn(); 
    ServerContextMgr::Instance()->GetMsgEpoll()->Remove(m_pMsgConn);
}

void ServerContext::InitMsgConnection()
{
    Socket* pSocket = new Socket(m_oServerInfo.nIP, m_oServerInfo.nMsgBusMsgPort);
    UNIG_ASSERT(pSocket, "can't new a connection to peer server");

    IOStream stream(pSocket->GetHandle(), pSocket->IsBlocking());
    ConnRequest connRequest;
    connRequest.m_nType = MsgType::MSG_CONN_REQUEST;
    connRequest.m_oServerInfo = ServerContextMgr::Instance()->Args().serverInfo;
    connRequest.Write(&stream);

    ConnResponse connResponse;
    connResponse.Read(&stream);
    
    if (connResponse.m_nResult != 1 || connResponse.m_nType != MsgType::MSG_CONN_RESPONSE)
    {
        delete pSocket;
        throw ServerContextException(
            string(   "Setup message connection failed\n"
                      "\tat ServerContext::InitMsgConnection")+
            SrcLocation(__FILE__, __LINE__));
    }
    
    MsgConnHandler* pMsgConn = new MsgConnHandler(pSocket);
    if (!pMsgConn)
    {
        delete pSocket;
        throw ServerContextException(
            string(   "Fail to create MsgConnHandler\n"
                      "\tat ServerContext::InitMsgConnection")+
            SrcLocation(__FILE__, __LINE__));
    }

    pMsgConn->Source(ServerContextMgr::Instance()->Args().serverInfo.nServerID);
    pMsgConn->Target(m_oServerInfo.nServerID, true);
    ServerContextMgr::Instance()->GetMsgEpoll()->Add(pMsgConn);
    SetMsgConn(pMsgConn);   
}


void ServerContext::InitBlockConnection()
{
    Socket* pSocket = new Socket(m_oServerInfo.nIP, m_oServerInfo.nMsgBusMsgPort+1);
    UNIG_ASSERT(pSocket, "can't new a connection to peer server");

    IOStream stream(pSocket->GetHandle(), pSocket->IsBlocking());
    ConnRequest connReqeust;
    connReqeust.m_nType = MsgType::BLOCK_CONN_REQUEST;
    connReqeust.m_oServerInfo = ServerContextMgr::Instance()->Args().serverInfo;
    connReqeust.Write(&stream);

    ConnResponse connResponse;
    connResponse.Read(&stream);
    
    if (connResponse.m_nResult != 1 || connResponse.m_nType != MsgType::BLOCK_CONN_RESPONSE)
    {
        delete pSocket;
        throw ServerContextException(
            string(   "Setup block connection failed\n"
                      "\tat ServerContext::InitBlockConnection")+
            SrcLocation(__FILE__, __LINE__));
    }
    
    BlockConnHandler* pBlockConn = new BlockConnHandler(pSocket);
    if (!pBlockConn)
    {
        delete pSocket;
        throw ServerContextException(
            string(   "Fail to create BlockConHandler \n"
                      "\tat ServerContext::initMsgConnection")+
            SrcLocation(__FILE__, __LINE__));
    }
    pBlockConn->Source(ServerContextMgr::Instance()->Args().serverInfo.nServerID);
    pBlockConn->Target(m_oServerInfo.nServerID, true);
    ServerContextMgr::Instance()->GetBlockEpoll()->Add(pBlockConn);
    SetBlockConn(pBlockConn);   
}


void ServerContext::SendBlock(void *pData, uint32 nLength)
{
    Guard<Mutex> g(m_oWriteMutex);
    if (nLength <= 0)
    {
        return;
    }
    m_pBlockConn->Write(&nLength, sizeof(nLength));
    m_pBlockConn->Write(pData, nLength);
}


void ServerContext::SendMsg(void *pData, uint32 nLength)
{
    Guard<Mutex> g(m_oWriteMutex);
    if (nLength <= 0)
    {
        return;
    }
    m_pMsgConn->Write(&nLength, sizeof(nLength));
    m_pMsgConn->Write(pData, nLength);
}

void ServerContext::RecvBlock(void*& pData, uint32& length)
{
    pData = 0;
    length = 0;
    
    try
    {
        Guard<Mutex> g(m_oBlockDataMutex);
        if (!m_lstBlockDataList.size())
        {
            m_oBlockDataCondition.wait();
        }
        BlockData& blockData = m_lstBlockDataList.front();
        if (!blockData.nLength || blockData.nSize < blockData.nLength)
        {
            m_oBlockDataCondition.wait();
        }
        
        pData = blockData.pData;
        length = blockData.nLength;
        m_lstBlockDataList.pop_front();
        return;
    }
    catch (exception& e)
    {
        throw ServerContextException(
            string(e.what())+"\n"+
            "\tat ServerContext::recvBlock"+ 
            SrcLocation(__FILE__, __LINE__));        
    }
}

MsgConnHandler* ServerContext::GetMsgConn()
{
    Guard<Mutex> g(m_oMutex);
    return m_pMsgConn;
}

void ServerContext::SetMsgConn(MsgConnHandler* pConn)
{
    Guard<Mutex> g(m_oMutex);
    m_pMsgConn = pConn;
}

void ServerContext::ResetMsgConn()
{
    if (m_pMsgConn)
    {
        delete m_pMsgConn;
    }
    
    m_pMsgConn = 0;
}

BlockConnHandler* ServerContext::GetBlockConn()
{
    Guard<Mutex> g(m_oMutex);
    return m_pBlockConn;
}

void ServerContext::SetBlockConn(BlockConnHandler* pConn)
{
    Guard<Mutex> g(m_oMutex);
    m_pBlockConn = pConn;
}

void ServerContext::ResetBlockConn()
{
    if (m_pBlockConn)
    {
        delete m_pBlockConn;
    }
    m_pBlockConn = 0;
}






//---------------------------------------------------------------------------
MsgThread::MsgThread()
    : m_bStopFlag(false)
{
    
}

MsgThread::~MsgThread()
{
    
}

void MsgThread::run()
{
    m_pEpoll = ServerContextMgr::Instance()->GetMsgEpoll();
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

void MsgThread::Stop()
{
    m_bStopFlag = true;
}

//---------------------------------------------------------------------------
BlockThread::BlockThread()
    : m_bStopFlag(false)
{
    
}

BlockThread::~BlockThread()
{
    
}

void BlockThread::run()
{
    m_pEpoll = ServerContextMgr::Instance()->GetBlockEpoll();
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

void BlockThread::Stop()
{
    m_bStopFlag = true;
}

//---------------------------------------------------------------------------

MsgDataListener::MsgDataListener(ServerSocket* pSocket)
: ServerSocketHandler(pSocket, NULL)
{

}

int MsgDataListener::OnRead(Socket* pSocket)
{
    //pSocket->ConfigureBlocking(false);
    
    MsgConnHandler* pMsgSocketHandler = new MsgConnHandler(pSocket);

    UNIG_ASSERT(pMsgSocketHandler, "Fail to create MsgConnHandler");

    m_pEpoll->Add(pMsgSocketHandler);
	printf("msgbus msg socket accept success\n");
    return 1;
}

//////////////////////////////////////////////////////////
BlockDataListener::BlockDataListener(ServerSocket* pSocket)
: ServerSocketHandler(pSocket, NULL)
{

}

int BlockDataListener::OnRead(Socket* pSocket)
{
    pSocket->ConfigureBlocking(false);
    
    BlockConnHandler* pBlockSocketHandler = new BlockConnHandler(pSocket);

    UNIG_ASSERT(pBlockSocketHandler, "Fail to create MsgConnHandler");

    m_pEpoll->Add(pBlockSocketHandler);
	printf("msgbus block socket accept success\n");
    return 1;
}
//    try
//    {
//        IOStream stream(pSocket->GetHandle(), pSocket->IsBlocking());
//        BasePacket pkt;
//        pkt.Read(&stream);
//        uint16 type = BaseMessage::Type(&pkt);
//        if (type == (Command::MSG_CONN_REQUEST))
//        {
//            AcceptMsgConn(pSocket, pkt);            
//        }
//        else if (type == (Command::BLOCK_CONN_REQUEST))
//        {
//            AcceptBlockConn(pSocket, pkt);
//        }
//    }
//    catch (exception& e)
//    {
//        throw ServerContextException(
//            string(e.what())+"\n"+
//            "\tat ServerContextAcceptor::OnRead"+
//            SrcLocation(__FILE__, __LINE__));
//    }
//    return 0;
//}
//
//void ServerContextAcceptor::AcceptMsgConn(Socket* pSocket, BasePacket& pkt)
//{
//    ConnRequest request;
//    request.Unpack(&pkt);
//
//    ServerContextMgr* serverMgr = ServerContextMgr::Instance();
//
//    ServerContext* pSrvCtx = serverMgr->GetSrvCtx(request.m_oServerInfo);
//    MsgConnHandler* pMsgSocketHandler = new MsgConnHandler(pSocket);
//
//    UNIG_ASSERT(pMsgSocketHandler, "Fail to create MsgConnHandler");
//
//    pMsgSocketHandler->Source(request.m_oServerInfo.nServerID, true);
//    pMsgSocketHandler->Target(serverMgr->Args().serverInfo.nServerID);
//    
//    IOStream stream(pSocket->GetHandle(), pSocket->IsBlocking());
//    pSrvCtx->SetMsgConn(pMsgSocketHandler);
//
//    // write the connection response to peer server
//    ConnResponse msgConnResp;
//    msgConnResp.m_nResult = CommandResult::SUCCESS;
//    msgConnResp.Write(&stream);  
//    m_pEpoll->Add(pMsgSocketHandler);
//}
//
//void ServerContextAcceptor::AcceptBlockConn(Socket* pSocket, BasePacket& pkt)
//{
//    ConnRequest blockCmdReq;
//    blockCmdReq.Unpack(&pkt);
//    ServerContextMgr* serverMgr = ServerContextMgr::Instance();
//    
//    ServerContext* pSrvCtx = serverMgr->GetSrvCtx(blockCmdReq.m_oServerInfo);
//    // We want to let BlockConnHandler to observe the block connection's
//    // exception, such as connection broken, but it does not work.
//    // How to do it? ...
//    BlockConnHandler* pBlockSocketHandler = new BlockConnHandler(pSocket);
//    UNIG_ASSERT(pBlockSocketHandler, "Fail to create BlockConnHandler");
//
//    pBlockSocketHandler->Source(blockCmdReq.m_oServerInfo.nServerID, true);
//    pBlockSocketHandler->Target(serverMgr->Args().serverInfo.nServerID);
//
//    IOStream stream(pSocket->GetHandle(), pSocket->IsBlocking());
//    pSrvCtx->SetBlockConn(pBlockSocketHandler);
//
//    ConnResponse blockConnResp;
//    blockConnResp.m_nResult = CommandResult::SUCCESS;
//    blockConnResp.Write(&stream); 
//    m_pEpoll->Add(pBlockSocketHandler);
//}
//---------------------------------------------------------------------------
int MsgConnHandler::OnRead()
{
    int nMessage = 0;
    while(1)
    {
        uint32 nLength = 0;
        if(m_oReadBuffer.Size() < sizeof(nLength))
        {
            return 0;
        }
        // read the package length
        m_oReadBuffer.Read(&nLength, sizeof(nLength), false);
        
        if(m_oReadBuffer.Size() < nLength + sizeof(nLength))
        {
            return 0;
        }
        printf("buffer length:%d, packet length:%d\n", m_oReadBuffer.Size(), nLength+4);
        m_oReadBuffer.Skip(sizeof(nLength));
        char* pPackageData = new char[nLength];
        memset(pPackageData, 0, nLength);
        m_oReadBuffer.Read(pPackageData, nLength, true);

        BasePacket pkt = BasePacket(pPackageData, nLength, true);
        HandleMessage(pkt);
        nMessage++;
    }
    return nMessage;
}


int MsgConnHandler::OnError()
{
    return 0;     
}

int MsgConnHandler::HandleMessage(BasePacket& pkt)
{
    //TODO 处理影子或者实体发过来的消息
    // 注意内存问题，pPkt是外边new出来的
    switch (BaseMessage::Type(&pkt))
    {
        case MsgType::MSG_CONN_REQUEST:
        {
            ConnRequest connReq;
            connReq.Unpack(&pkt);
            ServerContextMgr* serverMgr = ServerContextMgr::Instance();

            ServerContext* pSrvCtx = serverMgr->GetSrvCtx(connReq.m_oServerInfo);
            pSrvCtx->SetMsgConn(this);

            Source(connReq.m_oServerInfo.nServerID, true);
            Target(serverMgr->Args().serverInfo.nServerID);

            ConnResponse connResp;
            connResp.m_nType = MsgType::MSG_CONN_RESPONSE;
            connResp.m_nResult = 1;

            BasePacket pkt;
            connResp.Pack(&pkt);
            uint32 nLength = pkt.Length();

            Write(&nLength, sizeof(nLength));
            Write(pkt.Data(), pkt.Length());
            break;
        }
        case MsgType::AGENT_CREATE_REQUEST:
        {
            AgentCreateMessage* pAddAgent = new AgentCreateMessage;
            pAddAgent->Unpack(&pkt);
            UniG_MessageQueue::Instance().ILMInQueue().Push(pAddAgent);
            break;
        }

        case MsgType::AGENT_DELETE_REQUEST:
        {
            AgentDeleteMessage* pDelAgent = new AgentDeleteMessage;
            pDelAgent->Unpack(&pkt);
            UniG_MessageQueue::Instance().ILMInQueue().Push(pDelAgent);
            break;
        }

        case MsgType::SYN_AGENT_MESSAGE:
        {
            SynAgentMessage* pSynAgent = new SynAgentMessage;
            pSynAgent->Unpack(&pkt);
            UniG_MessageQueue::Instance().ILMInQueue().Push(pSynAgent);
            break;
        }
        case MsgType::SESSION_CREATE_REQUEST:
        {
            SessionCreateMessage* message = new SessionCreateMessage;
            message->Unpack(&pkt);
            UniG_MessageQueue::Instance().ILMInQueue().Push(message);
            break;
        }
        default:
            break;
    }
    return 0;
}

bool MsgConnHandler::HighPriorityThan(MsgConnHandler& conn)
{
    return m_nSource < conn.Source();
}

uint16 MsgConnHandler::Source()
{
    return m_nSource;
}

void MsgConnHandler::Source(uint16 source, bool peer)
{
    m_nSource = source;
    if (peer)
    {
        m_nPeer = m_nSource;
    }
}

uint16 MsgConnHandler::Target()
{
    return m_nTarget;
}

void MsgConnHandler::Target(uint16 target, bool peer)
{
    m_nTarget = target;
    if (peer)
    {
        m_nPeer = m_nTarget;
    }
}

uint16 MsgConnHandler::Peer()
{
    return m_nPeer;
}


int BlockConnHandler::OnRead()
{

    if(!m_bHasInit)
    {
        if(InitBlockConn() == 1)
        m_bHasInit = true;
    }

    ServerContext* pSrvCtx = ServerContextMgr::Instance()->FindSrvCtx(Peer());
    if (!pSrvCtx)
    {
        return -1;
    }
 
    uint32 nDataSize;
    BlockData* pBlockData;
    list<BlockData>& lstBlockData = pSrvCtx->m_lstBlockDataList;
    
    while (m_oReadBuffer.Size())
    {        
        pBlockData = 0;
        nDataSize = 0;
        if (lstBlockData.size())
        {
            list<BlockData>::iterator iter;
            for (iter = lstBlockData.begin(); iter != lstBlockData.end(); iter++)
            {
                if (!iter->nLength || iter->nSize < iter->nLength)
                {
                    // 有不完全的数据包,记录当前iter
                    pBlockData = &(*iter);
                    break;
                }
            } 
        }
        // pBlockData==NULL，所有没有执行上边的if,所以lstBlockData是空
        // 则新建一个BlockData塞到队列里
        if (!pBlockData)
        {
            lstBlockData.push_back(BlockData()); 
            pBlockData = &lstBlockData.back();
            memset(pBlockData, 0, sizeof(BlockData));
        }
        //cout << "packet length: " << blockData->_length << endl;
        if (pBlockData->nLength == 0)
        {
            // 不可能发生的情况
            if (m_oReadBuffer.Size() < sizeof(pBlockData->nLength))
            {
                //cout << "breaked" << endl;
                break;
            }
            // 如果BlockData的长度是0，则一定是先读包头
            m_oReadBuffer.Read(&pBlockData->nLength, sizeof(&pBlockData->nLength),true);

            // attention 
            pBlockData->pData = new char[pBlockData->nLength];
            if (!pBlockData->pData)
            {
                throw ServerContextException(
                    string("Fail to new char[]\n"
                        "\tat BlockConnHandler::execute")+
                    SrcLocation(__FILE__, __LINE__));
            }
        }
        // socket缓冲区长度+包已有长度不够一个包或者够一个包
        nDataSize = m_oReadBuffer.Size() < pBlockData->nLength - pBlockData->nSize ? 
            m_oReadBuffer.Size() : pBlockData->nLength - pBlockData->nSize;
        //cout << "orginal size: " << blockData->_size << endl;
        m_oReadBuffer.Read(pBlockData->pData+pBlockData->nSize, nDataSize, true);
        pBlockData->nSize += nDataSize;    
   
        // 至少有一个完整的包
        if (lstBlockData.size() && 
            lstBlockData.front().nLength &&
            lstBlockData.front().nSize == lstBlockData.front().nLength)
        {
            pSrvCtx->m_oBlockDataCondition.signal();
        }
    }    
    return 0;
}


int BlockConnHandler::OnError()
{
    return 0;
}

int BlockConnHandler::InitBlockConn()
{
    uint32 nLength = 0;
    if(m_oReadBuffer.Size() < sizeof(nLength))
    {
        return 0;
    }
    // read the package length
    m_oReadBuffer.Read(&nLength, sizeof(nLength), false);
    if(m_oReadBuffer.Size() < nLength + sizeof(nLength))
    {
        return 0;
    }
    m_oReadBuffer.Skip(sizeof(nLength));
    char* pPackageData = new char[nLength];
    memset(pPackageData, 0, nLength);
    m_oReadBuffer.Read(pPackageData, nLength, true);

    BasePacket pkt = BasePacket(pPackageData, nLength, true);
    if(BaseMessage::Type(&pkt) == MsgType::BLOCK_CONN_REQUEST)
    {
        ConnRequest connReq;
        connReq.Unpack(&pkt);
        ServerContextMgr* serverMgr = ServerContextMgr::Instance();

        ServerContext* pSrvCtx = serverMgr->GetSrvCtx(connReq.m_oServerInfo);
        pSrvCtx->SetBlockConn(this);

        Source(connReq.m_oServerInfo.nServerID, true);
        Target(serverMgr->Args().serverInfo.nServerID);

        ConnResponse connResp;
        connResp.m_nType = MsgType::BLOCK_CONN_RESPONSE;
        connResp.m_nResult = 1;

        BasePacket pkt;
        connResp.Pack(&pkt);
        uint32 nLength = pkt.Length();

        Write(&nLength, sizeof(nLength));
        Write(pkt.Data(), pkt.Length());
        return 1;
    }
    return 0;
}
bool BlockConnHandler::HighPriorityThan(BlockConnHandler& conn)
{
    return m_nSource < conn.Source();
}

uint16 BlockConnHandler::Source()
{
    return m_nSource;
}

void BlockConnHandler::Source(uint16 source, bool peer)
{
    m_nSource = source;
    if (peer)
    {
        m_nPeer = m_nSource;
    }
}

uint16 BlockConnHandler::Target()
{
    return m_nTarget;
}

void BlockConnHandler::Target(uint16 target, bool peer)
{
    m_nTarget = target;
    if (peer)
    {
        m_nPeer = m_nTarget;
    }
}

uint16 BlockConnHandler::Peer()
{
    return m_nPeer;
}
