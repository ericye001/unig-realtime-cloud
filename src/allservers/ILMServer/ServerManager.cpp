#include "ServerManager.h"
// common definations
#include "GlobalArgs.h"


// local class definations
#include "ServerManager.h"
#include <fstream>
#include "Config.h"
#include "ZoneMgr.h"
#include "PlanMgr.h"
ofstream logger("partiton.log");

//---------------------------------------------------------------------
ServerManager* ServerManager::_pInstance = 0;
ServerManager& ServerManager::instance()
{
    if (_pInstance)
    {
        return *_pInstance;
    }
    static ServerManager instance;
    _pInstance = &instance;
    return *_pInstance;
}


ServerManager::ServerManager()
{
    ServerEpoll::init(CommonArgs_T::instance()._maxGameServer+1+1+1);
    m_needReplay.clear();

    _prepGameServers.clear();
    _regularGameServers.clear();
    m_lstIdleServer.clear();    
    _prepRoleServers.clear();
    _regularRoleServers.clear();
    m_version._MaxServerId=0;
    m_version._Version=0;
    m_plan.toServerID=0;
    m_plan.fromServerID=0;
    m_plan.zoneID=0;
    m_plan.count=0;
    m_iFlag=0; //No Plan
    gettimeofday(&lastTime,NULL);
}

ServerManager::~ServerManager()
{
    
}

void ServerManager::addConsole(ConsoleListener* listener)
{

  ServerEpoll::instance().add(listener);

}


void ServerManager::onConsoleException(ConsoleListener* listener)
{

  ServerEpoll::instance().remove(listener);
  delete listener;

}


void ServerManager::addGameServer(GameServerListener* listener)
{
    _prepGameServers.push_back(listener);
    ServerEpoll::instance().add(listener);
}



void ServerManager::sendRemoveGameServer(uint16_t ServerID)
{

      ServerLeaveRequestMsg_T request ;
      request.m_nServerID = ServerID;
      vector<RoleServerListener*>::iterator curIt;
      
      for (curIt = _regularRoleServers.begin();
           curIt != _regularRoleServers.end();
           ++curIt)
        {
          try
            {
              (*curIt)->removeGS(request);

            }
          catch(StreamException_T e)
            {
              cout<<"ServerManager::sendRemoveGameServer error\n";
              ServerEpoll::instance().remove((*curIt));
              ServerManager::instance().removeRoleServer((*curIt)); 
            }
        }
      ServerEpoll::instance().wait(1);


}

void ServerManager::removeGameServer(GameServerListener* listener)
{
    //Send This Message To PM,and delete listener;

  m_iFlag=9;
  list<uint16_t> lstZone;
  listener->getZoneOnServer(lstZone);
  list<uint16_t>::iterator iterZone;
   for(iterZone = lstZone.begin(); iterZone != lstZone.end(); iterZone++)
        {
       
          UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone)->SetServerID(0);
        }
  // GameServerListener* another= findAnotherGameServer(listener->srvId())  ;

  // if(!another)
  {
   
     UniG_ILM_Config::Instance()->RecoveryManagedZone(lstZone, listener->getServerGroupID());
    
  }
  /*
  else
    {

      cout<<"There is another GS(ID="<<another->srvId()<<") exist,Give These maps to it!"<<endl;
      another->setServerGroupID(listener->getServerGroupID());
      
      for(iterZone = lstZone.begin(); iterZone != lstZone.end(); iterZone++)
        {
          cout<<"GameServer ID="<<another->srvId() <<" Has Zone ID="<<*iterZone<<" On"<<endl;
          another->addZone(*iterZone);
          UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone)->SetServerID(another->srvId());
        }
      
      cout<<" <--------------------------"<<endl;
    }
  **/
  
  
  if (!_pProxyMgrServerListener)
    { 
     return;
    }

  _pProxyMgrServerListener->gameServerLeave(listener);
  sendRemoveGameServer(listener->srvId());

  ServerEpoll::instance().wait(1);//Need to Send GameServer Leave Message immediately!!
  vector<GameServerListener*>::iterator curIt;
  for (curIt = _prepGameServers.begin(); 
       curIt != _prepGameServers.end();
       ++curIt)
    {
      if (*curIt == listener)
        {
          _prepGameServers.erase(curIt);    
          delete listener;
          return;
        }
    }
  
  for (curIt = _regularGameServers.begin();
       curIt != _regularGameServers.end();
       ++curIt)
    {
      if (*curIt == listener)
        {
          _regularGameServers.erase(curIt);        
          delete listener;
          return;
        }
    }
}



void ServerManager::addRoleServer(RoleServerListener* listener)
{
    _prepRoleServers.push_back(listener);
    ServerEpoll::instance().add(listener);
}





void ServerManager::removeRoleServer(RoleServerListener* listener)
{
   
  //Send This Message To PM,and delete listener;
  _pProxyMgrServerListener->roleServerLeave(listener);
   ServerEpoll::instance().wait(1);

    vector<RoleServerListener*>::iterator curIt;
    for (curIt = _prepRoleServers.begin(); 
        curIt != _prepRoleServers.end();
        ++curIt)
    {
        if (*curIt == listener)
        {
            _prepRoleServers.erase(curIt);
          
            delete listener;
            return;
        }
    }

    for (curIt = _regularRoleServers.begin();
        curIt != _regularRoleServers.end();
        ++curIt)
    {
        if (*curIt == listener)
        {
            _regularRoleServers.erase(curIt);
           
          
            delete listener;
            return;
        }
    }
}


RoleServerListener* ServerManager::findRoleServerById(uint16_t srvId)
{
    vector<RoleServerListener*>::iterator curIt;
    for (curIt = _regularRoleServers.begin(); 
        curIt != _regularRoleServers.end();
        ++curIt)
    {
        if ((*curIt)->srvId() == srvId)
        {
            return *curIt;
        }
    }
    return NULL;
}

GameServerListener* ServerManager::findAnotherGameServer(uint16_t srvId)
{
    vector<GameServerListener*>::iterator curIt;
    for (curIt = _regularGameServers.begin(); 
        curIt != _regularGameServers.end();
        ++curIt)
    {
        if ((*curIt)->srvId() != srvId)
        {
            return *curIt;
        }
    }
    return NULL;


}


GameServerListener* ServerManager::findGameServerById(uint16_t srvId)
{
    vector<GameServerListener*>::iterator curIt;
    for (curIt = _regularGameServers.begin(); 
        curIt != _regularGameServers.end();
        ++curIt)
    {
        if ((*curIt)->srvId() == srvId)
        {
            return *curIt;
        }
    }
    return NULL;
}



void ServerManager::getServersInSameGroup(uint16_t serverID, list<uint16_t>& lstServer)
{

  uint16_t groupID;
  GameServerListener* pServer = findGameServerById(serverID);
  if(!pServer)
    {
      lstServer.clear();
      return;
    }
  groupID = pServer->getServerGroupID();
  vector<GameServerListener*>::iterator iter;
  for(iter = _regularGameServers.begin(); iter != _regularGameServers.end(); iter++)
    {
      if(((*iter)->getServerGroupID() == groupID) && ((*iter)->srvId() != serverID))
        {
          lstServer.push_back((*iter)->srvId());
        }
    }
  return ;



}



