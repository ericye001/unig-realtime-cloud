#include "ProxyObject.h"
#include "ProxyObjectFactory.h"
#include "message/Message.h"
#include "common/Exception.h"
#include "socket/Epoll.h"
#include <set>

extern struct prxoyArg g_arg ;


//---------------------------------------------------------------------------
//  ProxyConnectLoginRequestMsg::ProxyConnectLoginRequestMsg
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------

ProxyConnectLoginRequestMsg::ProxyConnectLoginRequestMsg(const char *pHost, int nPort)
{
	proxyIp = inet_addr(pHost);
	proxyPort = htons(nPort);
	maxproxyid = 0;
	proxyid = 0;
	m_nType  = MsgType::PROXY_CONNECT_LOGIN_REQUEST;  

}
//---------------------------------------------------------------------------
//  ProxyConnectLoginRequestMsg::~ProxyConnectLoginRequestMsg
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
ProxyConnectLoginRequestMsg::~ProxyConnectLoginRequestMsg()
{
}

//---------------------------------------------------------------------------
//  ProxyConnectLoginRequestMsg::Pack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyConnectLoginRequestMsg::Pack(BasePacket* pkt)
{
	BaseMessage::Pack(pkt);
	pkt->Push(&proxyIp, sizeof(proxyIp));
	pkt->Push(&proxyPort, sizeof(proxyPort));
	pkt->Push(&maxproxyid, sizeof(maxproxyid));
	pkt->Push(&proxyid, sizeof(proxyid));
}

//---------------------------------------------------------------------------
//  ProxyConnectLoginRequestMsg::Unpack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyConnectLoginRequestMsg::Unpack(BasePacket* pkt)
{
	BaseMessage::Unpack(pkt);
	pkt->Pop(&proxyIp, sizeof(proxyIp));
	pkt->Pop(&proxyPort, sizeof(proxyPort));
	pkt->Pop(&maxproxyid, sizeof(maxproxyid));
	pkt->Pop(&proxyid, sizeof(proxyid));

}

//---------------------------------------------------------------------------
//  ProxyConnectLoginRequestMsg::Getip
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyConnectLoginRequestMsg::Getip()
{
	printf("\n the ip =%d\n",proxyIp);
}

//---------------------------------------------------------------------------
//  ProxyConnectLoginRequestMsg::Getport
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyConnectLoginRequestMsg::Getport()
{
	printf("\n the port =%d\n",proxyPort);
}


//---------------------------------------------------------------------------
//  ProxyConnectLoginRequestMsg::Size
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 ProxyConnectLoginRequestMsg::Size()
{
	return BaseMessage::Size()+sizeof(proxyIp) + sizeof(proxyPort) + sizeof(maxproxyid) + sizeof(proxyid);
}


//=============================================================================


//---------------------------------------------------------------------------
//  ProxyConnectLoginResponseMsg::ProxyConnectLoginResponseMsg
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
ProxyConnectLoginResponseMsg::ProxyConnectLoginResponseMsg()
{
	m_nType  = MsgType::PROXY_CONNECT_LOGIN_RESPONSE;
	//m_nType = PROXY_CONNECT_LOGIN_RESPONSE;
	serverIpItemNum = 0;
}

//---------------------------------------------------------------------------
//  ProxyConnectLoginResponseMsg::Unpack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyConnectLoginResponseMsg::Unpack(BasePacket* pkt)
{
	BaseMessage::Unpack(pkt);
	pkt->Pop(&result, sizeof(result));
	pkt->Pop(&proxyId, sizeof(proxyId));
	pkt->Pop(&maxproxyid, sizeof(maxproxyid));
	pkt->Pop(&serverIpItemNum, sizeof(serverIpItemNum));
	if (serverIpItemNum > 0)
	{
		servers.reserve(serverIpItemNum);
		GameServerInfo gsInfo;
		for (int i = 0; i < serverIpItemNum; ++i)
		{
			pkt->Pop(&gsInfo, sizeof(gsInfo));
			servers.push_back(gsInfo);
		}
	}
}

//---------------------------------------------------------------------------
//  ProxyConnectLoginResponseMsg::Pack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyConnectLoginResponseMsg::Pack(BasePacket* pkt)
{
	BaseMessage::Pack(pkt);
	pkt->Push(&result, sizeof(result));
	pkt->Push(&proxyId, sizeof(proxyId));
	pkt->Push(&maxproxyid, sizeof(maxproxyid));
	serverIpItemNum = servers.size();
	pkt->Push(&serverIpItemNum, sizeof(serverIpItemNum));
	for (int i = 0; i < serverIpItemNum; ++i)
	{
		pkt->Push(&servers[i], sizeof(servers[i]));
	}
}

//---------------------------------------------------------------------------
//  ProxyConnectLoginResponseMsg::Size
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 ProxyConnectLoginResponseMsg::Size()
{
	return BaseMessage::Size()+sizeof(result)+sizeof(proxyId) + sizeof(maxproxyid) +
		sizeof(serverIpItemNum)+sizeof(GameServerInfo)*serverIpItemNum;
}

