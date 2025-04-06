#include "ProxyMgrServerListener.h"
// common definations
#include "Map2SrvInfo.h"
#include "MapInfo.h"
#include "SrvInfo.h"
#include "GlobalArgs.h"

// message definations
#include "message/Server2ILM.h"
#include "message/PM2ILM.h"

// local class definations
#include "ServerManager.h"
#include "GameServerListener.h"

ProxyMgrServerAcceptor* ProxyMgrServerAcceptor::_pInstance = 0;
ProxyMgrServerAcceptor& ProxyMgrServerAcceptor::instance()
{
    if (_pInstance)
    {
        return *_pInstance;
    }
    _pInstance = new ProxyMgrServerAcceptor;
    if (!_pInstance)
    {
        throw StrException_T(
            string("Fail to new ProxyMgrServerAcceptor\n"
                "\tat ProxyMgrServerAcceptor::instance")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    return *_pInstance;
}

void ProxyMgrServerAcceptor::free()
{
    if (_pInstance)
    {
        delete _pInstance;
        _pInstance = 0;
    }
}

ProxyMgrServerAcceptor::ProxyMgrServerAcceptor()
{
    SocketInterface_T* pSocket = new ServerSocket_T(
        CommonArgs_T::instance()._acceptLoginServerPort);
    if (!pSocket)
    {
        throw StrException_T(
            string("Fail to new ServerSocket_T\n"
                "\t at ProxyMgrServerAcceptor::ProxyMgrServerAcceptor()")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    socket(pSocket);
    event(EPOLLIN|EPOLLHUP|EPOLLERR);
    action(this);
    //configureBlocking(false);
}

ProxyMgrServerAcceptor::~ProxyMgrServerAcceptor()
{
    
}

void ProxyMgrServerAcceptor::execute(Connection_T* pConn)
{
try
  {
    SocketInterface_T* pSocket = ((ServerSocket_T*)pConn->socket())->accept();
    
    if(!pSocket)
      {
        cout << "Failed to accept GameServer" << endl;
        return;
    }

    ProxyMgrServerListener* listener = new ProxyMgrServerListener(pSocket);
    if (!listener)
    {
        delete pSocket;
        throw StrException_T(
            string("Fail to new ProxyMgrServerListener(pSocket)\n"
                "\tat ProxyMgrServerAcceptor::execute")+
            SrcLocation_T(__FILE__, __LINE__));
    }
 
    ServerManager::instance().addProxyMgrServer(listener);  
  }
 catch(exception& e)
   {          
     cout<<" Create ProxyMgrServerListener Fail !!"<<endl;
   }  
  

}

void ProxyMgrServerAction::execute(Connection_T* pConn)
{    
    ProxyMgrServerListener* listener = 
        static_cast<ProxyMgrServerListener*>(pConn); 
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
                              
                    case MsgType::REMOVE_SERVER_RESPONSE:
                      {              
  
                        ServerLeaveResponseMsg_T reply;
                        reply.unpack(&pkg);
                        //  cout<<"  REMOVE_SERVER_RESPONSE   Game Server ID= "<< reply.m_nServerID   <<endl;
                        
                      }
                      
                      break;

                    case MsgType::UPDATE_MST_RESPONSE:
                      {
                        //cout << "----LoginServerAction_T::execute UPDATE_MST_RESPONSE" << endl;
                        listener->onUpdateMSTReply();
                      }
                      break;
                      
                    case MsgType::SERVER_LOAD_PM_REPLAY:
                      {
                        //cout << "----LoginServerAction_T::execute UPDATE_MST_RESPONSE" << endl ;
                        ServerLoadToPMReplyMsg srvloadreply;
                        // cout<<"SERVER_LOAD_PM_REPLAY RESULT:"<< srvloadreply._result <<endl;
                        listener->onUpdateServerLoadReply();
                      }
                      break;
                      
                    case MsgType::PM_CONNECT_ILM_REQUEST:
                      {    
        
                        cout<<"Get a Message PM_CONNECT_ILM_REQUEST!!!"<<endl;
                        PMConnectILMRequestMsg_T request;
                        request.unpack(&pkg);
                        
                        PMConnectILMResponseMsg_T response;
                        response.m_version._MaxServerId=9999;
                        response.m_version._Version=2;
                        response.write(listener);     
                        //cout<<"Rev MaxServerId: "<<request.m_version._MaxServerId<<"\t Versoin :"<<request.m_version._Version<<endl;
                        vector<GameServerListener*> _allGameServerlist;
                        vector<RoleServerListener*> _allRoleServerlist;
                        ServerManager::instance().getAllGameServer(_allGameServerlist);
                        ServerManager::instance().getAllRoleServer(_allRoleServerlist);
                        if(_allGameServerlist.size()>0)
                          {
                            listener->gameServerInfo(_allGameServerlist);
                           
                          }  
                      
                        if(_allRoleServerlist.size()>0)
                          {

                            listener->roleServerInfo(_allRoleServerlist);
                            
                          }   
                       
                        listener->event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR);
                        ServerEpoll::instance().setup(listener);
                      }
                      break;
                      
                      
                    case MsgType::ADD_SERVER_RESPONSE:
                      {
                        cout<<"Get  Add Server Response ! "<<endl; 
                        AddServerResponseMsg_T response;
                        response.unpack(&pkg);
                        if(response._addedServer._id>0)
                          {
                            (*listener).m_iComplete--;
                            if(!(*listener).m_iComplete)
                              {
                                cout<<" Add Server Success ! "<<endl; 
                              }
                          }
                        char* string={"ADD_SERVER_RESPONSE  :" };
                        ServerManager::instance().showCurTime(string);  
                        // cout<<" Add Server Success "<<endl
                        // <<"  <---------------------  "<<endl;
                          
                      }
                      break;

                    default:
                      // wrong message
                      cout << "Receive Unknown Message From PMServer" <<BaseMessage_T::type(&pkg) << endl;
                      break;
                    }
                } 
            }

       

          if(listener->getOutputBuffer()->isCompeleteMsg()) 
            {
              
              listener->event(EPOLLOUT|EPOLLOUT|EPOLLHUP|EPOLLERR);
              ServerEpoll::instance().setup(listener);
              listener->seteventType(EPOLLOUT);
            } 
  

          if(listener->geteventType()& EPOLLOUT)
            {

               while(listener->getOutputBuffer()->isCompeleteMsg()) 
                {
                  //cout<<" Send Sth To PM "<<endl;
                  listener->write();
  
                }      
               listener->event(EPOLLIN|EPOLLHUP|EPOLLERR);
               ServerEpoll::instance().setup(listener); 
            }
        }
      catch(exception& e)
        {
          cout << "exception: " << e.what() << endl
             << "\tat ProxyMgrServerAction::execute"
             << SrcLocation_T(__FILE__, __LINE__) << endl;
          ServerManager::instance().onProxyMgrServerException(listener);
          
        }

}

