#ifndef _PROXY_OBJECT_H__
#define _PROXY_OBJECT_H__
#include "socket/Epoll.h"
#include "message/Message.h"
#include "MsgCode.h"
#include <set>


#define PROXY_MANAGER_HOST  "192.168.0.106"
#define PROXY_MANGGER_PORT  4569

#define RPOXY_HOST "192.168.0.106"
#define PROXY_LISTEN_PORT  7899 ///




#pragma pack (1)

struct ClientInfo
{
	uint16 _port;
	uint32 _id;
	uint32 _ip;   

};

struct PROXY_MSG_HEADER
{
	uint16 mType;
	uint16 mServerID;
	uint16 mProxyID;
	uint16 mClientID;
};

struct GameServerInfo
{
	uint16 mid;
	uint32 mip;
	uint16  mPort;     //int  to uint16
	uint16 _infoBusPort;

};

typedef struct MSTVersion
{
	uint16 _MaxServerId;
	uint32 _Version;

}MSTVersion;

typedef struct SrvInfo
{
	uint16 _id;
	uint32 _ip;
	uint16 _gamePort;             // Server listening on for clients
	uint16 _infoBusPort;      // InfoBus listening on
}SrvInfo;

#pragma pack ()

struct prxoyArg
{
	string proxyManagerIP;
	int proxyManagerPort;
	string localIP;
	int localPort;

};



namespace MsgResult
{
	const int
		SUCCESS = 1,
			FAILURE = 0;
};


class ClientObject : public SocketHandler
{
	private:
		uint16 mClientID;
		uint8   mAuthentication;   //authentication  is 1, common data is 0
		static  uint16  mClientSum; 
	public:
		ClientObject(Socket* pSocket, uint32 id);
		~ClientObject(void);
		const int GetDataType() const {return mAuthentication;};
		void  SetDataType(){mAuthentication = 1;};  
		uint16 GetClientID()const {return mClientID;};
		static  uint16 GetClientSum() {return mClientSum;};
		virtual int OnRead();
		virtual int OnError();
};



class ServerObject : public SocketHandler
{
	private:
		uint16 mGameServerID;
	public:
		ServerObject(Socket* pSocket, uint16 id);
		uint16 GetServerID(){return mGameServerID;};
		~ServerObject(void);

		virtual int OnRead();
		virtual int OnError();
};


class ProxyManagerObject : public SocketHandler
{
	public:
		static ProxyManagerObject* Instance();
		static int Init(Epoll* pEpoll);
		//static bool PMConnectingBool(){return status;};
		void SetInstanceNULL() {mInstance = 0;};	
		static void Destroy();
		virtual int OnRead();
		virtual int OnError();  
	private:
		static ProxyManagerObject* mInstance;
		//static  bool status;
	private:
		ProxyManagerObject(Socket* pSocket);
		~ProxyManagerObject(void); 
		int ConnectGameServer(GameServerInfo& serverInfo,uint16 proxyid);
		//void SetConnecting(){status = true;};
	        //void SetNoConnecting(){status = false;};
};


class Proxy
{
	public:
		Proxy();
		~Proxy(void);
		static Proxy *Instance(){return mInstance;};
		static int Init();
		void PutServerObject(uint16, ServerObject* pServer);
		ServerObject* GetServerObjByID(uint16 id);
		void DeleServerObjByID(uint16 id);

		void  SetProxyID(uint16 id){mProxyID = id;};
		uint16  GetProxyID() {return mProxyID;};

		void PutAuthenInfo(uint32 autheninfo);
		int   FindAuthenInfo(uint32 autheninfo);
		void DeleAuthenInfo(uint32 autheninfo);
		void PutClientObj(uint16 id, ClientObject *pClient);
		ClientObject* GetClientObjByID(uint16 id);
		void  DeletClientObjByID(uint16 id);
				
	private:
		static Proxy* mInstance; 
		map<uint16, ServerObject*> mMapServerList;
		map<uint16, ClientObject*> mMapClientObj;
		vector <uint32> mAuthenInfo;
		uint16 mProxyID;
		
};




class ProxyConnectLoginRequestMsg : public BaseMessage
{
	public:
		ProxyConnectLoginRequestMsg(const char *pHost, int nPort);
		virtual ~ProxyConnectLoginRequestMsg(void);

		virtual void Unpack(BasePacket* pkt);
		virtual void Pack(BasePacket* pkt);
		void Getip();
		void Getport();   
		void SetProxyID(uint16 id){proxyid = id; };
		virtual uint32 Size();

		uint32 proxyIp;
		uint16 proxyPort;
		uint16 maxproxyid;
		uint16 proxyid;
};

class ProxyConnectLoginResponseMsg : public BaseMessage
{
	public:
		ProxyConnectLoginResponseMsg();
		virtual ~ProxyConnectLoginResponseMsg(){};

