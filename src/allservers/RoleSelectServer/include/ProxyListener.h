/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: ProxyListener.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 监听代理的消息

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _PROXY_LISTENER_H__
#define _PROXY_LISTENER_H__

class ProxyObject;
class ProxyObjectFactory : public SocketHandlerFactory
{
private:
    static ProxyObjectFactory* mProxyObjFactory;
public:
    virtual SocketHandler* Create(Socket* pSocket);

    ProxyObject* GetProxy(uint16 nProxyID);
    void Insert(uint16 nProxyID, ProxyObject* pProxy);
    static ProxyObjectFactory* Instance();

    static void Destroy();
    static int m_nProxyID;
    map<uint16, ProxyObject*> m_mapProxyObject;
};




class ProxyListener : public Runnable, public ServerSocketHandler
{
public:
    ProxyListener(ServerSocket* pSocket, 
		SocketHandlerFactory* pFactory);
    void run();
};

class ProxyObject : public SocketHandler
{
private:
    uint16 m_nProxyID;
public:
    ProxyObject(Socket* pSocket, int nId);
    ~ProxyObject(void);

	void SetID(uint16 id){ m_nProxyID = id;}
    virtual int OnRead();
    virtual int OnError();
};

class ProxyMsgSender : public Runnable
{
public:
    void run();
};

#endif
