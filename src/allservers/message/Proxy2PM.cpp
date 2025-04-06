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

#include "message/Proxy2PM.h"


/***********ProxyConnectGameServerRequest*********************

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


*********************************************8*/






ProxyConnectGameServerRequest::ProxyConnectGameServerRequest()
{
    _type = MsgType::PROXY_CONNECT_GAMESERVER_REQUEST;    
}

void ProxyConnectGameServerRequest::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_proxyid, sizeof(_proxyid));

}

void ProxyConnectGameServerRequest::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_proxyid, sizeof(_proxyid));
}

uint32_t ProxyConnectGameServerRequest::size()
{
    return BaseMessage_T::size()+ sizeof(_proxyid);
}





ProxyConnectGameServerReply::ProxyConnectGameServerReply()
{
    _type = MsgType::PROXY_CONNECT_GAMESERVER_REQUEST;    
}

void ProxyConnectGameServerReply::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));

}

void ProxyConnectGameServerReply::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
}

uint32_t ProxyConnectGameServerReply::size()
{
    return BaseMessage_T::size()+ sizeof(_result);
}







//---------------------------------------------------------------------------
ProxyConnectLoginRequestMsg_T::ProxyConnectLoginRequestMsg_T()
{
    _type = MsgType::PROXY_CONNECT_LOGIN_REQUEST; 
  
}

void ProxyConnectLoginRequestMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_proxyIp, sizeof(_proxyIp));
    pkt->pop(&_proxyPort, sizeof(_proxyPort));
    pkt->pop(&_maxproxyid, sizeof(_maxproxyid));
    pkt->pop(&_proxyid, sizeof(_proxyid));

}

void ProxyConnectLoginRequestMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_proxyIp, sizeof(_proxyIp));
    pkt->push(&_proxyPort, sizeof(_proxyPort));
    pkt->push(&_maxproxyid, sizeof(_maxproxyid));
    pkt->push(&_proxyid, sizeof(_proxyid));
}

uint32_t ProxyConnectLoginRequestMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_proxyIp) + sizeof(_proxyPort) + sizeof(_maxproxyid) + sizeof(_proxyid);
}

//---------------------------------------------------------------------------
ProxyConnectLoginResponseMsg_T::ProxyConnectLoginResponseMsg_T()
{
    _type = MsgType::PROXY_CONNECT_LOGIN_RESPONSE;
    _serverIpItemNum = 0;
    _servers.clear();
}

void ProxyConnectLoginResponseMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));
    pkt->pop(&_proxyId, sizeof(_proxyId));
    pkt->pop(&_maxproxyid, sizeof(_maxproxyid));
    pkt->pop(&_serverIpItemNum, sizeof(_serverIpItemNum));
    if (_serverIpItemNum > 0)
    {
        _servers.reserve(_serverIpItemNum);
        SrvInfo_T server;
        for (int i = 0; i < _serverIpItemNum; ++i)
        {
            pkt->pop(&server, sizeof(server));
            _servers.push_back(server);
        }
    }
}

void ProxyConnectLoginResponseMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
    pkt->push(&_proxyId, sizeof(_proxyId));
    pkt->push(&_maxproxyid, sizeof(_maxproxyid));
    _serverIpItemNum = _servers.size();
    pkt->push(&_serverIpItemNum, sizeof(_serverIpItemNum));
    for (int i = 0; i < _serverIpItemNum; ++i)
    {
        pkt->push(&_servers[i], sizeof(SrvInfo_T));
    }
}

uint32_t ProxyConnectLoginResponseMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_result)+sizeof(_proxyId) + sizeof(_maxproxyid) +
        sizeof(_serverIpItemNum)+sizeof(SrvInfo_T)*_serverIpItemNum;
}
//---------------------------------------------------------------------------
ProxyConnectLoginCompleteMsg_T::ProxyConnectLoginCompleteMsg_T()
{
    _type = MsgType::PROXY_CONNECT_LOGIN_COMPLETE;
}

void ProxyConnectLoginCompleteMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));
}

void ProxyConnectLoginCompleteMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
}

uint32_t ProxyConnectLoginCompleteMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_result);
}
//---------------------------------------------------------------------------
ProxyDisconnectLoginRequest_T::ProxyDisconnectLoginRequest_T()
{
    _type = MsgType::PROXY_DISCONNECT_LOGIN_REQUEST;
}
//---------------------------------------------------------------------------
ProxyDisconnectLoginResponse_T::ProxyDisconnectLoginResponse_T()
{
    _type = MsgType::PROXY_DISCONNECT_LOGIN_RESPONSE;
}

void ProxyDisconnectLoginResponse_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));
}

void ProxyDisconnectLoginResponse_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
}

uint32_t ProxyDisconnectLoginResponse_T::size()
{
    return BaseMessage_T::size()+sizeof(_result);
}

//---------------------------------------------------------------------------
ProxyDisconnectLoginCompleteRequest_T::ProxyDisconnectLoginCompleteRequest_T()
{
    _type = MsgType::PROXY_DISCONNECT_LOGIN_COMPLETE;
}

//---------------------------------------------------------------------------
ProxyDisconnectLoginCompleteResponse_T::ProxyDisconnectLoginCompleteResponse_T()
{
    _type = MsgType::PROXY_DISCONNECT_LOGIN_COMPLETE_RESPONSE;
}

void ProxyDisconnectLoginCompleteResponse_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));
}

void ProxyDisconnectLoginCompleteResponse_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
}

uint32_t ProxyDisconnectLoginCompleteResponse_T::size()
{
    return BaseMessage_T::size()+sizeof(_result);
}

//---------------------------------------------------------------------------
ProxyStatusQueryMsg_T::ProxyStatusQueryMsg_T()
{
    _type = MsgType::PROXY_STATUS_QUERY;
}

//---------------------------------------------------------------------------
ProxyStatusReplyMsg_T::ProxyStatusReplyMsg_T()
{
    _type = MsgType::PROXY_STATUS_REPLY;
}

void ProxyStatusReplyMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_playerNum1, sizeof(_playerNum1));
    pkt->pop(&_playerNum2, sizeof(_playerNum2));
}

void ProxyStatusReplyMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_playerNum1, sizeof(_playerNum1));
    pkt->push(&_playerNum2, sizeof(_playerNum2));
}

uint32_t ProxyStatusReplyMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_playerNum1)+sizeof(_playerNum2);
}

//---------------------------------------------------------------------------
UpdateServerIpTable_T::UpdateServerIpTable_T()
{
    _type = MsgType::UPDATE_SERVER_IP_TABLE;
    _srvs.clear();
}

void UpdateServerIpTable_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_srvNum, sizeof(_srvNum));
    _srvs.clear();
    SrvInfo_T srv;
    for (uint16_t i = 0; i < _srvNum; ++i)
    {
        pkt->pop(&srv, sizeof(srv));
        _srvs.push_back(srv);
    }
}

void UpdateServerIpTable_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    _srvNum = _srvs.size();
    pkt->push(&_srvNum, sizeof(_srvNum));
    for (uint16_t i = 0; i < _srvNum; ++i)
    {
        pkt->push(&_srvs[i], sizeof(SrvInfo_T));
    }
}

uint32_t UpdateServerIpTable_T::size()
{
    return BaseMessage_T::size()+sizeof(_srvNum)+_srvNum*sizeof(SrvInfo_T);
}





