#include "Define.h"
#include "User.h"
#include "GameSocket.h"

CUser::CUser(CGameSocket *pSocket)
{
    m_pSocket = pSocket;
    if(pSocket)
        pSocket->SetUser(this);
    m_nUserID = 0;
    m_nState = 0;
}

CUser::~CUser(void)
{

}