void ServerManager::setTimer()
{
  
    gettimeofday(&lastTime,NULL);

}

void ServerManager::showCurTime(char* string)
{
    struct timeval tpstart;
    gettimeofday(&tpstart,NULL);
    float curTime;
    curTime= 1000000*(tpstart.tv_sec-lastTime.tv_sec)+tpstart.tv_usec-lastTime.tv_usec;
    curTime/=1000;
    cout<<"[Time Use] "<<string<<"   "<< curTime << " ms "<<endl;
    gettimeofday(&lastTime,NULL);
}




void ServerManager::acceptGameServers()
{
 
 
    if (!_pProxyMgrServerListener)
      {      
        return;
      }
    while (_prepGameServers.size() > 0)
      {
        
      
        GameServerListener* listener = _prepGameServers.front(); 
        if(listener->isConnSucc())
          {         
            //   if(listener->srvId()==0)
            {      
              
              allocateSrvId(listener);
              cout<<" GameServer ID allocate to "<<listener->srvId()<<endl;
              allocateInfoBusPort(listener);   
              cout<<" GameServer ID  "<< listener->srvId() <<"  InfoBusPort Allocate to "<< listener->infoBusPort() <<endl;            
            }
            
            if(m_version._MaxServerId < listener->srvId())
              {
                m_version._MaxServerId=listener->srvId();          
              }
            
            if (_pProxyMgrServerListener->gameServerJoin(listener))
              {
                
                
              if (listener->join(true))
                {
                  
                  toRegular(listener);        
                  addGameServerToMST(listener);         
                  updateMST(); 
                  
                  char* string={"acceptGameServers End Time :" };
                  ServerManager::instance().showCurTime(string);    
                  queryRoleServerStat();
                  continue;
                }     
              
              }
            
            removeGameServer(listener);
          }
        else
          {    
            cout<<"Game Server ID  "<<listener->srvId() <<" is not Regular GameServer!! "<<endl;
            
            moveToEnd(listener);
            return;
            
          }
      }
    
    
}
  
 

void ServerManager::acceptRoleServers()
{
 
  if (!_pProxyMgrServerListener)
    {
      
      return;
    }
  while (_prepRoleServers.size() > 0)
    {
      
      //cout<<" ILM Has "<<_prepRoleServers.size()<< " prepRoleServers "<<endl;
      RoleServerListener* listener = _prepRoleServers.front(); 
      if(listener->isConnSucc())
        {
          allocateRoleSrvId(listener);
          cout<<"Role Server ID allocate to "<<listener->srvId()<<endl;
          
          if(listener->srvId()<3000)
            {             
              removeRoleServer(listener);
              return;
            }
          
          if (_pProxyMgrServerListener->roleServerJoin(listener))
            {
              
              if (listener->join(true))
                {
                  
                  toRegular(listener);
                  char* string={"acceptGameServers End Time :" };
                  ServerManager::instance().showCurTime(string);
                  continue;         
                }       
          
            }

          removeRoleServer(listener);
        }
      else
        {
          cout<<"Role Server ID  "<<listener->srvId() <<" is not Regular RoleServer!! "<<endl;
          moveToEnd(listener);
          return ;
        }
    } 
   
}





void ServerManager::printmst()
{



  /***********************MST ZONE 2 MAPUINT****************************************************************************/
  list<uint16_t> lstZone;
  UniG_ILM_Config::Instance()->GetAllZoneID(lstZone); 
  list<uint16_t>::iterator iterZone;

  for( iterZone = lstZone.begin() ; iterZone != lstZone.end() ; iterZone++ )
    {
      //  cout<<"Zone Id= "<<*iterZone<<endl;
      UniG_ILM_Zone* zone=UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone);
      list<uint32_t> lstMapUnit;
      zone->GetMapUnit(lstMapUnit);
      list<uint32_t>::iterator iterMapUint;
      for(iterMapUint=lstMapUnit.begin();iterMapUint!=lstMapUnit.end();iterMapUint++)
        {       
          cout<<"map uint ="<<*iterMapUint<<" in server ="<<zone->GetServerID() << " in zone ="<<zone->GetZoneID()<<endl;
        }
    }
  

  /******************map Neighbour Maps**********************************************************************************/

  multimap<uint32_t, uint32_t> mapNeighbourMaps;

  UniG_ILM_Config::Instance()->GetNeighbourMapUnit(mapNeighbourMaps);
  iterator_map iter;
  for(iter =mapNeighbourMaps.begin() ; iter != mapNeighbourMaps.end(); iter++)
    {
      
      // cout<<"mapunit id = "<<iter->first<<" is neib whit mapuint id ="<<iter->second<<endl;
    }

  /*******ALL MAP UNIT INFO LIST*******************************************************************************************/

  vector<MapUnitInfo> vecMapUnit;
  UniG_ILM_Config::Instance()->GetMapUnitInfo(vecMapUnit);

  //  cout<<"num is "<<num<<endl;
  
  vector<MapUnitInfo>::iterator iterMapUnit;
  for(iterMapUnit=vecMapUnit.begin();iterMapUnit!=vecMapUnit.end();iterMapUnit++)
    ;
    //  cout<<"map uint id ="<<iterMapUnit->mapUnitID<<" name = "<<iterMapUnit->mapUnitName<<endl;

  /********************************************************************************************************************************/

}





uint32_t ServerManager::fillServerAdjacentMaps(uint16_t serverID,list<uint32_t>& mapuints)
{
  list<uint16_t> lstZone;
  list<uint32_t> mapUnitsOnServer;
  findGameServerById(serverID)->getZoneOnServer(lstZone);
  list<uint16_t>::iterator iterZone;
  for( iterZone = lstZone.begin() ; iterZone != lstZone.end() ; iterZone++ )
    {
      //  cout<<"Zone Id= "<<*iterZone<<endl;
      UniG_ILM_Zone* zone=UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone);
      list<uint32_t> lstMapUnit;
      zone->GetMapUnit(lstMapUnit);
      list<uint32_t>::iterator iterMapUint;
      for(iterMapUint=lstMapUnit.begin();iterMapUint!=lstMapUnit.end();iterMapUint++)
        {       
          mapUnitsOnServer.push_back(*iterMapUint);
          //cout<<"map uint ="<<*iterMapUint<<" in server ="<<zone->GetServerID() << " in zone ="<<zone->GetZoneID()<<endl;
        }
    }
  if(mapUnitsOnServer.size() <= 0) return 0;
  
  lstZone.clear();
  
  UniG_ILM_Config::Instance()->GetAllZoneID(lstZone); 
  list<uint32_t>::const_iterator mapUnitsOnServerIt;
  
  for( iterZone = lstZone.begin() ; iterZone != lstZone.end() ; iterZone++ )
    {
      //  cout<<"Zone Id= "<<*iterZone<<endl;
      UniG_ILM_Zone* zone=UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone);
      list<uint32_t> lstMapUnit;
      zone->GetMapUnit(lstMapUnit);
      list<uint32_t>::iterator iterMapUint;
      for(iterMapUint=lstMapUnit.begin();iterMapUint!=lstMapUnit.end();iterMapUint++)
        for(mapUnitsOnServerIt = mapUnitsOnServer.begin(); mapUnitsOnServerIt != mapUnitsOnServer.end(); mapUnitsOnServerIt++)
          if(UniG_ILM_Config::Instance()->IsNeighbour(*mapUnitsOnServerIt,*iterMapUint))
            {            
              if((*mapUnitsOnServerIt)!=(*iterMapUint) && mapuints.end() == find(mapuints.begin(), mapuints.end(), *iterMapUint))
                 mapuints.push_back(*iterMapUint);

            }
      
    }
  return mapuints.size();
}








