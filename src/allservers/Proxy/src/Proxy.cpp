#include "socket/Epoll.h"
#include "ProxyObject.h"
#include "ProxyObjectFactory.h"
#include "common/Utils.h"

struct prxoyArg g_arg;
IniParser * m_pConfig = NULL;

void GetPropertyValue (string strSection, string strPropertyName,string & strValue);

bool InitConfigFile () 
{
	string tmpport;
	m_pConfig = new IniParser ("../config/Proxy.ini");
	m_pConfig->load ();
	GetPropertyValue ("server", "proxyManagerIP", g_arg.proxyManagerIP);
	GetPropertyValue ("server", "proxyManagerPort", tmpport);
	g_arg.proxyManagerPort = atoi (tmpport.c_str ());
	GetPropertyValue ("server", "localIP", g_arg.localIP);
	GetPropertyValue ("server", "localPort", tmpport);
	g_arg.localPort = atoi (tmpport.c_str ());
	delete m_pConfig;
	return true;
}

bool InitListen (Epoll * pEpoll) 
{
	ServerSocket * pListenSocket = new ServerSocket (g_arg.localIP.c_str (), g_arg.localPort);	// listen fd
	ClientObjectFactory *pClientFactory = ClientObjectFactory::Instance ();	//生成对象
	if (!pListenSocket || !pClientFactory)

	{
		printf ("\n pListenSocket  pClientFactory  error !\n");
		return false;
	}
	ServerSocketHandler * pAcceptor = new ServerSocketHandler (pListenSocket, pClientFactory);	//acceptor fd
	if (!pAcceptor)

	{
		printf ("\n ServerSocketHandler   error !\n");
		return false;
	}
	pEpoll->Add (pAcceptor);
	return true;
}

void GetPropertyValue (string strSection, string strPropertyName,string & strValue) 
{
	vector < Section * >sectionList;
	m_pConfig->getSection (strSection.c_str (), sectionList);
	m_pConfig->getKeyVal (**sectionList.begin (), strPropertyName.c_str (),
			strValue);
}


int main (int argc, char *argv[]) 
{

#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD (2, 2);
	WSAStartup (wVersionRequested, &wsaData);

#endif	/*  */
      
	 Epoll *pEpoll = new Epoll();    //有默认参数。
	 if(!pEpoll)
	 {
	   printf("error in create epoll\n");
	   exit(-1);
	 }
	 
	Proxy::Init ();
	InitConfigFile ();
	ProxyManagerObject::Init(pEpoll);
	InitListen (pEpoll);
	while (true)
	{
	        if (ProxyManagerObject::Instance())
		    {
		      pEpoll->Wait (500);
		    }
		else {
		      printf("\n begain  connect PM !\n");
		      fflush(stdout);
		      ProxyManagerObject::Init(pEpoll);
		     }
	}
	return 0;
}


