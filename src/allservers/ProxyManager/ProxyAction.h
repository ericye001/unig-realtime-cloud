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

#ifndef PM_STATEACTION_HPP
#define PM_STATEACTION_HPP
#include "common/EpollUtils.h"
#include "common/EpollUtils.h"
#include "common/SocketStream.h"
#include "message/PM2PROXY.h"
#include "message/Proxy2PM.h"
#include "message/Client2Login.h"
#include "message/PM2ILM.h"
#include "message/Server2ILM.h"
#include "Map2SrvInfo.h"
#include "ProxyManager.h"
//
// StateAction
//
class ProxyAction : public Action_T
{
public:
    virtual void execute(Connection_T* pConn);
private:
    void proxyJoin(Connection_T* pConn, BasePacket_T& pkg);
    void proxyJoinComplete(Connection_T* pConn, BasePacket_T& pkg);
    void proxyLeave(Connection_T* pConn);
    void proxyLeaveComplete(Connection_T* pConn);
};

//
// StateAcceptAction
//
class ProxyAcceptAction : public Action_T
{
public:
    virtual void execute(Connection_T* pConn);

};

#endif