uint32_t ServerManager::fillServerAdjacentMaps(uint16_t serverID,vector<uint32_t>& mapuints)
{
  list<uint16_t> lstZone;
  list<uint32_t> mapUnitsOnServer;
  findGameServerById(serverID)->getZoneOnServer(lstZone);
  list<uint16_t>::iterator iterZone;
  for( iterZone = lstZone.begin() ; iterZone != lstZone.end() ; iterZone++ )
    {
      //  cout<<"Zone Id= "<<*iterZone<<endl;
      UniG_ILM_Zone* zone=UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone);
      list<uint32_t> lstMapUnit;
      zone->GetMapUnit(lstMapUnit);
      list<uint32_t>::iterator iterMapUint;
      for(iterMapUint=lstMapUnit.begin();iterMapUint!=lstMapUnit.end();iterMapUint++)
        {       
          mapUnitsOnServer.push_back(*iterMapUint);
          //cout<<"map uint ="<<*iterMapUint<<" in server ="<<zone->GetServerID() << " in zone ="<<zone->GetZoneID()<<endl;
        }
    }
  if(mapUnitsOnServer.size() <= 0) return 0;
  
  lstZone.clear();
  
  UniG_ILM_Config::Instance()->GetAllZoneID(lstZone); 
  list<uint32_t>::const_iterator mapUnitsOnServerIt;
  
  for( iterZone = lstZone.begin() ; iterZone != lstZone.end() ; iterZone++ )
    {
      //  cout<<"Zone Id= "<<*iterZone<<endl;
      UniG_ILM_Zone* zone=UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone);
      list<uint32_t> lstMapUnit;
      zone->GetMapUnit(lstMapUnit);
      list<uint32_t>::iterator iterMapUint;
      for(iterMapUint=lstMapUnit.begin();iterMapUint!=lstMapUnit.end();iterMapUint++)
        for(mapUnitsOnServerIt = mapUnitsOnServer.begin(); mapUnitsOnServerIt != mapUnitsOnServer.end(); mapUnitsOnServerIt++)
          if(UniG_ILM_Config::Instance()->IsNeighbour(*mapUnitsOnServerIt,*iterMapUint))
            {            
              if((*mapUnitsOnServerIt)!=(*iterMapUint) && mapuints.end() == find(mapuints.begin(), mapuints.end(), *iterMapUint))
                 mapuints.push_back(*iterMapUint);

            }
      
    }
  return mapuints.size();
}











uint32_t ServerManager::generateMapUnit2Server(map<uint32_t,uint16_t>& map2server)
{
 
  // cout<<"Enter ServerManager::generateMapUnit2Server() ......"<<endl;
  map2server.clear();
  list<uint16_t> lstZone;
  UniG_ILM_Config::Instance()->GetAllZoneID(lstZone); 
  list<uint16_t>::iterator iterZone;

  for( iterZone = lstZone.begin() ; iterZone != lstZone.end() ; iterZone++ )
    {
      // cout<<"Zone Id= "<<*iterZone<<endl;
      UniG_ILM_Zone* zone=UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone);
      list<uint32_t> lstMapUnit;
      zone->GetMapUnit(lstMapUnit);
      list<uint32_t>::iterator iterMapUint;
      for(iterMapUint=lstMapUnit.begin();iterMapUint!=lstMapUnit.end();iterMapUint++)
        {
          map2server.insert(pair<uint32_t, uint16_t>(*iterMapUint,zone->GetServerID() ));
          cout<<"UpDateMST: MapUint ="<<*iterMapUint<<" In Server ="<<zone->GetServerID()<<endl;
        }
    }
 
  return map2server.size();
}




uint32_t  ServerManager::generateMapUnit2Server(vector<Map2SrvInfo_T>& map2server)
{

  //cout<<" ServerManager::generateMapUnit2Server() "<<endl;
  //cout<<"ServerManager::generateMapUnit2Server ..."<<endl;
  map2server.clear();
  list<uint16_t> lstZone;
  UniG_ILM_Config::Instance()->GetAllZoneID(lstZone); 
  list<uint16_t>::iterator iterZone;
  
  for( iterZone = lstZone.begin() ; iterZone != lstZone.end() ; iterZone++ )
    {
     
      UniG_ILM_Zone* zone=UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone);
      list<uint32_t> lstMapUnit;
      zone->GetMapUnit(lstMapUnit);
      list<uint32_t>::iterator iterMapUint;
      for(iterMapUint=lstMapUnit.begin();iterMapUint!=lstMapUnit.end();iterMapUint++)
        {
          
          Map2SrvInfo_T mstItem;
          mstItem._mapId=*iterMapUint;
          mstItem._srvId=zone->GetServerID();
          // cout<<"                                                                              "<<endl
          //    <<"           Map Unit ID="<< *iterMapUint  <<" In Server "<< zone->GetServerID() <<endl;
          map2server.push_back(mstItem);
        }
    }


  //   cout<<"              ----------------             "<<endl;
  return map2server.size();


}

void ServerManager::timedQueryServerStat()
{


    time_t curTime = time(NULL);
    if(curTime - _lastQueryTime < CommonArgs_T::instance()._gameServerStatQueryT)
    {
       return;
    }

    queryGameServerStat();
    queryRoleServerStat();
    sendServerLoadToPM();
	  
}



uint16_t ServerManager::getAllGameServerID(vector<uint16_t>& vcGServerID)
{
  if(_regularGameServers.size()==0) return _regularGameServers.size();
  vector<GameServerListener*>::iterator curIt;
  
  for (curIt = _regularGameServers.begin(); 
        curIt != _regularGameServers.end(); 
       curIt++)
    {        
      vcGServerID.push_back((*curIt)->srvId()); 
      
    } 
  return vcGServerID.size();
  
}



