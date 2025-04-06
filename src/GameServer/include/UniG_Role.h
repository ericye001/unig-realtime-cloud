/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_Role.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: ��Ϸ��ɫ�ķ�װ

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _UNIG_ROLE_H__
#define _UNIG_ROLE_H__

/**
 * ��Ϸ��ɫ�Ļ��࣬����Player��Monster��NPC�����Լ̳���
 */
class UniG_Role : public UniG_GameObject
{
public:
    /**
     * ���캯��
     */
    UniG_Role();

    /**
     * ��������
     */
    virtual ~UniG_Role();

    /**
     * ��ʼ��������������ؽ���Ϣ
     */
	virtual bool Initialize();

    /**
     *  ���ٽ�ɫ��Ϣ
     */
    virtual void Finalize();

    /**
     * �ص��ĺ������û�ʵ����������֡����
     */
    virtual void OnUpdate();

    /**
     * ��Ӧ��Ϣ�ĺ������û�ʵ�����������Լ�����Ϣ
     * @param pMsg ���������Ϸ�߼���Ϣ
     */
    virtual void OnMessage(ObjectMessage* pMsg);
    
    UniG_SubMap* GetSubMap();
};

#endif

