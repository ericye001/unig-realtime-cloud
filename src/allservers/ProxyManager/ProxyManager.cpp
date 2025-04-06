/*
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 
* 文件名称：
* 文件标识：
* 摘    要：
* 
*
* 修订记录：
*       修订日期    修订人    修订内容
*/

#include <time.h>
#include <string.h>

//
// common headers
//
#include "common/EpollUtils.h"
#include "common/SocketStream.h"


//
// local headers
//

#include "ProxyManager.h"
#include "ProxyAction.h"
#include "ILMAction.h"



ProxyManager* ProxyManager::_pInstance = 0;
time_t ProxyManager::_lastTime = 0;
///////////////////////////////////////////////////////////////////////////////
// ProxyManager
///////////////////////////////////////////////////////////////////////////////

ProxyManager& ProxyManager::instance()
{
    if (!_pInstance)
    {
        static ProxyManager pm;
        _pInstance = &pm;
    }
    return *_pInstance;
}

ProxyManager::ProxyManager()
{
    _lastTime = 0;
    _maxProxyID = 0;
    _maxLoginID = 0;
    _addServerCount=0;
    _ILMConnected=false;
    _version._MaxServerId=0;
    _version._Version=0;
    _session=1;
    //_ILMListener=NULL;
    _proxies.clear();
    _login.clear();
    _servers.clear();
    _srvload.clear();


    _roleservers.clear();
    _rolesrvload.clear();
    _sessionlist.clear();

 
}

ProxyManager::~ProxyManager()
{
}

int
ProxyManager::addLoginServer(LoginServerListener* pConn)
{


    ProxyManageEpoll::instance().add(pConn);
    _login.push_back(pConn);
    return 0;
}



int
ProxyManager::removeLoginServer(LoginServerListener* pConn)
{
    // do not consider Proxy leaving by now
  cout<<" Remove Before LoginServer Size: "<< _login.size()<<endl;
  ProxyManageEpoll::instance().remove(pConn);
  std::vector<LoginServerListener*>::iterator itLogin;
  for(itLogin=_login.begin();
      itLogin != _login.end(); 
      itLogin++)
    {
      if((*itLogin)->getLoginID() == pConn->getLoginID())
        {
          _login.erase(itLogin);
          cout<<"Remove   LoginServer ID : "<<  pConn->getLoginID() <<endl;
          return 0;
        }
    }
  cout<<"Remove After LoginServer Size: "<< _login.size()<<endl;
  return -1;
}



LoginServerListener* 
ProxyManager::findLoginServerbyID(uint16_t loginid)
{

   if(_login.empty())
     return NULL;
   
    vector<LoginServerListener*>::iterator curIt;
    for (curIt = _login.begin(); 
         curIt != _login.end();
         ++curIt)
    {
        if ((*curIt)->getLoginID() == loginid)
        {
            return *curIt;
        }
    }
    return NULL;

}

uint16_t
ProxyManager::allocLoginID()
{
    return ++_maxLoginID;
}

void ProxyManager::setLoginID(uint16_t maxid)
{
  _maxLoginID=maxid;
}




int
ProxyManager::addProxy(Connection_T *pConn1)
{

  NonBlockConnection_T* pConn= static_cast<NonBlockConnection_T*>(pConn1);
    if(!_lastTime)
    {
        _lastTime = time(&_lastTime);
    }

    ProxyItem proxyItem;
    proxyItem._pConnection = pConn;
    _proxies.push_back(proxyItem);

    return 0;
}


int
ProxyManager::addProxy(NonBlockConnection_T *pConn)
{
    if(!_lastTime)
    {
        _lastTime = time(&_lastTime);
    }

    ProxyItem proxyItem;
    proxyItem._pConnection = pConn;
    _proxies.push_back(proxyItem);

    return 0;
}

