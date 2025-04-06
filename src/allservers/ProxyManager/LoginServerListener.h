#ifndef PM_LOGINSERVER_ACTION_HPP
#define PM_LOGINSERVER_ACTION_HPP

// common definations
#include "common/EpollUtils.h"
#include "ProxyManager.h"
#include "message/Message.h"
#include "message/Login2PM.h"
// message definations
// local classes
class LoginServerAction;
class LoginServerAcceptor;
class LoginServerListener;

//
// LoginServerAction
//
class LoginServerAction : public Action_T
{
public:
    virtual void execute(Connection_T* pConn);
};


//         
// LoginServerAcceptor_T
//     
class LoginServerAcceptor : public NonBlockConnection_T, public Action_T
{
public:
    static LoginServerAcceptor& instance();
    static void free();
    virtual void execute(Connection_T* pConn);
    
private:
    LoginServerAcceptor();
    virtual ~LoginServerAcceptor();
    
    LoginServerAcceptor(const LoginServerAcceptor& rhs);
    LoginServerAcceptor& operator=(const LoginServerAcceptor& rhs);
    
private:
    static LoginServerAcceptor* _pInstance;
};

//
// LoginServerListener_T
//      
class LoginServerListener : public NonBlockConnection_T
{
public:
    LoginServerListener(SocketInterface_T* pSocket);
    virtual ~LoginServerListener();
    bool addClient(uint16_t loginid,AddClientToPMRequestMsg_T AddClientToPMRequest);
    uint16_t getLoginID();
    void setLoginID(uint16_t loginid);
private:
    uint16_t m_nLoginID; 
    LoginServerListener(const LoginServerListener& rhs);
    LoginServerListener& operator=(const LoginServerListener& rhs);
};
#endif