//
//
//
ProxyMgrServerListener::ProxyMgrServerListener(SocketInterface_T* pSocket)
{
  
    socket(pSocket);
    Action_T* pAction = new ProxyMgrServerAction;
    if (!pAction)
    {
        throw StrException_T(
            string("Fail to new ProxyMgrServerAction\n"
                "\tat ProxyMgrServerListener::ProxyMgrServerListener")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    action(pAction);
    event(EPOLLIN|EPOLLHUP|EPOLLERR);
    m_iComplete=0;

}

ProxyMgrServerListener::~ProxyMgrServerListener()
{
    
}

bool ProxyMgrServerListener::gameServerJoin(GameServerListener* listener)
{
  try
    {

      AddServerRequestMsg_T request;
      request._newServer = listener->srvInfo();
      ServerManager::instance().setMSTVersion(request.m_version);
      cout<<"ADD GameServer ip   : "<< request._newServer._ip<<endl
          <<"              port  : "<< request._newServer._gamePort<<endl
          <<"               id   : "<< request._newServer._id<<endl;
      request.write(this);
      event(EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLIN);
      ServerEpoll::instance().setup(this);
      m_iComplete++;
      return true;

    }
  catch (exception& e)
    {
      cout << "exception: " << e.what() << endl
           << "\tat ProxyMgrServerListener::join"
           << SrcLocation_T(__FILE__, __LINE__) << endl;
    }
  return false;

}
 





bool ProxyMgrServerListener::gameServerLeave(GameServerListener* listener)
{
  try
    {
      cout<<"GameServer "<<listener->srvId() <<"  Leave !"<<endl<<endl<<endl<<endl<<endl<<endl;
      ServerLeaveRequestMsg_T request ;
      request.m_nServerID=listener->srvId();
      request.write(this);
      event(EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLIN);
      ServerEpoll::instance().setup(this);
      return true;

    }
  catch (exception& e)
    {
      cout << "exception: " << e.what() << endl
           << "\tat ProxyMgrServerListener::gameServerleave"
           << SrcLocation_T(__FILE__, __LINE__) << endl;
    }
  return false;

}


bool ProxyMgrServerListener::gameServerInfo(vector<GameServerListener*> listener)
{
     
  try
    {
      vector<GameServerListener*>::const_iterator curIt;
      for (curIt = listener.begin(); 
           curIt != listener.end(); 
           curIt++)
        {
          gameServerJoin(*curIt);
      
        }

      
      
    }
  catch (exception& e)
    {
        cout << "exception: " << e.what() << endl
             << "\tat ProxyMgrServerListener::join"
             << SrcLocation_T(__FILE__, __LINE__) << endl;
    }
  return false;
    
}









bool ProxyMgrServerListener::roleServerJoin(RoleServerListener* listener)
{
  try
    {
      AddServerRequestMsg_T request;
      request._newServer = listener->srvInfo();
      request._newServer._infoBusPort = 0;
      cout<<"ADD RoleServer ip   : "<< request._newServer._ip<<endl
          <<"              port  : "<< request._newServer._gamePort<<endl
          <<"               id   : "<< request._newServer._id<<endl;
      ServerManager::instance().setMSTVersion(request.m_version);
      request.write(this);
      event(EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLIN);
      ServerEpoll::instance().setup(this);
      m_iComplete++;
      return true;      

    }
  catch (exception& e)
    {
      cout << "exception: " << e.what() << endl
           << "\tat ProxyMgrServerListener::roleServerJoin"
           << SrcLocation_T(__FILE__, __LINE__) << endl;
    }
  return false;
}
 


bool ProxyMgrServerListener::roleServerInfo(vector<RoleServerListener*> listener)
{
     
  try
    {
      vector<RoleServerListener*>::const_iterator curIt;
      for (curIt = listener.begin(); 
           curIt != listener.end(); 
           curIt++)
        {
          roleServerJoin(*curIt);
      
        }

      
      
    }
  catch (exception& e)
    {
        cout << "exception: " << e.what() << endl
             << "\tat ProxyMgrServerListener::roleServerInfo"
             << SrcLocation_T(__FILE__, __LINE__) << endl;
    }
  return false;
    
}



bool ProxyMgrServerListener::roleServerLeave(RoleServerListener* listener)
{
  try
    {
      cout<<"RoleServer "<<listener->srvId() <<"  Leave !"<<endl<<endl<<endl<<endl<<endl<<endl;
      ServerLeaveRequestMsg_T request ;
      request.m_nServerID=listener->srvId();
      request.write(this);
      event(EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLIN);
      ServerEpoll::instance().setup(this);
      return true;

    }
  catch (exception& e)
    {
      cout << "exception: " << e.what() << endl
           << "\tat ProxyMgrServerListener::roleServerleave"
           << SrcLocation_T(__FILE__, __LINE__) << endl;
    }
  return false;

}



void ProxyMgrServerListener::updateMST(UpdateMSTRequestMsg_T theMessage)
{
  //cout<<"ProxyMgrServerListener::updateMST() "<<endl;
  theMessage.write(this);
  event(EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLIN);
  ServerEpoll::instance().setup(this);
  m_iComplete++;
 //cout<<"ProxyMgrServerListener::updateMST() "<<endl;

}

void ProxyMgrServerListener::onUpdateMSTReply()
{
    m_iComplete--;
    event(EPOLLIN|EPOLLHUP|EPOLLERR);
    ServerEpoll::instance().setup(this);
    // cout<<"void ProxyMgrServerListener::onUpdateMSTReply()"<<endl;
}


bool ProxyMgrServerListener::isCompleteQuery()
{
  if(m_iComplete>0) return false;
  else return true;
}


void ProxyMgrServerListener::updateServerLoad(ServerLoadToPM theMessage)
{
  
 theMessage.write(this);
 event(EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLIN);
 ServerEpoll::instance().setup(this);
 m_iComplete++;


}

void ProxyMgrServerListener::onUpdateServerLoadReply()
{

  event(EPOLLIN|EPOLLHUP|EPOLLERR);
  ServerEpoll::instance().setup(this);
  m_iComplete--;
   
}
