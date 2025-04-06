#include "ProxyObjectFactory.h"

ClientObjectFactory* ClientObjectFactory::mClientObjFactory = 0;

//---------------------------------------------------------------------------
//  ClientObjectFactory::Create
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
SocketHandler* ClientObjectFactory::Create(Socket* pSocket)
{
	uint32 id = GenerateID();
	pSocket->ConfigureBlocking(false);
	ClientObject* pClient = new ClientObject((Socket*)pSocket, id);
//	mMapClientObj.insert(make_pair(id, pClient));
        Proxy::Instance()->PutClientObj(id,pClient);
	return pClient;  
}


//---------------------------------------------------------------------------
//  ClientObjectFactory::Instance
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
ClientObjectFactory* ClientObjectFactory::Instance()
{
	if(!mClientObjFactory)
	{
		mClientObjFactory = new ClientObjectFactory();
		mClientObjFactory->mCurID = 0;
	}
	return mClientObjFactory;
}


//---------------------------------------------------------------------------
//  ClientObjectFactory::GenerateID
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
uint32 ClientObjectFactory::GenerateID()
{
	mCurID++;
	return mCurID;
}


//---------------------------------------------------------------------------
//  ClientObjectFactory::Destroy
//---------------------------------------------------------------------------
//  Description:  
//  Parameters:   
//  Return:       
//  Exception:    
//  MT Safe:      
//  Note:         
//---------------------------------------------------------------------------
void ClientObjectFactory::Destroy()
{
	if(mClientObjFactory)
	{
		delete mClientObjFactory;
		mClientObjFactory = 0;
	}
}

