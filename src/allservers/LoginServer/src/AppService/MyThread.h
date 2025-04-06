#ifndef _MYTHREAD_H_INCLUDED
#define _MYTHREAD_H_INCLUDED

#pragma once
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// 超时类，以“毫秒”为单位
class MyTimerMS
{
public:
	MyTimerMS(int nIntervalSecs = 0) { m_nInterval = nIntervalSecs; m_tUpdateTime = 0; }		// =0 : 用于数组初始化
//	virtual ~MyTimerMS() {}

public:
	void	Update		()						{ m_tUpdateTime = clock(); }
	bool	IsTimeOut	()						{ return clock() >= m_tUpdateTime+m_nInterval; }
	bool	ToNextTime	()						{ if(IsTimeOut()) return Update(),true; else return false; }
	void	WaitNextTime()						{ clock_t tmNow = clock(); if(m_tUpdateTime+m_nInterval > tmNow) ::Sleep(m_tUpdateTime+m_nInterval-tmNow); Update(); }
	void	SetInterval	(int nMilliSecs)		{ m_nInterval = nMilliSecs; }

	void	Startup		(int nMilliSecs)		{ m_nInterval = nMilliSecs; Update(); }
	bool	TimeOver	()						{ if(IsActive() && IsTimeOut()) return Clear(),true; return false; }
	void	Clear		()						{ m_nInterval = m_tUpdateTime = 0; }
	bool	IsActive	()						{ return m_tUpdateTime != 0; }
	void	IncInterval	(int nMilliSecs, int nLimit)		{ m_nInterval = __min(m_nInterval+nMilliSecs, nLimit); }
	void	DecInterval	(int nMilliSecs)		{ m_nInterval = __max(m_nInterval-nMilliSecs, 0); }

	bool	IsTimeOut	(int nMilliSecs)		{ return clock() >= m_tUpdateTime+nMilliSecs; }
	bool	ToNextTime	(int nMilliSecs)		{ if(IsTimeOut(nMilliSecs)) return Update(),true; else return false; }
	bool	ToNextTick	(int nMilliSecs);
//	void	AppendInterval	(int nMilliSecs)	{ if(ToNextTime()) m_nInterval=nMilliSecs; else m_nInterval+=nMilliSecs; }	// ToNextTime(): 保证至少有nSecs秒

public: // get
	int		GetInterval()						{ return m_nInterval; }
	DWORD	GetUpdateTime()						{ return m_tUpdateTime; }

protected:
	int		m_nInterval;
	clock_t	m_tUpdateTime;
};

//////////////////////////////////////////////////////////////////////
class iLockObj
{
public:
	virtual void Lock	(void)	= 0;
	virtual void Unlock	(void)	= 0;
};

//////////////////////////////////////////////////////////////////////
class MyCriticalSection : public iLockObj
{
public:
	MyCriticalSection( DWORD spincount = 2000 )
	{
		//InitializeCriticalSectionAndSpinCount( &m_CritSec , spincount);
		InitializeCriticalSection( &m_CritSec );
	};

	virtual ~MyCriticalSection()
	{
		DeleteCriticalSection( &m_CritSec );
	};

	void    Lock(void)
	{
		EnterCriticalSection( &m_CritSec );
	};

	void    Unlock(void)
	{
		LeaveCriticalSection( &m_CritSec );
	};

private:
	CRITICAL_SECTION    m_CritSec;
};


//////////////////////////////////////////////////////////////////////
class MyMutex : public iLockObj
{
public:
	MyMutex() : m_hMutex(NULL) {}
	~MyMutex(void)	{ if (m_hMutex) ::CloseHandle(m_hMutex); }

	void Lock	(void)	{ if (m_hMutex) ::WaitForSingleObject(m_hMutex, INFINITE); }
	void Unlock	(void)	{ if (m_hMutex) ::ReleaseMutex(m_hMutex); }

private:
	HANDLE m_hMutex;

	// static
public:
	static MyMutex* CreateNew(const char* pszName = NULL, bool bExistTest = false)
	{
		HANDLE hMutex = ::CreateMutex(NULL, FALSE, pszName);
		if (!hMutex)
			return NULL;

		if (pszName && bExistTest)
		{
			if (ERROR_ALREADY_EXISTS == ::GetLastError())
			{
				::CloseHandle(hMutex);
				return NULL;
			}
		}


		MyMutex* pMutex = new MyMutex;
		if (!pMutex)
			return NULL;

		pMutex->m_hMutex = hMutex;
		return pMutex;
	}
};

//////////////////////////////////////////////////////////////////////
class MySingleLock 
{
public:
	MySingleLock(iLockObj& lock)
		: m_lock(lock)
	{ 		
		m_lock.Lock(); 
	}

	~MySingleLock	(void)	
	{
		m_lock.Unlock();
	}

private:
	iLockObj&	m_lock;
};

class iThreadEvent
{
public:
	virtual int			OnThreadCreate(void)		= 0;
	virtual int			OnThreadDestroy(void)		= 0;
	virtual int			OnThreadWorkEvent(void)		= 0;
	virtual int			OnThreadProcess(void)		= 0;
};

class MyThread
{
public:
	MyThread(iThreadEvent& event);
	virtual ~MyThread();

public:
	enum { SUSPEND = true, RUN = false };
	BOOL	Create	(BOOL bSuspend=SUSPEND, DWORD dwWorkInterval=INFINITE);
	BOOL	Close	(void);

	void	Resume	(void);
	void	Suspend	(void);
	void	SetWorkEvent	(void)	{ ::SetEvent(m_hWorkEvent); }

	DWORD	GetWorkInterval() const	{ return m_dwWorkInterval; }
	void	SetWorkInterval(DWORD dwWorkInterval)	{ ::InterlockedExchange((LPLONG)&m_dwWorkInterval ,dwWorkInterval); }

	enum {	STATUS_INIT = 0, 
		STATUS_SUSPEND, 
		STATUS_RUNNING, 
		STATUS_CLOSING,
		STATUS_CLOSED };

	int		GetStatus(void) const { return m_nStatus; }

protected:
	DWORD		m_dwWorkInterval;
	iThreadEvent&	m_event;

private:
	HANDLE		m_hThread;
	HANDLE		m_hExitEvent;
	HANDLE		m_hWorkEvent;

	DWORD			m_id;
	volatile int	m_nStatus;

	static DWORD WINAPI RunThread(LPVOID pThreadParameter);

public:
	static MyThread*	CreateNew(iThreadEvent& refEvent, BOOL bSuspend = SUSPEND, DWORD dwWorkInterval = INFINITE);
};


#endif 