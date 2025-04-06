#ifndef _GAMESOCKET_H
#define _GAMESOCKET_H

#include "IMessagePort.h"

class CUser;

class CGameSocket  
{
public:
	CGameSocket();
	virtual ~CGameSocket();

public:
	virtual void SendMsg(const void *buf, int nLen, int nPort) = 0;
	virtual void Close(void) = 0;
    virtual int GetClientId(void) = 0;
	virtual void SendMsg(const char *msg);
	void OnRecv(const void *buf, int nLen);
	void OnClose(int nErrorCode);
	void OnTimer(void);

    void SetUser(CUser *pUser) { m_pUser = pUser; }
    CUser* GetUser(void) { return m_pUser; }

    void SetState(int nState) { m_nState = nState; }
    int GetState(void) const { return m_nState; }

protected:
    void NetCommandLogin(const void *buf);
    CUser *m_pUser;
    int m_nState;
};

#endif 