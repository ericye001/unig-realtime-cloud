#include "UniG.h"

//---------------------------------------------------------------------------
//  MSTInfo::Instance
//---------------------------------------------------------------------------
//  Description:  return global single instance of class MSTInfo
//  Parameters:   None
//  Return:       instance of MSTInfo
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
MSTInfo* MSTInfo::m_pInstance = 0;

MSTInfo& MSTInfo::Instance()
{
    if (!m_pInstance)
    {
        if (!m_pInstance)
        {
            static MSTInfo instance;
            m_pInstance = &instance;
        }
    }
    return *m_pInstance;
}

//---------------------------------------------------------------------------
//  MSTInfo::MSTInfo
//---------------------------------------------------------------------------
//  Description:  MSTInfo's constructure
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
MSTInfo::MSTInfo()
{
    // No body
}
//---------------------------------------------------------------------------
//  MSTInfo::AllMST
//---------------------------------------------------------------------------
//  Description:  return global MST
//  Parameters:   None
//  Return:       void
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
vector<Map2SrvInfo>& MSTInfo::AllMST()
{
    Guard<Mutex> lock(m_oMutex);
    return m_vecAllMST;
}
//---------------------------------------------------------------------------
//  MSTInfo::AllMST
//---------------------------------------------------------------------------
//  Description:  setup global MST and pick up the MST server intersecting
//  Parameters:   None
//  Return:       void
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
void MSTInfo::AllMST(vector<Map2SrvInfo>& mst)
{
    Guard<Mutex> lock(m_oMutex);
    m_vecAllMST = mst;
    SeamlessService::Instance().MST(mst);
}

//---------------------------------------------------------------------------
//  MSTInfo::AllSrvs
//---------------------------------------------------------------------------
//  Description:  return all servers' information
//  Parameters:   None
//  Return:       void
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
vector<ServerInfo>& MSTInfo::AllSrvs()
{
    Guard<Mutex> lock(m_oMutex);
    return m_vecAllSrvs;
}
//---------------------------------------------------------------------------
//  MSTInfo::AllSrvs
//---------------------------------------------------------------------------
//  Description:  update all servers' information
//  Parameters:   None
//  Return:       void
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
void MSTInfo::AllSrvs(vector<ServerInfo>& allSrvs)
{
    Guard<Mutex> lock(m_oMutex);
    m_vecAllSrvs = allSrvs;
    if( SeamlessService::Instance().MST().size() == 0 )
        return;

    for (uint32 i = 0; i < m_vecAllSrvs.size(); ++i)
    {
        if (m_vecAllSrvs[i].nServerID ==  SeamlessService::Instance().GetServerInfo().nServerID)
        {
            continue;
        }
        try
        {   
            ServerContextMgr::Instance()->SetupConnection(m_vecAllSrvs[i]);
        }
        catch (exception& e)
        {
            cout << "exception: " << e.what() << endl
                << "\tat MSTInfo::allSrvs" 
                << SrcLocation(__FILE__, __LINE__) << endl;
        }
    }
}

vector<MapInfo>& MSTInfo::AllMaps()
{
    Guard<Mutex> lock(m_oMutex);
    return m_vecAllMaps;
}

void MSTInfo::AllMaps(vector<MapInfo>& allMaps)
{
    Guard<Mutex> lock(m_oMutex);
    m_vecAllMaps = allMaps;    
}


uint16 MSTInfo::MapOwner(uint16 nMapId)
{
    Guard<Mutex> lock(m_oMutex);
    for (uint16 i = 0; i < m_vecAllMST.size(); ++i)
    {
        if (m_vecAllMST[i].nMapID == nMapId)
        {
            return m_vecAllMST[i].nServerID;
        }
    }
    char tmp[1024];
    sprintf(tmp,    "Invalid mapId %u\n"
                    "\tat MSTInfo::mapOwner%s", 
                    nMapId,
                    SrcLocation(__FILE__, __LINE__).c_str());
    throw StrException(tmp);
}

//---------------------------------------------------------------------------
//  MSTInfo::Map2SrvInfo
//---------------------------------------------------------------------------
//  Description:  find the Map2SrvInfo from global MST according to
//                mapId specified
//  Parameters:   None
//  Return:       Map2SrvInfo about mapId specified
//  Exception:    string if no Map2SrvInfo found
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
Map2SrvInfo& MSTInfo::GetMSTPairByMap(uint16 nMapId)
{
    Guard<Mutex> lock(m_oMutex);
    for (uint16 i = 0; i < m_vecAllMST.size(); ++i)
    {
        if (m_vecAllMST[i].nMapID == nMapId)
        {
            return m_vecAllMST[i];
        }
    }
    char msg[1024];
    sprintf(msg,    "Invalid mapId(%u)\n"
                    "\tat MSTInfo::map2SrvInfo(%s)", 
                    nMapId,
                    SrcLocation(__FILE__, __LINE__).c_str());
    
    throw StrException(msg);
}
//---------------------------------------------------------------------------
//  MSTInfo::GetServerInfo
//---------------------------------------------------------------------------
//  Description:  find the SrvInfo from all servers' information according
//                to server id specified
//  Parameters:   None
//  Return:       SrvInfo about server id secified
//  Exception:    string if no SrvInfo found
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
ServerInfo& MSTInfo::GetServerInfo(uint16 nSrvID)
{
    Guard<Mutex> lock(m_oMutex);
    for (uint16 i = 0; i < m_vecAllSrvs.size(); ++i)
    {
        if (m_vecAllSrvs[i].nServerID == nSrvID)
        {
            return m_vecAllSrvs[i];
        }
    }
    char msg[1024];
    sprintf(msg,    "Invalid srvId(%u)\n"
                    "\tat MSTInfo::srvInfo(%s)", 
                    nSrvID,
                    SrcLocation(__FILE__, __LINE__).c_str());
    
    throw StrException(msg);
}
//---------------------------------------------------------------------------
//  MSTInfo::GetMapInfo
//---------------------------------------------------------------------------
//  Description:  return all servers' infomation
//  Parameters:   None
//  Return:       void" << endl;
//  Exception:    None
//  MT Safe:      true
//  Note:         None
//---------------------------------------------------------------------------
MapInfo& MSTInfo::GetMapInfo(uint16 nMapID)
{
    Guard<Mutex> lock(m_oMutex);
    for (uint16 i = 0; i < m_vecAllMaps.size(); ++i)
    {
        if (m_vecAllMaps[i].nMapID == nMapID)
        {
            return m_vecAllMaps[i];
        }
    }
    char msg[1024];
    sprintf(msg,    "Invalid mapId(%u)\n"
                    "\tat MSTInfo::mapInfo(%s)", 
                    nMapID,
                    SrcLocation(__FILE__, __LINE__).c_str());
    
    throw StrException(msg);
}


