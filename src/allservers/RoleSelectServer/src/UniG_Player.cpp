#include "UniG.h"

UniG_Player::UniG_Player(void)
{
}

UniG_Player::~UniG_Player(void)
{
    Finalize();
}

void UniG_Player::SetSession(UniG_Session* session) 
{
    m_pSession = session;
}

UniG_Session* UniG_Player::GetSession()
{
    return m_pSession;
}

void UniG_Player::Logon()
{

}

bool UniG_Player::Initialize()
{
    return true;
}


void UniG_Player::Finalize(void)
{

}

void UniG_Player::OnUpdate()
{

}

void UniG_Player::OnMessage(ObjectMessage* pMsg)
{
    switch(pMsg->
}


void UniG_Player::SendMsgToClient(ObjectMessage* pMessage)
{
    if(m_pSession)
    {
        m_pSession->EnqueMsg(pMessage);
    }
}



