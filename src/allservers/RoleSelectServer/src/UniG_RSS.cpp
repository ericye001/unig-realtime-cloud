#include "UniG.h"


UniG_RSS* UniG_RSS::m_pInstance = 0;

UniG_RSS* UniG_RSS::Instance(void)
{
    if(!m_pInstance)
    {
        m_pInstance = new UniG_RSS();
    }
    return m_pInstance;
}

UniG_RSS::UniG_RSS(void)
{
}

UniG_RSS::~UniG_RSS(void)
{
    Finalize();
}

void UniG_RSS::GetILMArgs(ILMArgs& args)
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

bool UniG_RSS::Initialize(void)
{
    // load the game server config file.
    m_pConfig = new IniParser("./../config/roleselectserver.ini");
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

    ProxyMsgSender* pProxySender = new ProxyMsgSender;
    if( pProxySender == NULL )
    {
        return false;
    }

    Epoll* pProxyEpoll = new Epoll();
    pProxyEpoll->Add(pProxyHandler);
    // start a thread to communicate with proxy.
    Thread proxyThread(pProxyHandler);
    Thread proxySThread(pProxySender);

    ILMArgs arg;
    GetILMArgs(arg);

    if(SeamlessService::Init(arg))
    {
        return true;
    }
    return false;
}

void UniG_RSS::Finalize(void)
{

}

void UniG_RSS::Update(void)
{
}


void UniG_RSS::ProcessILMMsg(BaseMessage* pMessage)
{
    if( !pMessage)
    {
        return ;
    }
    switch(pMessage->m_nType)
    {
    case MsgType::SERVER_STATUS_REPLY:      // 服务器状态查询
        {
            break;
        }
    default:
        {
            break;
        }
    }
}


void UniG_RSS::GetPropertyValue(string strSection, string strPropertyName, string& strValue)
{
    vector<Section*> sectionList;
    m_pConfig->getSection(strSection.c_str(), sectionList);
    m_pConfig->getKeyVal(**sectionList.begin(), strPropertyName.c_str(), strValue);
}

void UniG_RSS::GetPropertyValue(string strSection, string strPropertyName, int& iValue)
{
    vector<Section*> sectionList;
    m_pConfig->getSection(strSection.c_str(), sectionList);
    m_pConfig->getKeyVal(**sectionList.begin(), strPropertyName.c_str(), iValue);
}

void UniG_RSS::MainRun(void)
{
    vector<BaseMessage*> seamlessMsg;
    vector<BaseMessage*> proxyMsg;
    vector<BaseMessage*>::iterator iter;
    do 
    {	 
        // handle system message ,such as from ilm or ServerContextMgr.
        seamlessMsg.clear();
        UniG_MessageQueue::Instance().SeamlessOutQueue().FrontBatch(seamlessMsg, true);
        if(seamlessMsg.size() > 0)
        {
            for(iter = seamlessMsg.begin(); iter != seamlessMsg.end(); iter++)
            {
                ProcessILMMsg(*iter);
            }
        }
		UniG_MessageQueue::Instance().SeamlessOutQueue().PopBatch(seamlessMsg.size());
        proxyMsg.clear();
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

        }
		UniG_MessageQueue::Instance().ProxyInQueue().PopBatch(proxyMsg.size());
        // TODO 这里需要有时间限制，什么时候进行下一针更新
        Update();
        Thread::sleep(1);
    }while(true);
}
