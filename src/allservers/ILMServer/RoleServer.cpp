#include "RoleServer.h"
// common definations
#include "GlobalArgs.h"

// message definations


// local class definations
#include "ServerManager.h"
#include "Zone.h"
#include "ZoneMgr.h"


RoleServerAcceptor* RoleServerAcceptor::_pInstance = 0;
RoleServerAcceptor& RoleServerAcceptor::instance()
{
    if (_pInstance)
    {
        return *_pInstance;
    }
    _pInstance = new RoleServerAcceptor;
    if (!_pInstance)
    {
        throw StrException_T(
            string("Fail to new RoleServerAcceptor\n"
                "\tat RoleServerAcceptor::instance")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    return *_pInstance;
}

void RoleServerAcceptor::free()
{
    if (_pInstance)
    {
        delete _pInstance;
        _pInstance = 0;
    }
}

RoleServerAcceptor::RoleServerAcceptor()
{
    CommonArgs_T& args = CommonArgs_T::instance();
    SocketInterface_T* pSocket = new ServerSocket_T(args._acceptRoleServerPort);
    if (!pSocket)
    {
        throw StrException_T(
            string("Fail to new ServerSocket_T\n"
                "\t at RoleServerAcceptor::RoleServerAcceptor()")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    socket(pSocket);
    event(EPOLLIN|EPOLLHUP|EPOLLERR);
    action(this);
    //configureBlocking(false);
}

RoleServerAcceptor::~RoleServerAcceptor()
{
    
}

void RoleServerAcceptor::execute(Connection_T* pConn)
{
    SocketInterface_T* pSocket = ((ServerSocket_T*)pConn->socket())->accept();
    cout << "ILM accept RoleServer" << endl;

    if(!pSocket)
    {
        cout << "Failed to accept RoleServer" << endl;
        return;
    }
    RoleServerListener* listener = new RoleServerListener(pSocket);
    if (!listener)
    {
        delete pSocket;
        throw StrException_T(
            string("Fail to new RoleServerListener(pSocket)\n"
                "\tat RoleServerAcceptor::execute")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    ServerManager::instance().addRoleServer(listener);
    ServerManager::instance().setTimer();
}

RoleServerAction* RoleServerAction::_pInstance = 0;
RoleServerAction& RoleServerAction::instance()
{
    if (_pInstance)
    {
        return *_pInstance;
    }
    static RoleServerAction instance;
    _pInstance = &instance;
    return *_pInstance;
}

void RoleServerAction::execute(Connection_T* pConn)
{

    RoleServerListener* listener = static_cast<RoleServerListener*>(pConn);
    BasePacket_T pkg;
    try
      {
        
        if(listener->geteventType()& EPOLLIN )
          { 
            listener->read();    
            BasePacket_T pkg;  
            //if( pkg.getInputPacket(listener,true))
            while( pkg.getInputPacket(listener,true))
              {                
                switch(BaseMessage_T::type(&pkg))
                {
                  
                case MsgType::REMOVE_SERVER_RESPONSE:
                  {              

                    ServerLeaveResponseMsg_T reply;
                    reply.unpack(&pkg);
                    cout<<"  REMOVE_SERVER_RESPONSE  Role Server ID= "<< reply.m_nServerID   <<endl;
                  }

                  break;
          
                case MsgType::SERVER_STATUS_REPLY:
                  {              
                    // cout<<"ROLE_SERVER_STATUS_REPLY "<<endl;
                    ServerStatusReplyMsg_T reply;
                    reply.unpack(&pkg);
                    listener->onQueryStatReply(reply);     
                  }
                  
                  break;
                  
                case MsgType::UPDATE_MST_RESPONSE:
                  { 
                    UpdateMSTResponseMsg_T updateMSTResp;
                    updateMSTResp.unpack(&pkg);
                    listener->onUpdateMSTReply(updateMSTResp);
                  }
                  
                  break;  
                  
                case MsgType::SERVER_CONNECT_ILM_REQUEST:
                  {   
                     cout<<" Role Server Connect ILMServer Request "<<endl;
                     char* string={"Connectin  To  Send Connect Msg  Time :" };
                     ServerManager::instance().showCurTime(string);
                     // ServerManager::instance().setTimer();
                     ServerConnectILMRequestMsg_T request;                
                     request.unpack(&pkg);
                     request._srvInfo._infoBusPort=0;
                     listener->srvInfo()= request._srvInfo;        
                     listener->completeConn(true);
                    
                  }
                  
                  break;
                  
                default:
                  
                  cout << "Receive Unknown Message From ProxyManager Server " <<BaseMessage_T::type(&pkg) << endl;
                  
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
              // cout<<"EPOLLOUT OUT "<< listener->getOutputBuffer()->clength() <<endl;
              listener->write();

           
            }   
            listener->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);
            ServerEpoll::instance().setup(listener);
          }

      }
    
    catch (exception& e)
      {
        cout << "exception: " << e.what() << endl
             << "\tat RoleServerAction::execute"
             << SrcLocation_T(__FILE__, __LINE__) << endl;
        ServerEpoll::instance().remove(listener);
        ServerManager::instance().removeRoleServer(listener);
        return;
      }
    catch (...)
      {
        cout << "exception: " << endl
             << "\tat RoleServerAction::execute"
             << SrcLocation_T(__FILE__, __LINE__) << endl;
        ServerEpoll::instance().remove(listener);
        ServerManager::instance().removeRoleServer(listener);
        return;
      }

}

//
//
//
RoleServerListener::RoleServerListener(SocketInterface_T* pSocket)
 
{

  m_iplayerNum=0;
  m_iload=0;
  m_nCompleteQuery=0;
  m_bConnSucc=false;
  m_srvInfo._infoBusPort=0;
  socket(pSocket);
  event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
  action(&RoleServerAction::instance());
    
 
}

RoleServerListener::~RoleServerListener()
{

  
    
}

bool RoleServerListener::isConnSucc()
{

  return m_bConnSucc;

}


void RoleServerListener::clearQuery()
{

 m_nCompleteQuery=0;


}




void RoleServerListener::completeConn(bool _bSucc)
{
  m_bConnSucc=_bSucc;
}



bool RoleServerListener::join(bool success)
{

  ServerConnectILMResponseMsg_T response;    
  response._result = success;
  response._srvInfo = m_srvInfo;
  response._infoBusMaxMsgNum =10;
  response._infoBusBatchNum = 20;
  response.m_nMaxServerNum=30;
  response.m_nMaxMsgLength=40;

  try
    {
     
      response.write(this);
      event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
      ServerEpoll::instance().setup(this);
    }
  catch (exception& e)
    {
      cout << "exception: " << e.what() << endl
           << "\tat RoleServerListener::join"
           << SrcLocation_T(__FILE__, __LINE__) << endl;
      return false;
    }
 
  return true;

}


void RoleServerListener::leave()
{
    
}

void RoleServerListener::queryStat(ServerStatusQueryMsg_T& queryMsg)
{
    queryMsg.write(this);
    event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
    ServerEpoll::instance().setup(this);
    m_nCompleteQuery++;
}

void RoleServerListener::onQueryStatReply(ServerStatusReplyMsg_T& reply)
{

  m_iplayerNum = reply._playerNum;
  m_iload = reply._cpuLoad;
  if(m_nCompleteQuery >0 )
    {
      m_nCompleteQuery--;
    }
  else
    {
      m_nCompleteQuery = 0;
    }
  event(EPOLLIN|EPOLLHUP|EPOLLERR);
  ServerEpoll::instance().setup(this);
 
}


SrvInfo_T& RoleServerListener::srvInfo()
{
    return m_srvInfo;
}

uint16_t RoleServerListener::srvId()
{
    return m_srvInfo._id;
}

void RoleServerListener::srvId(uint16_t id)
{
    m_srvInfo._id = id;
}



void RoleServerListener::removeGS(ServerLeaveRequestMsg_T& request)
{
    request.write(this);
    event(EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
    ServerEpoll::instance().setup(this);

}



int32_t RoleServerListener::playerNum()
{
    return m_iplayerNum;
}

void RoleServerListener::playerNum(int32_t num)
{
    m_iplayerNum = num;
}

int32_t RoleServerListener::load()
{
    return m_iload;
}



bool RoleServerListener::unaccomplishedQuery()
{

  if(m_nCompleteQuery>0)
    {
      //return true;
      return  false;
    }
  else
    {
      return  false;
    }
}





void RoleServerListener::updateMST(UpdateMSTRequestMsg_T& updateMSTMsg)
{
    updateMSTMsg.write(this);
    event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
    ServerEpoll::instance().setup(this);
   
}

void RoleServerListener::onUpdateMSTReply(UpdateMSTResponseMsg_T& reply)
{
 
  
  event(EPOLLIN|EPOLLHUP|EPOLLERR);
  ServerEpoll::instance().setup(this);
}
