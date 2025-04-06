/*
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 
* ÎÄ¼þÃû³Æ£º
* ÎÄ¼þ±êÊ¶£º
* Õª    Òª£º
* 
*
* ÐÞ¶©¼ÇÂ¼£º
*       ÐÞ¶©ÈÕÆÚ    ÐÞ¶©ÈË    ÐÞ¶©ÄÚÈÝ
*/

//---------------------------------------------------------------------------
//  Copyright (c) 2002-2006, UOneNet, All right reserved
//
//  Class FormattedDate_T:
//    ï¿½ï¿½ï¿?
//  Class ObjectLevelLockable_T:
//    ï¿½ï¿½ï¿?
//  Class ClassLevelLockable_T:
//    ï¿½ï¿½ï¿?
//  Class Logger_T:
//    ï¿½Ö¾ï¿½ï¿½ï¿½//  Class LogRecord_T:
//    ï¿½Ö¾ï¿½ï¿½ï¿½
//  Class LoggerWriter_T:
//    ï¿½Ö¾ï¿½ï¿?
//---------------------------------------------------------------------------
#ifndef UTILS_HPP
#define UTILS_HPP

#include <list>
#include <set>
#include <string>
#include <pthread.h>
#include <memory>                       // Use auto_ptr
#include <cstdarg>              /* va_list */
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include "Thread.h"


using namespace std;
using namespace OpenThreads;

class LogRecord_T;
class FormattedDate_T;
class Logger_T;
class LogWriter_T;
class SrcLocation_T;
class Mutex_T;
class MutextLock_T;
class Condition_T;

template <bool flag, typename T, typename U> struct Select_T
{
  typedef T Result_T;
};
template <typename T, typename U> struct Select_T<false, T, U>
{
  typedef U Result_T;
};

template <int v> struct Int2Type_T
{
  enum {value = v};
};

template <typename T> struct Type2Type_T
{
  typedef T OriginalType_T;
};

class Mutex_T
{
friend class MutexLock_T;
friend class Condition_T;
public:
    Mutex_T();
private:
    pthread_mutex_t _mutex;
};

class MutexLock_T
{
public:
    MutexLock_T(Mutex_T& mutex);
    ~MutexLock_T();
private:
    Mutex_T& _mutex;
};
    
class Condition_T
{
public:
    Condition_T();
    ~Condition_T();
    
    void notifyOne();
    void notifyAll();
    
    void wait(Mutex_T& mutex);
    void timedWait(Mutex_T& mutex);
private:
    pthread_cond_t _condition;
};

class FormattedDate_T
{
  public:
    static char* getDate(char* pDate, size_t size, char* pFormat);

  private:
    FormattedDate_T(){};
    FormattedDate_T(const FormattedDate_T& rhs);
    FormattedDate_T& operator=(const FormattedDate_T& rhs);
    ~FormattedDate_T(){};
};


template <class Host_T>
class ObjectLevelLockable_T
{
  public:
    ObjectLevelLockable_T() {
       pthread_mutex_init(&_mutex, NULL);
    }

    virtual ~ObjectLevelLockable_T(){};

    class Lock_T;
    class ConditionObserver_T;
    class ConditionNotifier_T;

    friend class Lock_T;
    friend class ConditionObserver_T;
    friend class ConditionNotifier_T;

    class Lock_T
    {
      public:
        Lock_T(Host_T& host) : _host(host) {
          pthread_mutex_lock(&_host._mutex);
        }
        
        ~Lock_T() {
          pthread_mutex_unlock(&_host._mutex);
        }

      private:
        ObjectLevelLockable_T& _host;

        Lock_T(const Lock_T&);
        Lock_T& operator=(const Lock_T&);
    };

    class ConditionObserver_T
    {
      public:
        ConditionObserver_T(){
        };
        ConditionObserver_T(Host_T& host, pthread_cond_t& pCond) {
          pthread_cond_wait(&pCond, &host._mutex);
        }
        ~ConditionObserver_T() { };
      private:

        ConditionObserver_T(const ConditionObserver_T&);
        ConditionObserver_T& operator=(const ConditionObserver_T&);
    };

    class ConditionNotifier_T
    {
      public:
        ConditionNotifier_T(){};
        ConditionNotifier_T(pthread_cond_t& pCond) {
          pthread_cond_broadcast(&pCond);
        }
        ~ConditionNotifier_T(){};
      private:

        ConditionNotifier_T(const ConditionNotifier_T&);
        ConditionNotifier_T& operator=(const ConditionNotifier_T&);
   };
  private:
    pthread_mutex_t _mutex;
};

