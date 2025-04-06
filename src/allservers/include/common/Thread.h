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
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 
* 文件名称：ComInterface.h
* 文件标识：见配置管理计划书
* 摘    要：组件全局变量接口类定义。
* 
*
* 修订记录：
*       修订日期    修订人    修订内容
*/

#ifndef _OPENTHREADS_THREAD_
#define _OPENTHREADS_THREAD_

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#if defined __linux || defined __sun || defined __APPLE__
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/unistd.h>
#endif


namespace OpenThreads {

/**
 *  @class Thread
 *  @brief  This class provides an object-oriented thread interface.
 */
class Thread {
	public:

   
    /**
     *  Constructor
     */
    Thread();

    /**
     *  Destructor
     */
    virtual ~Thread();


    /**
     *  Return a pointer to the current running thread
     */
    static Thread *CurrentThread();


    /**
     *  Initialize Threading in a program.  This method must be called before
     *  you can do any threading in a program.
     */ 
    static void Init();

    /**
     *  Yield the processor.  
     *
     *  @note This method operates on the calling process.  And is
     *  equivalent to calling sched_yield().
     *
     *  @return 0 if normal, -1 if errno set, errno code otherwise.
     */
    static int Yield();

    /**
     *  Get a unique thread id.  This id is monotonically increasing.
     *
     *  @return a unique thread identifier
     */
    int getThreadId();

    /**
     *  Get the thread's process id.  This is the pthread_t or pid_t value
     *  depending on the threading model being used.
     *
     *  @return thread process id.
     */
    int getProcessId();

    /**
     *  Start the thread.  This method will configure the thread, set
     *  it's priority, and spawn it. 
     *
     *  @note if the stack size specified setStackSize is smaller than the 
     *  smallest allowable stack size,  the threads stack size will be set to 
     *  the minimum allowed, and may be retrieved via the getStackSize()
     *
     *  @return 0 if normal, -1 if errno set, errno code otherwise.
     */
    int start();
    int startThread();

    /**
     * Test the cancel state of the thread.  If the thread has been canceled
     * this method will cause the thread to exit now.  This method operates
     * on the calling thread.
     *
     * Returns 0 if normal, -1 if called from a thread other that this.
     */ 
    int testCancel();


    /**
     *  Cancel the thread.  Equivalent to SIGKILL.
     *
     *  @return 0 if normal, -1 if errno set, errno code otherwise.
     */
    virtual int cancel();

    /**
     *  Set the thread's desired stack size (in bytes).  
     *  This method is an attribute of the thread and must be called 
     *  *before* the start() method is invoked.
     *
     *  @note a return code of 13 (EACESS) means that the thread stack 
     *  size can no longer be changed. 
     *
     *  @return 0 if normal, -1 if errno set, errno code otherwise.
     */
    int setStackSize(size_t size);

    /**
     *  Get the thread's desired stack size.  
     *
     *  @return the thread's stack size.  0 indicates that the stack size
     *   has either not yet been initialized, or not yet been specified by
     *   the application.
     */
    size_t getStackSize();

    /**
     *  Detach the thread from the calling process.
     *
     *  @return 0 if normal, -1 if errno set, errno code otherwise.
     */
    int detach();

    /**
     *  Join the calling process with the thread
     *
     *  @return 0 if normal, -1 if errno set, errno code otherwise.
     */
    int join();

    /**
     *  Disable thread cancelation altogether. Thread::cancel() has no effect.
     *
     *  @return 0 if normal, -1 if errno set, errno code otherwise.
     */
    int setCancelModeDisable();

    /**
     *  Mark the thread to cancel aysncronously on Thread::cancel().
     *  (May not be available with process-level implementations).
     *
     *  @return 0 if normal, -1 if errno set, errno code otherwise.
     */
    int setCancelModeAsynchronous();

    /**
     *  Mark the thread to cancel at the earliest convenience on 
     *  Thread::cancel() (This is the default)
     *
     *  @return 0 if normal, -1 if errno set, errno code otherwise.
     */
    int setCancelModeDeferred();

    /**
     *  Query the thread's running status
     *
     *  @return true if running, false if not.
     */
    bool isRunning();

    /**
     *  Thread's run method.  Must be implemented by derived classes.
     *  This is where the action happens.
     */
    virtual void run() = 0;

    void setAutoDestroy(bool flag);

    /**
     *  Thread's cancel cleanup routine, called upon cancel(), after the
     *  cancelation has taken place, but before the thread exits completely. 
     *  This method should be used to repair parts of the thread's data
     *  that may have been damaged by a pre-mature cancel.  No-op by default.
     */
    virtual void cancelCleanup() {};
    
    bool isAutoDestroy();

	private:

    /**
     *  The Private Actions class is allowed to operate on private data.
     */    
    friend class ThreadPrivateActions;  

     /**
      *  Private copy constructor, to prevent tampering.
      */
    Thread(const Thread &t) {};
     
    /**
      *  Private copy assignment, to prevent tampering.
      */
    Thread &operator=(const Thread &t) {return *(this);};
     
    /**
     *  Implementation-specific data
     */
    void * _prvData;
    
    /**
     *  Is initialized flag
     */
    static bool s_isInitialized; 
    
    bool _destroyFlag;
};

class ThreadPrivateData {

    //-------------------------------------------------------------------------
    // We're friendly to Thread, so it can use our data.
    //
    friend class Thread;

    //-------------------------------------------------------------------------
    // We're friendly to PThreadPrivateActions, so it can get at some 
    // variables.
    //
    friend class ThreadPrivateActions;

	private:

    ThreadPrivateData() {};

    virtual ~ThreadPrivateData() {};

    volatile unsigned int stackSize;

    volatile bool stackSizeLocked;

    volatile bool isRunning;

    volatile bool isCanceled;

    volatile bool idSet;

    pthread_t tid;

    volatile int uniqueId;

    static int nextId;

    static pthread_key_t s_tls_key;

};
}

#endif // !_OPENTHREADS_THREAD_
