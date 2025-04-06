/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_GameObject.h
Author: Abone
Version: 2.0
Date: 2006-12-01
Description: 对游戏世界的基础对象的封装实现

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
     * 构造函数
     */
	UniG_GameObject();

    /**
     * 析构函数
     */
	virtual ~UniG_GameObject();

protected:

    UNIG_GUID m_nGuid;                                  ///< GUID
	int m_nType;                                        ///< 类型

	map<UNIG_GUID, UniG_GameObject*> m_mapChild;        ///< 子对象的映射
    ObjectMsgQueue m_queObjectMsg;                      ///< 游戏逻辑消息队列
    ObjectMsgQueue m_queSystemMsg;                      ///< 系统消息队列
    AgentInfo*    m_pAgentInfo;                         ///< 影子列表
    UniG_SubMap*  m_pSubMap;                            ///< 所在子地图
    bool m_bLocked;                                     ///< 对象是否锁定
    bool m_bAgent;                                      ///< 是否是影子
public:
    /**
     * 得到本对象类型,是一个整数值
     * @return 对象类型
     */
	int GetType();

    /**
     * 设置对象类型
     * @param nType 对象类型
     */
	void SetType(int nType);

    /**
     * 得到对象GUID
     * @return 对象GUID
     */
	UNIG_GUID GetGuid();

    /**
     * 设置对象GUID
     * @nId 对象id
     */
	void SetGuid(UNIG_GUID nId);


    void Lock();

    void UnLock();

    bool IsLocked();

    /**
     * 得到对象位置
     * @return 对象位置
     */
	Vector3& GetPosition(void);

    /**
     * 设置对象位置
     * @param position 对象位置
     */
	void SetPosition(const Vector3 &position);

public:
    /**
     * 根据ID查找本对象的子对象
     * @param nGuid 子对象ID
     * @return 子对象指针
     */
    UniG_GameObject* FindObject(UNIG_GUID nGuid);

    /**
     * 添加一个子对象
     * @param pObject 子对象
     */
	void AddChildObject(UniG_GameObject *pObject);

    /**
     * 删除一个子对象
     * @param pObject 子对象
     */
	void DelChildObject(UniG_GameObject *pObject);

    /**
     * 更新函数，帧同步函数的入口
     */
    void Update();

    /**
     * 对外提供的向自己消息队列里插入消息的入口
     * UniG_Session和处理无缝服务消息时会调用这个函数
     * @param pMessage 待处理消息
     */
    void EnqueMessage(ObjectMessage* pMessage);

    /**
     * 处理消息函数，在Update里被调用
     * @param pMessage 消息
     */
    void ProcessMessage(ObjectMessage* pMessage);

    /**
     * 判断对象是否合法
     * @return true:合法，false:非法
     */
    bool IsValid() const;

    /**
     * 派生类重载它，当添加一个子对象时会调用这个函数
     * @param pObject 子对象
     */
	virtual void OnAddChildObj(UniG_GameObject *pObject);

    /**
     * 派生类重载它，当删除一个子对象时会调用这个函数
     * @param pObject 子对象
     */
	virtual void OnDelChildObj(UniG_GameObject *pObject);
   
    /**
     * 派生类重载它，当对象更新时要调用这个函数
     */
	virtual void OnUpdate(void);

    virtual void OnMove(const Vector3& position, bool bStop);

    virtual void OnEnterSubMap(UniG_SubMap* pSubMap, MapRegion* pRegion, const Vector3& vPos);

    virtual void OnLeaveSubMap(UniG_SubMap* pSubMap, MapRegion* pRegion, const Vector3& vPos);

	virtual void OnRegionChange(MapRegion* pLast, MapRegion* pCur);

    void ComputeAgentList(MapRegion* pCur, vector<AgentInfo>* vecAgents);
    /**
     * 派生类重载它，当处理消息时要调用这个函数
     * @param pMessage 消息
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
     * 初始化一个对象
     * @return true:成功 false:失败
     */
	virtual bool Initialize(void);

    /**
     * 销毁一个对象
     * @return true:成功 false:失败
     */
	virtual void Finalize(void);

    /**
     * 本对象是否是影子
     */
    bool IsAgent();

    /**
     * 设置本对象是否是影子
     */
    void IsAgent(bool bAgent);

    /**
     * 向客户端发送处理结果消息
     */
    void SendMsgToClient(ObjectMessage* pMessage);
    
    /**
     * 向周边玩家广播处理结果消息(9 regions)
     */
    void SendMsgToNeighbour(ObjectMessage* pMessage, bool bIncludeSelf = true); 

    /**
     * 向周边玩家广播处理结果消息(objects in a circle)
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
