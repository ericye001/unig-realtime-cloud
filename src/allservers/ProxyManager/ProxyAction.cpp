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

#include "ProxyAction.h"
void ProxyAction::execute(Connection_T* pConn1)
{


  ProxyManager& pm = ProxyManager::instance();
  NonBlockConnection_T* pConn= static_cast<NonBlockConnection_T*>(pConn1);
 
      try
        {
          /**
             SocketStream_T stream = pConn->stream();  
             BasePacket_T pkg;
             pkg.read(&stream);
             
             if( !pkg.length())
             {
             cout<<" BasePacket length : "<< pkg.length() <<endl;
             }
             
             if(pkg.length()==0)
             {
             
             ProxyManageEpoll::instance().remove(pConn);
             pm.removeProxy(pConn);
             delete pConn;
             return;
             }         
          **/
          
          
          if(pConn->geteventType()& EPOLLIN )
            {
              
              //cout<<" EPOLLIN!! "<<endl;
              pConn->read();    
              BasePacket_T pkg;         
              while( pkg.getInputPacket(pConn,true))
                //if ( pkg.getInputPacket(pConn,true))
                {
                  
                  switch(BaseMessage_T::type(&pkg))
                    {
                                            
                    case  MsgType::PROXY_CONNECT_LOGIN_REQUEST:
                      {   
                        
                        //ProxyManager::instance().setTimer();
                        cout<<"Proxy Now Coming... "<<endl;
                        proxyJoin(pConn, pkg);   
                      }
                      break;
                      
                    case  MsgType::PROXY_CONNECT_LOGIN_COMPLETE:
                      
                      {              
                        //cout<<"Proxy Connect Complete! "<<endl;
                        proxyJoinComplete(pConn, pkg);
                        //char* string ={" Proxy Connect PM Complete Use Time :  : "};
                        //ProxyManager::instance(showCurTime(string);
                        
                      }
                      break;
                      
                      
                    case MsgType::SEND_SESSION_TO_PROXY_RESPONSE: 
                      {
                        
                        /*
                          char* string ={"[Time Use] :  Recieve Session Reply From Proxy  "};
                        ProxyManager::instance().showCurTime(string);
                        **/
                        
                        SendSession2ProxyResponseMsg_T ss2pRes;
                        ss2pRes.unpack(&pkg);
                        
                        struct timeval tpstart;
                        gettimeofday(&tpstart,NULL);
                        float curTime;
                        curTime=1000000*(tpstart.tv_sec-ss2pRes.start.tv_sec)+tpstart.tv_usec-ss2pRes.start.tv_usec;
                        curTime/=1000;
                        cout<< "[-----------Time Use---------------------------]  "<< curTime <<" ms "<<endl;
                        
                        
                        if(ss2pRes.m_nResult)
                          {
                            
                            LoginServerListener* pConn=ProxyManager::instance().findLoginServerbyID(ss2pRes.m_nLoginID);
                            if(!pConn)
                              {
                                cout<<"Can Not Find LoginServer ID : "<<ss2pRes.m_nLoginID <<endl;
                                return;
                              }
                            
                            AddClientToPMResponseMsg_T AddClientToPMResp;
                            AddClientToPMResp.m_nUserID=ss2pRes.m_nUserID;
                            AddClientToPMResp.m_nSession=ss2pRes.m_nSession;
                            ProxyItem* _tmp_proxy=ProxyManager::instance().findProxybyID(ss2pRes.m_nProxyID);
                            if(!_tmp_proxy)
                              {
                                cout<<"Can Not Find Proxy ID :  "<<ss2pRes.m_nProxyID <<endl;
                                return;
                              }
                            AddClientToPMResp.m_ProxySrv_Info._ip=ntohl(_tmp_proxy->_proxyIp);
                            AddClientToPMResp.m_ProxySrv_Info._port=ntohs(_tmp_proxy->_proxyPort);
                            AddClientToPMResp.m_nRoleServerID=ss2pRes.m_nRoleServerID;    
                            try
                              {
                                /*
                                  cout<<"AddClientToPMResponseMsg "<<endl
                                  <<"            User   ID   :"<< AddClientToPMResp.m_nUserID            <<endl
                                  <<"            SessionID   :"<< AddClientToPMResp.m_nSession            <<endl
                                  <<"            Proxy  Ip   :"<< AddClientToPMResp.m_ProxySrv_Info._ip   <<endl
                                  <<"            Proxy  Port :"<< AddClientToPMResp.m_ProxySrv_Info._port <<endl;
                                **/
                                
                                AddClientToPMResp.write(pConn);
                                pConn->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET|EPOLLOUT);
                                ProxyManageEpoll::instance().setup(pConn);
                                //  char* string1 ={"[Time Use] : AddClientToPMResp.write(pConn) End  "};
                                //  ProxyManager::instance().showCurTime(string1);
                                // cout<<" Send AddClientToPMResponseMsg_T To LoginServer "<< pConn->getLoginID() <<endl;
                                //cout<<" < -------------------------  AddClientToPMResp. size "<<  AddClientToPMResp.size()<< endl << endl<< endl<< endl<<endl;
                              }
                            catch (exception& e)
                              {
                                cout << "exception: " << e.what() << endl
                                     << "\tat ProxyAction::execute"
                                     << SrcLocation_T(__FILE__, __LINE__) << endl;
                                ProxyManager::instance().onLoginServerException(pConn);
                                return;
                                
                              }
                          }
                        else
                          {
                            cout<<" SendSession2ProxyResponseMsg Sent Failed "<<endl;
                          }
                        /*
                          char* string1 ={"[Time Use] : Add Client End Time  "};
                          ProxyManager::instance().showCurTime(string1);
                        */
                      }
                      break;
                      
           
                    case MsgType::PROXY_STATUS_REPLY:
                      {              
                        //cout<<"MsgType::PROXY_STATUS_REPLY "<<endl;
                        ProxyStatusReplyMsg_T statMsg;
                        statMsg.unpack(&pkg);
                        pm.updateProxy(pConn, &statMsg); 
                      }
                      break;
                      
                    case  MsgType::ADD_SERVER_RESPONSE:
                      {
                        
                        char* string ={"[Time Use] :  Recieve Add Server Response From Proxy  "};
                        ProxyManager::instance().showCurTime(string);
                        AddServerResponseMsg_T resp;
                        resp.unpack(&pkg);
                        ProxyManager::instance().onAddServerMessageReply(&resp);
                        
                      }
                      break;
                    case  MsgType::PROXY_DISCONNECT_LOGIN_REQUEST:
                      {              
                        cout << "----PROXY_DISCONNECT_PM_REQUEST----" << endl;
                        proxyLeave(pConn);
                      }
                      break;
                      
                      
                    case MsgType::PROXY_DISCONNECT_LOGIN_COMPLETE:
                      {              
                        cout << "----PROXY_DISCONNECT_PM_COMPLETE----" << endl;
                        proxyLeaveComplete(pConn);
                      }
                      break;
                      
                    default:
                      {
                        cout << "Receive Unknown Message From Proxy Server" << BaseMessage_T::type(&pkg) << endl;
                        
                        break;
                      }
                    }
                  
                }
              
            }
          

          if(pConn->getOutputBuffer()->isCompeleteMsg())
            {
              pConn->event(EPOLLOUT|EPOLLHUP|EPOLLERR|EPOLLET);
              ProxyManageEpoll::instance().setup(pConn);
              pConn->seteventType(EPOLLOUT);
              //cout<<" Change to EPOLLOUT!! "<<endl;
              
            } 
          else
            {

              pConn->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);
              ProxyManageEpoll::instance().setup(pConn);
              //cout<<" Change to EPOLLIN!! "<<endl;

            }
          
      



          if(pConn->geteventType()& EPOLLOUT)
            {
              //out<<" EPOLLOUT!! "<<endl;     
              while(pConn->getOutputBuffer()->isCompeleteMsg()) 
                {
                  //cout<<"EPOLLOUT OUT "<<pConn->getOutputBuffer()->clength() <<endl;
                  pConn->write();
                  
                }      
              pConn->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);
              ProxyManageEpoll::instance().setup(pConn);
              
            


            }

        }
    catch (SocketException_T& e)
    {
        cout << "SocketException_T: " << e.what() << endl;
    }    
    catch (StreamException_T& e)
    {
        cout << "StreamException_T: " << e.what() << endl;
        if (e.code() == 0)
        {// connection broken
          cout<<" Socket connection broke . At ProxyAction_T::execute . "<<endl;
     
        }
        else
        {
          cout<<" StreamException_T("") occured . At ProxyAction_T::execute . "<<endl;
           
        }
        cout << " ProxyManageEpoll::instance().remove(pConn) "  << endl;
        ProxyManageEpoll::instance().remove(pConn);
        pm.removeProxy(pConn);
        delete pConn;
       
    }
    catch (EpollException_T& e)
      {
        cout << "EpollException_T: " << e.what() << endl;    
        cout << " ProxyManageEpoll::instance().remove(pConn) "  << endl;
        ProxyManageEpoll::instance().remove(pConn);
        pm.removeProxy(pConn);
        delete pConn;
      }
    catch (string& e)
      {
        cout << "string exception: " << e << endl;
      }
    catch(StrException_T& e)
      {
        
        cout << "StrException : " << e.what() << endl;
        cout << " ProxyManageEpoll::instance().remove(pConn) "  << endl;
        ProxyManageEpoll::instance().remove(pConn);
        pm.removeProxy(pConn);
        delete pConn;

      }

    catch (...)
      {
        cout << "Unkown exception" << endl;
      }
       
    // cout << "\tat ProxyAction_T::execute" << SrcLocation_T(__FILE__, __LINE__) << endl;
}
void ProxyAction::proxyJoin(Connection_T* pConn1, BasePacket_T& pkg)
{

    NonBlockConnection_T* pConn= static_cast<NonBlockConnection_T*>(pConn1);
    ProxyManager& pm = ProxyManager::instance();
    ProxyConnectLoginRequestMsg_T reqMsg;
    reqMsg.unpack(&pkg);
    // pm.setProxyID(reqMsg._maxproxyid);
    ProxyConnectLoginResponseMsg_T respMsg;
    respMsg._maxproxyid=reqMsg._maxproxyid;
    // allocate a proxy_id for it
    //  if(reqMsg._proxyid>0)
      {
        //    respMsg._proxyId = reqMsg._proxyid;
      }
      //else

      
    if(!ProxyManager::instance().alreadyHaveThisProxy(&reqMsg))
      {
        respMsg._proxyId = pm.allocProxyID();
        cout<<" New Proxy Server , allocateProxy ID To "<< respMsg._proxyId  <<endl;
      }
    else
      {
        cout<<" There Already has a same Server ,so use it's proxyid "<<endl;
        respMsg._proxyId = reqMsg._proxyid;
      }
     
    if(!respMsg._proxyId)             // if allocate failed
      {
        respMsg._result = 0;
        respMsg._serverIpItemNum = 0;
        respMsg.write(pConn);     // send back a failed message    
        return;
      }
    // tell the proxy its id, as well as current MST

 
    respMsg._serverIpItemNum = pm.getServers(respMsg._servers);
    respMsg._result = 1;
    respMsg.write(pConn);
    pm.updateProxy(pConn, &reqMsg,respMsg._proxyId);

    pConn->event(EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);
    ProxyManageEpoll::instance().setup(pConn);
}

