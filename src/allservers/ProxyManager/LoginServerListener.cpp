#include "LoginServerListener.h"
// common definations
#include "GlobalArgs.h"
#include "message/Login2PM.h"
// message definations


LoginServerAcceptor* LoginServerAcceptor::_pInstance = 0;
LoginServerAcceptor& LoginServerAcceptor::instance()
{
    if (_pInstance)
    {
        return *_pInstance;
    }
    _pInstance = new LoginServerAcceptor;
    if (!_pInstance)
    {
        throw StrException_T(
            string("Fail to new LoginServerAcceptor\n"
                "\tat LoginServerAcceptor::instance")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    return *_pInstance;
}

void LoginServerAcceptor::free()
{
    if (_pInstance)
    {
        delete _pInstance;
        _pInstance = 0;
    }
}

LoginServerAcceptor::LoginServerAcceptor()
{
    SocketInterface_T* pSocket = new ServerSocket_T(
        CommonArgs_T::instance().m_iacceptLoginServerPort);
    if (!pSocket)
    {
        throw StrException_T(
            string("Fail to new ServerSocket_T\n"
                "\t at LoginServerAcceptor::LoginServerAcceptor()")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    socket(pSocket);
    event(EPOLLIN|EPOLLHUP|EPOLLERR);
    action(this);
    // configureBlocking(false);
}

LoginServerAcceptor::~LoginServerAcceptor()
{
    
}

void LoginServerAcceptor::execute(Connection_T* pConn)
{
    SocketInterface_T* pSocket = ((ServerSocket_T*)pConn->socket())->accept();
    cout << "ILM accept login server : " <<  pSocket->sfd()  << endl;

    if(!pSocket)
    {
        cout << "Failed to accept GameServer" << endl;
        return;
    }
    LoginServerListener* listener = new  LoginServerListener(pSocket);
    if (!listener)
    {
        delete pSocket;
        throw StrException_T(
            string("Fail to new LoginServerListener_T(pSocket)\n"
                "\tat LoginServerAcceptor::execute")+
            SrcLocation_T(__FILE__, __LINE__));
    }

    listener->setLoginID( ProxyManager::instance().allocLoginID() );
    ProxyManager::instance().addLoginServer(listener);
    cout << "Succ to accept LoginServer ID " << listener->getLoginID() << endl;
    cout << " < -----------------  " <<endl<< endl;
}

void LoginServerAction::execute(Connection_T* pConn)
{    
  
 
     LoginServerListener* listener = 
       static_cast<LoginServerListener*>(pConn);
     //  cout<<"void LoginServerAction::execute(Connection_T* pConn) "<<endl;
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
                case MsgType::ADD_CLIENT_TO_PM_REQUEST:
                  {
                    
                    
                    try
                      {
                        
                        AddClientToPMRequestMsg_T AddClient2PMReq;
                        AddClient2PMReq.unpack(&pkg);
                        /*
                          cout<<"  --------------->   "<< listener->getLoginID()  <<endl
                          <<"  ADD CLIENT(ID ="<< AddClient2PMReq.m_nUserID 
                          <<") TO PM REQUEST From LoginServer ID= "<< listener->getLoginID() << endl;
                        **/
                        ProxyManager::instance().setTimer();
                        listener->addClient(listener->getLoginID(),AddClient2PMReq);
                        
                      }
                    catch (exception& e)
                      {
                        cout << "exception: " << e.what() << endl
                             << "\tat LoginServerAction::execute"
                             << SrcLocation_T(__FILE__, __LINE__) << endl;
                        ProxyManager::instance().onLoginServerException(listener);
                        return;
                      }

                  }
                  break;
         
                default:
                  // wrong message
                  cout << "receive unknown message from login server  MsgType= " <<BaseMessage_T::type(&pkg) << endl;
                  break;
                }
            }
        }

          


      if(listener->getOutputBuffer()->isCompeleteMsg())
        {
          listener->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET|EPOLLOUT);
          ProxyManageEpoll::instance().setup(listener);
          listener->seteventType(EPOLLOUT);
          //cout<<"Login  Change To Epoll OUT "<<endl;
          
        } 
      else
        {
          listener->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);
          ProxyManageEpoll::instance().setup(listener);
          
        }
      
      if(listener->geteventType()& EPOLLOUT)
        {
          
          while(listener->getOutputBuffer()->isCompeleteMsg()) 
            {
              //cout<<"Login EPOLLOUT OUT "<< listener->getOutputBuffer()->clength() <<endl;
              listener->write();
             
            }      
          listener->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);
          ProxyManageEpoll::instance().setup(listener);
          
        }

    } 
     catch (exception& e)
       {
         cout << "exception: " << e.what() << endl
              << "\tat LoginServerAction::execute"
              << SrcLocation_T(__FILE__, __LINE__) << endl;
         ProxyManager::instance().onLoginServerException(listener);
         return;
       }





















     /*



     try
       {       
         pkg.read(listener); 
         if(pkg.length()<sizeof(uint32_t))
           {
             ProxyManager::instance().onLoginServerException(listener);
             return;
           }
       }
     catch (exception& e)
       {
         cout << "exception: " << e.what() << endl
              << "\tat LoginServerAction::execute"
              << SrcLocation_T(__FILE__, __LINE__) << endl;
         ProxyManager::instance().onLoginServerException(listener);
         return;
       }
     
     switch(BaseMessage_T::type(&pkg))
       {
       case MsgType::ADD_CLIENT_TO_PM_REQUEST:
         {


             try
               {
                 
                 AddClientToPMRequestMsg_T AddClient2PMReq;
                 AddClient2PMReq.unpack(&pkg);
                 
                 cout<<"  --------------->   "<< listener->getLoginID()  <<endl
                     <<"  ADD CLIENT(ID ="<< AddClient2PMReq.m_nUserID 
                     <<") TO PM REQUEST From LoginServer ID= "<< listener->getLoginID() << endl;
                 
                 ProxyManager::instance().setTimer();
                 listener->addClient(listener->getLoginID(),AddClient2PMReq);
               
               }
             catch (exception& e)
               {
                 cout << "exception: " << e.what() << endl
                      << "\tat LoginServerAction::execute"
                      << SrcLocation_T(__FILE__, __LINE__) << endl;
                 ProxyManager::instance().onLoginServerException(listener);
                 return;
               }

         }
         break;
         
       default:
         // wrong message
           cout << "receive unknown message from login server  MsgType= " <<BaseMessage_T::type(&pkg) << endl;
         break;
       }
 
     if(listener->geteventType()& EPOLLIN )
        {

          listener->read();    
          BasePacket_T pkg;
          if( pkg.getInputPacket(listener,true))
            {

              switch(BaseMessage_T::type(&pkg))
                {
                case MsgType::ADD_CLIENT_TO_PM_REQUEST:
                  {
                    try
                      {
                        AddClientToPMRequestMsg_T AddClient2PMReq;
                        AddClient2PMReq.unpack(&pkg);
                        cout<<"  --------------->   "<< listener->getLoginID()  <<endl
                            <<"  ADD CLIENT(ID ="<< AddClient2PMReq.m_nUserID 
                            <<") TO PM REQUEST From LoginServer ID= "<< listener->getLoginID() << endl;
                        ProxyManager::instance().setTimer();
                        listener->addClient(listener->getLoginID(),AddClient2PMReq);  
                      }
                    catch (exception& e)
                      {
                        cout << "exception: " << e.what() << endl
                             << "\tat LoginServerAction::execute"
                             << SrcLocation_T(__FILE__, __LINE__) << endl;
                        ProxyManager::instance().onLoginServerException(listener);
                        return;
                      }    
                  }
                  break;
         
                default:
                  // wrong message
                  cout << "receive unknown message from login server  MsgType= " <<BaseMessage_T::type(&pkg) << endl;
                  break;
                }
            }
        }


     if(listener->getOutputBuffer()->clength()< int(sizeof(uint32_t))) 
       {          
         
          listener->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);
          ProxyManageEpoll::instance().setup(listener);
          
       } 
     
     if(listener->geteventType()& EPOLLOUT)
       {
          
         if(listener->getOutputBuffer()->isCompeleteMsg()) 
           {
             cout<<"ADD_CLIENT_TO_PM_RESPONSE : "<<endl;
             
             listener->write();
             listener->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);
             ProxyManageEpoll::instance().setup(listener);
              
           }               
       }




     BasePacket_T pkg;
     try
       {       
         pkg.read(listener); 
         if(pkg.length()<sizeof(uint32_t))
           {
             ProxyManager::instance().onLoginServerException(listener);
             return;
           }
       }
     catch (exception& e)
       {
         cout << "exception: " << e.what() << endl
              << "\tat LoginServerAction::execute"
              << SrcLocation_T(__FILE__, __LINE__) << endl;
         ProxyManager::instance().onLoginServerException(listener);
         return;
       }
     
     switch(BaseMessage_T::type(&pkg))
       {
       case MsgType::ADD_CLIENT_TO_PM_REQUEST:
         {


             try
               {
                 
                 AddClientToPMRequestMsg_T AddClient2PMReq;
                 AddClient2PMReq.unpack(&pkg);
                 cout<<"  --------------->   "<< listener->getLoginID()  <<endl
                     <<"  ADD CLIENT(ID ="<< AddClient2PMReq.m_nUserID 
                     <<") TO PM REQUEST From LoginServer ID= "<< listener->getLoginID() << endl;
                 ProxyManager::instance().setTimer();
                 listener->addClient(listener->getLoginID(),AddClient2PMReq);

               }
             catch (exception& e)
               {
                 cout << "exception: " << e.what() << endl
                      << "\tat LoginServerAction::execute"
                      << SrcLocation_T(__FILE__, __LINE__) << endl;
                 ProxyManager::instance().onLoginServerException(listener);
                 return;
               }

         }
         break;
         
       default:
         // wrong message
           cout << "receive unknown message from login server  MsgType= " <<BaseMessage_T::type(&pkg) << endl;
         break;
       }

     **/

}

//
//
//
LoginServerListener::LoginServerListener(SocketInterface_T* pSocket)
{
  
    socket(pSocket);
    Action_T* pAction = new LoginServerAction;
    if (!pAction)
    {
        throw StrException_T(
            string("Fail to new LoginServerAction\n"
                "\tat LoginServerListener_T::LoginServerListener_T")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    action(pAction);
    event(EPOLLIN|EPOLLHUP|EPOLLERR);

}

LoginServerListener::~LoginServerListener()
{
  

    
}

bool LoginServerListener::addClient(uint16_t loginid,AddClientToPMRequestMsg_T AddClientToPMRequest)
{


  
  if(ProxyManager::instance().addClient2Proxy(loginid,AddClientToPMRequest))
    return false;
  else
    return true;
  
  //block !read from this proxy ,if success return true or return false

}


uint16_t  LoginServerListener::getLoginID()
{
  return m_nLoginID;
}


void LoginServerListener::setLoginID(uint16_t loginid)
{

  m_nLoginID=loginid;

}

