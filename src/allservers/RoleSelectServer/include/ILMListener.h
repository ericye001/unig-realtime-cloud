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
    * ��Һ��ILM������Ϣ�ĺ���(��ʱδ�õ�)
    */
    void MigrationResponse(StartPartitionResponseMsg& msg);
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
    * �����ѯServer״̬��Ϣ
    * @param pMsg ��ѯ��Ϣ
    */
    int Handle(ServerStatusQueryMsg* pMsg);

    /**
    * ��ILM����Ϣ
    */
    void MsgForward(BaseMessage& msg);
};

#endif
