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
            // 处理锁定地图消息
            break;
        }
        case MsgType::UNLOCK_MAP_REQUEST:
        {
            // 处理锁定地图消息
            ServerUnlockMapRequest* pUnlockRequest = (ServerUnlockMapRequest*)(pMessage);
            ServerUnlockMapResponse* pUnlockResponse = new ServerUnlockMapResponse;
            vector<uint16> vecMaps = pUnlockRequest->m_vecMapList;
            //for(int i=0; i< vecMaps.size(); i++)
            //{
            //    string regionName = MSTInfo::Instance().GetMapInfo(vecMaps[i]).szName;
            //    UniG_Region* pRegion = UniG_GameMap::Instance()->GetRegion(regionName);
            //    if(pRegion == NULL)
            //    {
            //        // TODO handle error.
            //    }
            //    else
            //    {
            //        pRegion->Unlock();
            //    }
            //}
            //pUnlockResponse->m_nResult = (uint8)1;
            //UniG_MessageQueue::Instance().SeamlessOutQueue().Push(pUnlockResponse);
            break;
        }
        case MsgType::PACK_MAP_REQUEST:
        {
            // 打包几个地图
            PackMapsRequest* pRequest = (PackMapsRequest*)pMessage;
            PackMapsResponse* pResponse = new PackMapsResponse();
            vector<uint32> vecMaps = pRequest->m_vecMapList;
            pResponse->m_nMapNum = pRequest->m_nPackageMapNum;
            //for(int i=0; i< vecMaps.size(); i++)
            //{
            //    string regionName = MSTInfo::Instance().GetMapInfo(vecMaps[i]).szName;
            //    UniG_Region* pRegion = UniG_GameMap::Instance()->GetRegion(regionName);
            //    if(pRegion == NULL)
            //    {
            //        // TODO handle error.
            //        pResponse->m_nMapNum--;
            //    }
            //    else
            //    {
            //        MapPackage tempMapInfo;
            //        tempMapInfo.nMapID = vecMaps[i];
            //        pRegion->Pack(tempMapInfo.pData, tempMapInfo.nLength);
            //        pResponse->m_vecMapPackateList.push_back(tempMapInfo);
            //    }
            //}
            UniG_MessageQueue::Instance().SeamlessOutQueue().Push(pResponse);
            break;
        }
        case MsgType::UNPACK_MAP_REQUEST:
        {
            // 解包地图
            UnpackMapsRequest* pRequest = (UnpackMapsRequest*)pMessage;
            UnpackMapsResponse* pResponse = new UnpackMapsResponse();
            pResponse->m_nResult = 0;
            int mapNum = pRequest->m_nMapNum;
            //for(int i=0; i< mapNum; i++)
            //{
            //    int regionNum = pRequest->m_vecMapPackateList[i].nMapID;
            //    string regionName = MSTInfo::Instance().GetMapInfo(regionNum).szName;
            //    UniG_Region* pRegion = UniG_GameMap::Instance()->GetRegion(regionName);
            //    int iResult = pRegion->Unpack(pRequest->m_vecMapPackateList[i].pData, pRequest->m_vecMapPackateList[i].nLength);
            //    
            //}
            UniG_MessageQueue::Instance().SeamlessOutQueue().Push(pResponse);
            break;
        }

        case MsgType::SERVER_STATUS_REPLY:
        {
            // 得到地图的人数然后填充
            ServerStatusReplyMsg* pMsg = (ServerStatusReplyMsg*)pMessage;
            for(int i=0; i< pMsg->m_nMapNum; i++)
            {
                pMsg->m_vecMaps[i].nPlayerNum = i+50;
                
                pMsg->m_nCpuLoad = 78;
                pMsg->m_pPlayerNum = 1000;
            }
            
            UniG_MessageQueue::Instance().SeamlessOutQueue().Push(pMsg);
            break;
        }
        case MsgType::AGENT_CREATE_REQUEST:
        {
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
            AgentDeleteMessage* pMsg = (AgentDeleteMessage*)pMessage;
            // TODO: first judge the type of object, then create the agent of indicate type.
            // but only player now.
            UniG_GameLogic* pLogic = UniG_GameServer::Instance().GetGameLogic();
            pLogic->GetPlayerMgr()->RemovePlayer(pMsg->m_nGuid);
            break;
        }

        case MsgType::SYN_AGENT_MESSAGE:
        {
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
            SessionCreateMessage* pMsg = (SessionCreateMessage*)pMessage;

            UniG_Player* pPlayer = (UniG_Player*)(UniG_Map::Instance()->GetSubMap("TestMap")->FindObject(pMsg->m_nObjType, pMsg->m_nGuid));
            uint16 serverID = pMsg->m_nServerID;
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
        UniG_MessageQueue::Instance().SeamlessInQueue().FrontBatch(seamlessMsg, true);
        if(seamlessMsg.size() > 0)
        {
            for(iter = seamlessMsg.begin(); iter != seamlessMsg.end(); iter++)
            {
                ProcessSeamlessMsg(*iter); 
            }
        }
        UniG_MessageQueue::Instance().SeamlessInQueue().PopBatch(seamlessMsg.size());

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