//=============================================================================

//---------------------------------------------------------------------------
//  ProxyConnectLoginCompleteMsg::ProxyConnectLoginCompleteMsg
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
ProxyConnectLoginCompleteMsg::ProxyConnectLoginCompleteMsg()
{
	m_nType = MsgType::PROXY_CONNECT_LOGIN_COMPLETE;
	result = MsgResult::SUCCESS;
}

//---------------------------------------------------------------------------
//  ProxyConnectLoginCompleteMsg::Unpack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyConnectLoginCompleteMsg::Unpack(BasePacket* pkt)
{
	BaseMessage::Unpack(pkt);
	pkt->Pop(&result, sizeof(result));
}

//---------------------------------------------------------------------------
//  ProxyConnectLoginCompleteMsg::Pack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyConnectLoginCompleteMsg::Pack(BasePacket* pkt)
{
	BaseMessage::Pack(pkt);
	pkt->Push(&result, sizeof(result));
}

//---------------------------------------------------------------------------
//  ProxyConnectLoginCompleteMsg::Size
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 ProxyConnectLoginCompleteMsg::Size()
{
	return BaseMessage::Size()+sizeof(result);
}

//=============================================================================



//---------------------------------------------------------------------------
//  ProxyStatusReplyMsg::ProxyStatusReplyMsg
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
ProxyStatusReplyMsg::ProxyStatusReplyMsg()
{
	m_nType = MsgType::PROXY_STATUS_REPLY;
}


//---------------------------------------------------------------------------
//  ProxyStatusReplyMsg::Pack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyStatusReplyMsg::Pack(BasePacket* pkt)
{
	BaseMessage::Pack(pkt);
	pkt->Push(&playerNum1, sizeof(playerNum1));
	pkt->Push(&playerNum2, sizeof(playerNum2));
}

//---------------------------------------------------------------------------
//  ProxyStatusReplyMsg::Size
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 ProxyStatusReplyMsg::Size()
{
	return BaseMessage::Size()+sizeof(playerNum1)+sizeof(playerNum2);
}

//---------------------------------------------------------------------------
//  ProxyStatusReplyMsg::SetStatus
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyStatusReplyMsg::SetStatus(uint16 _playerNum1,uint16 _playerNum2)
{
	playerNum1 = _playerNum1;
	playerNum2 = _playerNum2;
}


//===============================================================================


//---------------------------------------------------------------------------
//  SendSession2ProxyReqestMsg::SendSession2ProxyReqestMsg
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
SendSession2ProxyReqestMsg::SendSession2ProxyReqestMsg()
{

}

//---------------------------------------------------------------------------
//  SendSession2ProxyReqestMsg::Unpack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
	void
SendSession2ProxyReqestMsg::Unpack(BasePacket* pkt)
{
	BaseMessage::Unpack(pkt);
	pkt->Pop(&m_nUserID, sizeof(m_nUserID));
	pkt->Pop(&m_nSession, sizeof(m_nSession));
	pkt->Pop(&m_Clien_Info, sizeof(m_Clien_Info));
	pkt->Pop(&m_nLoginID,sizeof(m_nLoginID));
	pkt->Pop(&m_nRoleServerID,sizeof(m_nRoleServerID));
	pkt->Pop(&m_nProxyID,sizeof(m_nProxyID));
	pkt->Pop(&tm,sizeof(struct timeval));   //zhaoyg

}


//---------------------------------------------------------------------------
//  SendSession2ProxyReqestMsg::Size
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 SendSession2ProxyReqestMsg::Size()
{
	return BaseMessage::Size()+sizeof(m_nUserID)+sizeof(m_nSession)+sizeof(m_Clien_Info)+
		sizeof(m_nLoginID)+sizeof(m_nRoleServerID)+sizeof(m_nProxyID)+sizeof(struct timeval);

}

//===============================================================================


//---------------------------------------------------------------------------
//  SendSession2ProxyResponseMsg::SendSession2ProxyResponseMsg
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
SendSession2ProxyResponseMsg::SendSession2ProxyResponseMsg()
{

	m_nType = MsgType:: SEND_SESSION_TO_PROXY_RESPONSE ;
	m_nResult = MsgResult::SUCCESS; 
}



//---------------------------------------------------------------------------
//  SendSession2ProxyResponseMsg::Pack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
	void
SendSession2ProxyResponseMsg::Pack(BasePacket* pkt)
{
	BaseMessage::Pack(pkt);
	pkt->Push(&m_nResult, sizeof(m_nResult));
	pkt->Push(&m_nUserID,sizeof(m_nUserID));
	pkt->Push(&m_nSession,sizeof(m_nSession));
	pkt->Push(&m_nLoginID,sizeof(m_nLoginID));
	pkt->Push(&m_nRoleServerID,sizeof(m_nRoleServerID));
	pkt->Push(&m_nProxyID,sizeof(m_nProxyID));
	pkt->Push(&tm,sizeof(struct timeval));  //zhaoyg


}