void ProxyAction::proxyJoinComplete(Connection_T* pConn, BasePacket_T& pkg)
{
    ProxyConnectLoginCompleteMsg_T joinComplete;
    joinComplete.unpack(&pkg);
    if (joinComplete._result)
    {
      cout << "Proxy Join Complete!" << endl;
      ProxyManager::instance().updateProxy(pConn, joinComplete._result);
    }
    else
    {
        cout << "Result: false" << endl;
        ProxyManager::instance().removeProxy(pConn);
        ProxyManageEpoll::instance().remove(pConn);
        delete pConn;
    }
    cout<<"        <-----------------------         "<<  endl<< endl<< endl <<endl;
}

void ProxyAction::proxyLeave(Connection_T* pConn)
{
    ProxyManager::instance().updateProxy(pConn, false);
    ProxyDisconnectLoginResponse_T response;
    response._result = 1;
    response.write(pConn);
    cout << "----Response proxyLeave----" << endl;
}

void ProxyAction::proxyLeaveComplete(Connection_T* pConn)
{
    ProxyManager::instance().removeProxy(pConn);
    ProxyDisconnectLoginCompleteResponse_T response;
    response._result = 1;
    response.write(pConn);
    ProxyManageEpoll::instance().remove(pConn);    
    cout << "Delete proxy: " << pConn->fd() << endl;
    delete pConn;
}