void ServerManager::queryRoleServerStat()
{

    _lastQueryTime = time(NULL);
    if (_regularRoleServers.size() == 0)
      {
        return;
      }

    
   
    ServerStatusQueryMsg_T queryMsg;
    queryMsg._gsNum= ServerManager::instance().getAllGameServerID(queryMsg._gs);
    vector<RoleServerListener*>::iterator curIt;


    for (curIt = _regularRoleServers.begin(); 
         curIt != _regularRoleServers.end(); 
         curIt++)
      {
        if ((*curIt)->unaccomplishedQuery())
          {
            cout<<"Role Server "<<(*curIt)->srvId() <<" No Reply !"<<endl;
            (*curIt)->clearQuery();
            return ;

          }

      }
    // cout << "ILMServer post query messages tp Role Server... \n" ;
    for (curIt = _regularRoleServers.begin(); 
        curIt != _regularRoleServers.end(); 
        curIt++)
      {
        if((*curIt) && (*curIt)->isConnSucc())
          {
            try
              {
                (*curIt)->clearQuery();
                (*curIt)->queryStat(queryMsg);
                
              }
            catch(StreamException_T e)
              {
                cout<<"queryGameServerStat error "<<endl;
                if ( (*curIt) == _regularRoleServers.back() )
                  {
                    ServerEpoll::instance().remove((*curIt));
                    ServerManager::instance().removeRoleServer((*curIt)); 
                    break;
                  }
                                               
                ServerEpoll::instance().remove((*curIt));
                ServerManager::instance().removeRoleServer((*curIt)); 
              }
          }
      }



    if (waitServerReply(100))
      {
        
        
        for (curIt = _regularRoleServers.begin(); 
             curIt != _regularRoleServers.end(); 
             curIt++)
          {
            cout<<"Role Servers'ID "<<  (*curIt)->srvId()  << " has playerNum = "<<(*curIt)->playerNum()<<endl;
            //            sum += (*curIt)->playerNum();
          }
       
        
      }

    /*
    if (!waitServerReply(100))
      {
        for (curIt = _regularRoleServers.begin(); 
             curIt != _regularRoleServers.end(); 
             curIt++)
          {
            if ((*curIt)->unaccomplishedQuery())
              {
                cout << "No query reply from role server " 
                     << (*curIt)->srvInfo()._id << endl;
              }
          }
      }
    else
      {
        int sum = 0;
        for (curIt = _regularRoleServers.begin(); 
             curIt != _regularRoleServers.end(); 
             curIt++)
          {
            cout<<"playerNum = "<<(*curIt)->playerNum()<<endl;
            sum += (*curIt)->playerNum();
          }
       
        //cout<<"playerNum = "<<sum<<endl;
      }

    **/
}




void ServerManager::queryGameServerStat()
{

    time_t curTime = time(NULL);
    if(curTime - _lastQueryTime < CommonArgs_T::instance()._gameServerStatQueryT)
    {
       return;
    }
   

    if (_regularGameServers.size() == 0)
    {
        return;
    }
  
   
    ServerStatusQueryMsg_T queryMsg;
    
    vector<GameServerListener*>::iterator curIt;    


    for (curIt = _regularGameServers.begin(); 
         curIt != _regularGameServers.end(); 
         curIt++)
      {
        if ((*curIt)->unaccomplishedQuery())
          {
            (*curIt)->clearQuery();
            cout<<" Game Server "<<(*curIt)->srvId() <<" No Reply !"<<endl;
            return ;

          }
        else
          {
            // cout<<"playerNum = "<<(*curIt)->playerNum()<<endl;

          }

      }
    // cout << "ILMServer post query messages to Game Server... \n" ;

    for (curIt = _regularGameServers.begin(); 
        curIt != _regularGameServers.end(); 
        curIt++)
      {
       
        if((*curIt) && (*curIt)->isConnSucc())
          {

            try
              {

                (*curIt)->clearQuery();
                (*curIt)->queryStat(queryMsg);
               
              }
            catch(StreamException_T e)
              {
                cout<<"queryGameServerStat error\n";
                if ( (*curIt) == _regularGameServers.back() )
                  {
                    ServerEpoll::instance().remove((*curIt));
                    ServerManager::instance().removeGameServer((*curIt)); 
                    break;
                  }
                
                               
                ServerEpoll::instance().remove((*curIt));
                ServerManager::instance().removeGameServer((*curIt)); 
              }
          }
      }


    if (waitServerReply(100))
      {
        
        
        for (curIt = _regularGameServers.begin(); 
             curIt != _regularGameServers.end(); 
             curIt++)
          {
            cout<<"Game Servers'ID "<<  (*curIt)->srvId()  << " has playerNum = "<<(*curIt)->playerNum()<<endl;
            //            sum += (*curIt)->playerNum();
          }
       
        
      }


    /*
    if (!waitServerReply(10))
      {
        for (curIt = _regularGameServers.begin(); 
             curIt != _regularGameServers.end(); 
             curIt++)
          {
            if ((*curIt)->unaccomplishedQuery())
              {
                cout << "No query reply from game server " 
                     << (*curIt)->srvInfo()._id << endl;
              }

          }
      }
    else
      {
        int sum = 0;
        for (curIt = _regularGameServers.begin(); 
             curIt != _regularGameServers.end(); 
             curIt++)
          {
            cout<<"playerNum = "<<(*curIt)->playerNum()<<endl;
            sum += (*curIt)->playerNum();
          }
       
         // cout<<"playerNum = "<<sum<<endl;
      }


    **/
    
   
    


}


void ServerManager::sendServerLoadToPM()
{


  if (!_pProxyMgrServerListener)
    {
      return;
    }
  
  if (_regularGameServers.size() + _regularRoleServers.size() == 0)
    {
      return;
    }
  
  ServerLoadToPM srvloadmsg;  
  srvloadmsg._mapNum=_regularGameServers.size()+_regularRoleServers.size();   
  vector<GameServerListener*>::iterator curIt;    
  for (curIt = _regularGameServers.begin(); 
       curIt != _regularGameServers.end(); 
       curIt++)
    {

      if ((*curIt)->unaccomplishedQuery())
        {
          return ;
        }
      SrvToPM srvload;
      srvload._load=(*curIt)->playerNum();
      srvload._id=(*curIt)->srvId();
      srvloadmsg._srvload.push_back(srvload);

    }
  
  vector<RoleServerListener*>::iterator rolecurIt;    
  for (rolecurIt = _regularRoleServers.begin(); 
       rolecurIt != _regularRoleServers.end(); 
       rolecurIt++)
    {

      if ((*rolecurIt)->unaccomplishedQuery())
        {
          return ;
        }
      SrvToPM srvload;
      srvload._load=(*rolecurIt)->playerNum();
      srvload._id=(*rolecurIt)->srvId();
      // cout<<"  _srvload.size() "<< srvloadmsg._srvload.size() <<endl;
      //cout<<"  _regularRoleServers.size() "<< _regularRoleServers.size() <<endl;
      srvloadmsg._srvload.push_back(srvload);
    }

  try
    {

      _pProxyMgrServerListener->updateServerLoad(srvloadmsg);
      
      
    }
  catch(StreamException_T e)
    {
    
      onProxyMgrServerException(_pProxyMgrServerListener);
    }
 
   _lastQueryTime = time(NULL);

}




void ServerManager::updateMST2()
{


    UpdateMSTRequestMsg_T theMessage;
    GameServerListener* listener;
    vector<GameServerListener*>::iterator curIt;

    theMessage._allSrvNum = _regularGameServers.size();;
    for (curIt = _regularGameServers.begin(); 
        curIt != _regularGameServers.end(); 
        curIt++)
    {
        listener = *curIt;
        theMessage._allSrvs.push_back(listener->srvInfo());        
    }


    theMessage._MSTItemNum =generateMapUnit2Server(theMessage._MST);

    for (curIt = _regularGameServers.begin(); curIt != _regularGameServers.end(); curIt++)
    {
	
      try
        {
          listener = *curIt;
          listener->updateMST(theMessage);
          
        }
      catch(StreamException_T e)
        {
          if ( listener == _regularGameServers.back())
            {
              ServerEpoll::instance().remove(listener);
              ServerManager::instance().removeGameServer(listener); 
              break;
            }
          ServerEpoll::instance().remove(listener);
          ServerManager::instance().removeGameServer(listener); 
        }
          
    }

}




