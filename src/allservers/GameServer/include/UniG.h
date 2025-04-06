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

class UniG_GameObject;
class UniG_Role;
class UniG_Player;
class UniG_PlayerMgr;
class UniG_GameApp;

class UniG_Session;
class UniG_SessionMgr;
class UniG_Map;
class UniG_SubMap;

class ServerContext;
class ILMListener;
class ProxyListener;
class ServerContextMgr;
class MSTInfo;
class ObjectMessage;
class ObjectMsgQueue;
class UniG_GameServer;
class UniG_GameLogic;

#include "Vector3.h"
#include "ObjectContainer.h"

#include "TypeDefine.h"
#include "GSException.h"
#include "SeamlessMessage.h"

#include "GSMessage.h"
#include "UniG_MessageQueue.h"


#include "ServerContext.h"

#include "MSTInfo.h"
#include "SeamlessService.h"
#include "ILMListener.h"
#include "ProxyListener.h"

#include "UniG_Region.h"
#include "UniG_SubMap.h"

#include "PropertyTable.h"
#include "UniG_GameObject.h"
#include "UniG_ObjectTools.h"
#include "UniG_Role.h"
#include "UniG_Player.h"
#include "UniG_Session.h"
#include "UniG_SessionMgr.h"


#include "UniG_Map.h"
#include "UniG_GameServer.h"
#include "UniG_GameLogic.h"


#define UNIG_ASSERT(exp, describ)   \
    if(!exp){                       \
    printf("%s\nfile:%s line:%d\n",describ,__FILE__, __LINE__); \
    assert(exp); }                  \
    else{                           \
        assert(exp);                \
    }

#endif

