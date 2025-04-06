#ifndef _CONNECT_H
#define _CONNECT_H

#include "../LoginService/GameSocket.h"

class CConnect : public CGameSocket
{
public:
	CConnect(IMessagePort &port, int nClientId = 0);
	virtual ~CConnect();

	void Process(void);
	int GetClientId(void) { return m_nClientId; }
	void ClearSocket(void);
	void RecoverSocket(int nClientId);

public:
	int m_nPortExit;

private:
	int m_nClientId;
	IMessagePort &m_portMsg;

public:
	virtual void SendMsg(const void *buf, int nLen, int nPort);
	virtual void Close(void);
	// virtual void ResetNetwork(int nState);
};

#endif 