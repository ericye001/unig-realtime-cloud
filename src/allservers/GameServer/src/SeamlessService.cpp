#include "UniG.h"


int SeamlessService::m_nIsThisSrvDest = 0;
//---------------------------------------------------------------------------
//  SeamlessService::instance
//---------------------------------------------------------------------------
//  Description:  return global single instance of class SeamlessService
//  Parameters:   None
//  Return:       instance of SeamlessService
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
SeamlessService* SeamlessService::m_pInstance = 0;

SeamlessService& SeamlessService::Instance()
{
    return *m_pInstance;
}

//---------------------------------------------------------------------------
//  SeamlessService::SeamlessService
//---------------------------------------------------------------------------
//  Description:  SeamlessService's constructure
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
SeamlessService::SeamlessService(ILMArgs& args)
{
    m_oServerInfo.nIP = args.nSrvIp;
    m_oServerInfo.nPort = args.nSrvPort;
}
//---------------------------------------------------------------------------
//  SeamlessService::InitServerContextMgr
//---------------------------------------------------------------------------
//  Description:  Initialize seamless service
//  Parameters:   None
//  Return:       true: success; false: failure
//  Exception:    NoneSIGINT received.
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
bool SeamlessService::Init(ILMArgs& args)
{
    m_pInstance = new SeamlessService(args);
    if(m_pInstance)
	{
		m_pInstance->InitILM(args);
        m_pInstance->InitMsgBus();
        return true;
	}
    return false;
}
void SeamlessService::InitILM(ILMArgs& args)
{
	ILMListener* pILMSockHandler = new ILMListener(args);
    if(!pILMSockHandler)
    {
        throw SocketHandlerException("create a socket handler at SeamlessService::SeamlessService\n");
    }
    Thread* pILMThread = new Thread(pILMSockHandler);

}

void SeamlessService::InitMsgBus()
{
    ServerContextArgs scArgs;
    // read config info from config file£¬but now we get the info from ILM server
    /*
    int nMaxMsgLength;
    UniG_GameServer::Instance()->GetPropertyValue("msgbus", "MaxMsgLength", nMaxMsgLength);
    scArgs.nMaxMsgLength = nMaxMsgLength;

    int nMaxAsyncMsg;
    UniG_GameServer::Instance()->GetPropertyValue("msgbus", "MaxAsyncMsg", nMaxAsyncMsg);
    scArgs.nMaxAsyncMsg = nMaxAsyncMsg;

    int m_nAsyncBatchNum;
    UniG_GameServer::Instance()->GetPropertyValue("msgbus", "BatchNum", m_nAsyncBatchNum);
    scArgs.nBatchNum = m_nAsyncBatchNum;

    int nMaxSrvNum;
    UniG_GameServer::Instance()->GetPropertyValue("msgbus", "MaxSrvNum", nMaxSrvNum);
    scArgs.nMaxSrvNum = nMaxSrvNum;

    scArgs.serverInfo = m_oServerInfo;
    */
    scArgs.nMaxMsgLength = m_nMaxMsgLength;
    scArgs.nMaxAsyncMsg = m_nMaxAsyncMsg;
    scArgs.nBatchNum = m_nAsyncBatchNum;
    scArgs.nMaxSrvNum = m_nMaxServerNum;
    scArgs.serverInfo = m_oServerInfo;
    try
    {
        ServerContextMgr::Init(scArgs);    
    }
    catch (exception& e)
    {
        cout << "exception: " << e.what() << endl
            << "\tat SeamlessService::init"
            << SrcLocation(__FILE__, __LINE__) << endl;
        return;
    }  
}

//---------------------------------------------------------------------------
//  SeamlessService::ServerInfo
//---------------------------------------------------------------------------
//  Description:  return the this server's infomation
//  Parameters:   None
//  Return:       void
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
ServerInfo& SeamlessService::GetServerInfo()
{    
    return m_oServerInfo;
}

void SeamlessService::SetServerInfo(ServerInfo& srvInfo)
{
    m_oServerInfo = srvInfo;
}

//---------------------------------------------------------------------------
//  SeamlessService::MST
//---------------------------------------------------------------------------
//  Description:  setup global MST and pick up the MST server intersecting
//  Parameters:   None
//  Return:       void
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
void SeamlessService::MST(vector<Map2SrvInfo>& mst)
{
    // Find out all MST this server manageed
    m_vecIntersectingMST.clear();
    vector<Map2SrvInfo>::iterator iter;
    for(iter = mst.begin(); iter != mst.end(); iter++)
    {
        if(iter->nServerID == m_oServerInfo.nServerID)
        {
            m_vecIntersectingMST.push_back(*iter);
        }
    }
}
//---------------------------------------------------------------------------
//  SeamlessService::MST
//---------------------------------------------------------------------------
//  Description:  return this server's intersecting MST
//  Parameters:   None
//  Return:       void4
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
vector<Map2SrvInfo>& SeamlessService::MST()
{
    return m_vecIntersectingMST;
}

//---------------------------------------------------------------------------
//  SeamlessService::AdjoiningMaps
//---------------------------------------------------------------------------
//  Description:  return this server's adjoining maps
//  Parameters:   None
//  Return:       a vector containing adjoining maps' id
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
vector<uint16>& SeamlessService::AdjoiningMaps()
{
    return m_vecAdjoiningMaps;
}
//---------------------------------------------------------------------------
//  SeamlessService::AdjoiningMaps
//---------------------------------------------------------------------------
//  Description:  update this server's adjoining maps
//  Parameters:   None
//  Return:       void
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
void SeamlessService::AdjoiningMaps(vector<uint16>& adjoiningMaps)
{
    m_vecAdjoiningMaps = adjoiningMaps;
}
//---------------------------------------------------------------------------
//  SeamlessService::IsMapOnServer
//---------------------------------------------------------------------------
//  Description:  check whether the map specified by mapId is managed by this
//                server
//  Parameters:   None
//  Return:       true if managed by this server, else false
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
bool SeamlessService::IsMapOnServer(uint16 mapId)
{
    mapId += 1;
    for (uint16 i = 0; i < m_vecIntersectingMST.size(); ++i)
    {
        if (m_vecIntersectingMST[i].nMapID == mapId)
        {
            return true;
        }
    }
    return false;
}

void SeamlessService::MaxAsyncMsg(uint16 n)
{
    m_nMaxAsyncMsg = n;
}

void SeamlessService::AsyncBatchNum(uint16 n)
{
    m_nAsyncBatchNum = n;
}

void SeamlessService::MaxMsgLength(uint16 n)
{
    m_nMaxMsgLength = n;
}

void SeamlessService::MaxServerNum(uint16 n)
{
    m_nMaxServerNum = n;
}

