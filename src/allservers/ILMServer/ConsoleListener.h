#ifndef CONSOLE_LISTENER_HPP
#define CONSOLE_LISTENER_HPP

// common
#include "common/EpollUtils.h"

#include "message/Console2ILM.h"

#include "ServerManager.h"
#include "GlobalArgs.h"
// message definations
// local class definations

//
// ConsoleAction

//


class ConsoleAction;
class ConsoleAcceptor;
class ConsoleListener;

class ConsoleAction : public Action_T
{


public:
  static ConsoleAction& instance();
    
    virtual void execute(Connection_T* pConn);
 private:
    static ConsoleAction* _pInstance;
};


//         
// ConsoleAcceptor
//     
class ConsoleAcceptor : public NonBlockConnection_T , public Action_T
{
public:
    static ConsoleAcceptor& instance();
    static void free();
    
    virtual void execute(Connection_T* pConn);
    
private:
    ConsoleAcceptor();
    virtual ~ConsoleAcceptor();
    
    ConsoleAcceptor(const ConsoleAcceptor& rhs);
    ConsoleAcceptor& operator=(const ConsoleAcceptor& rhs);
    
private:
    static ConsoleAcceptor* _pInstance;
};

//
// ConsoleListener
//      
class ConsoleListener : public NonBlockConnection_T
{
public:
    ConsoleListener(SocketInterface_T* pSocket);
    virtual ~ConsoleListener();
         
   
private:
    ConsoleListener(const ConsoleListener& rhs);
    ConsoleListener& operator=(const ConsoleListener& rhs);
   
};



#endif

