#include "UniG.h"

uint64  UniG_GameServer::m_nLastFrameTime = 0;
uint64  UniG_GameServer::m_nCurFrameTime = 0;
uint32  UniG_GameServer::m_nElapsedTime = 0;

UniG_GameServer* UniG_GameServer::m_pInstance = 0;

UniG_GameServer& UniG_GameServer::Instance()
{
    if (!m_pInstance)
    {
        static UniG_GameServer instance;
        m_pInstance = &instance;
    }

    return *m_pInstance;
}

void UniG_GameServer::PackObject(UniG_GameObject* pSrc, UniG_GameObject* pDest, void* pMsg, int bToSelf)
{
    MapPackage* pMapInfo = (MapPackage*)pMsg;
    AgentCreateMessage* pAgentCreateMsg = new AgentCreateMessage;
    pDest->Serialize(pAgentCreateMsg);
    pMapInfo->nAgentNum++;
    pMapInfo->vAgentList.push_back(pAgentCreateMsg);
}

void UniG_GameServer::LockObject(UniG_GameObject* pSrc, UniG_GameObject* pDest, void* pMsg, int isLock)
{
    ObjectMessage* pMessage = new ObjectMessage;
    pMessage->m_nType = MsgType::LOCK_OBJECT_REQUEST;
    pMessage->m_nMsgLength = sizeof(UNIG_GUID);
    pMessage->m_pMsgData = new char[4];
    UNIG_GUID nID = pDest->GetGuid();
    PtrMemCpy(pMessage->m_pMsgData, &nID, 4);
    ((UniG_Player*)(pDest))->GetSession()->EnqueMsg(pMessage, true);
}

void UniG_GameServer::ChangeServer(UniG_GameObject* pSrc, UniG_GameObject* pDest, void* pMsg, int nServer)
{
    // tell the client to change server.
    uint16 nThisServer = nServer;
    ObjectMessage* pMessage = new ObjectMessage;
    pMessage->m_nType = MsgType::CHANGE_SERVERID;
    pMessage->m_nMsgLength = 2;
    pMessage->m_pMsgData = new char[2];
    PtrMemCpy(pMessage->m_pMsgData, &nThisServer, 2);
    ((UniG_Player*)(pDest))->GetSession()->EnqueMsg(pMessage, true);
}


void UniG_GameServer::ClearAgent(UniG_GameObject* pSrc, UniG_GameObject* pDest, void* pMsg, int bToSelf)
{
    MapRegion* pRegion = (MapRegion*)pMsg;

    if(SeamlessService::m_nIsThisSrvDest == 0)
    {
        if(!pDest->IsAgent())
        {
            pDest->IsAgent(true);
        }
        else
        {
            pRegion->ObjectLeave(pDest->GetPosition(), pDest->GetGuid(), pDest->GetType());
            UNIG_GUID nGuid = pDest->GetGuid();
            UniG_GameServer::Instance().GetGameLogic()->GetPlayerMgr()->RemovePlayer(nGuid);
        }
    }
    else
    {

    }
}

void UniG_GameServer::DeSearialize(MapPackage& data)
{
    int nRegionID = data.nMapID;
    int nAgentNum = data.nAgentNum;
    bool bIsMapOnServer = SeamlessService::Instance().IsMapOnServer(nRegionID);
    for(int i = 0; i < nAgentNum; i++)
    {
        UniG_GameLogic* pLogic = UniG_GameServer::Instance().GetGameLogic();
        UniG_GameObject*  pNewAgent;
        pNewAgent = pLogic->GetPlayerMgr()->GetPlayerByID(data.vAgentList[i]->m_nGuid);
        if(pNewAgent)
        {
            // there is aready a agent or entiry?
        }
        else
        {
            pNewAgent = pLogic->AllocObject(data.vAgentList[i]->m_nObjType);
            pNewAgent->DeSerialize(data.vAgentList[i]);
            pNewAgent->IsAgent(false);
            UniG_Map::Instance()->GetSubMap("TestMap")->ObjectEnter(pNewAgent->GetType(), pNewAgent->GetPosition(),
                pNewAgent->GetGuid(), pNewAgent);
        }
    }
}


