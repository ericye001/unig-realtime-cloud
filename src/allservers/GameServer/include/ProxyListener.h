/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: ProxyListener.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: communication with proxy server

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _PROXY_LISTENER_H__
#define _PROXY_LISTENER_H__

class ProxyObject;

/**
 * use to create ProxyObject
 */
class ProxyObjectFactory : public SocketHandlerFactory
{
private:
    static ProxyObjectFactory* m_pInstance;
    map<uint16, ProxyObject*> m_mapProxyObject;

private:
    ProxyObjectFactory(){};
    ~ProxyObjectFactory(){};

    bool operator =(ProxyObjectFactory& trh);

public:
    static ProxyObjectFactory* Instance();
    static void Destroy();
    virtual SocketHandler* Create(Socket* pSocket);
 
    ProxyObject* GetProxy(uint16 nProxyID);
    void Insert(uint16 nProxyID, ProxyObject* pProxy);

};




class ProxyListener : public Runnable, public ServerSocketHandler
{
public:
    ProxyListener(ServerSocket* pSocket, SocketHandlerFactory* pFactory = ProxyObjectFactory::Instance());
    virtual void run();
};

class ProxyObject : public SocketHandler
{
private:
    uint16 m_nProxyID;
public:
    ProxyObject(Socket* pSocket, uint16 nId);
    ~ProxyObject(void);

    virtual int OnRead();
    virtual int OnError();

    uint16 GetID(){return m_nProxyID;}
    void SetID(uint16 nId){m_nProxyID = nId;}
};

class ProxyMsgSender : public Runnable
{
public:
    virtual void run();
};

#endif
