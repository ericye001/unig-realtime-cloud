/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: SeamlessService.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 无缝服务类

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _SEAMLESS_SERVICE_H__
#define _SEAMLESS_SERVICE_H__

inline int SyncMsgHandler(void* pMsg, int length){return 0;}

class SeamlessService
{
public:
    // return the global single instance's reference of class ServerEngine_T
    static SeamlessService& Instance();
    // initialize the ServerEngine_T: setup ServerContextMgr, connect IGO, etc
    bool static Init(ILMArgs& args);

    vector<Map2SrvInfo>& MST();
    void MST(vector<Map2SrvInfo>& mst);

    // set and return the adjoining servers of this server
    vector<uint16>& AdjoiningSrvs();
    void AdjoiningSrvs(vector<uint16>& adjoiningSrvs);

    // set and return the adjoining maps of this serve that r
    vector<uint16>& AdjoiningMaps();
    void AdjoiningMaps(vector<uint16>& adjoiningMaps);

    // return true if this server manage that map given by arg, else return false
    bool IsMapOnServer(uint16 mapId);

    void MaxAsyncMsg(uint16 n);
    void AsyncBatchNum(uint16 n);

    void MaxMsgLength(uint16 n);
    void MaxServerNum(uint16 n);

    ServerInfo& GetServerInfo();
    void SetServerInfo(ServerInfo& srvInfo);

    MsgQueue& GetCommondQueue() {return m_oCommonQueue;}
    MsgQueue& GetResultQueue() {return m_nResultQueue;}


private:
    SeamlessService(ILMArgs& args);
    SeamlessService(const SeamlessService& rhs);
    SeamlessService& operator=(const SeamlessService& rhs);

    void InitILM(ILMArgs& args);
private:
    static SeamlessService* m_pInstance;

    ServerInfo m_oServerInfo;
    int m_nMaxAsyncMsg;
    int m_nAsyncBatchNum;
    int m_nMaxMsgLength;
    int m_nMaxServerNum;

    MsgQueue m_oCommonQueue;
    MsgQueue m_nResultQueue;

};

#endif

