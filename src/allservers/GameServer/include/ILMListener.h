/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: ILMListener.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: �����ILMServe����ͨѶ����ӦILMServer������

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _ILM_LISTENER_H__
#define _ILM_LISTENER_H__

class ILMListener : public Runnable
{
public:
    ILMListener(ILMArgs& args);
    ~ILMListener();
    void run();
    void Stop();
private:
    Epoll* m_pEpoll;
    bool m_bStopFlag;
};

/**
 * ��������ILM��Ϣ����
 */
class ILMObject : public SocketHandler
{
public:
    /**
     * ���캯������ʼ����ILM�����ӹ���
     */
    ILMObject(Socket* pSocket);

    /**
     * ��������
     */
    
    virtual ~ILMObject();
    
    /**
     * ��ILM�������ݷ��͹���
     */
    virtual int OnRead ();

    /**
     * ����ILMͨѶ��������
     */
    virtual int OnError();

    /**
     * ��ʼ����ILM������
     * @param args ����
     * @see ILMArgs
     */
    void Init(ILMArgs& args);   

private:
    
    ILMObject(const ILMObject& rhs);
    ILMObject& operator=(const ILMObject& rhs);
        
    /**
     * handler the packet from ILM
     */

    int Handle(BasePacket& pkt);

    /**
     * �������MST��Ϣ
     * @param pMsg MST��Ϣ
     */
    int Handle(UpdateMSTRequestMsg* pMsg);

    /**
     * �����ѯServer״̬��Ϣ
     * @param pMsg ��ѯ��Ϣ
     */
    int Handle(ServerStatusQueryMsg* pMsg);

    /**
     * ������������Ϣ(�������պͷ�������)
     * @param pMsg �����Ϣ
     */
    int Handle(StartPartitionRequestMsg* pMsg);

    /**
     * ����server�����뿪��Ϣ
     * @param pMsg �����뿪��Ϣ
     */
    int Handle(ServerLeaveRequest* pMsg);

    /**
     * ����������ͼ��Ϣ
     * @param pMsg ������ͼ��Ϣ
     */
    int Handle(ServerLockMapRequest* pMsg);

    /**
     * �����ͷ�������ͼ��Ϣ
     * @param pMsg �ͷŶԵ�ͼ������
     */
    int Handle(ServerUnlockMapRequest* pMsg);
    
    /** 
     * ������ҷ��͵�ͼ��Ϣ
     */
    int TransferMigrators(StartPartitionRequestMsg* pMsg);

    /**
     * ���հ����Ϣ
     */
    int AcceptMigrators(StartPartitionRequestMsg* pMsg);
    
    /** 
     * ȡ�����(������ʵ�ְ�)
     */
    int UndoMigration(StartPartitionRequestMsg* pMsg);
    
    /**
     * ��ILM����Ϣ
     */
    void MsgForward(BasePacket& pkt);
};



#endif