int
ProxyManager::removeProxy(Connection_T *pConn)
{
    // do not consider Proxy leaving by now
  //ProxyManager::instance().showServers();
  cout<<" rm before proxy size: "<< _proxies.size()<<endl;
  std::vector<ProxyItem>::iterator itProxy;
  for(itProxy=_proxies.begin(); itProxy != _proxies.end(); itProxy++)
    {
      if(itProxy->_pConnection == pConn)
        {
          _proxies.erase(itProxy);
          cout<<"rm after  proxy size: "<< _proxies.size()<<endl;
          return 0;
        }
    }
  cout<<"rm after proxy size: "<< _proxies.size()<<endl;
  return -1;
}

void
ProxyManager::updateProxy(Connection_T* pConn, bool valid)
{

    for (uint32_t i = 0; i < _proxies.size(); ++i)
    {
        if (_proxies[i]._pConnection == pConn)
        {
            _proxies[i]._valid = valid;
            return;
        }
    }
    
    throw string("Proxy not found\n\tat ProxyManager::updateProxy")+
          SrcLocation_T(__FILE__, __LINE__);
}


int
ProxyManager::queryProxies()
{
    CommonArgs_T& args = CommonArgs_T::instance();

   
    time_t curTime;
    curTime = time(&curTime);
    if(curTime - _lastTime < args.m_iproxyQueryFreq)
    {
      return 0;
    }

    if(_proxies.empty())
    {
     
      return 0;
    }

    //ProxyManager::instance().showServers();
  

    std::vector<ProxyItem>::const_iterator itProxy;
    ProxyStatusQueryMsg_T queryMsg;
    for(itProxy=_proxies.begin(); itProxy != _proxies.end(); itProxy++)
    {
      if(itProxy->_valid)
        {
          //cout<<"queryProxies _proxies.size() "<<_proxies.size()<<endl;
          NonBlockConnection_T *pConn = itProxy->_pConnection;
          if(_proxies.empty())
            {
              return 0;
            }
          // write to the stream
          try
            {
              if(_proxies.empty())
                {
                  return 0;
                }
              queryMsg.write(pConn);
              pConn->event(EPOLLHUP|EPOLLERR|EPOLLOUT);
              ProxyManageEpoll::instance().setup(pConn);
               // ProxyManageEpoll::instance().wait(10);
            }
          catch (exception& e)
            {
              cout << "exception: " << e.what() << endl
                   << "\tat ConsoleAction_T::execute"
                   << SrcLocation_T(__FILE__, __LINE__) << endl;   
              updateProxy(pConn, false);
              removeProxy(pConn);
              ProxyManageEpoll::instance().remove(pConn);
              delete pConn;
            }
          
          // cout << "Write to proxy" << endl;
        }
      else
        {
       
          

        }
    }
    _lastTime = time(&_lastTime);

    return 0;
}

void
ProxyManager::updateProxy(Connection_T* pConn1, ProxyStatusReplyMsg_T* pStatMsg)
{
    // find by connection
    NonBlockConnection_T* pConn= static_cast<NonBlockConnection_T*>(pConn1);
    std::vector<ProxyItem>::iterator proxyIt;
    for(proxyIt=_proxies.begin(); proxyIt!=_proxies.end(); proxyIt++)
    {
        if(proxyIt->_pConnection == pConn)
        {
            break;
        }
    }

    // if found
    if(proxyIt!=_proxies.end())
    {
      //cout<<"proxy connection ptr: "<<(int)(proxyIt->_pConnection)<<endl;
      proxyIt->_playerNum1 = pStatMsg->_playerNum1;
      proxyIt->_playerNum2 = pStatMsg->_playerNum2;
      cout<<"proxy player number1: "<<proxyIt->_playerNum1<<endl;
      cout<<"proxy player number2: "<<proxyIt->_playerNum2<<endl;
      cout<<"proxy state updated"<<endl;
      cout<<"               <-----------------------              "<<endl;
      return;
    }
    // if not found
    else
    {
        ProxyItem proxyItem;

        proxyItem._pConnection = pConn;
        proxyItem._playerNum1 = pStatMsg->_playerNum1;
        proxyItem._playerNum2 = pStatMsg->_playerNum2;
        _proxies.push_back(proxyItem);
        cout<<"strage: state update message cause a new connection added?"<<endl;
        return;
    }
}

