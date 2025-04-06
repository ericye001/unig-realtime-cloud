#include "Define.h"
#include "QueryThread.h"


CQueryThread::CQueryThread(void)
{
    m_pThread = NULL;
    m_pSqlConnect = NULL;
    ::InitializeCriticalSection(&m_crit);
}

CQueryThread::~CQueryThread(void)
{
    Exit();

    ::DeleteCriticalSection(&m_crit);
}

bool CQueryThread::Init(const char *pszHost, int nPort, const char *pszDatabase, const char *pszUser, const char *pszPassword)
{
    m_pSqlConnect = mysql_init((MYSQL*)NULL);
    if(!m_pSqlConnect) {
        printf("初始化MySQL出错\n");
        return false;
    }

    if(!mysql_real_connect(m_pSqlConnect, pszHost, pszUser, pszPassword, pszDatabase, nPort, NULL, 0)) {
        printf("连接MySQL服务器出错\n");
        mysql_close(m_pSqlConnect);
        m_pSqlConnect = NULL;
        return false;
    }

    m_pThread = MyThread::CreateNew(*this, false, 50);

    return true;
}

void CQueryThread::Exit(void)
{
    delete m_pThread;
    m_pThread = NULL;
}

int CQueryThread::OnThreadCreate(void)
{
    return 0;
}

int CQueryThread::OnThreadProcess(void)
{
    QueryCommandSet pQueryCommandSet;

    if(!m_setQueryCmd.empty())
    {
        ::EnterCriticalSection(&m_crit);
        pQueryCommandSet = m_setQueryCmd;
        m_setQueryCmd.clear();
        ::LeaveCriticalSection(&m_crit);
    } 
    else 
    {
        return 0;
    }

    QueryCommandSet::iterator iter = pQueryCommandSet.begin();
    for(;iter != pQueryCommandSet.end();) {
        QueryCommand cmd = *iter;
        DWORD dwStartTime = timeGetTime();
        Query(cmd);
        printf("[数据库] 查询时间:%d\n", timeGetTime() - dwStartTime);

        QueryCommandSet::iterator itLast = iter;
        iter++;
        pQueryCommandSet.erase(itLast);
    }

    return 0;
}

int CQueryThread::OnThreadWorkEvent(void)
{
    return 0;
}

int CQueryThread::OnThreadDestroy(void)
{
    return 0;
}

void CQueryThread::Query(QueryCommand &cmd)
{
    char fmtSql[256] = "";
    sprintf(fmtSql, "select password, id from account where name = '%s';", cmd.szUser);

    if(!mysql_query(m_pSqlConnect, fmtSql)) {
        MYSQL_RES *res = mysql_store_result(m_pSqlConnect);

        int nNumRows = mysql_num_rows(res);
        if(nNumRows == 1) {
            MYSQL_FIELD *fd = mysql_fetch_field(res);
            if(fd) {
                MYSQL_ROW row = mysql_fetch_row(res);

                if(strcmp(cmd.szPassword, row[0]) == 0) {
                    QueryResult result;
                    result.nQueryId = cmd.nQueryId;
                    result.nUserId = atoi(row[1]);
                    m_setQueryResult.push_back(result);

                    mysql_free_result(res);
                    return;
                }
            }
        }

        mysql_free_result(res);
    }

    QueryResult result;
    result.nQueryId = cmd.nQueryId;
    result.nUserId = 0;

    m_setQueryResult.push_back(result);

    return;
}

void CQueryThread::AddQuery(unsigned int nQueryId, const char *pszUser, const char *pszPassword)
{
    ::EnterCriticalSection(&m_crit);
    QueryCommand cmd;
    cmd.nQueryId = nQueryId;
    strncpy(cmd.szUser, pszUser, 32);
    strncpy(cmd.szPassword, pszPassword, 32);

    m_setQueryCmd.push_back(cmd);

    ::LeaveCriticalSection(&m_crit);
}

QueryResult& CQueryThread::TakeResult(void)
{
    ::EnterCriticalSection(&m_crit);
    QueryResult result = {0, 0};
    QueryResultSet::iterator iter = m_setQueryResult.begin();

    if(iter != m_setQueryResult.end()) {
        result = *iter;
        m_setQueryResult.erase(iter);
        ::LeaveCriticalSection(&m_crit);
        return result;
    }

    ::LeaveCriticalSection(&m_crit);
    return result;
}