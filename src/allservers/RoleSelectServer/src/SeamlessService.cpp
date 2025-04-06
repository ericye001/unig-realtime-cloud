#include "UniG.h"

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
