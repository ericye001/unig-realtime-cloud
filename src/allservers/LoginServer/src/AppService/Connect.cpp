#include "Define.h"
#include "Connect.h"

CConnect::CConnect(IMessagePort &port, int nClientId /* = 0 */)
: m_portMsg(port), m_nClientId(nClientId), m_nPortExit(0)
{

}

CConnect::~CConnect()
{

}

void CConnect::SendMsg(const void *buf, int nLen, int nPort)
{
	if(!buf || nLen <= 0)
		return;

	if(!m_portMsg.IsValid())
		return;

    IMessage msg(_SYSMSG_CLIENT, nLen, buf, m_nClientId, m_nClientId);
    m_portMsg.SendMsg(msg, nPort);

	// IMessage msg(_MSG_CLIENT, nLen, buf, m_socket, m_socket);

	// m_portMsg.SendMsg(msg, m_portMsg.GetPort(), _PORTGROUP_ALL);
}

void CConnect::ClearSocket(void)
{
	m_nClientId = 0;
}

void CConnect::RecoverSocket(int nClientId)
{
	m_nClientId = nClientId;
}

void CConnect::Close(void)
{
//	if(m_socket != 0) {
//		IMessage msg(_MSG_CONNECT_SHUTDOWN, 0, NULL, m_socket, m_socket);
//		m_portMsg.SendMsg(msg, _PORT_SOCKETSERVICE, _PORTGROUP_ALL);
//	}
}