void
ProxyManager::updateProxy(Connection_T *pConn1, ProxyConnectLoginRequestMsg_T *pMsg ,uint16_t proxyid)
{
    // find by connection
    NonBlockConnection_T* pConn= static_cast<NonBlockConnection_T*>(pConn1);
    std::vector<ProxyItem>::iterator proxyIt;
    for(proxyIt=_proxies.begin(); proxyIt!=_proxies.end(); proxyIt++)
    {
        if(proxyIt->_pConnection == pConn)
        {
            break;
        }
    }

    // if found
    if(proxyIt!=_proxies.end())
    {
      
       //cout<<"Proxy Connection ptr: "<<(int)(proxyIt->_pConnection)<<endl;
       //cout<<"proxy player number1: "<<proxyIt->_playerNum1<<endl;
       //cout<<"proxy player number2: "<<proxyIt->_playerNum2<<endl;
       proxyIt->_proxyIp = pMsg->_proxyIp;
       proxyIt->_proxyPort = pMsg->_proxyPort;
       proxyIt->_proxyid=proxyid;
       // cout<<"Proxy Ip: "   <<   ntohl(proxyIt->_proxyIp)   << endl;
       // cout<<"Proxy Port: " <<   ntohs(proxyIt->_proxyPort) << endl;
       // cout<<"Proxy Id: "   <<   proxyIt->_proxyid          << endl;
       cout<<"Proxy Address Updated"<<endl;
      
        return;
    }
    // if not found
    else
    {


        ProxyItem proxyItem;
        proxyItem._pConnection = pConn;
        proxyItem._playerNum1 = 0;
        proxyItem._playerNum2 = 0;
        proxyItem._proxyIp = pMsg->_proxyIp;
        proxyItem._proxyPort = pMsg->_proxyPort;
        proxyItem._proxyid=proxyid;
        _proxies.push_back(proxyItem);

        cout<<"strage: state update message cause a new connection added?"<<endl;
        return;
    }
}



uint16_t
ProxyManager::alreadyHaveThisProxy(ProxyConnectLoginRequestMsg_T *pMsg)
{
    // find by ProxyItm ip prot _proxyIp   _proxyPort;
    std::vector<ProxyItem>::iterator proxyIt;
    for(proxyIt=_proxies.begin(); proxyIt!=_proxies.end(); proxyIt++)
    {
        if(proxyIt->_proxyIp == pMsg->_proxyIp && proxyIt->_proxyPort == pMsg->_proxyPort )
        {
          uint16_t _id;
          _id= proxyIt->_proxyid;
          ProxyManageEpoll::instance().remove(proxyIt->_pConnection);
          ProxyManager::instance().removeProxy(proxyIt->_pConnection);
          return _id;
        }
    }
    
    return 0;

}

void ProxyManager::setTimer()
{

  gettimeofday(&lastTime,NULL);

}

