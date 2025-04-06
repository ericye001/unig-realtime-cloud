#include "ConsoleListener.h"


// local class definations

ConsoleAcceptor* ConsoleAcceptor::_pInstance = 0;
ConsoleAcceptor& ConsoleAcceptor::instance()
{
    if (_pInstance)
    {
      return *_pInstance;
    }
    _pInstance = new ConsoleAcceptor;
    if (!_pInstance)
    {
        throw StrException_T(
            string("Fail to new ConsoleAcceptor\n"
                "\tat ConsoleAcceptor::instance")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    return *_pInstance;
}

void ConsoleAcceptor::free()
{
    if (_pInstance)
    {
        delete _pInstance;
        _pInstance = 0;
    }
}

ConsoleAcceptor::ConsoleAcceptor()
{

  cout << "ConsoleAcceptor::ConsoleAcceptor() " << endl;
    SocketInterface_T* pSocket = new ServerSocket_T(
        CommonArgs_T::instance()._acceptConsolePort);
    if (!pSocket)
    {
        throw StrException_T(
            string("Fail to new ServerSocket_T\n"
                "\t at ConsoleAcceptor::ConsoleAcceptor()")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    socket(pSocket);
    event(EPOLLIN|EPOLLHUP|EPOLLERR);
    action(this);
    //configureBlocking(false);
 
}

ConsoleAcceptor::~ConsoleAcceptor()
{
    
}

void ConsoleAcceptor::execute(Connection_T* pConn)
{
    SocketInterface_T* pSocket = ((ServerSocket_T*)pConn->socket())->accept();
   
  
    if(!pSocket)
    {
        cout << "Failed to accept Console" << endl;
        return;
    }
    ConsoleListener* listener = new ConsoleListener(pSocket);
    if (!listener)
    {
        delete pSocket;
        throw StrException_T(
            string("Fail to new ConsoleListener(pSocket)\n"
                "\tat ConsoleAcceptor::execute")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    ServerManager::instance().addConsole(listener);
    cout<<" add a ConsoleListener "<<endl;
}





ConsoleAction* ConsoleAction::_pInstance = 0;
ConsoleAction& ConsoleAction::instance()
{
    if (_pInstance)
    {
        return *_pInstance;
    }
    static ConsoleAction instance;
    _pInstance = &instance;
    return *_pInstance;
}


void ConsoleAction::execute(Connection_T* pConn)
{    
  
   
  ConsoleListener* listener =  static_cast<ConsoleListener*>(pConn);
  BasePacket_T pkg;
    try
    {
      if(listener->geteventType()& EPOLLIN )
        {
          listener->read();    
          BasePacket_T pkg;          
          // if( pkg.getInputPacket(listener,true))
          while( pkg.getInputPacket(listener,true))
            {
              switch(BaseMessage_T::type(&pkg))
                {                  
                case MsgType::SERVER_STATUS_QUERY:
                  {      
                    GetPlanMsgRequest_T getPlan;  
                    getPlan.unpack(&pkg);
                    cout<<"   From ID : "<<getPlan._fromID <<endl
                        <<"    To  ID : "<<getPlan._toID  <<endl
                        <<"To  zoneID : "<<getPlan._zoneID  <<endl;
                      
                    ServerManager::instance().getPlanFromConsole(getPlan._fromID,getPlan._toID,getPlan._zoneID);

                  }
                  break;

                default:
                  {
                    cout << "Receive Unknown Message From Console  Type:" <<BaseMessage_T::type(&pkg) << " LEN :"<< pkg.length() << endl;                
                  }
                    break;
                }
            }
        }
      
      if(listener->getOutputBuffer()->isCompeleteMsg())
        {
          listener->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET|EPOLLOUT);
          ServerEpoll::instance().setup(listener);
          listener->seteventType(EPOLLOUT);

        } 
      else
        {
          listener->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);
          ServerEpoll::instance().setup(listener);
          
        }

      if(listener->geteventType()& EPOLLOUT)
        {
          
          while(listener->getOutputBuffer()->isCompeleteMsg()) 
            {
              
              listener->write();
             
            }      
           listener->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);
           ServerEpoll::instance().setup(listener);
              
        }
    }
    catch (exception& e)
    {
      cout << "exception: " << e.what() << endl
           << "\tat ConsoleAction::execute"
           << SrcLocation_T(__FILE__, __LINE__) << endl;
      ServerManager::instance().onConsoleException(listener);
      return;
    }

   
        
    
}

//
//
//
ConsoleListener::ConsoleListener(SocketInterface_T* pSocket)
{
  

    socket(pSocket);
    event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
    action(&ConsoleAction::instance());

    

}

ConsoleListener::~ConsoleListener()
{
    
}



