/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: GSMessage.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 消息封装，来自proxy的消息和游戏逻辑消息

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _GS_MESSAGE_H__
#define _GS_MESSAGE_H__

/**
 * 来自代理消息的封装
 */
class ProxyMessage : public BaseMessage
{
public:
    uint16 m_nServerID;
    uint16 m_nProxyID;
    uint16 m_nClientID;
    uint16 m_nLength;
    char*  m_pData;
    
    ProxyMessage();
    virtual ~ProxyMessage();
    virtual void Pack(BasePacket* pPacket);
    virtual void Unpack(BasePacket* pPacket);
    virtual uint32 Size();
};


class ObjectMessage : public BaseMessage
{
public:
    ObjectMessage();
    virtual ~ObjectMessage();

    virtual void Pack(BasePacket* pPacket);
    virtual void Unpack(BasePacket* pPacket);
    virtual uint32 Size();

public:
    uint16 m_nMsgLength;
    char*  m_pMsgData;
};


class SystemMessage : public BaseMessage
{
public:
    SystemMessage();
    virtual ~SystemMessage();

    virtual void Pack(BasePacket* pPacket);
    virtual void Unpack(BasePacket* pPacket);
    virtual uint32 Size();

public:
    uint32 m_nWparam;
    uint32 m_nLparam;
};



/**
 * 实体和和影子之间发的消息
 */
class SynAgentMessage : public ObjectMessage
{
public:
    SynAgentMessage();
    virtual ~SynAgentMessage();

    virtual void Pack(BasePacket* pPacket);
    virtual void Unpack(BasePacket* pPacket);
    virtual uint32 Size();

public:
    int  m_nRegionID;
    int  m_nObjType;
    UNIG_GUID m_nDestObject;
    uint16 m_nInOut;
    uint16 m_nObjMsgType;
};


/**
 * 游戏逻辑消息队列
 */
class ObjectMsgQueue
{
public:
    ObjectMsgQueue();
    ~ObjectMsgQueue();

    uint16 MaxAsynMsg();
    void MaxAsynMsg(uint16 maxMsgNum);

    uint16 BatchNum();
    void BatchNum(uint16 batchNum);

    uint32 Size();

    void Push(ObjectMessage* pMsg);
    ObjectMessage* Front();
    void Pop();

    void FrontBatch(vector<ObjectMessage*>& vc, bool all = false);
    void PopBatch(uint16 count);

    void RequestIncoming();
    void NotifyIncoming();

private:
    ObjectMsgQueue(const ObjectMsgQueue& rhs);
    ObjectMsgQueue& operator=(const ObjectMsgQueue& rhs);

    list<ObjectMessage*> m_lstQueue;
    uint16 m_nMaxAsyncMsg;
    uint16 m_nBatchNum;
    Mutex m_oMutex;
    Condition m_oCondition;
};

#endif

