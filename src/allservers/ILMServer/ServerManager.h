#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include <vector>

// common definations
#include "Map2SrvInfo.h"
#include "MapInfo.h"
#include "SrvInfo.h"
#include "common/EpollUtils.h"
#include "common/SocketStream.h"
#include "common/Utils.h"
// message definations
#include "message/Server2ILM.h"
#include "message/PM2ILM.h"

// local class definations
#include "ConsoleListener.h"
#include "ProxyMgrServerListener.h"
#include "GameServerListener.h"
#include "RoleServer.h"
#include "message/MSTVersion.h"
using namespace std;

class ServerManager;
class SrvIdGenerator;
class ConsoleAction;
class ConsoleAcceptor;
class ConsoleListener;

typedef EpollSingleton_T<ServerManager> ServerEpoll;
typedef uint16_t server_id_t;
typedef uint16_t map_id_t;
typedef uint16_t zone_id_t;
//
// ServerManager
//


struct Plan
{

  uint16_t toServerID;
  uint16_t fromServerID;
  uint16_t zoneID;
  uint8_t count;
};


class   ServerManager
{
public:
    static ServerManager& instance();
    ~ServerManager();
  


 public:
    void addConsole(ConsoleListener* listener);
    void onConsoleException(ConsoleListener* listener);



 public:
    /****GAME SERVER OPERATION ***********************************/  
    void addGameServer(GameServerListener* listener);
    void removeGameServer(GameServerListener* listener);
    GameServerListener* findGameServerById(uint16_t srvId);
    GameServerListener* findAnotherGameServer(uint16_t srvId);
    bool getAllGameServer(vector<GameServerListener* >& listener);

    void sendRemoveGameServer(uint16_t ServerID);


    void setTimer();
    void showCurTime(char* string);

    uint16_t getAllGameServerID(vector<uint16_t>& vcGServerID);



 private:
    void toRegular(GameServerListener* listener);
    void allocateSrvId(GameServerListener* listener);
    void allocateInfoBusPort(GameServerListener* listener);

    void allocateRoleSrvId(RoleServerListener* listener);

 public:
  
    void acceptGameServers();
    void acceptRoleServers();
    void timedQueryServerStat();

    void sendServerLoadToPM();

    /****ROLE SERVER OPERATION ***********************************/  
    void addRoleServer(RoleServerListener* listener);
    void removeRoleServer(RoleServerListener* listener);
    RoleServerListener* findRoleServerById(uint16_t srvId);
    bool getAllRoleServer(vector<RoleServerListener* >& listener);

 private:
    void toRegular(RoleServerListener* listener);
    void moveToEnd(GameServerListener* listener);
    void moveToEnd(RoleServerListener* listener);
 public:
    void updateMST();
    bool waitServerReply(uint32_t timeout);
    
     
    void addProxyMgrServer(ProxyMgrServerListener* listener);
    void onProxyMgrServerException(ProxyMgrServerListener* listener);
    void setMSTVersion(MSTVersion  m_version);
    MSTVersion getMSTVersion();



    GameServerListener* getUrgentServer();
    uint32_t getServerLoad(uint16_t serverID);

    void makePlanWhileRequired();
    void getPlanFromConsole(uint16_t fromID,uint16_t toID,uint16_t zoneID);

    void doPlan();
    
    void sendLockToServer(uint16_t zoneID);
    void sendUnLockToServer(uint16_t zoneID);
    void startPartition();




    bool waitGameServerReply(uint32_t timeout);

    void changeMST();
    void recoverMST();
    void updateMST2();


    void getServersInSameGroup(uint16_t serverID, list<uint16_t>& lstServer);


    //mst info
    uint32_t generateMapUnit2Server(map<uint32_t,uint16_t>& map2server);
    uint32_t generateMapUnit2Server(vector<Map2SrvInfo_T>& _MST);

    //test
    void printmst();

    uint32_t fillServerAdjacentMaps(uint16_t serverID,list<uint32_t>& mapuints);
    uint32_t fillServerAdjacentMaps(uint16_t serverID,vector<uint32_t>& mapuints);
    bool hasIdleServer();

    uint16_t getIdleServer();

    GameServerListener* getMinLoadServer(void);

    
    list<uint16_t> m_needReplay;
    uint16_t m_iFlag;
    Plan m_plan;
    MSTVersion  m_version;
private:
    ServerManager();    
    ServerManager(const ServerManager& rhs);
    ServerManager& operator=(const ServerManager& rhs);


    void addGameServerToMST(GameServerListener* listener);
    void queryGameServerStat();
    void queryRoleServerStat();

private:
    static ServerManager* _pInstance;
    vector<GameServerListener*> _prepGameServers;
    vector<GameServerListener*> _regularGameServers;
    vector<GameServerListener*> m_lstIdleServer;

    vector<RoleServerListener*> _prepRoleServers;
    vector<RoleServerListener*> _regularRoleServers;
   
    ProxyMgrServerListener* _pProxyMgrServerListener;
    time_t _lastQueryTime;
    time_t _lastMoveTime;
    struct timeval lastTime;
};

class SrvIdGenerator
{
public:
    static SrvIdGenerator& instance();
    void GSrvIdRecover(uint16_t maxserverid);
    uint16_t allocateOneGSID();

    void RSrvIdRecover(uint16_t maxserverid);
    uint16_t allocateOneRSID();

    uint16_t allocateOneOtherID();
private:
    SrvIdGenerator();
    SrvIdGenerator(const SrvIdGenerator& rhs);
    SrvIdGenerator& operator=(const SrvIdGenerator& rhs);
    
    static SrvIdGenerator* _pInstance;
    uint16_t _gsrvIdCursor;
    uint16_t _rsrvIdCursor;
    uint16_t _otherCursor;
};


#endif