void ServerManager::updateMST()
{   
  if( m_iFlag > 0 ) 
    {
      //return;
    }
  cout<<" ServerManager::updateMST()  "<<endl;
  UpdateMSTRequestMsg_T theMessage;
  
  GameServerListener* listener;
  vector<GameServerListener*>::iterator curIt;

  theMessage._allSrvNum = _regularGameServers.size();
  for (curIt = _regularGameServers.begin(); 
       curIt != _regularGameServers.end(); 
       curIt++)
    {
      listener = *curIt;
      theMessage._allSrvs.push_back(listener->srvInfo());
      /*
        cout << "add server info to update message:" << endl
        << "    id         : " << listener->srvInfo()._id << endl
        << "    ip         : " << listener->srvInfo()._ip << endl
        << "    game port  : " << listener->srvInfo()._gamePort << endl
             << "    bus port   : " << listener->srvInfo()._infoBusPort << endl;     
      **/    
    }
  
  
  theMessage._MSTItemNum =generateMapUnit2Server(theMessage._MST);
    
  for (curIt = _regularGameServers.begin(); curIt != _regularGameServers.end(); curIt++)
    {
      
      try
        {
          listener = *curIt;
          listener->updateMST(theMessage);
          
        }
      catch(StreamException_T e)
        {
          if ( listener == _regularGameServers.back())
            {
              ServerEpoll::instance().remove(listener);
              ServerManager::instance().removeGameServer(listener); 
              break;
            }
          ServerEpoll::instance().remove(listener);
          ServerManager::instance().removeGameServer(listener); 
        }
      
      
    }


    if (!waitServerReply(100))
      {
        for (curIt = _regularGameServers.begin(); 
            curIt != _regularGameServers.end(); 
             curIt++)
          {
            GameServerListener* listener = *curIt;
            
            if (listener->unaccomplishedQuery()) 
              {
                cout << "No updateMST reply from game server " 
                     << listener->srvId() << endl;
                
              }
          }

      }

  /*
  if(_pProxyMgrServerListener)
    {
      
      RoleServerListener* rolelistener;
      vector<RoleServerListener*>::iterator rolecurIt;
      
      theMessage._allSrvNum = _regularGameServers.size()+_regularRoleServers.size();
      for (rolecurIt = _regularRoleServers.begin(); 
           rolecurIt != _regularRoleServers.end(); 
             rolecurIt++)
        {
          rolelistener = *rolecurIt;
          theMessage._allSrvs.push_back(rolelistener->srvInfo());
        
            cout << "add role server info to update message:" << endl
            << "    id         : " << rolelistener->srvInfo()._id << endl
            << "    ip         : " << rolelistener->srvInfo()._ip << endl
            << "    game port  : " << rolelistener->srvInfo()._gamePort << endl
                 << "    bus port   : " << rolelistener->srvInfo()._infoBusPort << endl;   
               
          }
      
      try
        {
          
          // _pProxyMgrServerListener->updateMST(theMessage);
          
        }
        catch(StreamException_T e)
          {
            onProxyMgrServerException(_pProxyMgrServerListener); 
            // theMessage.write(_pProxyMgrServerListener);
          }
    }
    // Waiting for all response
  
**/  
  

}




bool ServerManager::waitServerReply(uint32_t timeout)
{
  
  
  bool _unaccomplishedQuery=false;
  
  
  
  while (true)
    {
      ServerEpoll::instance().wait(timeout);
      if( _regularGameServers.size()+_regularRoleServers.size()==0) return true;


      if( _regularGameServers.size()>0)
        {
          vector<GameServerListener*>::iterator curIt;
      
          for (curIt = _regularGameServers.begin(); 
               curIt != _regularGameServers.end(); 
               curIt++)
            {
              if ((*curIt)->unaccomplishedQuery())
                {
                  _unaccomplishedQuery=true;
                }
            }
        }

      if( _regularRoleServers.size()>0)
        {
          vector<RoleServerListener*>::iterator curIt;
      
          for (curIt = _regularRoleServers.begin(); 
               curIt != _regularRoleServers.end(); 
               curIt++)
            {
              if ((*curIt)->unaccomplishedQuery())
                {
                  _unaccomplishedQuery=true;
                }
            }
        }

      
      if(!_unaccomplishedQuery)  return true;
     
      if (--timeout <= 0)
        {
          return false;
        }
      
     
    }
  return true;
}








bool ServerManager::waitGameServerReply(uint32_t timeout)
{
  
 
  if(! _regularGameServers.size())
    {
      return true;
    }
  while (true)
    {
      
      ServerEpoll::instance().wait(timeout);
      if( m_needReplay.size() ==0)
        {
          m_plan.count=0;
          return true;
        
        }
      if (--timeout <= 0)
        {
          m_plan.count++;
          return false;
        }
      
    }
  return false;
}























 bool ServerManager::getAllGameServer(vector<GameServerListener*>& listener)
{

  if(_regularGameServers.size()==0) return false;
  vector<GameServerListener*>::iterator curIt;
  
  for (curIt = _regularGameServers.begin(); 
        curIt != _regularGameServers.end(); 
       curIt++)
    {        
      
      listener.push_back(*curIt); 
      
    }
  
  return true;
  
}







 bool ServerManager::getAllRoleServer(vector<RoleServerListener*>& listener)
{

  if(_regularRoleServers.size()==0) return false;
  vector<RoleServerListener*>::iterator curIt;
  
  for (curIt = _regularRoleServers.begin(); 
        curIt != _regularRoleServers.end(); 
       curIt++)
    {        
      
      listener.push_back(*curIt); 
      
    }
  
  return true;
  
}


void ServerManager::toRegular(RoleServerListener* listener)
{
    vector<RoleServerListener*>::iterator curIt;
    for (curIt = _prepRoleServers.begin(); 
        curIt != _prepRoleServers.end(); 
        curIt++)
    {        
        if (*curIt == listener)
        {
          _prepRoleServers.erase(curIt);
          break;
        }
    }
    _regularRoleServers.push_back(listener);

   
}




//**************************************************************************


void ServerManager::toRegular(GameServerListener* listener)
{
    vector<GameServerListener*>::iterator curIt;
    for (curIt = _prepGameServers.begin(); 
        curIt != _prepGameServers.end(); 
        curIt++)
    {        
        if (*curIt == listener)
        {
          _prepGameServers.erase(curIt);
          break;
        }
    }
    _regularGameServers.push_back(listener);


}


void ServerManager::moveToEnd(GameServerListener* listener)
{
    vector<GameServerListener*>::iterator curIt;
    for (curIt = _prepGameServers.begin(); 
        curIt != _prepGameServers.end(); 
        curIt++)
    {        
        if (*curIt == listener)
        {
          _prepGameServers.erase(curIt);
          break;
        }
    }
   _prepGameServers.push_back(listener);


}


void ServerManager::moveToEnd(RoleServerListener* listener)
{
    vector<RoleServerListener*>::iterator curIt;
    for (curIt = _prepRoleServers.begin(); 
        curIt != _prepRoleServers.end(); 
        curIt++)
    {        
        if (*curIt == listener)
        {
          _prepRoleServers.erase(curIt);
          break;
        }
    }
   _prepRoleServers.push_back(listener);


}

