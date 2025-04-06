/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_GameObject.h
Author: Abone
Version: 2.0
Date: 2006-12-01
Description: the abstract of the objects in the world

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

	UniG_GameObject();

	virtual ~UniG_GameObject();

protected:

    UNIG_GUID m_nGuid;                                  ///< GUID
	uint32 m_nType;                                     ///< ����

	map<UNIG_GUID, UniG_GameObject*> m_mapChild;        ///< �Ӷ����ӳ��
    ObjectMsgQueue m_queObjectMsg;                      ///< ��Ϸ�߼���Ϣ����
    ObjectMsgQueue m_queSystemMsg;                      ///< ϵͳ��Ϣ����
    AgentInfo*    m_pAgentInfo;                         ///< Ӱ���б�
    UniG_SubMap*  m_pSubMap;                            ///< �����ӵ�ͼ
    bool m_bLocked;                                     ///< �����Ƿ�����
    bool m_bAgent;                                      ///< �Ƿ���Ӱ��

public:

    //////////////////////////////////////////////////////////////////////////
    // some get and set function
    uint32 GetType();

    void SetType(uint32 nType);

    UNIG_GUID GetGuid();

    void SetGuid(UNIG_GUID nId);

    void Lock();

    void UnLock();

    bool IsLocked();

    Vector3& GetPosition(void);

    void SetPosition(const Vector3 &position);

    UniG_SubMap* GetSubMap();
    /////////////////////////////////////////////////////////////////////////////////

public:
    /**
     * find the subobject by guid
     */
    UniG_GameObject* FindObject(UNIG_GUID nGuid);

    /**
     * add a sub-object
     */
	void AddChildObject(UniG_GameObject *pObject);

    /**
     * delete a sub-object
     */
	void DelChildObject(UniG_GameObject *pObject);


    void Update();

    void ComputeAgentList(MapRegion* pCur, vector<AgentInfo>* vecAgents);

    void EnqueMessage(ObjectMessage* pMessage);

    void ProcessMessage(ObjectMessage* pMessage);


public:

    ////////////////////////////////////////////////////////////////////////////
    // some callback function
    virtual void OnAddChildObj(UniG_GameObject *pObject);

    virtual void OnDelChildObj(UniG_GameObject *pObject);

    virtual void OnUpdate(void);

    virtual void OnMove(const Vector3& position, bool bStop);

    virtual void OnEnterSubMap(UniG_SubMap* pSubMap, MapRegion* pRegion, const Vector3& vPos);

    virtual void OnSeeByPlayer(UniG_GameObject* pSaw);

    virtual void OnLeaveSubMap(UniG_SubMap* pSubMap, MapRegion* pRegion, const Vector3& vPos);

    virtual void OnRegionChange(MapRegion* pLast, MapRegion* pCur);

    virtual void OnMessage(ObjectMessage* pMessage);

    /**
     * serialize an object to buffer
     * @param pBuffer the data buffer
     * @param nLength the length of buffer
     */
    virtual void Serialize(AgentCreateMessage* pMessage);

    /**
     * deserialize a buffer to a object
     * @param pBuffer the data buffer
     * @param nLength the buffer length
     */
    virtual void DeSerialize(AgentCreateMessage* pMessage);

    virtual bool Initialize(void);

    virtual void Finalize(void);

    bool IsAgent();

    void IsAgent(bool bAgent);

    virtual void RemoveAgent(uint16 nSrvID);

    virtual void AddAgent(uint16 nSrvID);

    void SendMsgToClient(ObjectMessage* pMessage);

    void SendMsgToNeighbour(ObjectMessage* pMessage, bool bIncludeSelf = true); 

    void SendMsgToNeighbour(ObjectMessage* pMessage, Vector3& vCenter, float fRadius); 

private:
    void SyncProperty();

    void SyncProperty(char* pBuffer, int nLength);

    void Assitant(ObjectMessage* pMessage);

};

#endif 
