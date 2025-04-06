#include "Define.h"
#include "AppService.h"
#include "Connect.h"

#pragma comment(lib, "..\\lib\\ServerShell.lib")

BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
                      )
{
    return TRUE;
}

IAppService* AppServiceCreate(IMessagePort& pMsgPort, DWORD dwMsgPrcInterval, DWORD dwCntPrcInterval)
{
    CAppService *pAppService = new CAppService(pMsgPort);

    if(!pAppService) 
        return NULL;

    if(!pAppService->Create(dwMsgPrcInterval, dwCntPrcInterval)) {
        delete pAppService;
        return NULL;
    }

    return pAppService;
}

CAppService::CAppService(IMessagePort &portMsg)
: m_portMsg(portMsg)
{
	m_pThread = NULL;
}

CAppService::~CAppService()
{
	if(m_pThread) {
		delete m_pThread;
		m_pThread = NULL;
	}
}

void CAppService::Release(void)
{
	delete this;
}

bool CAppService::Create(DWORD dwMsgPrcInterval, DWORD dwCntPrcInterval)
{
	 m_pThread = MyThread::CreateNew(*this, false, dwCntPrcInterval);
	 if(!m_pThread)
	 	return false;

	m_timerConnectPrc.Startup(dwMsgPrcInterval);

    CLoginService::GetInstance()->Init(&m_portMsg);

    printf("LoginService起动成功\n");
	return true;
}

void CAppService::ProcessMsg(void)
{
	if(!m_portMsg.IsValid())
		return;

	IMessage *pPortMsg = NULL;

	while ((pPortMsg = m_portMsg.TakeMsg()) != NULL) {
        switch(pPortMsg->nSysMsg) {
        case _SYSMSG_CONNECT_LOGIN:
            {
                CConnect *pConnect = new CConnect(m_portMsg, pPortMsg->GetFrom());

                if(pConnect) {
                    m_setConnect[pPortMsg->GetFrom()] = pConnect;
                }
            }
            break;
        case _SYSMSG_CONNECT_LOGOUT:
            {
                CONNECT_SET::iterator iter = m_setConnect.find(pPortMsg->nFrom);
                if(iter != m_setConnect.end()) {
                    CConnect *pConnect = (*iter).second;
                    pConnect->ClearSocket();
                    pConnect->OnClose(0);

                    delete pConnect;

                    m_setConnect.erase(iter);
                }
            }
            break;
        case _SYSMSG_CLIENT:
            {
                CONNECT_SET::iterator iter = m_setConnect.find(pPortMsg->nTo);
                if(iter != m_setConnect.end()) {
                    CConnect *pConnect = (*iter).second;
                    if(pConnect) 
                        pConnect->OnRecv(pPortMsg->bufMsg, pPortMsg->nMsgSize);
                }
            }
            break;
        case _SYSMSG_SERVICE:
            {
                CLoginService::GetInstance()->OnMessage((void*)pPortMsg->GetBuf(), pPortMsg->GetSize());
            }
            break;
        default:
            break;
        }

        try
        {
            pPortMsg->Release();
        }
        catch (...)
        {
        	printf("pPortMsg->Release() exception.\n");
        }

		pPortMsg = NULL;
	}
}

int CAppService::OnThreadCreate(void)
{
	return 0;
}

int CAppService::OnThreadWorkEvent(void)
{
	return 0;
}

int CAppService::OnThreadProcess(void)
{
	ProcessMsg();

	if(m_timerConnectPrc.ToNextTime()) {
        CLoginService::GetInstance()->Loop();
	}

	return 0;
}

int CAppService::OnThreadDestroy(void)
{
	return 0;
}