//---------------------------------------------------------------------------
//  SendSession2ProxyResponseMsg::Size
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 SendSession2ProxyResponseMsg::Size()
{
	return BaseMessage::Size()+sizeof(m_nResult)+sizeof(m_nUserID)+sizeof(m_nSession)+sizeof(m_nLoginID)+
		sizeof(m_nRoleServerID)+sizeof(m_nProxyID)+sizeof(struct timeval);

}

//=============================================================================


//---------------------------------------------------------------------------
//  AddServerRequestMsg::AddServerRequestMsg
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
AddServerRequestMsg::AddServerRequestMsg()
{
	memset(&_newServer,0,sizeof(_newServer));
	memset(&m_version,0,sizeof(m_version));
}

//---------------------------------------------------------------------------
//  AddServerRequestMsg::Pack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void AddServerRequestMsg::Pack(BasePacket* pkt)
{
	BaseMessage::Pack(pkt);
	pkt->Push(&_newServer, sizeof(_newServer));
	pkt->Push(&m_version,sizeof(m_version));

}

//---------------------------------------------------------------------------
//  AddServerRequestMsg::Unpack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void AddServerRequestMsg::Unpack(BasePacket* pkt)
{
	BaseMessage::Unpack(pkt);
	pkt->Pop(&_newServer, sizeof(_newServer));
	pkt->Pop(&m_version, sizeof(m_version));

}

//---------------------------------------------------------------------------
//  AddServerRequestMsg::Size
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 AddServerRequestMsg::Size()
{
	return BaseMessage::Size() + sizeof(_newServer) + sizeof(m_version);
}

//======================================================================


//---------------------------------------------------------------------------
//  AddServerResponseMsg::AddServerResponseMsg
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
AddServerResponseMsg::AddServerResponseMsg()
{
	m_nType = MsgType::ADD_SERVER_RESPONSE;
	memset(&_addedServer,0,sizeof(_addedServer));
	memset(&m_version,0,sizeof(m_version));
}

//---------------------------------------------------------------------------
//  AddServerResponseMsg::Pack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void AddServerResponseMsg::Pack(BasePacket* pkt)
{
	BaseMessage::Pack(pkt);
	pkt->Push(&_addedServer, sizeof(_addedServer));
	pkt->Push(&m_version,sizeof(m_version));

}

//---------------------------------------------------------------------------
//  AddServerResponseMsg::Unpack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void AddServerResponseMsg::Unpack(BasePacket* pkt)
{
	BaseMessage::Unpack(pkt);
	pkt->Pop(&_addedServer, sizeof(_addedServer));
	pkt->Pop(&m_version, sizeof(m_version));

}

//---------------------------------------------------------------------------
//  AddServerResponseMsg::Size
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 AddServerResponseMsg::Size()
{
	return BaseMessage::Size() + sizeof(_addedServer) + sizeof(m_version);
}

//===========================================================================


//  在这揽义嚎咖端靠的消坑靠揩西
//---------------------------------------------------------------------------
//  ProxyMessage::ProxyMessage
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
ProxyMessage::ProxyMessage()
{
	m_nType = MsgType::MESSAGE_FROM_PROXY;
	m_pData = 0;
	m_nLength = 0;
	m_nServerID = 0;
	m_nProxyID = 0;
	m_nClientID = 0;
}

//---------------------------------------------------------------------------
//  ProxyMessage::~ProxyMessage
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
ProxyMessage::~ProxyMessage()
{
	if(m_pData)
	{
		delete[] m_pData;
		m_pData = 0;
	}
}

//---------------------------------------------------------------------------
//  ProxyMessage::Pack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyMessage::Pack(BasePacket* pkt)
{
	BaseMessage::Pack(pkt);
	pkt->Push(&m_nServerID, sizeof(m_nServerID));
	pkt->Push(&m_nProxyID, sizeof(m_nProxyID));
	pkt->Push(&m_nClientID, sizeof(m_nClientID));
	pkt->Push(&m_nLength, sizeof(m_nLength));
	pkt->Push(m_pData, m_nLength);
}

//---------------------------------------------------------------------------
//  ProxyMessage::Unpack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyMessage::Unpack(BasePacket* pkt)
{
	BaseMessage::Unpack(pkt);
	pkt->Pop(&m_nServerID, sizeof(m_nServerID));
	pkt->Pop(&m_nProxyID, sizeof(m_nProxyID));
	pkt->Pop(&m_nClientID, sizeof(m_nClientID));
	pkt->Pop(&m_nLength, sizeof(m_nLength));
	m_pData = new char[m_nLength];
	pkt->Pop(m_pData, m_nLength);
}

