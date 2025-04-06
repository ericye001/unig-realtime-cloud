#ifndef _LOGINSERVICE_H
#define _LOGINSERVICE_H

#include "User.h"
#include "IMessagePort.h"
#include <map>

#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <mysql.h>

#include "QueryThread.h"


class CLoginService {
public:
    CLoginService(void);
    ~CLoginService(void);

    bool Init(IMessagePort *portMsg);
    void Loop(void);
    void Exit(void);

    CUser* Login(CGameSocket *pSocket, const char *pszUser, const char *pszPassword);
    void Logout(unsigned int nUserId);
    CUser* GetUser(unsigned int nUserId);

    void OnMessage(void *buf, int nLen);

    void SendLoginResult(CGameSocket *pSocket, unsigned int nUserId);
    void SendAddClientToPMRequest(unsigned int nUserId);
    void SendS2CProxyInfo(unsigned nUserId, unsigned int nProxyIp, unsigned short nPort, unsigned int nRSID, unsigned int nSession);
    void NetCommandAddClientToPMResponse(void *buf);

    typedef std::map<unsigned int, CUser*> UserSet;
    UserSet m_setUser;

    static CLoginService m_pInstance;
    static CLoginService* GetInstance(void);

    IMessagePort *m_portMsg;
    int m_nProxyManagerPort;

    // MYSQL *m_pSqlConnect;

    CQueryThread m_pQueryThread;

    unsigned int m_nQueryIDMaker;
    typedef std::map<unsigned int, CGameSocket*> QuerySocketSet;
    QuerySocketSet m_setQuerySocket;
};

#endif 