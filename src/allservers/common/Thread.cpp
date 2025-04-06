/*
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 
* 文件名称：
* 文件标识：
* 摘    要：
* 
*
* 修订记录：
*       修订日期    修订人    修订内容
*/

/*
* Copyright (c) 2006, UoneNet
* All rights reserved.
* 
* 文件名称：ComInterface.h
* 文件标识：见配置管理计划书
* 摘    要：组件全局变量接口类定义。
* 
*/

#include "common/Thread.h"

using namespace OpenThreads;

extern int errno;
const char *OPENTHREAD_VERSION_STRING =
    "OpenThreads v1.2preAlpha, Posix Threads(Public Implementation)";

#ifdef DEBUG
# define DPRINTF(arg) printf arg
#else
# define DPRINTF(arg)
#endif

//-----------------------------------------------------------------------------
// Initialize the static unique ids.
//
int ThreadPrivateData::nextId = 0;

bool Thread::s_isInitialized = false;
pthread_key_t ThreadPrivateData::s_tls_key;

struct ThreadCleanupStruct
{
    OpenThreads::Thread *thread;
    volatile bool *runflag;
};

//-----------------------------------------------------------------------------
// This cleanup handler is necessary to ensure that the thread will cleanup
// and set its isRunning flag properly.
//
void thread_cleanup_handler(void *arg)
{
    ThreadCleanupStruct *tcs = static_cast<ThreadCleanupStruct *>(arg);
    tcs->thread->cancelCleanup();
    *(tcs->runflag) = false;
}

//-----------------------------------------------------------------------------
// Class to support some static methods necessary for pthread's to work
// correctly.
//

namespace OpenThreads
{

class ThreadPrivateActions
{

    //-------------------------------------------------------------------------
    // We're friendly to Thread, so it can issue the methods.
    //
    friend class Thread;

private:

    //-------------------------------------------------------------------------
    // pthreads standard start routine.
    //
    static void *StartThread(void *data)
    {
        Thread *thread = static_cast<Thread *>(data);
        ThreadPrivateData *pd =
            static_cast<ThreadPrivateData *>(thread->_prvData);

        ThreadCleanupStruct tcs;
        tcs.thread = thread;
        tcs.runflag = &pd->isRunning;

        // Set local storage so that Thread::CurrentThread() can return
        //the right thing
        int status = pthread_setspecific(ThreadPrivateData::s_tls_key, thread);
        assert(status == 0);

        //pthread_cleanup_push(thread_cleanup_handler, &tcs);

        pd->isRunning = true;
        thread->run();
        pd->isRunning = false;

        //pthread_cleanup_pop(0);

        if(thread->isAutoDestroy())
        {
            delete thread;
        }

        return 0;

    };
};

}

//----------------------------------------------------------------------------
//
// Decription: Constructor
//
// Use: public.
//
Thread::Thread()
        : _destroyFlag(false)
{
    if(!s_isInitialized)
        Init();

    ThreadPrivateData *pd = new ThreadPrivateData();
    pd->stackSize = 0;
    pd->stackSizeLocked = false;
    pd->idSet = false;
    pd->isRunning = false;
    pd->isCanceled = false;
    pd->uniqueId = pd->nextId;
    pd->nextId++;

    _prvData = static_cast<void *>(pd);
}

//----------------------------------------------------------------------------
//
// Decription: Destructor
//
// Use: public.
//
Thread::~Thread()
{
    ThreadPrivateData *pd = static_cast<ThreadPrivateData *>(_prvData);

    if(pd->isRunning)
    {
        //---------------------------------------------------------------------
        // Kill the thread when it is destructed
        //
        setCancelModeAsynchronous();
        cancel();
    }

    delete pd;
}

Thread *Thread::CurrentThread()
{
    Thread *thread =
        static_cast<Thread*>(pthread_getspecific(ThreadPrivateData::s_tls_key));

    return thread;
}

//-----------------------------------------------------------------------------
//
// Description: Initialize Threading
//
// Use: public.
//
void Thread::Init()
{
    if(s_isInitialized)
        return;

    // Allocate a key to be used to access thread local storage
    int status = pthread_key_create(&ThreadPrivateData::s_tls_key, NULL);
    assert(status == 0);

    s_isInitialized = true;
}

//-----------------------------------------------------------------------------
//
// Description: Get a unique identifier for this thread.
//
// Use: public
//
int Thread::getThreadId()
{
    ThreadPrivateData *pd = static_cast<ThreadPrivateData *> (_prvData);
    return pd->uniqueId;
}

