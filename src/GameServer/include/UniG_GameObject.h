/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_GameObject.h
Author: Abone
Version: 2.0
Date: 2006-12-01
Description: ����Ϸ����Ļ�������ķ�װʵ��

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _UNIG_GAMEOBJECT_H__
#define _UNIG_GAMEOBJECT_H__

class AgentInfo
{
public:
    void SetEntity(UniG_GameObject* pEntity);

    set<uint16> GetAgentList();
    
    void UpdateAgent(set<uint16> newServers);
    
private:
    set<uint16> m_setAgentList;
    UniG_GameObject* m_pEntity;
};

class UniG_GameObject : public CPropertyTable
{
public:
    DECLARE_PROTERTY_TABLE(UniG_GameObject, 1)
public:
    /**
     * ���캯��
     */
	UniG_GameObject();

    /**
     * ��������
     */
	virtual ~UniG_GameObject();

protected:

    UNIG_GUID m_nGuid;                                  ///< GUID
	int m_nType;                                        ///< ����

	map<UNIG_GUID, UniG_GameObject*> m_mapChild;        ///< �Ӷ����ӳ��
    ObjectMsgQueue m_queObjectMsg;                      ///< ��Ϸ�߼���Ϣ����
    ObjectMsgQueue m_queSystemMsg;                      ///< ϵͳ��Ϣ����
    AgentInfo*    m_pAgentInfo;                         ///< Ӱ���б�
    UniG_SubMap*  m_pSubMap;                            ///< �����ӵ�ͼ
    bool m_bLocked;                                     ///< �����Ƿ�����
    bool m_bAgent;                                      ///< �Ƿ���Ӱ��
public:
    /**
     * �õ�����������,��һ������ֵ
     * @return ��������
     */
	int GetType();

    /**
     * ���ö�������
     * @param nType ��������
     */
	void SetType(int nType);

    /**
     * �õ�����GUID
     * @return ����GUID
     */
	UNIG_GUID GetGuid();

    /**
     * ���ö���GUID
     * @nId ����id
     */
	void SetGuid(UNIG_GUID nId);


    void Lock();

    void UnLock();

    bool IsLocked();

    /**
     * �õ�����λ��
     * @return ����λ��
     */
	Vector3& GetPosition(void);

    /**
     * ���ö���λ��
     * @param position ����λ��
     */
	void SetPosition(const Vector3 &position);

public:
    /**
     * ����ID���ұ�������Ӷ���
     * @param nGuid �Ӷ���ID
     * @return �Ӷ���ָ��
     */
    UniG_GameObject* FindObject(UNIG_GUID nGuid);

    /**
     * ���һ���Ӷ���
     * @param pObject �Ӷ���
     */
	void AddChildObject(UniG_GameObject *pObject);

    /**
     * ɾ��һ���Ӷ���
     * @param pObject �Ӷ���
     */
	void DelChildObject(UniG_GameObject *pObject);

    /**
     * ���º�����֡ͬ�����������
     */
    void Update();

    /**
     * �����ṩ�����Լ���Ϣ�����������Ϣ�����
     * UniG_Session�ʹ����޷������Ϣʱ������������
     * @param pMessage ��������Ϣ
     */
    void EnqueMessage(ObjectMessage* pMessage);

    /**
     * ������Ϣ��������Update�ﱻ����
     * @param pMessage ��Ϣ
     */
    void ProcessMessage(ObjectMessage* pMessage);

    /**
     * �ж϶����Ƿ�Ϸ�
     * @return true:�Ϸ���false:�Ƿ�
     */
    bool IsValid() const;

    /**
     * �������������������һ���Ӷ���ʱ������������
     * @param pObject �Ӷ���
     */
	virtual void OnAddChildObj(UniG_GameObject *pObject);

    /**
     * ����������������ɾ��һ���Ӷ���ʱ������������
     * @param pObject �Ӷ���
     */
	virtual void OnDelChildObj(UniG_GameObject *pObject);
   
    /**
     * �����������������������ʱҪ�����������
     */
	virtual void OnUpdate(void);

    virtual void OnMove(const Vector3& position, bool bStop);

    virtual void OnEnterSubMap(UniG_SubMap* pSubMap, MapRegion* pRegion, const Vector3& vPos);

    virtual void OnLeaveSubMap(UniG_SubMap* pSubMap, MapRegion* pRegion, const Vector3& vPos);

	virtual void OnRegionChange(MapRegion* pLast, MapRegion* pCur);

    void ComputeAgentList(MapRegion* pCur, vector<AgentInfo>* vecAgents);
    /**
     * ����������������������ϢʱҪ�����������
     * @param pMessage ��Ϣ
     */
    virtual void OnMessage(ObjectMessage* pMessage);

    /**
     * serialize an object to buffer
     * @param pBuffer the data buffer
     * @param nLength the length of buffer
     */
    void Serialize(AgentCreateMessage* pMessage);

    /**
     * deserialize a buffer to a object
     * @param pBuffer the data buffer
     * @param nLength the buffer length
     */
	void DeSerialize(AgentCreateMessage* pMessage);


    /**
     * ��ʼ��һ������
     * @return true:�ɹ� false:ʧ��
     */
	virtual bool Initialize(void);

    /**
     * ����һ������
     * @return true:�ɹ� false:ʧ��
     */
	virtual void Finalize(void);

    /**
     * �������Ƿ���Ӱ��
     */
    bool IsAgent();

    /**
     * ���ñ������Ƿ���Ӱ��
     */
    void IsAgent(bool bAgent);

    /**
     * ��ͻ��˷��ʹ�������Ϣ
     */
    void SendMsgToClient(ObjectMessage* pMessage);
    
    /**
     * ���ܱ���ҹ㲥��������Ϣ(9 regions)
     */
    void SendMsgToNeighbour(ObjectMessage* pMessage, bool bIncludeSelf = true); 

    /**
     * ���ܱ���ҹ㲥��������Ϣ(objects in a circle)
     */
    void SendMsgToNeighbour(ObjectMessage* pMessage, Vector3& vCenter, float fRadius); 

    virtual void RemoveAgent(uint16 nSrvID);

    virtual void AddAgent(uint16 nSrvID);

    void SyncProperty();

    void SyncProperty(char* pBuffer, int nLength);

    
private:
    void Assitant(ObjectMessage* pMessage);

};

#endif 
