#include "Define.h"
#include "LoginService.h"
#include "GameSocket.h"
#include "protocal.h"

#include <string>


#pragma comment(lib, "libmysql.lib")

CLoginService CLoginService::m_pInstance;

CLoginService* CLoginService::GetInstance(void)
{
    return &m_pInstance;
}

CLoginService::CLoginService(void)
{
    m_nQueryIDMaker = 1000;
}

CLoginService::~CLoginService(void)
{
    Exit();
}

bool CLoginService::Init(IMessagePort *portMsg)
{
    m_portMsg = portMsg;

    char buf[256] = "";
    ::GetPrivateProfileString("proxymgr", "ip", NULL, buf, 256, ".\\LoginService.cfg");
    int nPort = ::GetPrivateProfileInt("proxymgr", "port", 0, ".\\LoginService.cfg");

    m_nProxyManagerPort = m_portMsg->AddServerPort(buf, nPort);

    char bufDatabaseIP[256] = "";
    ::GetPrivateProfileString("database", "ip", "127.0.0.1", bufDatabaseIP, 256, ".\\LoginService.cfg");
    int nDatabasePort = ::GetPrivateProfileInt("database", "port", 3306, ".\\LoginService.cfg");
    char bufDatabase[32] = "";
    ::GetPrivateProfileString("database", "db", "games", bufDatabase, 32, ".\\LoginService.cfg");

    char bufUser[32] = "";
    ::GetPrivateProfileString("database", "user", "games", bufUser, 32, ".\\LoginService.cfg");

    char bufPwd[32] = "";
    ::GetPrivateProfileString("database", "password", "123456", bufPwd, 32, ".\\LoginService.cfg");

    m_pQueryThread.Init(bufDatabaseIP, nDatabasePort, bufDatabase, bufUser, bufPwd);


    return true;
}

void CLoginService::Loop(void)
{
    QueryResult result = m_pQueryThread.TakeResult();

    while(result.nQueryId != 0) {
        QuerySocketSet::iterator iter = m_setQuerySocket.find(result.nQueryId);
        if(iter != m_setQuerySocket.end()) {
            CGameSocket *pSocket = (*iter).second;
            SendLoginResult(pSocket, result.nUserId);
            if(result.nUserId == 0)
                printf("[客户端] %d 登录失败\n", pSocket->GetClientId());
            else 
                printf("[客户端] %d 验证通过\n", pSocket->GetClientId());

            if(result.nUserId != 0) {
                CUser *pUser = new CUser(pSocket);
                m_setUser[result.nUserId] = pUser;
                pUser->SetState(1);
                pUser->m_nUserID = result.nUserId;
                pSocket->SetState(2);
                printf("[客户端] userid:%d 向PM发起请求...\n", result.nUserId);
                SendAddClientToPMRequest(result.nUserId);
				// 客户端网络没处理粘包问题
				//::Sleep(200);
                // SendS2CProxyInfo(result.nUserId, 12345678, 2345, 234);
            }

            m_setQuerySocket.erase(iter);
        }

        result = m_pQueryThread.TakeResult();
    }
}

void CLoginService::Exit(void)
{
    UserSet::iterator itUser = m_setUser.begin();
    for(;itUser != m_setUser.end();itUser++) {
        CUser *pUser = (*itUser).second;
        delete pUser;
    }

    m_setUser.clear();
}

void CLoginService::OnMessage(void *buf, int nLen)
{
    stProtocalHeader *pMsgHead = (stProtocalHeader*)buf;

    switch(pMsgHead->usType) {
    case ADD_CLIENT_TO_PM_RESPONSE:
        NetCommandAddClientToPMResponse(buf);
        break;
    }
}

