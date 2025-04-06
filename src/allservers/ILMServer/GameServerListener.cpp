#include "GameServerListener.h"
// common definations
#include "GlobalArgs.h"

// message definations


// local class definations
#include "ServerManager.h"
#include "Zone.h"

#include "ZoneMgr.h"

GameServerAcceptor* GameServerAcceptor::_pInstance = 0;
GameServerAcceptor& GameServerAcceptor::instance()
{
    if (_pInstance)
    {
        return *_pInstance;
    }
    _pInstance = new GameServerAcceptor;
    if (!_pInstance)
    {
        throw StrException_T(
            string("Fail to new GameServerAcceptor\n"
                "\tat GameServerAcceptor::instance")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    return *_pInstance;
}

void GameServerAcceptor::free()
{
    if (_pInstance)
    {
        delete _pInstance;
        _pInstance = 0;
    }
}

GameServerAcceptor::GameServerAcceptor()
{
    CommonArgs_T& args = CommonArgs_T::instance();
    SocketInterface_T* pSocket = new ServerSocket_T(args._acceptGameServerPort);
    if (!pSocket)
    {
        throw StrException_T(
            string("Fail to new ServerSocket_T\n"
                "\t at GameServerAcceptor::GameServerAcceptor()")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    socket(pSocket);
    event(EPOLLIN|EPOLLHUP|EPOLLERR);
    action(this);
    //configureBlocking(false);
}

GameServerAcceptor::~GameServerAcceptor()
{
    
}

void GameServerAcceptor::execute(Connection_T* pConn)
{
    SocketInterface_T* pSocket = ((ServerSocket_T*)pConn->socket())->accept();
   
    if(!pSocket)
    {
        cout << "Failed to accept GameServer" << endl;
        return;
    }
    GameServerListener* listener = new GameServerListener(pSocket);
    if (!listener)
    {
        delete pSocket;
        throw StrException_T(
            string("Fail to new GameServerListener(pSocket)\n"
                "\tat GameServerAcceptor::execute")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    ServerManager::instance().addGameServer(listener);
    ServerManager::instance().setTimer();
      
}

GameServerAction* GameServerAction::_pInstance = 0;
GameServerAction& GameServerAction::instance()
{
    if (_pInstance)
    {
        return *_pInstance;
    }
    static GameServerAction instance;
    _pInstance = &instance;
    return *_pInstance;
}

void GameServerAction::execute(Connection_T* pConn)
{

    GameServerListener* listener = static_cast<GameServerListener*>(pConn);
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
                  
                case MsgType::SERVER_STATUS_REPLY:
                  {              
     
                    //cout<<"SERVER_STATUS_REPLY "<<endl;
                    ServerStatusReplyMsg_T reply;
                    reply.unpack(&pkg);
                    listener->onQueryStatReply(reply);  

                  }

                  break;
                  
                case MsgType::UPDATE_MST_RESPONSE:
                  {

                    cout<<" UPDATE_MST_RESPONSE  "<<endl;
                    UpdateMSTResponseMsg_T updateMSTResp;
                    updateMSTResp.unpack(&pkg);
                    listener->onUpdateMSTReply(updateMSTResp);
                   
                  }

                  break;  
                      
                case MsgType::SERVER_CONNECT_ILM_REQUEST:
                  {      
                     cout<<" SERVER_CONNECT_ILM_REQUEST "<<endl;
                     char* string={"Connectin  To  Send Connect Msg  Time :" };
                     ServerManager::instance().showCurTime(string);  
                     //ServerManager::instance().setTimer();
                     ServerConnectILMRequestMsg_T request;                
                     request.unpack(&pkg);
                     cout<<"size :"<<request.size()<<endl;
                     cout<<"ip :"<<request._srvInfo._ip<<endl;
                     cout<<"port :"<<request._srvInfo._gamePort<<endl;
                     listener->srvInfo()= request._srvInfo;
                     listener->completeConn(true);
                   
                        
                  }

                  break;
                 
                case MsgType::LOCK_MAP_RESPONSE:
                  {      
                    
                    ServerLockMapResponse response;
                    response.unpack(&pkg);
                    listener->onServerLockMapReply(response);
      
                  }
                  break;

                case MsgType::UNLOCK_MAP_RESPONSE:
                  {      

                    cout<<"    UNLOCK_MAP_RESPONSE  " <<endl;
                    ServerUnLockMapResponse response;
                    response.unpack(&pkg);
                    if(response.m_nResult>0)
                      {
                        
                        ServerManager::instance().m_needReplay.remove(listener->srvId());
                      }
                  }
                  break;


                  case MsgType::START_PARTITION_RESPONSE:
                  {      

                    StartPartitionResponseMsg_T response;
                    response.unpack(&pkg);
                    listener->StartPartitionReply(response);
         
                  }

                  break;
                      
                default:

                  cout << "Receive Unknown Message From Game Server" <<BaseMessage_T::type(&pkg) << endl;
                  
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
              //cout<<"GameServer EPOLLOUT OUT "<< listener->getOutputBuffer()->clength() <<endl;
              listener->write();
             
            }      
           listener->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);
           ServerEpoll::instance().setup(listener);
              
        }
      
    }
    catch (exception& e)
    {
      cout << "exception: " << e.what() << endl
           << "\tat GameServerAction::execute"
           << SrcLocation_T(__FILE__, __LINE__) << endl;
      ServerEpoll::instance().remove(listener);
      ServerManager::instance().removeGameServer(listener);
      return;
    }




}

//
//
//
GameServerListener::GameServerListener(SocketInterface_T* pSocket)
 
{

  m_iplayerNum=0;
  m_iload=0;
  m_nCompleteQuery=0;
  m_nServerGroupID=0;
  m_bConnSucc=false;
  socket(pSocket);
  event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
  action(&GameServerAction::instance());

    
}



GameServerListener::~GameServerListener()
{

  
    
}



void GameServerListener::clearQuery()
{

 m_nCompleteQuery=0;


}


bool GameServerListener::isConnSucc()
{
  
  return m_bConnSucc;
}


void GameServerListener::completeConn(bool _bSucc)
{
  m_bConnSucc=_bSucc;
}



bool GameServerListener::join(bool success)
{

  // make response to game server, tell it has been added
   
  ServerConnectILMResponseMsg_T response;    
  CommonArgs_T& args = CommonArgs_T::instance();
  // make response to game server, tell it has been added
  response._result = success;
  response._srvInfo = m_srvInfo;
  response._infoBusMaxMsgNum = args._infoBusMaxMsgNum;
  response._infoBusBatchNum = args._infoBusBatchNum;
  response.m_nMaxServerNum = 999;
  response.m_nMaxMsgLength =  888;
  cout<<"GameServer Join::   ID :"<<m_srvInfo._id<<endl
      <<"                    IP :"<<m_srvInfo._ip<<endl
      <<"              GamePort :"<<m_srvInfo._gamePort<<endl
      <<"              UBusPort :"<<m_srvInfo._infoBusPort<<endl;
 
  try
    {
     
      response.write(this);
      event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
      ServerEpoll::instance().setup(this);
    }
  catch (exception& e)
    {
      cout << "exception: " << e.what() << endl
           << "\tat GameServerListener::join"
           << SrcLocation_T(__FILE__, __LINE__) << endl;
      return false;
    }
  cout<<"          <-------------------------------------------------"<<endl;
  return true;
}


void GameServerListener::leave()
{
    
}

void GameServerListener::queryStat(ServerStatusQueryMsg_T& queryMsg)
{
    queryMsg.write(this);
    event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
    ServerEpoll::instance().setup(this);
    m_nCompleteQuery++;
}

void GameServerListener::onQueryStatReply(ServerStatusReplyMsg_T& reply)
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
  vector<MapInfo_T>::iterator iter;
  
  for(uint16_t i=0;i<reply._mapNum;i++)
    {

      uint16_t zoneID = UniG_ILM_Config::Instance()->GetZoneIDByMapUnitID( reply._maps[i]._id );
      UniG_ILM_Zone* pZone  = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(zoneID);
      pZone->SetZoneLoad(0);
      
    }
 
  for(uint16_t i=0;i<reply._mapNum;i++)
    {

      uint16_t zoneID = UniG_ILM_Config::Instance()->GetZoneIDByMapUnitID( reply._maps[i]._id );
      UniG_ILM_Zone* pZone  = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(zoneID);
      pZone->SetZoneLoad( pZone->GetZoneLoad()+ reply._maps[i]._playerNum);
      
    }

  event(EPOLLIN|EPOLLHUP|EPOLLERR);
  ServerEpoll::instance().setup(this);
 
}

void GameServerListener::updateMST(UpdateMSTRequestMsg_T& updateMSTMsg)
{

  cout<<"UpdateMSTRequestMsg  Size : "<< updateMSTMsg.size()
      <<"                   Number : "<< updateMSTMsg._allSrvNum
      <<"               MSTItemNum : "<< updateMSTMsg._MSTItemNum
      <<endl;
  updateMSTMsg.write(this);
  event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR);
  ServerEpoll::instance().setup(this); 
  ServerManager::instance().m_needReplay.push_back(srvId());
  //m_nCompleteQuery++;
}

