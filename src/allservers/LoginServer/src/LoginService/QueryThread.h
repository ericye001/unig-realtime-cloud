#ifndef _QUERYTHREAD_H
#define _QUERYTHREAD_H

#include <windows.h>
#include "..\AppService\MyThread.h"
#include <mysql.h>
#include <list>

struct QueryCommand {
    unsigned int nQueryId;
    char szUser[32];
    char szPassword[32];
};

struct QueryResult {
    unsigned int nQueryId;
    unsigned int nUserId;
};

class CQueryThread : public iThreadEvent {
public:
    CQueryThread(void);
    ~CQueryThread(void);

    bool Init(const char *pszHost, int nPort, const char *pszDatabase, const char *pszUser, const char *pszPassword);
    void AddQuery(unsigned int nQueryId, const char *pszUser, const char *pszPassword);
    void Exit(void);
    QueryResult& TakeResult(void);

    int OnThreadCreate(void);
    int OnThreadDestroy(void);
    int OnThreadWorkEvent(void);
    int OnThreadProcess(void);

protected:
    void Query(QueryCommand &cmd);

protected:
    MYSQL *m_pSqlConnect;
    MyTimerMS m_timerConnectPrc;
    MyThread *m_pThread;
    typedef std::list<QueryCommand> QueryCommandSet;

    QueryCommandSet m_setQueryCmd;

    typedef std::list<QueryResult> QueryResultSet;
    QueryResultSet m_setQueryResult;

    CRITICAL_SECTION m_crit;
};

#endif 