//-----------------------------------------------------------------------------
//
// Description: Get the thread's process id
//
// Use: public
//
int Thread::getProcessId()
{
    ThreadPrivateData *pd = static_cast<ThreadPrivateData *> (_prvData);
    if(pd->idSet == false)
        return (unsigned int) pthread_self();

    return (int)(pd->tid);
}

//-----------------------------------------------------------------------------
//
// Description: Determine if the thread is running
//
// Use: public
//
bool Thread::isRunning()
{
    ThreadPrivateData *pd = static_cast<ThreadPrivateData *> (_prvData);
    return pd->isRunning;
}

//-----------------------------------------------------------------------------
//
// Description: Start the thread.
//
// Use: public
//
int Thread::start()
{
    int status;
    pthread_attr_t thread_attr;

    status = pthread_attr_init( &thread_attr );
    if(status != 0)
    {
        return status;
    }

    ThreadPrivateData *pd = static_cast<ThreadPrivateData *> (_prvData);

    size_t defaultStackSize;
    pthread_attr_getstacksize( &thread_attr, &defaultStackSize);
    if(status != 0)
    {
        return status;
    }

    if(defaultStackSize < pd->stackSize)
    {
        pthread_attr_setstacksize( &thread_attr, pd->stackSize);
        if(status != 0)
        {
            return status;
        }
    }

    //-------------------------------------------------------------------------
    // Now get what we actually have...
    //
    pthread_attr_getstacksize( &thread_attr, &defaultStackSize);
    if(status != 0)
    {
        return status;
    }

    pd->stackSize = defaultStackSize;

    //-------------------------------------------------------------------------
    // Prohibit the stack size from being changed.
    //
    pd->stackSizeLocked = true;

    if(status != 0)
    {
        return status;
    }
    status = pthread_create(&(pd->tid), &thread_attr,
                            ThreadPrivateActions::StartThread,
                            static_cast<void *>(this));

    if(status != 0)
    {
        return status;
    }

    pd->idSet = true;
    return 0;
}

//-----------------------------------------------------------------------------
//
// Description: Alternate thread start routine.
//
// Use: public
//
int Thread::startThread()
{
    return start();
}

//-----------------------------------------------------------------------------
//
// Description: Join the thread.
//
// Use: public
//
int Thread::detach()
{
    ThreadPrivateData *pd = static_cast<ThreadPrivateData *> (_prvData);
    return pthread_detach(pd->tid);
}

//-----------------------------------------------------------------------------
//
// Description: Join the thread.
//
// Use: public
//
int Thread::join()
{
    void *threadResult = 0; // Dummy var.
    ThreadPrivateData *pd = static_cast<ThreadPrivateData *> (_prvData);
    return pthread_join(pd->tid, &threadResult);
}

//-----------------------------------------------------------------------------
//
// Description: test the cancel state of the thread.
//
// Use: public
//
int Thread::testCancel()
{
    ThreadPrivateData *pd = static_cast<ThreadPrivateData *> (_prvData);

    if(pthread_self() != pd->tid)
        return -1;

    pthread_testcancel();

    return 0;
}


//-----------------------------------------------------------------------------
//
// Description: Cancel the thread.
//
// Use: public
//
int Thread::cancel()
{
    ThreadPrivateData *pd = static_cast<ThreadPrivateData *> (_prvData);
    pd->isCanceled = true;
    int status = pthread_cancel(pd->tid);
    return status;
}

//-----------------------------------------------------------------------------
//
// Description: Disable cancelibility
//
// Use: public
//
int Thread::setCancelModeDisable()
{
    return pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, 0 );
}

//-----------------------------------------------------------------------------
//
// Description: set the thread to cancel immediately
//
// Use: public
//
int Thread::setCancelModeAsynchronous()
{
    int status = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
    if(status != 0)
        return status;

    return pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, 0);
}

//-----------------------------------------------------------------------------
//
// Description: set the thread to cancel at the next convienent point.
//
// Use: public
//
int Thread::setCancelModeDeferred()
{
    int status = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
    if(status != 0)
        return status;

    return pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, 0);
}

//-----------------------------------------------------------------------------
//
// Description: Get the thread's stack size.
//
// Use: public
//
size_t Thread::getStackSize()
{
    ThreadPrivateData *pd = static_cast<ThreadPrivateData *> (_prvData);

    return pd->stackSize;
}


//-----------------------------------------------------------------------------
//
// Description:  Yield the processor
//
// Use: protected
//
int Thread::Yield()
{
    return sched_yield();
}


void Thread::setAutoDestroy(bool flag)
{
    _destroyFlag = flag;
}


bool Thread::isAutoDestroy()
{
    return 	_destroyFlag;
}
