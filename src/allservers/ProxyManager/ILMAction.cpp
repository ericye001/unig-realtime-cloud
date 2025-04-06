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

#include "ILMAction.h"

void
ILMAction::execute(Connection_T* pConn)
{
 
  try
    {
      
      NonBlockConnection_T* listener = 
        static_cast<NonBlockConnection_T*>(pConn);
    

      if(listener->geteventType()& EPOLLIN )
        {
          // cout<<" ILMAction::execute( )  EPOLLIN "<< listener->geteventType()  <<endl;
          listener->read();            
          BasePacket_T pkg;
          // if( pkg.getInputPacket(listener,true))
          while( pkg.getInputPacket(listener,true))
            {
              // cout<<" BaseMessage_T::type(&pkg) "<<BaseMessage_T::type(&pkg) <<endl;
              switch(BaseMessage_T::type(&pkg))
                {
                 
                   case MsgType::REMOVE_SERVER_REQUEST:
                     {
                   
                       ServerLeaveRequestMsg_T req;
                       req.unpack(&pkg);
                       if(ProxyManager::instance().removeServer(req.m_nServerID))
                         {

                           ServerLeaveResponseMsg_T resp;
                           resp.m_nResult = 1;
                           resp.m_nServerID=req.m_nServerID;
                           resp.write(listener);
                         }
                       else
                         {
                   
                           ServerLeaveResponseMsg_T resp;
                           resp.m_nResult = 0;
                           resp.m_nServerID=req.m_nServerID;
                           resp.write(listener);
                           
                         }
                      
                       
                     }
                     break;
                     
                   case MsgType::UPDATE_MST_REQUEST:
                     {
                       
                       UpdateMSTRequestMsg_T req;
                       req.unpack(&pkg);
                       // update MST and ServerInfo here
                       ProxyManager::instance().updateServers(req._allSrvs);
                       // write the response
                       UpdateMSTResponseMsg_T resp;
                       resp._result = 1;
                       resp.write(listener);
                     }
                     break;
                  
                  
                    case MsgType::PM_CONNECT_ILM_RESPONSE:
                      {
                        cout<<"PM Connect ILM Response !!"<<endl;
                        //<<"   <------------------    "<<endl<<endl<<endl<<endl<<endl;
                        //PMConnectILMResponseMsg_T respMsg;
                        //respMsg.unpack(&pkg);
                        //ProxyManager::instance()._version._MaxServerId=respMsg.m_version._MaxServerId;
                        //ProxyManager::instance()._version._Version=respMsg.m_version._Version;  
                        // cout << "connect response received !\t  MaxVerServerId:" <<respMsg.m_version._MaxServerId<<"\t version:"<<respMsg.m_version._Version<< endl;
                        
                        
                      }
                      break;
                  

                      case MsgType::ADD_SERVER_REQUEST:
                        {
                          
                          //   cout<<"  -----------------> "<<endl
                          // <<"Add Server To Proxy !! "<<endl;
                          
                          AddServerRequestMsg_T req;
                          req.unpack(&pkg);
                          //cout<<"Add Server To Proxy,Server Ip  : "<< req._newServer._ip<<endl
                          //  <<"                    Server Port: "<< req._newServer._gamePort<<endl
                          //  <<"                    Server Id  : "<< req._newServer._id<<endl;
                          //ProxyManager::instance()._version._MaxServerId=req.m_version._MaxServerId;
                          //ProxyManager::instance()._version._Version=req.m_version._Version;
                          char* string ={"[Time Use] :  Recieve Add Server request From ILM  "};
                          ProxyManager::instance().showCurTime(string);
                          ProxyManager::instance().onAddServerMessage(listener, &req);
                          
                        }
                        break;
                        
                       case MsgType::SERVER_LOAD_PM:
                         {
                           
                           //cout<<"  -----------------> "<<endl
                           //   <<"SERVER_LOAD_PM !! "<<endl;
                           ServerLoadToPM req;
                           req.unpack(&pkg);
                           ProxyManager::instance().updateServerLoad(req._srvload);
                           
                         }
                         break;
                  
                       case MsgType::MAP_UNIT_RESPONSE:
                         {
                           
                           MapunitResponse_T response;
                           cout<<BaseMessage_T::type(&pkg)<<endl;
                           // response.unpack(&pkg); 
                           // cout<<response._etc<<"|||||"<<response.type1()<<endl;
                           
                           
                         }
                         
                         break;
                         
                      default:
                        cout << "Receive Unknown Message in ILMAction_T::execute" << endl<<BaseMessage_T::type(&pkg)<<endl;
                        return;
                }
            }





          if(listener->getOutputBuffer()->isCompeleteMsg()) 
            {
              //cout<<"IN IN IN "<<endl;
              listener->event(EPOLLOUT|EPOLLIN|EPOLLHUP|EPOLLERR);
              ProxyManageEpoll::instance().setup(listener);
              listener->seteventType(EPOLLOUT);
              
            } 
          else
            {
              
              listener->event(EPOLLIN|EPOLLHUP|EPOLLERR);
              ProxyManageEpoll::instance().setup(listener);

            }   


        }
  
      if(listener->geteventType()& EPOLLOUT)
        {
         
          while(listener->getOutputBuffer()->isCompeleteMsg()) 
            {
              // cout<<"OUT IN OUT "<<endl;
              //cout<<" ILMAction::execute( )  EPOLLOUT "<< listener->geteventType()  <<endl;
              listener->write();

              
            }   
          listener->event(EPOLLIN|EPOLLHUP|EPOLLERR);
          ProxyManageEpoll::instance().setup(listener);
        }
        
      
    }
  catch(exception& e)
    {
     

      cout << "exception: " << e.what() << endl
           << "\tat ILMAction_T::execute"
           << SrcLocation_T(__FILE__, __LINE__) << endl;

      
      ProxyManageEpoll::instance().remove(pConn);
      ProxyManager::instance().onILMServerException();
      delete pConn;
        
    }

         
}

