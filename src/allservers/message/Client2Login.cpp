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

#include "message/Client2Login.h"

//
// PreJoinRequest
//
PreJoinRequestMsg_T::PreJoinRequestMsg_T()
{
    _type = MsgType::PREJOIN_REQUEST;
}

void PreJoinRequestMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    char c = 0xff;   

    while(c)
    {
        pkt->pop(&c, sizeof(char));
        if(c) _user.push_back(c);
    }
    while(c)
    {
        pkt->pop(&c, sizeof(char));
        if(c) _pass.push_back(c);
    }
}

void PreJoinRequestMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    
    const char *ptr = _user.c_str();
    pkt->push((void*)ptr, _user.size());
    ptr += _user.size();
    pkt->push((void*)ptr, sizeof(char));
    
    ptr = _pass.c_str();
    pkt->push((void*)ptr, _pass.size());
    ptr += _pass.size();
    pkt->push((void*)ptr, sizeof(char));
}

uint32_t PreJoinRequestMsg_T::size()
{
    return BaseMessage_T::size()+_user.size()+1+_pass.size()+1;
}


//
// Redirect
//
RedirectMsg_T::RedirectMsg_T()
{
    _type = MsgType::REDIRECT;
}

void RedirectMsg_T::unpack(BasePacket_T* pkt)
{
    BaseMessage_T::unpack(pkt);
    pkt->pop(&_result, sizeof(_result));
    pkt->pop(&_priProxyAddr, sizeof(_priProxyAddr));
    pkt->pop(&_priProxyPort, sizeof(_priProxyPort));
    pkt->pop(&_secProxyAddr, sizeof(_secProxyAddr));
    pkt->pop(&_secProxyPort, sizeof(_secProxyPort));
    pkt->pop(&_serverID, sizeof(_serverID));
}

void RedirectMsg_T::pack(BasePacket_T* pkt)
{
    BaseMessage_T::pack(pkt);
    pkt->push(&_result, sizeof(_result));
    pkt->push(&_priProxyAddr, sizeof(_priProxyAddr));
    pkt->push(&_priProxyPort, sizeof(_priProxyPort));
    pkt->push(&_secProxyAddr, sizeof(_secProxyAddr));
    pkt->push(&_secProxyPort, sizeof(_secProxyPort));
    pkt->push(&_serverID, sizeof(_serverID));
}

uint32_t RedirectMsg_T::size()
{
    return BaseMessage_T::size()+sizeof(_result)+
                       +sizeof(_priProxyAddr)
                       +sizeof(_priProxyPort)
                       +sizeof(_secProxyAddr)
                       +sizeof(_secProxyPort)
                       +sizeof(_serverID);
}


