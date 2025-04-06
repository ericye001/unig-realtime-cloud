#ifndef IGO_LOGINSERVER_ACTION_HPP
#define IGO_LOGINSERVER_ACTION_HPP

// common definations
#include "common/EpollUtils.h"

// message definations
#include "message/Server2IGO.h"
#include "message/Login2IGO.h"

// local classes
#include "GameServerListener.h"

class LoginServerAction_T;
class LoginServerAcceptor_T;
class LoginServerListener_T;

//
// LoginServerAction
//
class LoginServerAction_T : public Action_T
{
public:
    virtual void execute(Connection_T* pConn);
};


//         
// LoginServerAcceptor_T
//     
class LoginServerAcceptor_T : public Connection_T, public Action_T
{
public:
    static LoginServerAcceptor_T& instance();
    static void free();
    
    virtual void execute(Connection_T* pConn);
    
private:
    LoginServerAcceptor_T();
    virtual ~LoginServerAcceptor_T();
    
    LoginServerAcceptor_T(const LoginServerAcceptor_T& rhs);
    LoginServerAcceptor_T& operator=(const LoginServerAcceptor_T& rhs);
    
private:
    static LoginServerAcceptor_T* _pInstance;
};

//
// LoginServerListener_T
//      
class LoginServerListener_T : public Connection_T
{
public:
    LoginServerListener_T(SocketInterface_T* pSocket);
    virtual ~LoginServerListener_T();
        
    bool gameServerJoin(GameServerListener_T* listener);
    bool gameServerReJoin(GameServerListener_T* listener);
    void updateMST(UpdateMSTRequestMsg_T& updateMSTMsg);
    void onUpdateMSTReply(UpdateMSTResponseMsg_T& reply);
    
private:
    LoginServerListener_T(const LoginServerListener_T& rhs);
    LoginServerListener_T& operator=(const LoginServerListener_T& rhs);
};
#endif