void ProxyAcceptAction::execute(Connection_T* pConn)
{

    SocketInterface_T* pSocket = ((ServerSocket_T*)pConn->socket())->accept();
    if (!pSocket)
    {
        cout << "Fail to accept client" << endl;

        return;
    }

    Action_T* pAction = new ProxyAction();
    if (!pAction)
    {
        cout << "Fail to create Action_T (ProxyAcceptAction)" << endl;
        return;
    }

    // Connection_T* pConn2Proxy = new Connection_T(pSocket, EPOLLIN | EPOLLHUP | EPOLLERR, pAction);


    NonBlockConnection_T* pConn2Proxy = new NonBlockConnection_T(pSocket, EPOLLIN|EPOLLHUP|EPOLLERR, pAction);
    if (!pConn2Proxy)
    {
        cout << "Fail to create Connection_T" << endl;
        return;
    }
   
    // pConn2Proxy->configureBlocking(false);
    try
      {
       
        ProxyManageEpoll::instance().add(pConn2Proxy);
      }
    catch(EpollException_T& e)
      {
        cout << e.what() << endl;
      }
    ProxyManager &proxyManager = ProxyManager::instance();
    proxyManager.addProxy(pConn2Proxy);
    cout<<"Add a Proxy (fd="<< pConn2Proxy->fd() <<")  Into ProxyList "<<endl;
   
}
