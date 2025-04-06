#ifndef ILM_ROLESERVER_ACTION_HPP
#define ILM_ROLESERVER_ACTION_HPP

// common definations
#include "SrvInfo.h"
#include "common/EpollUtils.h"

// message definations
#include "message/Server2ILM.h"
#include "message/PM2ILM.h"
                      
class RoleServerAction;
class RoleServerAcceptor;
class RoleServerListener;                                                                              
//
// RoleServerAction
//
class RoleServerAction : public Action_T
{
public:
    static RoleServerAction& instance();
    
    virtual void execute(Connection_T* pConn);
private:
    static RoleServerAction* _pInstance;
};
       
//         
// RoleServerAcceptor
//     
class RoleServerAcceptor : public NonBlockConnection_T, public Action_T
{
public:
    static RoleServerAcceptor& instance();
    static void free();
    
    virtual void execute(Connection_T* pConn);
    
private:
    RoleServerAcceptor();
    virtual ~RoleServerAcceptor();
    
    RoleServerAcceptor(const RoleServerAcceptor& rhs);
    RoleServerAcceptor& operator=(const RoleServerAcceptor& rhs);
    
private:
    static RoleServerAcceptor* _pInstance;
};

//
// RoleServerListener
//      
class RoleServerListener : public NonBlockConnection_T
{
public:
    RoleServerListener(SocketInterface_T* pSocket);
    virtual ~RoleServerListener();
    bool join(bool success);
    void leave();
    void queryStat(ServerStatusQueryMsg_T& queryMsg);
    void onQueryStatReply(ServerStatusReplyMsg_T& reply); 
    SrvInfo_T& srvInfo();
    

    bool unaccomplishedQuery();

    uint16_t srvId();
    void srvId(uint16_t id);
    int playerNum();
    void playerNum(int32_t num);
    int32_t load();
    void removeGS(ServerLeaveRequestMsg_T& request);
    void updateMST(UpdateMSTRequestMsg_T& updateMSTMsg);    
    void onUpdateMSTReply(UpdateMSTResponseMsg_T& reply);
    

    bool isConnSucc();
    void completeConn(bool _bSucc);

    void clearQuery();
   
private:
    RoleServerListener(const RoleServerListener& rhs);
    RoleServerListener& operator=(const RoleServerListener& rhs);
    
private:
    bool      m_bConnSucc;
    uint16_t  m_nCompleteQuery;
    SrvInfo_T m_srvInfo;
    int32_t   m_iplayerNum;
    int32_t   m_iload;
};

#endif