void GameServerListener::onUpdateMSTReply(UpdateMSTResponseMsg_T& reply)
{

  event(EPOLLIN|EPOLLHUP|EPOLLERR);
  ServerEpoll::instance().setup(this);
  ServerManager::instance().m_needReplay.remove(srvId());
  // m_nCompleteQuery--;
}


SrvInfo_T& GameServerListener::srvInfo()
{
    return m_srvInfo;
}

uint16_t GameServerListener::srvId()
{
    return m_srvInfo._id;
}

void GameServerListener::srvId(uint16_t id)
{
    m_srvInfo._id = id;
}

int32_t GameServerListener::playerNum()
{
    return m_iplayerNum;
}

void GameServerListener::playerNum(int32_t num)
{
    m_iplayerNum = num;
}

int32_t GameServerListener::load()
{
    return m_iload;
}

void GameServerListener::infoBusPort(uint16_t port)
{
    m_srvInfo._infoBusPort = port;
}


uint16_t  GameServerListener::infoBusPort()
{

  return  m_srvInfo._infoBusPort;

}
bool GameServerListener::unaccomplishedQuery()
{

  if(m_nCompleteQuery>0)
    return true;
  else
    return  false;
}

void GameServerListener::setServerGroupID(uint16_t groupID)
{
  m_nServerGroupID=groupID;
}