//---------------------------------------------------------------------------
//  ProxyMessage::Size
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 ProxyMessage::Size()
{
	return  BaseMessage::Size() +
		sizeof(m_nServerID) + 
		sizeof(m_nProxyID) + 
		sizeof(m_nClientID) + 
		sizeof(m_nLength) +
		m_nLength;
}

//=================================================================

//---------------------------------------------------------------------------
//  SendProxyid2GS::SendProxyid2GS
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
SendProxyid2GS::SendProxyid2GS()
{
	m_nType = MsgType::PROXY_CONNECT_GAMESERVER_REQUEST;
}


//---------------------------------------------------------------------------
//  SendProxyid2GS::Pack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void SendProxyid2GS::Pack(BasePacket* pkt)
{
	BaseMessage::Pack(pkt);
	pkt->Push(&m_nProxyID, sizeof(m_nProxyID));

}

//---------------------------------------------------------------------------
//  SendProxyid2GS::Size
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 SendProxyid2GS::Size()
{
	return  BaseMessage::Size() + sizeof(m_nProxyID);

}

//===============================================================


//---------------------------------------------------------------------------
//  SendProxyid2GSres::SendProxyid2GSres
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
SendProxyid2GSres::SendProxyid2GSres()
{

}


//---------------------------------------------------------------------------
//  SendProxyid2GSres::Unpack
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void SendProxyid2GSres::Unpack(BasePacket* pkt)
{
	BaseMessage::Unpack(pkt);
	pkt->Push(&mResult, sizeof(mResult));

}

//---------------------------------------------------------------------------
//  SendProxyid2GSres::Size
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 SendProxyid2GSres::Size()
{
	return  BaseMessage::Size() + sizeof(mResult);

}

//==========================================================================





uint16 ClientObject::mClientSum = 0;    //Init static variable,the number of client 

//---------------------------------------------------------------------------
//  ClientObject::ClientObject
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
	ClientObject::ClientObject(Socket* pSocket, uint32 id)
: SocketHandler(pSocket), mClientID(id),mAuthentication(0)
{
	mClientSum++;
}

//---------------------------------------------------------------------------
//  ClientObject::~ClientObject
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
ClientObject::~ClientObject(void)
{
	mClientSum--;

}

//---------------------------------------------------------------------------
//  ClientObject::OnRead
//---------------------------------------------------------------------------
//  Description:  proxy receive  packet from client, and send to game server.
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
int ClientObject::OnRead()           //proxy 与 Client  的靠
{

	uint32 nLen = 0;
	uint32 nAuthCode = 0;
	struct PROXY_MSG_HEADER *pProxy = NULL;
	while(true)
	{
		nLen = 0;
		if(m_oReadBuffer.Read(&nLen, sizeof(nLen), false) < 0)
		{
			return -1;
		}

		if(m_oReadBuffer.Size() < sizeof(nLen) + nLen)
		{
			// 型暾?
			break;
		}

		char* pProxyData = new char[sizeof(nLen) + nLen];  //070131
		if (pProxyData == NULL)
		{
			return -1;
		}

		m_oReadBuffer.Read(pProxyData,sizeof(nLen)+nLen,true);   

		if (nLen < sizeof(struct PROXY_MSG_HEADER))
		{       delete[] pProxyData;
			pProxyData = NULL;
			continue;
		}

		pProxy = (struct PROXY_MSG_HEADER *)(pProxyData + sizeof(nLen));

		if(GetDataType() == 0)
		{      printf("\n get client nAuthtication package !\n");
			nAuthCode = pProxy->mClientID; 
			nAuthCode <<= 16;
			nAuthCode += pProxy->mProxyID;

			if ( Proxy::Instance()->FindAuthenInfo(nAuthCode) == 0)
			{ 
				printf("\nAuthtication  failure \n");
				ProxyManagerObject::Instance()->GetEpoll()->Remove(this);  /// 需要考虑

				delete[] pProxyData;
				return 0;
			}
			else 
			{
				SetDataType();
				delete[] pProxyData;

				pProxyData = NULL;
				continue;
			}
		}
		pProxy->mProxyID = Proxy::Instance()->GetProxyID();
		pProxy->mClientID = GetClientID();

                printf("\n=================== get  client package =================!\n");
                printf("\npProxy->mProxyID=%d\n",pProxy->mProxyID);
		printf("pProxy->mClientID=%d\n",pProxy->mClientID);
		printf("\npProxy->mServerID=%d\n",pProxy->mServerID);
		
		ServerObject* pServer = Proxy::Instance()->GetServerObjByID(pProxy->mServerID);
		if(!pServer)
		{
			printf("can't find server by id=%d\n",pProxy->mServerID);

			delete[] pProxyData;
			pProxyData = NULL;
			continue;

		}


		pServer->Write(pProxyData,nLen+sizeof(nLen));

                printf("\n=================== send to GS successful! =================!\n");
		delete[] pProxyData;
		pProxyData = NULL;



	}
}



