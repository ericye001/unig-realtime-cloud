#ifndef _USER_H
#define _USER_H

class CGameSocket;

class CUser {
public:
    CUser(CGameSocket *pSocket);
    ~CUser(void);

    void SetState(int nState) { m_nState = nState; }
public:
    CGameSocket *m_pSocket;
    unsigned int m_nUserID;
    int m_nState; // 0Ϊδ��ʼ��֤ 1Ϊ�Ѿ���֤ 2Ϊ�Ѿ�����proxyID
};

#endif 