uint16_t GameServerListener::getServerGroupID()
{


  return m_nServerGroupID;


}




void  GameServerListener::getIsland(list<uint16_t>& lstIsLand)
{
  lstIsLand.empty();
  list<uint16_t>::iterator iter;
  for(iter = m_lstZone.begin(); iter != m_lstZone.end(); iter++)
    {
      UniG_ILM_Zone* pZone  = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iter);
      if(pZone == NULL)
        {
          continue;
        }
      else
        {
          if(pZone->IsIsland())
            {
              lstIsLand.push_back(pZone->GetZoneID());
            }
        }
    }
}









void GameServerListener::addZone(uint16_t zoneID)
{

  if(find(m_lstZone.begin(), m_lstZone.end(), zoneID) == m_lstZone.end())
	{		
          UniG_ILM_Zone* pZone;
          pZone = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(zoneID);
          if(pZone)
		{
                  m_lstZone.push_back(zoneID);
                  pZone->SetServerID(m_srvInfo._id);
                  //cout<<"zone  = "<<pZone->GetZoneID()<<" in Server ="<<pZone->GetServerID()<<endl;
                  /*
                  list<MapUnitInfo> lstMap;
                  UniG_ILM_Config::Instance()->GetMapUnitByZone(zoneID, lstMap);
                  list<MapUnitInfo>::iterator iterMap;
                  for( iterMap = lstMap.begin() ; iterMap != lstMap.end() ; iterMap++ )
                    {
                      (*iterMap).serverID=m_srvInfo._id;
                      cout<<"mapUnitName  = "<<(*iterMap).mapUnitName<<" in Server ="<<(*iterMap).serverID<<endl;
                    }
                  **/
		}
         
	}

}


void GameServerListener::removeZone(uint16_t zoneID)
{
	list<uint16_t>::iterator iterZone;
	iterZone = find(m_lstZone.begin(), m_lstZone.end(), zoneID);
	if(iterZone != m_lstZone.end())
	{
		m_lstZone.erase(iterZone);
	}
}


void GameServerListener::getZoneButIslandOnServer(list<uint16_t>& lstZone)
{
	list<uint16_t>::iterator iter;
	UniG_ILM_Zone* pZone;
	for(iter = m_lstZone.begin(); iter != m_lstZone.end(); iter++)
	{
          pZone = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iter);
		if(pZone && !pZone->IsIsland())
		{
			lstZone.push_back(*iter);
		}
	}
}

void GameServerListener::getZoneOnServer(list<uint16_t>& lstZone)
{


  lstZone = m_lstZone;


}


void GameServerListener::ServerLockMapCmd(ServerLockMapRequest lockreq)
{
    lockreq.write(this);
    event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
    ServerEpoll::instance().setup(this);
    ServerManager::instance().m_needReplay.push_back(srvId());
}


void GameServerListener::onServerLockMapReply(ServerLockMapResponse response)
{

  if(response.m_nResult>0)
    {                        
      ServerManager::instance().m_needReplay.remove(srvId());
    }
  event(EPOLLIN|EPOLLHUP|EPOLLERR);
  ServerEpoll::instance().setup(this);
}


void GameServerListener::ServerUnLockMapCmd(ServerUnLockMapRequest unlockreq)
{
    unlockreq.write(this);
    event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
    ServerEpoll::instance().setup(this);
    ServerManager::instance().m_needReplay.push_back(srvId());
}


void GameServerListener::onServerUnLockMapReply(ServerUnLockMapResponse response)
{

  if(response.m_nResult>0)
    {                        
      ServerManager::instance().m_needReplay.remove(srvId());
    }
  event(EPOLLIN|EPOLLHUP|EPOLLERR);
  ServerEpoll::instance().setup(this);
}


void GameServerListener::StartPartition(StartPartitionRequestMsg_T partitionreq)
{

  partitionreq.write(this);
  event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
  ServerEpoll::instance().setup(this);
  ServerManager::instance().m_needReplay.push_back(srvId());

}


void GameServerListener::StartPartitionReply(StartPartitionResponseMsg_T reply)
{


  if(reply._result>0)
    {                        
      ServerManager::instance().m_needReplay.remove(srvId());
    }
  event(EPOLLIN|EPOLLHUP|EPOLLERR);
  ServerEpoll::instance().setup(this);

}

