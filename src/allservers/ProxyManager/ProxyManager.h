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


#ifndef ILMSERVER_PROXYMANAGER_HPP
#define ILMSERVER_PROXYMANAGER_HPP
#include "LoginServerListener.h"
#include "message/Proxy2PM.h"
#include "message/Client2Login.h"
#include "message/PM2ILM.h"
#include "message/Login2PM.h"
#include "message/Server2ILM.h"
#include "message/MSTVersion.h"
#include "ServerLoadToPM.h"
#include "Map2SrvInfo.h"
#include "SrvInfo.h"
#include "GlobalArgs.h"
class LoginServerListener;

class ProxyManager;
typedef EpollSingleton_T<ProxyManager> ProxyManageEpoll;
//
// ProxyItem
//
class ProxyItem
{
public:
    ProxyItem();
    ~ProxyItem();

    uint32_t _proxyIp;
    uint16_t _proxyPort;
    int _playerNum1;
    int _playerNum2;
    uint16_t _proxyid;
    NonBlockConnection_T *_pConnection;
    bool _valid;
    bool operator<(const ProxyItem& proxy) const;
};


//
// ProxyManager
//
class  ProxyManager
{
public:
    static ProxyManager& instance();

    int addProxy(Connection_T* pConn);

    int addProxy( NonBlockConnection_T* pConn);
    int removeProxy(Connection_T* pConn);
    void updateProxy(Connection_T *pConn, bool valid);   
 
    int queryProxies();
    void updateProxy(Connection_T* pConn, ProxyStatusReplyMsg_T* pStatMsg);
    void updateProxy(Connection_T* pConn, ProxyConnectLoginRequestMsg_T* pStatMsg,uint16_t proxyid);
    

    void showServers();
    void setTimer();
    void showCurTime(char* string);

    uint32_t addClient2Proxy(uint16_t loginid,AddClientToPMRequestMsg_T AddClientToPMRequest);
    

    ProxyItem* allocPriProxyForClient(void);
    ProxyItem* allocPriProxyForClient(uint16_t proxyid);
    uint16_t getRoleServerID();


    void logout(uint64_t guid);
    uint16_t allocProxyID();
    void setProxyID(uint16_t maxid);
    ProxyItem* findProxybyID(uint16_t proxyid);    
    uint16_t alreadyHaveThisProxy(ProxyConnectLoginRequestMsg_T *pMsg);


    uint16_t getServers(std::vector<SrvInfo_T> &servers);
    void updateServers(UpdateMSTRequestMsg_T *pUpdateMsg);
    void updateServers(std::vector<SrvInfo_T>& allSrvs);
    void updateServerLoad(std::vector<SrvToPM> srvload);
    bool removeServer(uint16_t ServerID);


    void startAcceptLogin();
    void onILMServerException();


    int startAcceptProxy();
    int connectILM();
    void ILMDie(void);

   

    void loginServerListener(LoginServerListener* pConn);
    void  onLoginServerException(LoginServerListener* pConn);
    uint16_t allocLoginID();
    void setLoginID(uint16_t maxid);
    int  addLoginServer(LoginServerListener* pConn);
    int  removeLoginServer(LoginServerListener* pConn);
    LoginServerListener* findLoginServerbyID(uint16_t loginid);

    void onAddServerMessageReply( AddServerResponseMsg_T *pMsg);

    void onAddServerMessage(NonBlockConnection_T* pConn, AddServerRequestMsg_T *pMsg);

private:
    uint32_t createSession(uint32_t userid);


private:
    ProxyManager();
    ~ProxyManager();
    ProxyManager(const ProxyManager &rhs);
    ProxyManager& operator=(const ProxyManager &rhs);


public:
    bool _ILMConnected;
    MSTVersion _version;
    uint16_t _addServerCount;

private:
    static ProxyManager* _pInstance;
    std::vector<ProxyItem> _proxies;
    std::vector<LoginServerListener *> _login;

    NonBlockConnection_T* _ILMListener;

    std::vector<SrvInfo_T> _servers;
    std::vector<SrvToPM> _srvload;


    std::vector<SrvInfo_T> _roleservers;
    std::vector<SrvToPM> _rolesrvload;

    static time_t _lastTime;
    int _maxProxyID;
    int _maxLoginID;
    time_t _lastTryILM;
    uint32_t _session;
    map<uint32_t,uint32_t> _sessionlist;
   
    struct timeval lastTime;
};















#endif