uint32_t ServerManager::getServerLoad(uint16_t serverID)
{
  
  //cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!:::"<<serverID<<endl;
  return findGameServerById(serverID)->playerNum();


}



void ServerManager::makePlanWhileRequired()
{

 
  if(m_iFlag>0 || m_plan.zoneID>0) 
    {
      
      _lastMoveTime= time(NULL);
      // cout<<" Already has a plan  "<<endl;
      return;
    }
  time_t curTime = time(NULL);
  if(curTime - _lastMoveTime < CommonArgs_T::instance()._gameServerStatQueryT)
    {
   
      return;
    }
  
  //cout<<" ServerManager::makePlanWhileRequired() "<<endl;
    UniG_ILM_ZoneMgr::Instance()->SetRandomLoad(35);
    //UniG_ILM_ServerMgr::Instance()->PrintServerInfo();
    GameServerListener* pServer = getUrgentServer();
    if(pServer)
      {
        //printf("urgent server:%d, load:%d\n",pServer->srvId(), pServer->playerNum());
        uint16_t zoneID, fromID, toID;
        if(UniG_ILM_PlanMgr::Instance()->MakePlan(pServer->srvId(),zoneID, fromID, toID))
          {
           
            printf("zone:%d, from server:%d, to server:%d\n", zoneID, fromID, toID);
            UniG_ILM_Zone* pZone = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(zoneID);
            if(pZone)
              {
                cout<<"Make a Plan here  "<<endl;
                m_plan.toServerID=toID;
                m_plan.fromServerID=fromID;
                m_plan.zoneID=zoneID;
                m_plan.count=0;
                m_iFlag=1; //decide a plan
              }
          }
      }
    else
      {
        //cout<<"no urgent server!"<<endl;
      }
    _lastMoveTime= time(NULL);
    // ServerManager::instance().printmst();
}



void ServerManager::getPlanFromConsole(uint16_t fromID,uint16_t toID,uint16_t zoneID)
{

 
  if(m_iFlag>0 || m_plan.zoneID>0) 
    {
      
      cout<<" Already has a plan  "<<endl;

      return;
    }

    GameServerListener* pToServer = findGameServerById(toID) ;
    GameServerListener* pFromServer = findGameServerById(fromID) ;
    
    if(pToServer && pFromServer)
      {
        
        if( UniG_ILM_ZoneMgr::Instance()->GetServerIDByZoneID(zoneID) == fromID )
          {
            cout<<"Make a Plan here  "<<endl;
            m_plan.toServerID=toID;
            m_plan.fromServerID=fromID;
            m_plan.zoneID=zoneID;
            m_plan.count=0;
            m_iFlag=1; //decide a plan
          }
        else
          {
            cout<<"Plan Made Fail ! Zone "<<zoneID <<" Not In Server "<< fromID<<" But In Server "<<"   UniG_ILM_ZoneMgr::Instance()->GetServerIDByZoneID(zoneID)  "<<endl;

          }
      }
    else
      {
        cout<<"Can Not Find Server "<<endl;


      }

}




void ServerManager::doPlan()
{
  if(m_iFlag==0&&m_plan.zoneID==0) 
    {
      return;
    }

  switch(m_iFlag)
    {
    case  1 :   //send lock cmd
      {
        printmst();
        m_needReplay.clear();
        // cout<<"  Do Plan Step: 1 - send lock cmd !"<<endl;
        // cout<<"  m_needReplay.size() :"<< m_needReplay.size()<<endl<<endl;
        sendLockToServer(m_plan.zoneID);
        
        ServerManager::instance().setTimer();
        if( waitGameServerReply(30))
          {

            char* string={" Do Plan Step: 1 Lock   :" };
            ServerManager::instance().showCurTime(string);
            m_iFlag=3;//all server replied!
            cout<<" <------------------------  "<<endl<<endl<<endl<<endl<<endl;
            return ;
          }
      }
      break;
    case  2 : //wait for lock reply
      {

        if(m_plan.count % 21 == 0 )
          {


            cout<<" Do Plan Step: 2 - wait for lock reply "<< int(m_plan.count) <<endl;
            m_iFlag=9;
          }
        // cout<<"  Do Plan Step: 2 - wait for all server map lock reply ! "<<endl;
        //cout<<"  m_needReplay.size() :"<< m_needReplay.size()<<endl<<endl;;
        if( waitGameServerReply(30))
          {
            //char* string={" Do Plan Step: 1 Lock   :" };
            //ServerManager::instance().showCurTime(string);
            m_iFlag=3;//all server replied!
            cout<<" <------------------------  "<<endl<<endl<<endl<<endl<<endl;
            return ;
          }
        else
          {
            
            // m_iFlag=99;   //error
          }
      }
      break;

    case  3 : //send partition cmd
      {
        m_needReplay.clear();
        cout<<" Do Plan Step: 3 - send partition cmd !"<<endl;
        startPartition();
       
        ServerManager::instance().setTimer();
        if( waitGameServerReply(30))
          {
            //char* string={" Do Plan Step: 3  partition  :" };
            //ServerManager::instance().showCurTime(string);
            m_iFlag=5;//all server replied!
            cout<<" <------------------------  "<<endl<<endl<<endl<<endl<<endl;
            return ;
          }
      }
      break;

    case  4 : //wait for partition reply
      {

          
        if(m_plan.count % 21 == 0 )
          {


            cout<<" Do Plan Step: 4 - wait for partition reply "<< int(m_plan.count) <<endl;
            m_plan.count=0;
            m_iFlag=9;
          }

        if(  waitGameServerReply(30))
          {

            //char* string={" Do Plan Step: 3  partition  :" };
            //ServerManager::instance().showCurTime(string);
            m_iFlag=5;//all server replied!
            cout<<" <------------------------  "<<endl<<endl<<endl<<endl<<endl;
            return ;
          }

      }
      break;

    case 5 :  //UpDateMST
      {
        
        m_needReplay.clear();
        cout<<" Do Plan Step: 5 - UpDateMST "<<endl;
        changeMST();
        updateMST();
        ServerManager::instance().setTimer();
        if( waitGameServerReply(30))
          {
            
            //char* string={" Do Plan Step: 5  UpDateMST  :" };
            //ServerManager::instance().showCurTime(string);
            m_iFlag=7;//all server replied!
            cout<<" <------------------------  "<<endl<<endl<<endl<<endl<<endl;
            return ;
          }
      }

      break;

      
    case 6 :  //wait for UpDateMST reply 
      {
        
        cout<<" Do Plan Step: 6 - wait for all Server UpDateMST reply "<<endl;
        if( waitGameServerReply(30))
          {
            // char* string={" Do Plan Step: 3  UpDateMST  :" };
            //ServerManager::instance().showCurTime(string);
            m_iFlag=7;//all server replied!
            cout<<" <------------------------  "<<endl<<endl<<endl<<endl<<endl;
            return ;
          }
        else
          {
              m_iFlag=99;   //error
          }
      }

      break;

    case  7 :   //send unlock cmd
      {

        m_needReplay.clear();
        cout<<" Do Plan Step: 7 - send map unlock cmd"<<endl;
        sendUnLockToServer(m_plan.zoneID);
        ServerManager::instance().setTimer();
        if( waitGameServerReply(30))
          {
            //char* string={" Do Plan Step: 7  unlock  :" };
            //ServerManager::instance().showCurTime(string);

            m_iFlag=9;//all server replied! 
            cout<<" <------------------------  "<<endl<<endl<<endl<<endl<<endl;
            return ;
          }
        cout<<endl<<endl<<endl <<" -------   Unlock Server Num  ------  "<<  m_needReplay.size()<< endl<<endl<<endl<<endl<<endl;
        
      }
      break;
    case  8 : //wait for unlock reply
      {

        if(m_plan.count % 21 == 0 )
          {


            cout<<" Do Plan Step: 2 - wait for unlock reply "<< int(m_plan.count) <<endl;
            m_iFlag=99;
          }
        cout<<" Do Plan Step:  8 - wait for all server map unlock reply "<<  m_needReplay.size() <<endl;
        if( waitGameServerReply(30))
          {

            //char* string={" Do Plan Step: 7  unlock  :" };
            //ServerManager::instance().showCurTime(string);

            m_iFlag=9;//all server replied! 
            cout<<" <------------------------  "<<endl<<endl<<endl<<endl<<endl;
            return ;
          }
        else
          {
            // m_iFlag=99;   //error
          }
      }
      break;

    case  9 : 
      {

         cout<<" Do Plan Step: 9 - Success"<<endl;
         //char* string={" Do Plan Step: 9  Success  :" };
         //ServerManager::instance().showCurTime(string);
         m_plan.toServerID=0;
         m_plan.fromServerID=0;
         m_plan.zoneID=0;
         m_plan.count=0;
         m_iFlag=0;
         m_needReplay.clear();

         printmst();
         cout<<" <------------------------  "<<endl<<endl<<endl<<endl<<endl;
      }
      break;

    case  99 : 
      {

        cout<<" Do Plan Step:  99 - Error~ And Do Recover MST"<<endl;
        recoverMST();
        updateMST2();
        // if( waitGameServerReply(10))
          {
            m_needReplay.clear();
            m_iFlag=9;//all server replied!
            cout<<" <------------------------  "<<endl<<endl<<endl<<endl<<endl;
            return ;
          }
        
      }
      break;


   default:

     cout << " unknown flag in Plan server" <<m_iFlag << endl;
     cout<<" <------------------------  "<<endl;
     break;
      

    }

}