UniG_GameServer::UniG_GameServer(void)
:m_pGameLogic(0)
{

}

UniG_GameServer::~UniG_GameServer(void)
{
    Finalize();
}

void UniG_GameServer::GetILMArgs(ILMArgs& args)
{
    string strILMIp;
    GetPropertyValue("server", "ILMIP",strILMIp);
    args.nILMIp = InetAddress::StrToInet((char*)(strILMIp.c_str()));

    int iILMPort;
    GetPropertyValue("server", "ILMPort", iILMPort);
    args.nILMPort = iILMPort;

    string strServerIp;
    GetPropertyValue("server", "ServerIP",strServerIp);
    args.nSrvIp = InetAddress::StrToInet((char*)(strServerIp.c_str()));

    int iServerPort;
    GetPropertyValue("server", "ServerPort", iServerPort);
    args.nSrvPort = iServerPort;
}

bool UniG_GameServer::Initialize(void)
{
    // load the game server config file.
    m_pConfig = new IniParser("./../config/gameserver.ini");
    m_pConfig->load();

    // get the ip address of localhost
    string strServerIP; 
    GetPropertyValue("server", "ServerIP", strServerIP);

    // get the port that listen the proxy
    int iPort;
    GetPropertyValue("server", "ServerPort", iPort);

    ServerSocket* pProxySocketListener = new  ServerSocket(strServerIP, iPort);
    if(pProxySocketListener == NULL)
    {
        return false;
    }

    ProxyListener* pProxyHandler = new ProxyListener(pProxySocketListener, ProxyObjectFactory::Instance());
    if(pProxyHandler == NULL)
    {
        return false;
    }

    ////////////////////////////////////////////////////////////////
    Epoll* pProxyEpoll = new Epoll();
    pProxyEpoll->Add(pProxyHandler);

    // start a thread to communicate with proxy.
    Thread* pProxyThread = new Thread(pProxyHandler);
    Thread* pProxySendThread = new Thread(new ProxyMsgSender);


    ILMArgs arg;
    GetILMArgs(arg);

    if(SeamlessService::Init(arg))
    {
        return true;
    }
    return false;
}


void UniG_GameServer::InitGameLogic(UniG_GameLogic* pGameLogic)
{
    assert(pGameLogic);
    m_pGameLogic = pGameLogic;
    m_pGameLogic->Initialize();
}

void UniG_GameServer::Finalize(void)
{
    if( m_pGameLogic )
    {
        delete m_pGameLogic;
        m_pGameLogic = 0;
    }
}

void UniG_GameServer::Update(void)
{
    if( m_pGameLogic )
        m_pGameLogic->Update();
}