void ProxyManager::showCurTime(char* string)
{


  struct timeval tpend;
  gettimeofday(&tpend,NULL);
  float curTime;
  curTime=1000000*(tpend.tv_sec-lastTime.tv_sec)+tpend.tv_usec-lastTime.tv_usec;
  curTime/=1000;
  cout<<"[Time Use] "<<string<<"   "<< curTime <<" ms "<<endl;
  gettimeofday(&lastTime,NULL);



}
uint32_t ProxyManager::addClient2Proxy(uint16_t loginid,AddClientToPMRequestMsg_T AddClientToPMRequest)
{
 
  
  if(_proxies.size()<1)
    return 1;
  
  ProxyItem* _tmp_proxy=allocPriProxyForClient();
  if(!_tmp_proxy) 
    {
      cout<<" No Proxy ... "<< endl;
      return 1;
    }

  uint32_t _tmp_usersession=createSession(AddClientToPMRequest.m_nUserID);
  // cout<<"Create Session For User ID :"<<AddClientToPMRequest.m_nUserID <<" \t SessionID : "<<_tmp_usersession<<endl;
  uint16_t _tmp_rsid=getRoleServerID();
  NonBlockConnection_T* pConn = _tmp_proxy->_pConnection;


  try
    { 
      SendSession2ProxyReqestMsg_T ss2pReq;
      ss2pReq.m_nUserID  = AddClientToPMRequest.m_nUserID;
      ss2pReq.m_nSession = _tmp_usersession;
      ss2pReq.m_nLoginID = loginid;
      ss2pReq.m_nProxyID = _tmp_proxy->_proxyid ;
      ss2pReq.m_nRoleServerID    = _tmp_rsid;
      ss2pReq.m_Clien_Info._id   = AddClientToPMRequest.m_Clien_Info._id;
      ss2pReq.m_Clien_Info._ip   = AddClientToPMRequest.m_Clien_Info._ip;
      ss2pReq.m_Clien_Info._port = AddClientToPMRequest.m_Clien_Info._port;
      gettimeofday(&ss2pReq.start,NULL); 
      ss2pReq.write(pConn);
      //cout<<"  SendSession2ProxyReqestMsg_T size() "<<ss2pReq.size()<<endl;
      pConn->event(EPOLLHUP|EPOLLERR|EPOLLOUT|EPOLLIN);
      ProxyManageEpoll::instance().setup(pConn);
      //ProxyManageEpoll::instance().wait(5);

    }
  catch (exception& e)
    {
      cout << "exception: " << e.what() << endl
           << "\tat ConsoleAction_T::execute"
           << SrcLocation_T(__FILE__, __LINE__) << endl;   
      updateProxy(pConn, false);
      removeProxy(pConn);
      ProxyManageEpoll::instance().remove(pConn);
      delete pConn;
      return 1;
    }

  return 0;
}


uint32_t  ProxyManager::createSession(uint32_t userid)
{


    map<uint32_t,uint32_t >::iterator gotIt;
    // cout<<"sessionlist size():"<<_sessionlist.size()<<endl;
    for(gotIt=_sessionlist.begin();gotIt!=_sessionlist.end();gotIt++)
      {
        if(gotIt->first==userid)
          return gotIt->second;      
      }

      _sessionlist.insert(make_pair(userid,_session++));
      return _session;
 
}

uint16_t
ProxyManager::allocProxyID()
{
    return ++_maxProxyID;
}

void ProxyManager::setProxyID(uint16_t maxid)
{
  _maxProxyID=maxid;
}






bool 
ProxyManager::removeServer(uint16_t ServerID)
{

  
  cout<<"Remove Server(id="<<ServerID<<") "<<endl<<endl<<endl<<endl<<endl;

  std::vector<SrvInfo_T>::iterator serverIt;

  for(serverIt=_servers.begin(); serverIt!=_servers.end(); serverIt++)
    {
      if(serverIt->_id == ServerID)
        {
          cout<<" Before Remove GameServer ID ="<<ServerID<< "  GameServer List Size ="<<_servers.size()<<  endl;
          _servers.erase(serverIt);
          cout<<" Before Remove GameServer ID ="<<ServerID<< "  GameServer List Size ="<<_servers.size()<<  endl;
          return true;
        }
     
    }
  
  for(serverIt=_roleservers.begin(); serverIt!=_roleservers.end(); serverIt++)
    {
      if(serverIt->_id == ServerID)
        {
          cout<<" Before Remove RoleServer ID ="<<ServerID<< "  RoleServer List Size ="<<_roleservers.size()<<  endl;
          _roleservers.erase(serverIt);
          cout<<" Before Remove RoleServer ID ="<<ServerID<< "  RoleServer List Size ="<<_roleservers.size()<<  endl;
          return true;
        }
  
    }
  return false;
}