		virtual void Unpack(BasePacket* pkt);
		virtual void Pack(BasePacket* pkt);
		virtual uint32 Size();
		uint16  GetServerIpItemNum(){return serverIpItemNum;};
		const uint16  GetProxyID()const{return proxyId;};
		uint8   result;
		uint16  proxyId;
		uint16  maxproxyid;
		uint16  serverIpItemNum;
		vector  <GameServerInfo> servers;
};

class ProxyConnectLoginCompleteMsg : public BaseMessage
{
	public:
		ProxyConnectLoginCompleteMsg();
		virtual ~ProxyConnectLoginCompleteMsg(){};

		void SetResult(uint8 _result){ result = _result;};
		virtual void Unpack(BasePacket* pkt);
		virtual void Pack(BasePacket* pkt);
		virtual uint32 Size();

		uint8 result;
};

class ProxyStatusReplyMsg : public BaseMessage
{
	public:
		ProxyStatusReplyMsg();
		virtual ~ProxyStatusReplyMsg(){};

		virtual void Pack(BasePacket* pkt);
		virtual uint32 Size();
		void SetStatus(uint16 _playerNum1,uint16 _playerNum2 = 0);

		uint16 playerNum1;
		uint16 playerNum2;    
};

class SendSession2ProxyReqestMsg : public BaseMessage
{
	public:
		SendSession2ProxyReqestMsg();
		virtual void Unpack(BasePacket* pkt);
		virtual uint32 Size();
		uint32  GetCode()const {return m_nSession;}

		uint16  GetLoginID() const {return m_nLoginID;}
		uint16  GetRoleServerID()const {return m_nRoleServerID;}
		uint16  GetProxyID()const {return m_nProxyID; };
		uint32  GetUserID() const {return m_nUserID;};
		uint32  GetSession() const {return m_nSession;};

		uint32  m_nUserID;
		uint32  m_nSession;
		ClientInfo m_Clien_Info;

		uint16 m_nLoginID;
		uint16 m_nRoleServerID;
		uint16 m_nProxyID;
		struct timeval tm;

};

class SendSession2ProxyResponseMsg : public BaseMessage
{
	public:
		SendSession2ProxyResponseMsg();
		//virtual void Unpack(BasePacket* pkt);
		virtual void Pack(BasePacket* pkt);
		virtual uint32 Size();

		void  SetLoginID(uint16 LoginID)  {m_nLoginID =LoginID;}
		void  SetRoleServerID(uint16 RoleServerID){m_nRoleServerID =RoleServerID ;}
		void  SetProxyID(uint16 ProxyID) {m_nProxyID = ProxyID; };
		void  SetUserID(uint32 UserID) {m_nUserID = UserID;};
		void  SetSession(uint32 Session){m_nSession=Session;};
		uint8 m_nResult;
		uint32  m_nUserID;
		uint32  m_nSession;
		uint16  m_nLoginID;
		uint16  m_nRoleServerID;
		uint16 m_nProxyID;
		struct timeval tm;




};

class AddServerRequestMsg : public BaseMessage
{
	public:
		AddServerRequestMsg();

		virtual void Unpack(BasePacket* pkt);
		virtual void Pack(BasePacket* pkt);
		virtual uint32 Size();

		SrvInfo _newServer;
		MSTVersion  m_version;

};

class AddServerResponseMsg : public BaseMessage
{
	public:
		AddServerResponseMsg();

		virtual void Unpack(BasePacket* pkt);
		virtual void Pack(BasePacket* pkt);
		virtual uint32  Size();

		SrvInfo _addedServer;    // login set id to 0 to indicate an error
		MSTVersion  m_version;
};


/**
 * 来自代理消息的封装
 */
class ProxyMessage : public BaseMessage
{
	public:
		uint16 m_nLength;
		uint16 m_nServerID;
		uint16 m_nProxyID;
		uint16 m_nClientID;
		char*  m_pData;

		ProxyMessage();
		virtual ~ProxyMessage();
		uint16 GetHeaderLength(){return sizeof(m_nServerID)+sizeof(m_nProxyID)+sizeof(m_nClientID);}
		virtual void Pack(BasePacket* pPacket);
		virtual void Unpack(BasePacket* pPacket);
		virtual uint32 Size();
};

class SendProxyid2GS : public BaseMessage
{
	public:
		uint16 m_nProxyID;

		SendProxyid2GS();
		virtual ~SendProxyid2GS(){};
		virtual void Pack(BasePacket* pPacket);
		virtual uint32 Size();
};

class SendProxyid2GSres : public BaseMessage
{
	public:
		uint8 mResult;

		SendProxyid2GSres();
		virtual ~SendProxyid2GSres(){};
		virtual void Unpack(BasePacket* pPacket);
		virtual uint32 Size();
};

#endif



