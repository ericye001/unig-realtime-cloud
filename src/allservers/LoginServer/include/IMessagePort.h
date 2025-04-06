#ifndef _IMESSAGEPORT_H
#define _IMESSAGEPORT_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mmsystem.h>

#ifdef SERVERSHELL_EXPORTS
#define SERVERAPP __declspec(dllexport)
#else
#define SERVERAPP __declspec(dllimport)
#endif 

#pragma warning(disable : 4273)

#define _PORT_CLIENT 0

enum {
    _SYSMSG_CLIENT = 0,
    _SYSMSG_SERVICE,
    _SYSMSG_CONNECT_LOGIN,
    _SYSMSG_CONNECT_LOGOUT,
};

class IAppService {
public:
	virtual void Release(void) = 0;
};

class SERVERAPP IMessage {
public:
	IMessage(void)
	{
		nFrom = -1;
		nTo = -1;
		nMsgSize = 0;
		nSysMsg = 0;
		
		bufMsg[0] = 0;
	}

	IMessage(int nMsg, int nSize, const void *buf, int from = -1, int to = -1)
	{
		assert(nSize >= 0);
		
		nMsgSize = nSize;
		nSysMsg = nMsg;
		nFrom = from;
		nTo = to;
		memcpy(bufMsg, buf, nSize);

		bufMsg[nSize] = 0;
	}

	~IMessage(void)
	{
	}

	int GetSize(void) const { return nMsgSize; }
	const void* GetBuf(void) const { return bufMsg; }
	int GetFrom(void) const { return nFrom; }
	int GetTo(void) const { return nTo; }

	void Release(void);

    // void* operator new(size_t nObjSize);
    // void operator delete(void* pObjPtr);
	
	int nFrom;
	int nTo;
	int nMsgSize;
	int nSysMsg;

	enum { _MAX_MSGSIZE = 512 };
	
	char bufMsg[_MAX_MSGSIZE];
};

class IMessagePort {
public:
	virtual void Release(void) = 0;
	virtual bool IsValid(void) const = 0;
	virtual bool SendMsg(IMessage &msg, int nPort) = 0;
	virtual IMessage* TakeMsg(void) = 0;
    virtual int AddServerPort(const char *pszServerIp, int nPort) = 0;
    virtual void DelServerPort(int nPort) = 0;
};

#endif 