uint16_t
ProxyManager::getServers(std::vector<SrvInfo_T> &servers)
{
  

    std::vector<SrvInfo_T>::const_iterator serverIt;
    uint16_t _serversnum;
    for(serverIt=_servers.begin(); serverIt!=_servers.end(); serverIt++)
    {
      servers.push_back(*serverIt);
      _serversnum++;           
    }

    for(serverIt=_roleservers.begin(); serverIt!=_roleservers.end(); serverIt++)
    {
    
      servers.push_back(*serverIt);
      _serversnum++;
    }
    return _serversnum;
}


void 
ProxyManager::showServers()
{


  cout<<endl<<endl<<endl<<" PM Have   "<< " GameServer ="<<_servers.size() <<" RoleServer =" << _roleservers.size()<< endl;
  std::vector<SrvInfo_T>::const_iterator serverIt;
  if(_servers.size()) 
    {
      for(serverIt=_servers.begin(); serverIt!=_servers.end(); serverIt++)
        {
          
          cout<<"Game Server ID      :"<<serverIt->_id<<endl
              <<"            IP      :"<<serverIt->_ip<<endl
              <<"      GamePort      :"<<serverIt->_gamePort<<endl
              <<"   infoBusPort      :"<<serverIt->_infoBusPort<<endl;
        }
    }
  
  if(_roleservers.size())
    {
      for(serverIt=_roleservers.begin(); serverIt!=_roleservers.end(); serverIt++)
        {
          cout<<"Role Server ID      :"<<serverIt->_id<<endl
              <<"            IP      :"<<serverIt->_ip<<endl
              <<"      GamePort      :"<<serverIt->_gamePort<<endl
              <<"   infoBusPort      :"<<serverIt->_infoBusPort<<endl;
        }
    }


}









void
ProxyManager::updateServers(UpdateMSTRequestMsg_T *pUpdateMsg)
{
    _servers.clear();
    _roleservers.clear();
    //  _servers = pUpdateMsg->_allSrvs;
                                                                                                    
    cout << "ProxyManager's ServerInfo Updated" << endl;
    // cout << _servers.size()+_roleservers.size() << " record in ServerInfo" << endl;
    std::vector<SrvInfo_T>::const_iterator srvIt;
    for(srvIt = pUpdateMsg->_allSrvs.begin(); srvIt != pUpdateMsg->_allSrvs.end(); srvIt++)
    {
      // cout << "    server      : " << srvIt->_id  << endl;
      // cout << "    address     : " << srvIt->_ip << endl;
      // cout << "    game port   : " << srvIt->_gamePort << endl;
      // cout << "    infobus port: " << srvIt->_infoBusPort << endl;

        if(srvIt->_id>3000)
          {
            _roleservers.push_back(*srvIt);
          }
        else
          {
            _servers.push_back(*srvIt);
          }
    }
   
}


void
ProxyManager::updateServers(vector<SrvInfo_T>& allSrvs)
{
    _servers.clear();
    _roleservers.clear();
                                                                                               
    cout << "ProxyManager's ServerInfo updated" << endl;
    cout << allSrvs.size() << " record in ServerInfo" << endl;
    std::vector<SrvInfo_T>::const_iterator srvIt;
    for(srvIt = allSrvs.begin(); srvIt != allSrvs.end(); srvIt++)
    {
      /*
        cout << "    Server      : " << srvIt->_id << endl;
        cout << "    Address     : " << srvIt->_ip << endl;
        cout << "    Game port   : " << srvIt->_gamePort << endl;
        cout << "    Infobus port: " << srvIt->_infoBusPort << endl;
      **/
        cout << "              -----------------   "  << endl;
        if(srvIt->_id>3000)
          {
            _roleservers.push_back(*srvIt);
          }
        else
          {
            _servers.push_back(*srvIt);
          }
    }
   
   
}

void ProxyManager::updateServerLoad(vector<SrvToPM> srvload)
{

    _srvload.clear();
    _srvload =srvload;
                                                                                                    
    // cout << "ProxyManager's ServerLoad updated" << endl;
    // cout << _srvload.size() << " Record in ServerLoad" << endl;
    /*
    std::vector<SrvToPM>::const_iterator srvloadIt;
    for(srvloadIt = _srvload.begin(); srvloadIt != _srvload.end(); srvloadIt++)
    {
      cout << "  Server  : " << srvloadIt->_id <<  endl;
      cout << "  Load    : " << srvloadIt->_load << endl;
      cout << "         ----------------   "  << endl;
    }
    **/


}