void CLoginService::NetCommandAddClientToPMResponse(void *buf)
{
    stAddClientToPMResponse *pMsg = (stAddClientToPMResponse*)buf;

    printf("[客户端] %d 取得Proxy信息\n", pMsg->nUserID);
    printf("[客户端] %d 向客户端发送Proxy信息\n", pMsg->nUserID);

	SendS2CProxyInfo(pMsg->nUserID, pMsg->pProxySvrInfo._ip, pMsg->pProxySvrInfo._port, pMsg->RoleServerID, pMsg->nSession);

    UserSet::iterator iter = m_setUser.find(pMsg->nUserID);

    if(iter != m_setUser.end())
    {
        CUser *pUser = (*iter).second;

        CGameSocket *pSocket = pUser->m_pSocket;
        if(pSocket)
            pSocket->Close();

        Logout(pMsg->nUserID);
    }
}

CUser* CLoginService::Login(CGameSocket *pSocket, const char *pszUser, const char *pszPassword)
{
    if(!pSocket)
        return NULL;

    printf("[客户端] user:%s password:%s 请求登录\n", pszUser, pszPassword);
    unsigned int nQueryId = m_nQueryIDMaker++;

    m_setQuerySocket[nQueryId] = pSocket;
    pSocket->SetState(1);
    m_pQueryThread.AddQuery(nQueryId, pszUser, pszPassword);

    return NULL;
}

void CLoginService::Logout(unsigned int nUserId)
{
    UserSet::iterator iter = m_setUser.find(nUserId);
    if(iter != m_setUser.end()) {
        CUser *pUser = (*iter).second;
        if(pUser)
            delete pUser;

        m_setUser.erase(iter);

        printf("[客户端] %d 断开\n", nUserId);
    }
}

CUser* CLoginService::GetUser(unsigned int nUserId)
{
    UserSet::iterator iter = m_setUser.find(nUserId);
    if(iter != m_setUser.end()) {
        CUser *pUser = (*iter).second;
            return pUser;
    }

    return NULL;
}

void CLoginService::SendLoginResult(CGameSocket *pSocket, unsigned int nUserId)
{
    if(!pSocket)
        return;

    stS2CLogin msg;
    msg.usSize = sizeof(stS2CLogin) - 4;
    msg.usType = _MSG_S2C_USER_LOGIN;
    // msg.ucExt = 0;
    msg.nUserId = nUserId;

    pSocket->SendMsg(&msg, sizeof(stS2CLogin), _PORT_CLIENT);
}

void CLoginService::SendAddClientToPMRequest(unsigned int nUserId)
{
    stAddClientToPMRequest msg;
    msg.usSize = sizeof(stAddClientToPMRequest) - 4;
    msg.usType = ADD_CLIENT_TO_PM_REQUEST;
    // msg.ucExt = 0;
    msg.nUserID = nUserId;
    msg.nSession = 0;
    msg.pClientInfo._id = 0;
    msg.pClientInfo._ip = 0;
    msg.pClientInfo._port = 0;

    IMessage msgPack(_SYSMSG_SERVICE, sizeof(stAddClientToPMRequest), &msg, 0, 0);
    m_portMsg->SendMsg(msgPack, m_nProxyManagerPort);
}

void CLoginService::SendS2CProxyInfo(unsigned nUserId, unsigned int nProxyIp, unsigned short nPort, unsigned int nRSID, unsigned int nSession)
{
    stS2CProxyInfo msg;

    msg.usSize = sizeof(stS2CProxyInfo) - 4;
    msg.usType = _MSG_S2C_PROXY_INFO;
    // msg.ucExt = 0;
    msg.ulProxyIP = nProxyIp;
    msg.usPort = nPort;
	msg.nRSID = nRSID;
    msg.nSession = nSession;

    CUser *pUser = GetUser(nUserId);
    if(pUser) {
        CGameSocket *pSocket = pUser->m_pSocket;
        if(pSocket) {
            pSocket->SendMsg(&msg, sizeof(stS2CProxyInfo), _PORT_CLIENT);
        }
    }
}