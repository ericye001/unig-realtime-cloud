#ifndef _PROXY_OBJECT_FACTORY_H__
#define _PROXY_OBJECT_FACTORY_H__
#include "socket/Epoll.h"
#include "ProxyObject.h"
class ClientObjectFactory : public SocketHandlerFactory
{
	private:
		static ClientObjectFactory* mClientObjFactory;
		uint32 mCurID;
		//map<uint16, ClientObject*> mMapClientObj;
	public:
		virtual SocketHandler* Create(Socket* pSocket);
		//ClientObject* GetClientObjByID(uint16 id);
		//void  DeletClientObjByID(uint16 id);
		static ClientObjectFactory* Instance();
		static void Destroy();
		uint32 GenerateID();
};
#endif