ProxyItem*
ProxyManager::allocPriProxyForClient(void)
{
    if(_proxies.empty())
        return NULL;
        
    // Sort proxies by their connection count
    sort(_proxies.begin(), _proxies.end());
    
    // If the first proxy is valid, return it to client
    if (_proxies[0]._valid)
    {
        return &(_proxies[0]);
    }
    
    // Find next valid proxy
    for (uint32_t i = 1; i < _proxies.size(); ++i)
    {
        if (_proxies[i]._valid)
        {
            return &_proxies[i];
        }
    }    
    return NULL;    
}
/*
ProxyItem*
ProxyManager::allocSecProxyForClient(void)
{
    if(_proxies.size() < 2)
        return NULL;

    sort(_proxies.begin(), _proxies.end());
    return &(_proxies[1]);
}

**/


ProxyItem*
ProxyManager::allocPriProxyForClient(uint16_t proxyid)
{
    if(_proxies.empty())
        return NULL;
        
    // Sort proxies by their connection count
    sort(_proxies.begin(), _proxies.end());
    
    // If the first proxy is valid, return it to client
    if (_proxies[0]._valid&&_proxies[0]._proxyid!=proxyid)
    {
        return &(_proxies[0]);
    }
    
    // Find next valid proxy
    for (uint32_t i = 1; i < _proxies.size(); ++i)
    {
        if (_proxies[i]._valid&&_proxies[i]._proxyid!=proxyid)
        {
            return &_proxies[i];
        }
    }    
    return NULL;    
}



ProxyItem* 
ProxyManager::findProxybyID(uint16_t proxyid)
{
  
   if(!_proxies.size())
     {
       cout<<" No Proxy Exist !"<<endl;
       return NULL;
     }
   for (uint32_t i = 0; i < _proxies.size(); ++i)
    {
     
        if (_proxies[i]._valid&&_proxies[i]._proxyid==proxyid)
        {
            return &_proxies[i];
        }
    }   
  
   return NULL; 

}




uint16_t ProxyManager::getRoleServerID()
{


  if(!_roleservers.size())
    {
      return 0 ;
    }


  uint16_t _rsid=_roleservers.begin()->_id;
 
  std::vector<SrvToPM>::iterator srvloadIt;
  uint16_t _rsload=0;
  for(srvloadIt = _rolesrvload.begin(); srvloadIt != _rolesrvload.end(); srvloadIt++)
    {
      if( srvloadIt->_load <_rsload)
        {
          _rsid=srvloadIt->_id;
          
        }

    }
  
 return _rsid ;
  /*


  if(_rolesrvload.size()<1)
    {
      return 0;
    }
  uint16_t _rsid=_rolesrvload.begin()->_id;
  uint16_t _rsload=0;
  
  std::vector<SrvToPM>::const_iterator srvloadIt;

  for(srvloadIt = _rolesrvload.begin(); srvloadIt != _rolesrvload.end(); srvloadIt++)
    {
      if( srvloadIt->_load <_rsload)
        {
          _rsid=srvloadIt->_id;
        }

    }
 
 
  return _rsid ;
  */
}


/*
ProxyItem*
ProxyManager::allocSecProxyForClient(uint16_t proxyid)
{
    if(_proxies.size() < 2)
        return NULL;

    sort(_proxies.begin(), _proxies.end());
    return &(_proxies[1]);
}
**/

