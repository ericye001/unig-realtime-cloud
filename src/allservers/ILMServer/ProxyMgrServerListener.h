#ifndef IGO_LOGINSERVER_ACTION_HPP
#define IGO_LOGINSERVER_ACTION_HPP

// common definations
#include "common/EpollUtils.h"
#include "common/Buffer.h"
// message definations
#include "message/Server2ILM.h"
#include "message/PM2ILM.h"

// local classes
#include "GameServerListener.h"
#include "RoleServer.h"

class ProxyMgrServerAction;
class ProxyMgrServerAcceptor;
class ProxyMgrServerListener;

class ProxyMgrServerAction : public Action_T
{
public:
    virtual void execute(Connection_T* pConn);
};

class ProxyMgrServerAcceptor : public NonBlockConnection_T, public Action_T
{
public:
    static ProxyMgrServerAcceptor& instance();
    static void free();    
    virtual void execute(Connection_T* pConn);
    
private:
    ProxyMgrServerAcceptor();
    virtual ~ProxyMgrServerAcceptor();
    
    ProxyMgrServerAcceptor(const ProxyMgrServerAcceptor& rhs);
    ProxyMgrServerAcceptor& operator=(const ProxyMgrServerAcceptor& rhs);
    
private:
    static ProxyMgrServerAcceptor* _pInstance;
};

//
// ProxyMgrServerListener_T
//    
  
class ProxyMgrServerListener : public NonBlockConnection_T
{
 public:
  ProxyMgrServerListener(SocketInterface_T* pSocketint);
  virtual ~ProxyMgrServerListener();    
  bool gameServerJoin(GameServerListener* listener);
  bool gameServerInfo(vector<GameServerListener*> listener);
  bool gameServerLeave(GameServerListener* listener);



  bool roleServerJoin(RoleServerListener* listener);
  bool roleServerInfo(vector<RoleServerListener*> listener);
  bool roleServerLeave(RoleServerListener* listener);

  void updateMST(UpdateMSTRequestMsg_T theMessage);
  void onUpdateMSTReply();

  void updateServerLoad(ServerLoadToPM theMessage);
  void onUpdateServerLoadReply();

  bool isCompleteQuery();


 private:
  ProxyMgrServerListener(const ProxyMgrServerListener& rhs);
  ProxyMgrServerListener& operator=(const ProxyMgrServerListener& rhs);

 public:
  int16_t  m_iComplete;
  
    
};
#endif