void UniG_GameServer::ProcessSeamlessMsg(BaseMessage* pMessage)
{
    if( !pMessage)
    {
        return ;
    }

    switch(pMessage->m_nType)
    {
    case MsgType::LOCK_MAP_REQUEST:
        {
            // lock  regions
            ServerLockMapRequest* pLockMapMsg = (ServerLockMapRequest*)pMessage;
            int nMapNum = pLockMapMsg->m_nLockMapNum;
            if(nMapNum > 0)
            {
                for(int i=0; i< nMapNum; i++)
                {
                    int nRow, nCol;
                    ObjectTools::Instance()->TransfromRegionID(pLockMapMsg->m_vecMapList[i], nRow, nCol);
                    MapRegion* pRegion = UniG_Map::Instance()->GetSubMap("TestMap")->GetRegion(nRow, nCol);
                    if(pRegion)
                    {
                        pRegion->Lock();
                    }
                }
            }
            ServerLockMapResponse* pResponse = new ServerLockMapResponse;
            pResponse->m_nResult = 1;
            UniG_MessageQueue::Instance().ILMOutQueue().Push(pResponse);
            break;
        }
    case MsgType::UNLOCK_MAP_REQUEST:
        {
            // unlock regions
            ServerUnlockMapRequest* pUnlockRequest = (ServerUnlockMapRequest*)(pMessage);
            int nMapNum = pUnlockRequest->m_nUnlockMapNum;
            if(nMapNum > 0)
            {
                for(int i=0; i< nMapNum; i++)
                {
                    int nRow, nCol;
                    ObjectTools::Instance()->TransfromRegionID(pUnlockRequest->m_vecMapList[i], nRow, nCol);
                    MapRegion* pRegion = UniG_Map::Instance()->GetSubMap("TestMap")->GetRegion(nRow, nCol);
                    if(pRegion)
                    {
                        pRegion->For_Each(NULL, (void*)pRegion, 0, UniG_GameServer::ClearAgent);
                        pRegion->UnLock();
                    }
                }
            }
            ServerUnlockMapResponse* pUnlockResponse = new ServerUnlockMapResponse;
            pUnlockResponse->m_nResult = 1;
            UniG_MessageQueue::Instance().ILMOutQueue().Push(pUnlockResponse);
            break;
        }
    case MsgType::PACK_MAP_REQUEST:
        {
            // pack some region data(include roles on the region);
            PackMapsRequest* pRequest = (PackMapsRequest*)pMessage;
            PackMapsResponse* pResponse = new PackMapsResponse();
            vector<uint32> vecMaps = pRequest->m_vecMapList;
            pResponse->m_nMapNum = pRequest->m_nPackageMapNum;
            for(int i=0; i< vecMaps.size(); i++)
            {
                int nRow, nCol;
                ObjectTools::Instance()->TransfromRegionID(vecMaps[i], nRow, nCol);
                MapRegion* pRegion = UniG_Map::Instance()->GetSubMap("TestMap")->GetRegion(nRow, nCol);
                if(pRegion == NULL)
                {
                    // TODO handle error.
                    pResponse->m_nMapNum--;
                }
                else
                { 
                    MapPackage tempMapInfo;
                    tempMapInfo.nMapID = vecMaps[i];
                    tempMapInfo.nAgentNum = 0;
                    // tell all the client lock itself
                    pRegion->For_Each(NULL, NULL, 1, UniG_GameServer::LockObject);
                    pRegion->For_Each(NULL, &tempMapInfo, 0, UniG_GameServer::PackObject);
                    tempMapInfo.nMapID = vecMaps[i];
                    pResponse->m_vecMapPackateList.push_back(tempMapInfo);
                }
            }
            UniG_MessageQueue::Instance().ILMOutQueue().Push(pResponse);
            break;
        }
    case MsgType::UNPACK_MAP_REQUEST:
        {
            UnpackMapsRequest* pRequest = (UnpackMapsRequest*)pMessage;
            UnpackMapsResponse* pResponse = new UnpackMapsResponse();
            pResponse->m_nResult = 0;
            int mapNum = pRequest->m_nMapNum;
            for(int i=0; i< mapNum; i++)
            {
                int nRow, nCol;
                ObjectTools::Instance()->TransfromRegionID(pRequest->m_vecMapPackateList[i].nMapID, nRow, nCol);
                MapRegion* pRegion = UniG_Map::Instance()->GetSubMap("TestMap")->GetRegion(nRow, nCol);
                if(pRegion)
                {
                    UniG_GameServer::DeSearialize(pRequest->m_vecMapPackateList[i]);
                }   
                uint16 nCurServerID = SeamlessService::Instance().GetServerInfo().nServerID;
                pRegion->For_Each(NULL, NULL, nCurServerID, UniG_GameServer::ChangeServer);
            }
            pResponse->m_nResult = 1;
            UniG_MessageQueue::Instance().ILMOutQueue().Push(pResponse);
            break;
        }

    case MsgType::SERVER_STATUS_REPLY:
        {
            // the pMessage will be delete when run over this function
            // we new a return message,and fill with value
            ServerStatusReplyMsg* pTempMsg = (ServerStatusReplyMsg*)pMessage;
            ServerStatusReplyMsg* pReplyMsg = new ServerStatusReplyMsg();

            *pReplyMsg = *pTempMsg;

            for(int i=0; i< pTempMsg->m_nMapNum; i++)
            {
                int nRow, nCol;
                ObjectTools::Instance()->TransfromRegionID(pTempMsg->m_vecMaps[i].nMapID, nRow, nCol);
                MapRegion* pIterRegion = UniG_Map::Instance()->GetSubMap("TestMap")->GetRegion(nRow, nCol);
                if(pIterRegion)
                {
                    pReplyMsg->m_vecMaps[i].nPlayerNum = pIterRegion->total();
                    pReplyMsg->m_vecMaps[i].nMapID = pTempMsg->m_vecMaps[i].nMapID;
                }
            }
            pReplyMsg->m_nMapNum = pTempMsg->m_nMapNum;
            pReplyMsg->m_nCpuLoad = 50;
            UniG_MessageQueue::Instance().ILMOutQueue().Push(pReplyMsg);
            break;
        }
    case MsgType::AGENT_CREATE_REQUEST:
        {
            printf("创建影子\n");
            AgentCreateMessage* pMsg = (AgentCreateMessage*)pMessage;
            // TODO: first judge the type of object, then create the agent of indicate type.
            // but only player now.
            UniG_GameLogic* pLogic = UniG_GameServer::Instance().GetGameLogic();
            UniG_GameObject*  pNewAgent;
            pNewAgent = pLogic->GetPlayerMgr()->GetPlayerByID(pMsg->m_nGuid);
            if(pNewAgent)
            {
                pNewAgent->IsAgent(true);
            }
            else
            {
                pNewAgent = pLogic->AllocObject(pMsg->m_nObjType);
                pNewAgent->DeSerialize(pMsg);
                pNewAgent->IsAgent(true);
                UniG_Map::Instance()->GetSubMap("TestMap")->ObjectEnter(pNewAgent->GetType(), pNewAgent->GetPosition(),
                    pNewAgent->GetGuid(), pNewAgent);
            }
            break;
        }

    case MsgType::AGENT_DELETE_REQUEST:
        {
            printf("删除影子\n");
            AgentDeleteMessage* pMsg = (AgentDeleteMessage*)pMessage;
            // TODO: first judge the type of object, then create the agent of indicate type.
            // but only player now.
            // session

            UniG_GameLogic* pLogic = UniG_GameServer::Instance().GetGameLogic();
            UniG_Player* pPlayer = pLogic->GetPlayerMgr()->GetPlayerByID(pMsg->m_nGuid);
            if( pPlayer )
            {
                UniG_SessionMgr::Instance()->DestroySession(SessionKey( pPlayer->GetSession()->ProxyID(), pPlayer->GetSession()->ClientID()) );
                pPlayer->Logout();
                pLogic->GetPlayerMgr()->RemovePlayer(pMsg->m_nGuid);
            }
            break;
        }

    case MsgType::SYN_AGENT_MESSAGE:
        {
            printf("收到同步消息\n");
            SynAgentMessage* pMsg = (SynAgentMessage*)pMessage;
            UniG_GameObject* pObj;
            int nRegionCol, nRegionRow;
            ObjectTools::Instance()->TransfromRegionID(pMsg->m_nRegionID, nRegionRow, nRegionCol);
            MapRegion* pRegion = UniG_Map::Instance()->GetSubMap("TestMap")->GetRegion(nRegionRow, nRegionCol);
            if(pRegion)
            {
                pObj = pRegion->FindObject(pMsg->m_nDestObject, pMsg->m_nObjType);
                if(pObj)
                {
                    pObj->ProcessMessage((ObjectMessage*)pMessage);
                }
            }
            break;
        }
    case MsgType::SESSION_CREATE_REQUEST:
        {
            printf("创建Session\n");
            SessionCreateMessage* pMsg = (SessionCreateMessage*)pMessage;

            UniG_Player* pPlayer = (UniG_Player*)(UniG_Map::Instance()->GetSubMap("TestMap")->FindObject(pMsg->m_nObjType, pMsg->m_nGuid));
            uint16 serverID = SeamlessService::Instance().GetServerInfo().nServerID;//pMsg->m_nServerID;
            uint16 proxyID = pMsg->m_nProxyID;
            uint16 clientID = pMsg->m_nClientID;
            if(pPlayer)
            {
                if(!UniG_SessionMgr::Instance()->GetSessioin(serverID, proxyID, clientID))
                {
                    UniG_Session* pSession = UniG_SessionMgr::Instance()->CreateSession(serverID, proxyID, clientID);
                    if(pSession)
                    {
                        pPlayer->SetSession(pSession);
                        pSession->SetPlayer(pPlayer);
                    }
                }
            }
            break;
        }
    case MsgType::SYSTEM_MESSAGE:
        {
            UniG_SessionMgr::Instance()->ProcessClientMsg((ProxyMessage*)pMessage);
            break;
        }
    default:
        {
            break;
        }
    }
}


