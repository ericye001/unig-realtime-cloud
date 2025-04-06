#ifndef ILM_GAMESERVER_ACTION_HPP
#define ILM_GAMESERVER_ACTION_HPP

// common definations
#include "SrvInfo.h"

#include "common/EpollUtils.h"

// message definations
#include "message/Server2ILM.h"
#include "message/PM2ILM.h"
     


                 
class GameServerAction;
class GameServerAcceptor;
class GameServerListener;                                                                              
//
// GameServerAction
//
class GameServerAction : public Action_T
{
public:
    static GameServerAction& instance();
    
    virtual void execute(Connection_T* pConn);
private:
    static GameServerAction* _pInstance;
};
       
//         
// GameServerAcceptor
//     
class GameServerAcceptor : public NonBlockConnection_T, public Action_T
{
public:
    static GameServerAcceptor& instance();
    static void free();
    
    virtual void execute(Connection_T* pConn);
    
private:
    GameServerAcceptor();
    virtual ~GameServerAcceptor();
    
    GameServerAcceptor(const GameServerAcceptor& rhs);
    GameServerAcceptor& operator=(const GameServerAcceptor& rhs);
    
private:
    static GameServerAcceptor* _pInstance;
};

//
// GameServerListener
//      
class GameServerListener : public NonBlockConnection_T
{
public:
    GameServerListener(SocketInterface_T* pSocket);
    virtual ~GameServerListener();
    bool join(bool success);
    void leave();
   
    
    SrvInfo_T& srvInfo();
    
    uint16_t srvId();
    void srvId(uint16_t id);    
    uint16_t infoBusPort();
    void infoBusPort(uint16_t port);
    
    int playerNum();
    void playerNum(int32_t num);
    int32_t load();

    bool unaccomplishedQuery();

    void setServerGroupID(uint16_t groupID);
    uint16_t getServerGroupID();

    void  getIsland(list<uint16_t>& lstIsLand);

    //zone mgr
    void addZone(uint16_t zoneID);
    void removeZone(uint16_t zoneID);

    void getZoneButIslandOnServer(list<uint16_t>& lstZone);
    void getZoneOnServer(list<uint16_t>& lstZone);

    void ServerLockMapCmd(ServerLockMapRequest lockreq);
    void onServerLockMapReply(ServerLockMapResponse response);

    void ServerUnLockMapCmd(ServerUnLockMapRequest unlockreq);
    void onServerUnLockMapReply(ServerUnLockMapResponse response);

    void StartPartition(StartPartitionRequestMsg_T cmd);
    void StartPartitionReply(StartPartitionResponseMsg_T reply);


    void queryStat(ServerStatusQueryMsg_T& queryMsg);
    void onQueryStatReply(ServerStatusReplyMsg_T& reply);
    
    void updateMST(UpdateMSTRequestMsg_T& updateMSTMsg);    
    void onUpdateMSTReply(UpdateMSTResponseMsg_T& reply);

    bool isConnSucc();
    void completeConn(bool _bSucc);
    void clearQuery();
private:
    GameServerListener(const GameServerListener& rhs);
    GameServerListener& operator=(const GameServerListener& rhs);
    
private:
    bool      m_bConnSucc;
    uint16_t  m_nCompleteQuery;
    SrvInfo_T m_srvInfo;
    int32_t   m_iplayerNum;
    int32_t   m_iload;
    list<uint16_t> m_lstZone;       
    uint16_t m_nServerGroupID;
};

#endif
