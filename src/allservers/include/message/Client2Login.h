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

#ifndef MESSAGE_CLIENT_LOGIN_HPP
#define MESSAGE_CLIENT_LOGIN_HPP

#include "Message.h"

class PreJoinRequestMsg_T : public BaseMessage_T
{
public:
    PreJoinRequestMsg_T();
    virtual ~PreJoinRequestMsg_T(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    string _user;
    string _pass;
};

class RedirectMsg_T : public BaseMessage_T
{
public:
    RedirectMsg_T();
    virtual ~RedirectMsg_T(){};
    
    virtual void unpack(BasePacket_T* pkt);
    virtual void pack(BasePacket_T* pkt);
    virtual uint32_t size();
    
    uint16_t _result;             // 1 - user/pass correct; 0 - otherwise
    uint32_t _priProxyAddr;
    uint16_t _priProxyPort;
    uint32_t _secProxyAddr;
    uint16_t _secProxyPort;
    uint16_t _serverID;
};


#endif