void UniG_GameServer::GetPropertyValue(string strSection, string strPropertyName, string& strValue)
{
    vector<Section*> sectionList;
    m_pConfig->getSection(strSection.c_str(), sectionList);
    m_pConfig->getKeyVal(**sectionList.begin(), strPropertyName.c_str(), strValue);
}

void UniG_GameServer::GetPropertyValue(string strSection, string strPropertyName, int& iValue)
{
    vector<Section*> sectionList;
    m_pConfig->getSection(strSection.c_str(), sectionList);
    m_pConfig->getKeyVal(**sectionList.begin(), strPropertyName.c_str(), iValue);
}

void UniG_GameServer::MainRun(void)
{
    m_nLastFrameTime = GetTimeMsec();

    vector<BaseMessage*> seamlessMsg;
    vector<BaseMessage*> proxyMsg;
    vector<BaseMessage*>::iterator iter;
    do 
    {	
        // handle system message ,such as from ilm or ServerContextMgr.
        seamlessMsg.clear();
        UniG_MessageQueue::Instance().ILMInQueue().FrontBatch(seamlessMsg, true);
        if(seamlessMsg.size() > 0)
        {
            for(iter = seamlessMsg.begin(); iter != seamlessMsg.end(); iter++)
            {
                ProcessSeamlessMsg(*iter); 
            }
        }
        UniG_MessageQueue::Instance().ILMInQueue().PopBatch(seamlessMsg.size());

        // process the message from proxy server
        proxyMsg.clear();

        // Guard<Mutex> AutoMutex(m_pMutex);


        UniG_MessageQueue::Instance().ProxyInQueue().FrontBatch(proxyMsg, true);
        if(proxyMsg.size()>0)
        {
            vector<BaseMessage*>::iterator iter ;
            ProxyMessage* pToProcMsg = 0;
            for (iter = proxyMsg.begin(); iter!=proxyMsg.end(); iter++)
            {
                pToProcMsg = (ProxyMessage*)(*iter);
                UniG_SessionMgr::Instance()->ProcessClientMsg(pToProcMsg);
            }

            UniG_MessageQueue::Instance().ProxyInQueue().PopBatch(proxyMsg.size());
        }

        m_nCurFrameTime = GetTimeMsec();
        m_nElapsedTime += m_nCurFrameTime - m_nLastFrameTime;

        if( m_nElapsedTime > 50 )
        {
            // TODO 这里需要有时间限制，什么时候进行下一针更新
            Update();
            m_nElapsedTime = 0;
        }

        m_nLastFrameTime = m_nCurFrameTime;

        Thread::sleep(1);
    }while(true);
}

UniG_GameLogic* UniG_GameServer::GetGameLogic()
{
    return m_pGameLogic;
}
