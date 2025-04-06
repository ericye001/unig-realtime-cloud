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

#ifndef PM_ILM_ACTION_HPP
#define PM_ILM_ACTION_HPP



#include "message/Server2ILM.h"
#include "message/PM2ILM.h"
#include "message/Console2ILM.h"
#include "ProxyManager.h"
class ILMAction : public Action_T
{
public:
    virtual void execute(Connection_T* pConn);
};

#endif
