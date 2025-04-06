#ifndef __UNIG_H__
#define __UNIG_H__

#include "stdarg.h"

#include "thread/Thread.h"
#include "thread/Mutex.h"
#include "thread/Condition.h"
#include "thread/Guard.h"

#include "common/Common.h"
#include "common/Exception.h"
#include "common/Utils.h"

#include "message/Message.h"

#include "socket/Epoll.h"
#include "socket/Socket.h"

class UniG_Session;
class UniG_SessionMgr;

class ILMListener;
class ProxyListener;
class ObjectMessage;
class ObjectMsgQueue;

class UniG_GameServer;

#include "TypeDefine.h"
#include "RSSException.h"
#include "SeamlessMessage.h"

#include "RSSMessage.h"
#include "UniG_MessageQueue.h"

#include "SeamlessService.h"
#include "ILMListener.h"
#include "ProxyListener.h"

#include "UniG_Session.h"
#include "UniG_SessionMgr.h"

#include "UniG_RSS.h"


#define UNIG_ASSERT(exp, describ)   \
    if(!exp){                       \
    printf("%s\nfile:%s line:%d\n",describ,__FILE__, __LINE__); \
    assert(exp); }                  \
    else{                           \
    assert(exp);                \
}

#endif