template <class Host_T> class ClassLevelLockable_T
{
  public:
    class Lock_T;
    class ConditionObserver_T;
    class ConditionNotifier_T;

    friend class Lock_T;
    friend class ConditionObserver_T;
    friend class ConditionNotifier_T;

    class Lock_T
    {
      public:
        Lock_T() {
          if (!_pInitializer)
          {
            static Initializer_T initializer;
            _pInitializer = &initializer;
          }
          pthread_mutex_lock(&_mutex);
        }
        Lock_T(Host_T&) {
          pthread_mutex_lock(&_mutex);
        }
        ~Lock_T() {
          pthread_mutex_unlock(&_mutex);
        }

      private:
        Lock_T(const Lock_T&);
        Lock_T& operator=(const Lock_T&);
    };

    class ConditionObserver_T
    {
      public:
        ConditionObserver_T(){
        };
        ConditionObserver_T(pthread_cond_t& pCond) {
          pthread_cond_wait(&pCond, &_mutex);
        }
        ~ConditionObserver_T() { };
      private:

        ConditionObserver_T(const ConditionObserver_T&);
        ConditionObserver_T& operator=(const ConditionObserver_T&);
    };

    class ConditionNotifier_T
    {
      public:
        ConditionNotifier_T(){};
        ConditionNotifier_T(pthread_cond_t& pCond) {
          pthread_cond_broadcast(&pCond);
        }
      private:
        ConditionNotifier_T(const ConditionNotifier_T&);
        ConditionNotifier_T& operator=(const ConditionNotifier_T&);
   };

  private:
    static pthread_mutex_t _mutex;
    struct Initializer_T;
    friend struct Initializer_T;

    struct Initializer_T
    {
        Initializer_T() {
          pthread_mutex_init(&_mutex, NULL);
        }
        ~Initializer_T() { };
    };

    static Initializer_T* _pInitializer;
};

template <class Host_T>
  typename ClassLevelLockable_T<Host_T>::Initializer_T*
  ClassLevelLockable_T<Host_T>::_pInitializer;

template <class Host_T> pthread_mutex_t  ClassLevelLockable_T<Host_T>::_mutex;


class Logger_T : public ClassLevelLockable_T<Logger_T>
{
  public:
    static void initialize(const char* pPath, const char* prefix);
        static void initialize(const string& path, const string& prefix);
    static void logIt(
        const char* pLevel,  const char* pAction, const char* pContent, ...);
    static LogRecord_T* get();
    static bool remove(LogRecord_T* pLogRec);
    static void requestIncoming();
    static int size();
    static void stop();

  private:
    Logger_T(){};
    Logger_T(const Logger_T& rhs);
    Logger_T& operator=(const Logger_T& rhs);
    virtual ~Logger_T();

    static LogWriter_T* _pLogger;
    static list<LogRecord_T*> _logRecords;
    static pthread_cond_t _incoming;
};

class LogWriter_T : public Thread
{
  friend class Logger_T;
  public:
    void run();

  private:
    LogWriter_T(const char* pPath, const char* pServiceId);
    LogWriter_T(const LogWriter_T& rhs);
    LogWriter_T& operator=(const LogWriter_T& rhs);
    virtual ~LogWriter_T();

    void setCancelFlag(bool flag);
    bool getCancelFlag();
    bool write(LogRecord_T* pLogRec);
    void getFileName(char* pFileName);

    string _path;
    string _prefix;
    bool _cancelFlag;
    string _fileName;
    FILE* _pf;
};

class LogRecord_T
{
  public:
    LogRecord_T(const char* pTime, const char* pLevel, const char* pAction,
        const char* pContent);

    char _time[12];
    char _level[3];
        char _action[30];
    char _content[2048];
};

class SrcLocation_T : public string
{
public:
    SrcLocation_T(char* file, int line);
};

class StrException_T : public exception
{
public:
    StrException_T(const char* what) throw();
    StrException_T(const string& what) throw();
    ~StrException_T()throw(){};

    virtual const char* what() const throw();

private:
    std::string _what;
};

typedef struct GetoptInfo {
	unsigned int init;	/* When properly initialized, set to 0xF123456F */
	int err;	/* if error message should be printed */
	int ind;	/* index into parent argv vector */
	int opt;	/* character checked for validity */
	char *arg;	/* argument associated with option */
	char *place;	/* saved position in an arg */
} GetoptInfo_T, *GetoptInfoPtr_T;

void initDaemon( void );
char* getSubStr( const char* p_strIn,int pos, const char* p_delimer,
        char* p_strOut);
void createPath( const string& path );
string& ltrim(string& s);
string& rtrim(string& s);
void start(const char* process);
void stop(const char* process);
void kill(const char* process);
void doSignalSIGTERM(sighandler_t handler);
void setSignalHandler(int sig, sighandler_t handler);

/* Getopt routines */
void getoptReset(GetoptInfo *const opt);
int getopt(GetoptInfo *const opt, int nargc, char **const nargv, const char *const ostr);


#endif