//---------------------------------------------------------------------------
//  ClientObject::OnError
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
int ClientObject::OnError()
{ 
        printf("\n>>>>>>>>>>>>>>>>>>> delete clientID=%d   >>>>>>>>>>>>>>>>>\n",GetClientID());
	Proxy::Instance()->DeletClientObjByID(GetClientID());//zhaoyg

	return 0;
}

//===========================================================================




//---------------------------------------------------------------------------
//  ServerObject::
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
	ServerObject::ServerObject(Socket* pSocket, uint16 id)
: SocketHandler(pSocket), mGameServerID(id)
{

}

//---------------------------------------------------------------------------
//  ServerObject::~ServerObject
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
ServerObject::~ServerObject(void)
{

}

//---------------------------------------------------------------------------
//  ServerObject::OnRead
//---------------------------------------------------------------------------
//  Description:  proxy receive packet from game server, and send to client
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
int ServerObject::OnRead()   //proxy 与 gs 的靠。  完成
{

	uint32 nLen;
	uint16 nClientID = 0;
	struct PROXY_MSG_HEADER *pProxy = NULL;



	while(true)
	{
		nLen = 0;
		if(m_oReadBuffer.Read(&nLen, sizeof(nLen), false) < 0)
		{
			return -1;
		}

		if(m_oReadBuffer.Size() < nLen + sizeof(nLen))
		{
			// 型暾?
			break;
		}
		// 很庖宓膎ew

		char* pProxyData = new char[sizeof(nLen) + nLen];
		if (pProxyData == NULL)
		{
			return -1;
		}

		m_oReadBuffer.Read(pProxyData,sizeof(nLen)+nLen,true);

		if (nLen < sizeof(struct PROXY_MSG_HEADER)) //deformity  package
		{  
			delete[] pProxyData;  //
			pProxyData = NULL;
			continue;
		}


		pProxy = (struct PROXY_MSG_HEADER *)(pProxyData + sizeof(nLen));

		nClientID = pProxy->mClientID;
                 

		 printf("\n=================== get  GS   package =================!\n");
		 printf("\npProxy->mProxyID=%d\n",pProxy->mProxyID);
		printf("pProxy->mClientID=%d\n",nClientID);
		printf("\npProxy->mServerID=%d\n",pProxy->mServerID);

		ClientObject* pClient = Proxy::Instance()->GetClientObjByID(nClientID);
		if(!pClient)
		{
			printf("can't find client by id=%d\n",nClientID);

			delete[] pProxyData;  //
			pProxyData = NULL;
			continue;;

		}


		pClient->Write(pProxyData,sizeof(nLen)+nLen);


		delete[] pProxyData;
		pProxyData = NULL;




	}

}

//---------------------------------------------------------------------------
//  ServerObject::OnError
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
int ServerObject::OnError()
{
        printf("\n******************delete  ServerID=%d================\n",GetServerID());
	Proxy::Instance()->DeleServerObjByID(GetServerID());   //zhaoyg
	return 0;
}


//==================================================================

//************************ProxyManagerObject*************************



//is a static variable pointer, point to ProxyManagerObject  object 

ProxyManagerObject* ProxyManagerObject::mInstance = 0; 
//bool ProxyManagerObject::status = false;

//---------------------------------------------------------------------------
//  ProxyManagerObject::ProxyManagerObject
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
	ProxyManagerObject::ProxyManagerObject(Socket* pSocket)
: SocketHandler(pSocket)
{

}

//---------------------------------------------------------------------------
//  ProxyManagerObject::~ProxyManagerObject
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
ProxyManagerObject::~ProxyManagerObject(void)
{

}

//---------------------------------------------------------------------------
//  ProxyManagerObject::Instance
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
ProxyManagerObject* ProxyManagerObject::Instance()
{
	return mInstance;
}



