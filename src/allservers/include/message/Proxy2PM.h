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

#ifndef PROXY_PM_HPP
#define PROXY_PM_HPP
#include "Message.h"
#include "SrvInfo.h"



class ProxyConnectGameServerRequest : public BaseMessage_T
{
public:
    ProxyConnectGameServerRequest();
    virtual ~ProxyConnectGameServerRequest(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    
    virtual uint32_t size();

    uint16_t _proxyid;
};


class ProxyConnectGameServerReply : public BaseMessage_T
{
public:
    ProxyConnectGameServerReply();
    virtual ~ProxyConnectGameServerReply(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    
    virtual uint32_t size();

    uint8_t _result;
};







//------------------------------------------------------------------
class ProxyConnectLoginRequestMsg_T : public BaseMessage_T
{
public:
    ProxyConnectLoginRequestMsg_T();
    virtual ~ProxyConnectLoginRequestMsg_T(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    
    virtual uint32_t size();
    
    uint32_t _proxyIp;
    uint16_t _proxyPort;
    uint16_t _maxproxyid;
    uint16_t _proxyid;
};

class ProxyConnectLoginResponseMsg_T : public BaseMessage_T
{
public:
    ProxyConnectLoginResponseMsg_T();
    virtual ~ProxyConnectLoginResponseMsg_T(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint8_t _result;
    uint16_t _proxyId;
    uint16_t _maxproxyid;
    uint16_t _serverIpItemNum;
    vector<SrvInfo_T> _servers;
};

class ProxyConnectLoginCompleteMsg_T : public BaseMessage_T
{
public:
    ProxyConnectLoginCompleteMsg_T();
    virtual ~ProxyConnectLoginCompleteMsg_T(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint8_t _result;
};
    
class ProxyDisconnectLoginRequest_T : public BaseMessage_T
{
public:
    ProxyDisconnectLoginRequest_T();
    virtual ~ProxyDisconnectLoginRequest_T(){};
};


class ProxyDisconnectLoginResponse_T : public BaseMessage_T
{
public:
    ProxyDisconnectLoginResponse_T();
    virtual ~ProxyDisconnectLoginResponse_T(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint8_t _result;
};

class ProxyDisconnectLoginCompleteRequest_T : public BaseMessage_T
{
public:
    ProxyDisconnectLoginCompleteRequest_T();
    virtual ~ProxyDisconnectLoginCompleteRequest_T(){};
};


class ProxyDisconnectLoginCompleteResponse_T : public BaseMessage_T
{
public:
    ProxyDisconnectLoginCompleteResponse_T();
    virtual ~ProxyDisconnectLoginCompleteResponse_T(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint8_t _result;
};

class ProxyStatusQueryMsg_T : public BaseMessage_T
{
public:
    ProxyStatusQueryMsg_T();
    virtual ~ProxyStatusQueryMsg_T(){};
};

class ProxyStatusReplyMsg_T : public BaseMessage_T
{
public:
    ProxyStatusReplyMsg_T();
    virtual ~ProxyStatusReplyMsg_T(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint16_t _playerNum1;
    uint16_t _playerNum2;    
};

class UpdateServerIpTable_T : public BaseMessage_T
{
public:
    UpdateServerIpTable_T();
    virtual ~UpdateServerIpTable_T(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();

    uint16_t _srvNum;
    vector<SrvInfo_T> _srvs;
};
#endif

