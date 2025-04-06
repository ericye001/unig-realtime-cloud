#ifndef _APPSERVICE_H
#define _APPSERVICE_H

#include "MyThread.h"
#include "..\LoginService\LoginService.h"

#include "IMessagePort.h"

#pragma warning(disable : 4786)

#include <map>

class CConnect;

class CAppService : public IAppService, public iThreadEvent
{
public:
	CAppService(IMessagePort &portMsg);
	virtual ~CAppService();

	void Release(void);

	bool Create(DWORD dwMsgPrcInterval, DWORD dwCntPrcInterval);

	void ProcessMsg(void);
	

private:
	int OnThreadCreate(void);
	int OnThreadDestroy(void);
	int OnThreadWorkEvent(void);
	int OnThreadProcess(void);

protected:
	IMessagePort &m_portMsg;
	MyTimerMS m_timerConnectPrc;
	MyThread *m_pThread;

	typedef std::map<SOCKET, CConnect*> CONNECT_SET;
	CONNECT_SET m_setConnect;
};

#endif
