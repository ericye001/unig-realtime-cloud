#include "Define.h"
#include "GameSocket.h"

#include "protocal.h"
#include "LoginService.h"

CGameSocket::CGameSocket()
{
    m_pUser = NULL;
    m_nState = 0;
}

CGameSocket::~CGameSocket()
{

}

void CGameSocket::SendMsg(const char *msg)
{
	if(!msg)
		return;
}

void CGameSocket::OnRecv(const void *buf, int nLen)
{
	if(!buf)
		return;

    stProtocalHeader *pMsg = (stProtocalHeader*)buf;
    switch(pMsg->usType) {
    case _MSG_C2S_USER_LOGIN:
        NetCommandLogin(buf);
        break;
    }
}

void CGameSocket::OnClose(int nErrorCode)
{
    if(m_pUser) {
        CLoginService::GetInstance()->Logout(m_pUser->m_nUserID);
    }
}

void CGameSocket::OnTimer(void)
{
}

void CGameSocket::NetCommandLogin(const void *buf)
{
    stC2SLogin *pMsgLogin = (stC2SLogin*)buf;
    // printf("user:%s password:%s login\n", pMsgLogin->szUser, pMsgLogin->szPassword);

    CLoginService::GetInstance()->Login(this, pMsgLogin->szUser, pMsgLogin->szPassword);

    /*stS2CLogin pMsgResult;
    pMsgResult.usSize = sizeof(stS2CLogin) - 2;
    pMsgResult.usType = _MSG_S2C_USER_LOGIN;
    pMsgResult.ucExt = 0;
    pMsgResult.ucResult = 1;

    SendMsg(&pMsgResult, sizeof(stS2CLogin), _PORT_CLIENT);

    stS2CProxyInfo pMsgProxyInfo;
    pMsgProxyInfo.usSize = sizeof(stS2CProxyInfo) - 2;
    pMsgProxyInfo.usType = _MSG_S2C_PROXY_INFO;
    pMsgProxyInfo.ucExt = 0;
    pMsgProxyInfo.ulProxyIP = 0xc0a8007B;
    pMsgProxyInfo.usPort = 5003;

    SendMsg(&pMsgResult, sizeof(stS2CProxyInfo), _PORT_CLIENT);*/
}