void   ServerManager::changeMST()
{

  UniG_ILM_Zone* pZone = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(m_plan.zoneID);
  if(pZone)
    {
      
      pZone->SetServerID(m_plan.toServerID);
      GameServerListener* pFrom = findGameServerById(m_plan.fromServerID);
      GameServerListener* pTo = findGameServerById(m_plan.toServerID);
      pTo->addZone(m_plan.zoneID);
      pFrom->removeZone(m_plan.zoneID);
    }
  
  
}




void ServerManager::recoverMST()
{

  UniG_ILM_Zone* pZone = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(m_plan.zoneID);
  if(pZone)
    {
      
      pZone->SetServerID(m_plan.toServerID);
      GameServerListener* pFrom = findGameServerById(m_plan.fromServerID);
      GameServerListener* pTo = findGameServerById(m_plan.toServerID);
      pFrom->addZone(m_plan.zoneID);
      pTo->removeZone(m_plan.zoneID);
    }
  
  
}




void ServerManager::sendLockToServer(uint16_t zoneID)
{
  m_needReplay.clear();
  vector<GameServerListener*>::iterator curIt;
  ServerLockMapRequest lockreq;
  UniG_ILM_Zone* pZone = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(zoneID);
  list<uint32_t> lstMapUnit;
  pZone->GetMapUnit(lstMapUnit);
  list<uint32_t>::iterator iterMapUint;
  lockreq.m_nLockMapNum=lstMapUnit.size();
  if(lockreq.m_nLockMapNum>0)
    {
      for(iterMapUint=lstMapUnit.begin();iterMapUint!=lstMapUnit.end();iterMapUint++)
        {   
          lockreq.m_vecMapList.push_back(*iterMapUint);
          cout<<"map uint ="<<*iterMapUint<<" in server ="<<pZone->GetServerID() << " in zone ="<<pZone->GetZoneID()<<endl;
        }
    }

  for (curIt = _regularGameServers.begin(); curIt != _regularGameServers.end(); curIt++)
    {   

      try
        {
          (*curIt)->ServerLockMapCmd(lockreq);
         
        }
      catch(StreamException_T e)
        {
          if ( *curIt == _regularGameServers.back())
            {
              ServerEpoll::instance().remove(*curIt);
              ServerManager::instance().removeGameServer(*curIt); 
              break;
            }
          ServerEpoll::instance().remove(*curIt);
          ServerManager::instance().removeGameServer(*curIt); 

          m_plan.toServerID=0;
          m_plan.fromServerID=0;
          m_plan.zoneID=0;
          m_plan.count=0;
          m_iFlag=0; //No Plan
          return;
        }    
    }

  m_iFlag=2;//already sent MapLockCmd

}







void ServerManager::sendUnLockToServer(uint16_t zoneID)
{
  m_iFlag=8;
  m_needReplay.clear();
  vector<GameServerListener*>::iterator curIt;
  ServerUnLockMapRequest unlockreq;
  UniG_ILM_Zone* pZone = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(zoneID);
  list<uint32_t> lstMapUnit;
  pZone->GetMapUnit(lstMapUnit);
  list<uint32_t>::iterator iterMapUint;
  unlockreq.m_nUnLockMapNum=lstMapUnit.size();
  if(unlockreq.m_nUnLockMapNum>0)
    {
      for(iterMapUint=lstMapUnit.begin();iterMapUint!=lstMapUnit.end();iterMapUint++)
        {   
          unlockreq.m_vecMapList.push_back(*iterMapUint);
          // cout<<"map uint ="<<*iterMapUint<<" in server ="<<zone->GetServerID() << " in zone ="<<zone->GetZoneID()<<endl;
        }
    }
  for (curIt = _regularGameServers.begin(); curIt != _regularGameServers.end(); curIt++)
    {   

      try
        {
          
          (*curIt)->ServerUnLockMapCmd(unlockreq);     
  
          
        }
      catch(StreamException_T e)
        {
          if ( *curIt == _regularGameServers.back())
            {
              ServerEpoll::instance().remove(*curIt);
              ServerManager::instance().removeGameServer(*curIt); 
              break;
            }
          ServerEpoll::instance().remove(*curIt);
          ServerManager::instance().removeGameServer(*curIt); 

          m_plan.toServerID=0;
          m_plan.fromServerID=0;
          m_plan.zoneID=0;
          m_plan.count=0;
          m_iFlag=0; //No Plan
          return;
        }    
    }

 //already sent MapUnLockCmd

}