//---------------------------------------------------------------------------
//  ProxyManagerObject::OnRead
//---------------------------------------------------------------------------
//  Description:  receive four type packet from proxy manager. 
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
int ProxyManagerObject::OnRead()     //肯钟隤M的靠，并建b与gs 的l娇?
{

	GameServerInfo gmserviceinfo;
	char *pdata = NULL;  //卓颗
	char *pbuffer =NULL;
	uint32  lensize = 0;
	uint32   length = 0;
	uint32  constlen = 0;
	uint32 tmpcode = 0;


	ProxyStatusReplyMsg proxystatus;

	ProxyConnectLoginResponseMsg proxyLoginRes;  //070111

	ProxyConnectLoginCompleteMsg proxyLoginComplete;


	SendSession2ProxyReqestMsg ProxySessionCode;
	SendSession2ProxyResponseMsg ProxySessionRes;

	AddServerRequestMsg ProxyAddServerReq;
	AddServerResponseMsg ProxyAddServerRes;

	constlen = sizeof(constlen);

	while(true)
	{
		if ( m_oReadBuffer.Size() <= constlen )
		{
			return  0;
		}


		m_oReadBuffer.Read(&length,constlen, false);
		if (m_oReadBuffer.Size() < length + constlen)
		{

			return 0;
		}

		m_oReadBuffer.Read(&length,constlen, true);
		pdata = new char[length];

		m_oReadBuffer.Read(pdata, length, true);
		BasePacket  packet(pdata,length,true);


		switch(BaseMessage::Type(&packet))
		{
			case MsgType::PROXY_CONNECT_LOGIN_RESPONSE: 
				{

					proxyLoginRes.Unpack(&packet);
					//0201 mInstance->SetProxyID( proxyLoginRes.GetProxyID() );
					Proxy::Instance()->SetProxyID(proxyLoginRes.GetProxyID());  //

					proxyLoginComplete.result = MsgResult::SUCCESS;;

					BasePacket  proxyLoginCompletePacket(true);
					proxyLoginComplete.Pack(&proxyLoginCompletePacket);
					lensize = proxyLoginCompletePacket.Length();

					pbuffer = new char[lensize + sizeof(lensize)];
					memcpy(pbuffer,&lensize,sizeof(lensize));
					memcpy(pbuffer+sizeof(lensize),proxyLoginCompletePacket.Data(),lensize);
					Write(pbuffer,lensize+sizeof(lensize));

					for (int i = 0; i < proxyLoginRes.GetServerIpItemNum(); ++i)
					{
						printf("\nget GS !\n");
						ConnectGameServer(proxyLoginRes.servers[i],Proxy::Instance()->GetProxyID());

					}
					delete[] pbuffer;
					pbuffer = NULL;

				}
				break;
			case MsgType::ADD_SERVER_REQUEST:
				{
					ProxyAddServerReq.Unpack(&packet);
					printf("\ngmserviceinfo.mid =%d\n",ProxyAddServerReq._newServer._id);
					printf("\ngmserviceinfo.mip =%d\n",ProxyAddServerReq._newServer._ip);
					printf("\ngmserviceinfo.mPort=%d\n",ProxyAddServerReq._newServer._gamePort);

					gmserviceinfo.mid = ProxyAddServerReq._newServer._id;
					gmserviceinfo.mip = ProxyAddServerReq._newServer._ip;
					gmserviceinfo.mPort = ProxyAddServerReq._newServer._gamePort;

					memcpy(&ProxyAddServerRes._addedServer,&ProxyAddServerReq._newServer,sizeof(ProxyAddServerReq._newServer));
					memcpy(&ProxyAddServerRes.m_version,&ProxyAddServerReq.m_version,sizeof(ProxyAddServerReq.m_version));

					//ProxyAddServerRes.Write(Stream());

					BasePacket  ProxyAddServerResPacket(true);
					ProxyAddServerRes.Pack(&ProxyAddServerResPacket);

					lensize = ProxyAddServerResPacket.Length();

					pbuffer = new char[lensize + sizeof(lensize)];
					memcpy(pbuffer,&lensize,sizeof(lensize));
					memcpy(pbuffer+sizeof(lensize),ProxyAddServerResPacket.Data(),lensize);
					Write(pbuffer,lensize+sizeof(lensize));
					printf("\n ADD GS!\n");
					ConnectGameServer(gmserviceinfo,Proxy::Instance()->GetProxyID());	    
					delete[] pbuffer;
					pbuffer = NULL;


				}
				break;
			case  MsgType::SEND_SESSION_TO_PROXY_REQUEST:
				{


					ProxySessionCode.Unpack(&packet);   

					tmpcode =  ProxySessionCode.GetCode();
					//0201mAuthenInfo.push_back(tmpcode);
					Proxy::Instance()->PutAuthenInfo(tmpcode);

					memcpy(&ProxySessionRes.tm,&ProxySessionCode.tm,sizeof(struct timeval));

					ProxySessionRes.SetLoginID(ProxySessionCode.GetLoginID());
					ProxySessionRes.SetProxyID(ProxySessionCode.GetProxyID());
					ProxySessionRes.SetRoleServerID(ProxySessionCode.GetRoleServerID());
					ProxySessionRes.SetUserID(ProxySessionCode.GetUserID());
					ProxySessionRes.SetSession(ProxySessionCode.GetSession());

					BasePacket  SessionResPacket(true);
					ProxySessionRes.Pack(&SessionResPacket);

					lensize = SessionResPacket.Length();

					pbuffer = new char[lensize + sizeof(lensize)];

					memcpy(pbuffer,&lensize,sizeof(lensize));
					memcpy(pbuffer+sizeof(lensize),SessionResPacket.Data(),lensize);
					Write(pbuffer,lensize+sizeof(lensize));

					delete[] pbuffer;
					pbuffer = NULL;


				}
				break;
			case  MsgType::PROXY_STATUS_QUERY:
				{

					proxystatus.SetStatus(ClientObject::GetClientSum());  //set proxy status

					BasePacket  proxyStatusPacket(true);
					proxystatus.Pack(&proxyStatusPacket);
					lensize = proxyStatusPacket.Length();

					pbuffer = new char[lensize + sizeof(lensize)];

					memcpy(pbuffer,&lensize,sizeof(lensize));
					memcpy(pbuffer+sizeof(lensize),proxyStatusPacket.Data(),lensize);
					Write(pbuffer,lensize+sizeof(lensize));
					delete[] pbuffer;
					pbuffer = NULL;


				}
				break;
			default:
				break;
		}



	}
	return 0;
}


