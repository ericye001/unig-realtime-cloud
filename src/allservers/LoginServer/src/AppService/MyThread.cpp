#include "Define.h"

#ifndef _MYTHREAD_H_INCLUDED
#include "MyThread.h"
#endif 

#include <process.h>

const int TIME_WAITINGCLOSE	= 3000;		// 3000ms

DWORD WINAPI MyThread::RunThread(LPVOID pThreadParameter)
{
	MyThread*	pThread	=(MyThread*)pThreadParameter;
	if (!pThread)
		return 1;

	// init
	if (-1 == pThread->m_event.OnThreadCreate())
		return 2;

	// work now
	HANDLE hEvent[2] = { pThread->m_hExitEvent, pThread->m_hWorkEvent };
	while (true)
	{
		int rval = ::WaitForMultipleObjects(2, hEvent, false, pThread->m_dwWorkInterval);
		if (WAIT_OBJECT_0 == rval)
		{
			// exit event is set
			break;
		}
		else if (WAIT_OBJECT_0+1 == rval)
		{	
			// work event is set
			try
			{	
				if (-1 == pThread->m_event.OnThreadWorkEvent())
					break;
			}	
			catch(...)
			{
				// myDebugWarning("CATCH: *** MyThread::RunThread Call OnThreadWorkEvent crash! *** at %s, %d", __FILE__, __LINE__);
			}
		}
		else if (WAIT_TIMEOUT == rval)
		{
			try
			{
				// work interval is time out
				if (-1 == pThread->m_event.OnThreadProcess())
					break;
			}
			catch(...)
			{	
				// myDebugWarning("CATCH: *** MyThread::RunThread Call OnThreadProcess crash! *** at %s, %d", __FILE__, __LINE__);
			}	
		}
		else
		{
			// here should log error msg
			break;
		}
	}

	pThread->m_nStatus = MyThread::STATUS_CLOSED;
	return pThread->m_event.OnThreadDestroy();
}
MyThread::MyThread(iThreadEvent& event)
: m_event(event), m_hThread(NULL), m_hExitEvent(NULL), m_hWorkEvent(NULL)
{
	m_id				= 0;
	m_dwWorkInterval	= 0;
	m_nStatus	= STATUS_INIT;
}

MyThread::~MyThread()
{
	if (STATUS_INIT == m_nStatus)
		return;

	// resume if suspend
	if (STATUS_SUSPEND == m_nStatus)
		this->Resume();

	// close thread...
	if (STATUS_CLOSING != m_nStatus)
		this->Close();

	// waiting for close
	if (WAIT_TIMEOUT == ::WaitForSingleObject(m_hThread, TIME_WAITINGCLOSE))
	{
		::TerminateThread(m_hThread, 4444);
	}

	// release all resources
	if (m_hThread)
	{
		::CloseHandle(m_hThread);
		m_hThread	=NULL;
	}

	if (m_hWorkEvent)
	{
		::CloseHandle(m_hWorkEvent);
		m_hWorkEvent	=NULL;
	}

	if (m_hExitEvent)
	{
		::CloseHandle(m_hExitEvent);
		m_hExitEvent	=NULL;
	}

	m_id = 0;
}

//////////////////////////////////////////////////////////////////////
BOOL MyThread::Close()
{
	if (::SetEvent(m_hExitEvent))
	{
		m_nStatus = STATUS_CLOSING;
		return true;
	}
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
BOOL MyThread::Create(BOOL bSuspend/*=true*/, DWORD dwWorkInterval/*=INFINITE*/)
{
	if (STATUS_INIT != m_nStatus)
		return false;

	// exit event
	m_hExitEvent =CreateEvent(NULL,		// no security
		FALSE,	// auto reset
		FALSE,	// initial event reset
		NULL);	// no name
	if (!m_hExitEvent)
		return false;

	// work event
	m_hWorkEvent =CreateEvent(NULL,		// no security
		FALSE,	// auto reset
		FALSE,	// initial event reset
		NULL);	// no name
	if (!m_hWorkEvent)
		return false;

	// thread
	DWORD dwCreationFlags = 0;
	if (bSuspend)
		dwCreationFlags	=CREATE_SUSPENDED;

	typedef unsigned (__stdcall *MYTHREAD_FUNC)(void *);
	m_hThread	= (HANDLE)_beginthreadex (	NULL,			// default security
		0,				// default stack size
		(MYTHREAD_FUNC)RunThread,		// pointer to thread routine
		(void *)this,			// argument for thread
		dwCreationFlags,// start it right away if 0, else suspend
		(unsigned *)&m_id);
	if (!m_hThread)
	{
		::CloseHandle(m_hExitEvent);
		m_hExitEvent	=NULL;

		::CloseHandle(m_hWorkEvent);
		m_hWorkEvent	=NULL;

		return false;
	}

	m_dwWorkInterval = dwWorkInterval;

	if (bSuspend)
		m_nStatus = STATUS_SUSPEND;
	else
		m_nStatus = STATUS_RUNNING;

	return true;
}

//////////////////////////////////////////////////////////////////////
void MyThread::Resume()
{
	if (STATUS_SUSPEND != m_nStatus)
		return;

	if (-1 != ::ResumeThread(m_hThread))
		m_nStatus = STATUS_RUNNING;
}

//////////////////////////////////////////////////////////////////////
void MyThread::Suspend()
{
	if (STATUS_RUNNING != m_nStatus)
		return;

	if (-1 != ::SuspendThread(m_hThread))
		m_nStatus = STATUS_SUSPEND;
}

//////////////////////////////////////////////////////////////////////
MyThread*	MyThread::CreateNew(iThreadEvent& refEvent, BOOL bSuspend/* = SUSPEND*/, DWORD dwWorkInterval/* = INFINITE*/)
{
	MyThread* pThread = new MyThread(refEvent);
	if (!pThread)
		return NULL;

	if (!pThread->Create(bSuspend, dwWorkInterval))
	{
		delete pThread;
		return NULL;
	}

	return pThread;
}