void
ProxyManager::onAddServerMessageReply( AddServerResponseMsg_T *resp)
{
  if ( (*resp)._addedServer._id<6000 && (*resp)._addedServer._infoBusPort>0 )
    {
      _servers.push_back((*resp)._addedServer);
      cout<<" Add Server To Game Server ID !!"<< (*resp)._addedServer._id <<endl;
      cout<<" Game Servers Size= "<<_servers.size()<<endl;
    }
  else
    {
      
      _roleservers.push_back((*resp)._addedServer);
      cout<<" Add Server To Role Server ID !!"<< (*resp)._addedServer._id <<endl;
      cout<<" Role Servers Size= "<<_roleservers.size()<<endl;
    } 
  _addServerCount--;

  if(_addServerCount ==0)
    {
      showServers();
      (*resp).write(_ILMListener);
      char* string ={"[Time Use] :  Send  Add Server Response To  ILM  "};
      ProxyManager::instance().showCurTime(string);
      _ILMListener->event(EPOLLOUT|EPOLLHUP|EPOLLERR);
      ProxyManageEpoll::instance().setup(_ILMListener);
      // ProxyManageEpoll::instance().wait(5);
    }
  else
    {
      
      cout<<"There "<< _addServerCount <<" Proxies Not Reply For AddServer Request "<<endl;
    }
}
// onAddGameServerMessage
// login server receive ADD_SERVER_REQUEST message from ILM,
// then call this function to handle it.
// TODO:
// 1. for each proxy, foward the message to it

void
ProxyManager::onAddServerMessage(NonBlockConnection_T *pConn, AddServerRequestMsg_T *pMsg)
{


 
      std::vector<ProxyItem>::const_iterator prIt;
      AddServerResponseMsg_T resp;
      resp._addedServer = pMsg->_newServer;
      if( _proxies.empty() )
        {
          if ( (*pMsg)._newServer._id<6000 && (*pMsg)._newServer._infoBusPort>0 )
            {
              _servers.push_back((*pMsg)._newServer);
              cout<<" Add Server To Game Server ID !!"<< (*pMsg)._newServer._id <<endl;
              cout<<" Game Servers Size= "<<_servers.size()<<endl;
            }
          else
            {
              
              _roleservers.push_back((*pMsg)._newServer);
              cout<<" Add Server To Role Server ID !!"<< (*pMsg)._newServer._id <<endl;
              cout<<" Role Servers Size= "<<_roleservers.size()<<endl;
            }
          resp.write(pConn);
          pConn->event(EPOLLOUT|EPOLLHUP|EPOLLERR);
          ProxyManageEpoll::instance().setup(pConn);
         

        }
      // cout<<"ProxyManager::onAddGameServerMessage"<<endl;
      for(prIt = _proxies.begin(); prIt != _proxies.end(); prIt++)
        {
           
          pMsg->write(prIt->_pConnection);
          _addServerCount++;
          cout << "Send ADD SERVER CMD To Proxy ! "  <<endl;
          
          prIt->_pConnection->event(EPOLLOUT|EPOLLHUP|EPOLLERR);
          ProxyManageEpoll::instance().setup(prIt->_pConnection);
            
        }


      //  ProxyManageEpoll::instance().wait(5);
     

}




void 
ProxyManager::startAcceptLogin()
{
 try
    {
      ProxyManageEpoll::instance().add(&LoginServerAcceptor::instance());
                        
    }
    catch(SocketException_T& e)
    {

      cout<<" exception : "<<endl
          <<"  LoginServer_T::startAcceptLogin() SocketException_T occured"<<endl
          <<e.what() << endl;
      exit(1);
    }
}

int 
ProxyManager::startAcceptProxy()
{
  CommonArgs_T& args = CommonArgs_T::instance();
  try
    {
      SocketInterface_T* pSocket = new ServerSocket_T(args.m_iProxyMgrPort);
      if (!pSocket)
        {
          cout << "Fail to create ServerSocket_T(LoginServer)" << endl;
          exit(1);
        }
       
        Action_T* pAction = new ProxyAcceptAction();
        if (!pAction)
        {
            cout << "Fail to create StateAcceptAction_T  at LoginServer_T::startAcceptProxy" << endl;
            delete pSocket;
            exit(1);
        }
       
        Connection_T* pConn = new Connection_T(pSocket, EPOLLIN|EPOLLERR|EPOLLHUP, pAction);
        if (!pConn)
        {
            cout << "Fail to create Connection_T at LoginServer_T::startAcceptProxy" << endl;
            delete pSocket;
            delete pAction;
            exit(1);
        }

        ProxyManageEpoll::instance().add(pConn);
        cout << "The Proxy Connection Acceptor(port ="<<args.m_iProxyMgrPort <<" started  at LoginServer_T::startAcceptProxy()" << endl;

    }
    catch(SocketException_T& e)
    {

      cout<<"SocketException_T ( "<<e.what() << " ) occured "<< " at LoginServer_T::startAcceptLogin() ! EXTI!"<<endl;
      exit(1);
    }

    return 0;
}