//---------------------------------------------------------------------------
//  ProxyManagerObject::OnError
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
int ProxyManagerObject::OnError()
{  
      printf("\n Set  PM  SetNoConnecting \n");
      fflush(stdout);
      SetInstanceNULL();
       //SetNoConnecting();
	return 1;
}


//---------------------------------------------------------------------------
//  ProxyManagerObject::Init
//---------------------------------------------------------------------------
//  Description:  building a ProxyManagerObject object,and be point to mInstance
//  Parameters:    
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
int ProxyManagerObject::Init(Epoll* pEpoll)
{
	// 请求Server量硇趴
	char *pbuffer = NULL;
	uint32  datalen=0;
	try
	{
		Socket* pProxyMgrSocket = new Socket(g_arg.proxyManagerIP.c_str(), g_arg.proxyManagerPort);  //connect  fd
		if(!pProxyMgrSocket)
		{
			printf("error in connect to proxy manager\n");
			exit(-1);
		}

		pProxyMgrSocket->ConfigureBlocking(false);


		
	        mInstance = new ProxyManagerObject(pProxyMgrSocket);
		
		mInstance->SetEpoll(pEpoll);

		pEpoll->Add(mInstance);   //070130
		ProxyConnectLoginRequestMsg proxyLoginRequest(g_arg.localIP.c_str(),g_arg.localPort);
		BasePacket  packet(true);
		proxyLoginRequest.SetProxyID(Proxy::Instance()->GetProxyID());
		proxyLoginRequest.Pack(&packet);

		datalen = packet.Length();//070130
		pbuffer = new char[sizeof(datalen) + datalen]; //070130
		memcpy(pbuffer,&datalen,sizeof(datalen));   //070130
		memcpy(pbuffer+sizeof(datalen),packet.Data(),datalen);  //070130
		mInstance->Write(pbuffer,datalen+sizeof(datalen));   //070130
		
		delete[] pbuffer;  //070130
		return 1;
	}
	catch (exception& e)
	{
		cout << "exception: " << e.what() << endl
			<< "\tat ConnectGameServer::execute"
			<< SrcLocation(__FILE__, __LINE__) << endl;

		return -1;
	}
}





//---------------------------------------------------------------------------
//  ProxyManagerObject::Destroy
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ProxyManagerObject::Destroy()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = 0;
	}
}


//---------------------------------------------------------------------------
//  ProxyManagerObject::ConnectGameServer
//---------------------------------------------------------------------------
//  Description:  connect to game server,
//  Parameters:   serverInfo is a reference(in), proxyid(in)
//  Return:       success retrun 0,error  return -1
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
int ProxyManagerObject::ConnectGameServer(GameServerInfo& serverInfo,uint16 proxyid)
{
	SendProxyid2GS sendproxyid;
	SendProxyid2GSres  sendproxres;
	BasePacket sendPacket(true);
	BasePacket resPacket(true);

	try
	{       
		Socket* pSocket = new Socket(serverInfo.mip, serverInfo.mPort);

		if (pSocket == NULL)
			return -1;
			
		pSocket->ConfigureBlocking(true);   //
		ServerObject* pServer = new ServerObject(pSocket, serverInfo.mid);
		if(pServer)
		{   

			sendproxyid.m_nProxyID = proxyid;
			sendproxyid.Pack(&sendPacket);


			sendPacket.Write(pServer->Stream());  ////send proxyid to game server. request

			resPacket.Read(pServer->Stream());  /// receive response from game server.


			sendproxres.Unpack(&resPacket);

			if (sendproxres.m_nType ==MsgType::PROXY_CONNECT_GAMESERVER_RESPONSE )
			{ 
				pServer->GetSocket()->ConfigureBlocking(false); //设主亲枞?

				Proxy::Instance()->PutServerObject(serverInfo.mid,pServer);  //
				m_pEpoll->Add(pServer);
				return 0;
			}
		}
	}
	catch (exception& e)
	{
		cout << "exception: " << e.what() << endl
			<< "\tat ConnectGameServer::execute"
			<< SrcLocation(__FILE__, __LINE__) << endl;

		return -1;
	}

	return -1;
}

//========================================================================


Proxy* Proxy::mInstance = 0;

//---------------------------------------------------------------------------
//  Proxy::
//---------------------------------------------------------------------------
//  Description:  connect to game server,
//  Parameters:   serverInfo is a reference(in), proxyid(in)
//  Return:       success retrun 0,error  return -1
//  Exception:    
//  MT Safe:      
//  Note:         
//--------------------------------------------------------------------------