void  ServerManager::startPartition()
{
  if(m_plan.toServerID && m_plan.fromServerID && m_plan.zoneID)
    {
      m_needReplay.clear();
      GameServerListener* pFrom = findGameServerById(m_plan.fromServerID);
      GameServerListener* pTo = findGameServerById(m_plan.toServerID);
      if(!pFrom || !pTo)
        {
          m_iFlag=99;//one or both Server down !!!
          return;
         }
      UniG_ILM_Zone* pZone = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(m_plan.zoneID);
      list<uint32_t> lstMapUnit;
      StartPartitionRequestMsg_T partitionreq;
      pZone->GetMapUnit(lstMapUnit);
      list<uint32_t>::iterator iterMapUint;
      partitionreq._mapNum=lstMapUnit.size();
      if(partitionreq._mapNum>0)
        {
          for(iterMapUint=lstMapUnit.begin();iterMapUint!=lstMapUnit.end();iterMapUint++)
            {   
              partitionreq._maps.push_back(*iterMapUint);
              
            } 
        }
      partitionreq._serverFrom = m_plan.fromServerID ;
      partitionreq._serverTo = m_plan.toServerID  ;
       
      try
        {
     
          pFrom->StartPartition(partitionreq);
         
        }
      catch(StreamException_T e)
        {
  
          ServerEpoll::instance().remove(pFrom);
          ServerManager::instance().removeGameServer(pFrom); 
          /*
            m_plan.toServerID=0;
            m_plan.fromServerID=0;
            m_plan.zoneID=0;
            m_plan.count=0;
            m_iFlag=99; //
          **/
          return;
        }    
      

      try
        {
          
          partitionreq.write(pTo);
          pTo->event(EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
          ServerEpoll::instance().setup(pTo);
          
        }
      catch(StreamException_T e)
        {

          ServerEpoll::instance().remove(pTo);
          ServerManager::instance().removeGameServer(pTo); 
          /*
            m_plan.toServerID=0;
            m_plan.fromServerID=0;
            m_plan.zoneID=0;
            m_plan.count=0;
            m_iFlag=99; //
          **/
          return;
        }
      
      
      m_iFlag=4; //send partition cmd end.then wait for partition reply from server
      
    }
}

void ServerManager::allocateSrvId(GameServerListener* listener)
{
  listener->srvId(SrvIdGenerator::instance().allocateOneGSID());  
  if(listener->srvId() == 2 )
    SrvIdGenerator::instance().GSrvIdRecover(0);
  //listener->srvId(1); 
}

void ServerManager::allocateRoleSrvId(RoleServerListener* listener)
{
     listener->srvId(SrvIdGenerator::instance().allocateOneRSID()+3000);   
}


void ServerManager::allocateInfoBusPort(GameServerListener* listener)
{
    listener->infoBusPort(listener->srvId()+
                          CommonArgs_T::instance()._baseInfoBusPort+SrvIdGenerator::instance().allocateOneOtherID());
    
}


void ServerManager::addProxyMgrServer(ProxyMgrServerListener* listener)
{
  _pProxyMgrServerListener = listener;
  ServerEpoll::instance().add(_pProxyMgrServerListener);
}



void ServerManager::onProxyMgrServerException(ProxyMgrServerListener* listener)
{

  ServerEpoll::instance().remove(listener);
  delete listener;
  _pProxyMgrServerListener = 0;
}





 void ServerManager::setMSTVersion(MSTVersion  _version)
{
  m_version._MaxServerId=_version._MaxServerId;
  m_version._Version=_version._Version;
  //SrvIdGenerator::instance().SrvIdRecover(m_version._MaxServerId);
}


MSTVersion ServerManager::getMSTVersion()
{

  return m_version;
}



GameServerListener* ServerManager::getUrgentServer()
{


	if(_regularGameServers.size() <=1 )
	{
		return NULL;
	}
	GameServerListener* minLoadServer;
	GameServerListener* maxLoadServer;
	vector<GameServerListener*>::iterator iter;
	minLoadServer = maxLoadServer = _regularGameServers.front();
	for(iter = _regularGameServers.begin(); iter != _regularGameServers.end(); iter++)
	{
		if((*iter)->playerNum() < minLoadServer->playerNum())
		{
			minLoadServer = *iter;
		}
		else if((*iter)->playerNum() > maxLoadServer->playerNum())
		{
			maxLoadServer = *iter;
		}
	}

	if( (maxLoadServer->playerNum() - minLoadServer->playerNum() >CommonArgs_T::instance()._playerNumBalanceValue) 
		&& (maxLoadServer->playerNum() > CommonArgs_T::instance()._playerNumBalanceValue ))
	{
		return maxLoadServer;
	}
	return NULL;


}


bool ServerManager::hasIdleServer()
  
{
  // cout<<"hasIdleServer "<<endl;
  return  !m_lstIdleServer.empty();
}


uint16_t ServerManager::getIdleServer()
{
  return m_lstIdleServer.front()->srvId();
}



void ServerManager::addGameServerToMST(GameServerListener* pServer)

{

  list<uint16_t> lstManagedZone;
  list<uint16_t>::iterator iterZone;
  uint16_t groupID;
  bool bResult = UniG_ILM_Config::Instance()->GetManagedZone(lstManagedZone, groupID);
  pServer->setServerGroupID(groupID);
  //  cout<<"GroupID= "<<pServer->getServerGroupID()<<endl;
  // cout<<"lstManagedZone size="<<lstManagedZone.size()<<endl;
  if(bResult)
    {
      
      for(iterZone = lstManagedZone.begin(); iterZone != lstManagedZone.end(); iterZone++)
        {
          cout<<"GameServer ID="<<pServer->srvId() <<" Has Zone ID="<<*iterZone<<" On"<<endl;
          pServer->addZone(*iterZone);
          UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone)->SetServerID(pServer->srvId());
        }
     
    }
  else
    {
      
      m_lstIdleServer.push_back(pServer);
    }
  // ServerEpoll::instance().add(pServer);
}


GameServerListener* ServerManager::getMinLoadServer(void)
{
	GameServerListener* pIdleServer;
	GameServerListener* pServer;
	int32_t minLoad;
	vector<GameServerListener*>::iterator iter;
	pIdleServer = _regularGameServers.front();
	minLoad = pIdleServer->playerNum();
	for(iter = _regularGameServers.begin(); iter != _regularGameServers.end(); iter++)
	{
		pServer = *iter;
		if(pServer->playerNum() < minLoad)
                  {
                    pIdleServer = pServer;
                    minLoad = pServer->playerNum();
                  }
	}
	return pIdleServer;
}







SrvIdGenerator* SrvIdGenerator::_pInstance = 0;
SrvIdGenerator& SrvIdGenerator::instance()
{
    if (_pInstance)
    {
        return *_pInstance;
    }
    static SrvIdGenerator instance;
    _pInstance = &instance;
    return *_pInstance;
}

SrvIdGenerator::SrvIdGenerator()
  : _gsrvIdCursor(0),_rsrvIdCursor(3000),_otherCursor(0)
{

}

void SrvIdGenerator::GSrvIdRecover(uint16_t maxserverid)
{

  _gsrvIdCursor=maxserverid;
}

uint16_t SrvIdGenerator::allocateOneGSID()
{

  ++_gsrvIdCursor;
  if(_gsrvIdCursor >3000 )
    {
      GSrvIdRecover(0);
    }
  return _gsrvIdCursor;
}

void SrvIdGenerator::RSrvIdRecover(uint16_t maxserverid)
{

  _rsrvIdCursor=maxserverid;
}

uint16_t SrvIdGenerator::allocateOneRSID()
{

  ++_rsrvIdCursor;
  return _rsrvIdCursor;
}


uint16_t SrvIdGenerator::allocateOneOtherID()
{
  _otherCursor++;
  return _otherCursor;
}