int
ProxyManager::connectILM()
{
    if (_ILMConnected) return 0; 
    time_t now;
    now = time(&now);
    if(now - _lastTryILM < 6) return 0;

    _lastTryILM = now;
 
    try
    {

        CommonArgs_T& args = CommonArgs_T::instance();
        Socket_T* pSocket = new Socket_T(args.m_sILMServerIp, args.m_iILMServerPort);
        SocketStream_T stream(pSocket->sfd());

        Action_T* pAction = new ILMAction();
        if(!pAction)
        {
       
            cout << "Fail to create ILMAction_T  at LoginServer_T::connectILM" << endl;
            delete pSocket;

            return -1;
        }
        NonBlockConnection_T* pConn = new NonBlockConnection_T(pSocket, EPOLLOUT|EPOLLIN|EPOLLERR|EPOLLHUP, pAction);
        if(!pConn)
        {

            cout << "Fail to create Connection_T at LoginServer_T::connectILM !" << endl;
            delete pSocket;
            delete pAction;

            return -1;
        }       
        PMConnectILMRequestMsg_T reqMsg;
        reqMsg.m_version._MaxServerId=_version._MaxServerId;
        reqMsg.m_version._Version=_version._Version;
        reqMsg.write(pConn);
        //cout << "connect request sent" << endl;      
        _ILMConnected = true;
        _ILMListener=pConn;
        ProxyManageEpoll::instance().add(pConn);
        cout<<" Connecting ILM (ip="<<args.m_sILMServerIp.c_str()<<" ;port="<<args.m_iILMServerPort<<") "<<endl;
        return 0;
    }
    catch(SocketException_T& e)
    {

        cout<<" SocketException_T( "<< e.what() <<" ) occured. At ProxyManager::connectILM ."<<endl;
    }
    catch(StreamException_T& e)
    {

      cout<<" StreamException_T( "<< e.what() <<" ) occured . At ProxyManager::connectILM ."<<endl;

    }
    catch(EpollException_T& e)
    {

      cout<<" EpollException_T( "<< e.what() <<" ) occured. At ProxyManager::connectILM ."<<endl;
       
    }
    catch(...)
    {

      cout<<" Unknown Exception( ) occured. At ProxyManager::connectILM ."<<endl;
       
    }

    return -1;
}







void
ProxyManager::ILMDie(void)
{
    _ILMConnected = false;

}


void ProxyManager::onILMServerException()
{

 _ILMConnected = false;
 _ILMListener =NULL;

}


void 
ProxyManager::loginServerListener(LoginServerListener *pConn)
{

  addLoginServer(pConn);
  

}



void 
ProxyManager::onLoginServerException(LoginServerListener* pConn)
{
  removeLoginServer(pConn);
  delete pConn;

}

///////////////////////////////////////////////////////////////////////////////
// ProxyItem
///////////////////////////////////////////////////////////////////////////////
ProxyItem::ProxyItem()
{
    _proxyIp = 0;
    _proxyPort = 0;
    _playerNum1 = _playerNum2 = 65535;
    _pConnection = 0;
    _proxyid=0;
    _valid = false;
}

ProxyItem::~ProxyItem()
{


}

bool ProxyItem::operator<(const ProxyItem& proxy) const
{
    if(_playerNum1 < proxy._playerNum1)
    {
        return true;
    }
    return false;
}