Proxy::Proxy():mProxyID(0)
{
}

//---------------------------------------------------------------------------
//  Proxy::
//---------------------------------------------------------------------------
//  Description:  connect to game server,
//  Parameters:   serverInfo is a reference(in), proxyid(in)
//  Return:       success retrun 0,error  return -1
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------

Proxy::~Proxy()
{
}

//---------------------------------------------------------------------------
//  Proxy::
//---------------------------------------------------------------------------
//  Description:  connect to game server,
//  Parameters:   serverInfo is a reference(in), proxyid(in)
//  Return:       success retrun 0,error  return -1
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
int Proxy::Init()
{
	if(!mInstance)
	{
		mInstance = new Proxy();
	}
	return 0;
}


//---------------------------------------------------------------------------
//  Proxy::
//---------------------------------------------------------------------------
//  Description:  connect to game server,
//  Parameters:   serverInfo is a reference(in), proxyid(in)
//  Return:       success retrun 0,error  return -1
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void Proxy::PutServerObject(uint16 id, ServerObject* pServer)
{
	mMapServerList.insert(make_pair(id, pServer));
}

//---------------------------------------------------------------------------
//  Proxy::
//---------------------------------------------------------------------------
//  Description:  connect to game server,
//  Parameters:   serverInfo is a reference(in), proxyid(in)
//  Return:       success retrun 0,error  return -1
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------

ServerObject* Proxy::GetServerObjByID(uint16 id)
{

	map<uint16, ServerObject*>::iterator iter = mMapServerList.find(id);
	if(iter != mMapServerList.end())
	{
		return iter->second;
	}

	return NULL;
}
//---------------------------------------------------------------------------
//  Proxy::
//---------------------------------------------------------------------------
//  Description:  connect to game server,
//  Parameters:   serverInfo is a reference(in), proxyid(in)
//  Return:       success retrun 0,error  return -1
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------

void Proxy::DeleServerObjByID(uint16 id)
{
	map<uint16, ServerObject*>::iterator iter = mMapServerList.find(id);
	if(iter != mMapServerList.end())
	{
		mMapServerList.erase(iter);
	}
}

//---------------------------------------------------------------------------
//  Proxy::
//---------------------------------------------------------------------------
//  Description:  connect to game server,
//  Parameters:   serverInfo is a reference(in), proxyid(in)
//  Return:       success retrun 0,error  return -1
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------

void Proxy::PutAuthenInfo(uint32 autheninfo)
{
	mAuthenInfo.push_back(autheninfo);
}

//---------------------------------------------------------------------------
//  Proxy::
//---------------------------------------------------------------------------
//  Description:  connect to game server,
//  Parameters:   serverInfo is a reference(in), proxyid(in)
//  Return:       success retrun 0,error  return -1
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
int Proxy::FindAuthenInfo(uint32  autheninfo)
{
	vector<uint32>::iterator itervector;

	while ( ( itervector = find(mAuthenInfo.begin(),mAuthenInfo.end(),autheninfo) ) != mAuthenInfo.end()  )
	{
		mAuthenInfo.erase(itervector);
		return 1;
	}

	return 0;


}


//---------------------------------------------------------------------------
//  Proxy::
//---------------------------------------------------------------------------
//  Description:  connect to game server,
//  Parameters:   serverInfo is a reference(in), proxyid(in)
//  Return:       success retrun 0,error  return -1
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void Proxy::DeleAuthenInfo(uint32 autheninfo)
{

}

//---------------------------------------------------------------------------
//  Proxy::
//---------------------------------------------------------------------------
//  Description:  connect to game server,
//  Parameters:   serverInfo is a reference(in), proxyid(in)
//  Return:       success retrun 0,error  return -1
//  Exception:
//  MT Safe:
//  Note:
//---------------------------------------------------------------------------

ClientObject* Proxy::GetClientObjByID(uint16 id)
{
	map<uint16, ClientObject*>::iterator iter = mMapClientObj.find(id);
	if(iter != mMapClientObj.end())
	{
		return iter->second;
	}
	return NULL;
}

//---------------------------------------------------------------------------
//  ClientObjectFactory::DeletClientObjByID
//---------------------------------------------------------------------------
//  Description:
//  Parameters:
//  Return:
//  Exception:
//  MT Safe:
//  Note:
//---------------------------------------------------------------------------

void Proxy::DeletClientObjByID(uint16 id)
{
	map<uint16, ClientObject*>::iterator iter = mMapClientObj.find(id);
	if(iter != mMapClientObj.end())
	{
		mMapClientObj.erase(iter);
	}

}
//---------------------------------------------------------------------------
//  proxy::
//---------------------------------------------------------------------------
//  Description:
//  Parameters:
//  Return:
//  Exception:
//  MT Safe:
//  Note:
//---------------------------------------------------------------------------

void Proxy::PutClientObj(uint16 id, ClientObject *pClient)
{
	 mMapClientObj.insert(make_pair(id, pClient));
}




