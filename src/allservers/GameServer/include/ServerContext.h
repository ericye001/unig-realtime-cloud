/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: ServerContext.h
Author: Abone
Version: 2.0
Date: 2006-12-18
Description: implemention with other servers

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 
#ifndef _SERVER_CONTEXT_H__
#define _SERVER_CONTEXT_H__

class ServerContextMgr;
class ServerContext;
class MsgThread;
class BlockThread;
class ServerContextAcceptor;
class MsgConnHandler;
class BlockConnHandler;


struct ServerContextArgs
{
    uint16 nMaxAsyncMsg;
    uint16 nBatchNum;
    uint16 nMaxSrvNum;
    uint32 nMaxMsgLength; 
    ServerInfo serverInfo;
};

struct BlockData
{
    char* pData;      // data pointer
    uint32 nLength;   // total length
    uint32 nSize;     // current size of data, <= _length
};

class ServerContextMgr
{
friend class ServerContext;
friend class MsgThread;
friend class BlockThread;

public:
    /**
     * initialize the msgbus between serves
     * establish the listen socket, wait for other game server to connect
     */
    static void Init(ServerContextArgs& args);

    /**
     * destroy the connection
     */
    static void Deinit();

    /**
     * get the ServerContextMgr instance
     */
    static ServerContextMgr* Instance();    
    
    /**
     * get the server context by server info
     * if the server context has been created ,return it
     * else create a server context.
     * the ServerContext is useed to be communicated.
     */
    ServerContext* GetSrvCtx(ServerInfo& item);

    /**
     * get the server context by server id
     */
    ServerContext* FindSrvCtx(uint16 srvId);

    /**
     * create server context by the given server info
     */
    ServerContext* CreateSrvCtx(ServerInfo& item);    

    /**
     * remove a server context
     */
    void RemoveSrvCtx(ServerInfo& item);
    
    /**
     * set up a connection withe the peer server
     */
    void SetupConnection(ServerInfo& server);    

    /**
     * destroy a connection withe the peer server
     */
    void DestroyConnection(ServerInfo& server);
    
    /**
     * send a buffer to the given server(big buffer)
     */
    int SendBlock(uint16 nSrvId, void* pData,  uint32 nLength);

    /**
     * receive a block of data
     */
    void RecvBlock(uint16 nSrvId, void*& pData, uint32& nLength);

    /**
     * send a buffer a the given server(small buffer)
     */
    int SendMsg(uint16 nSrvId, void* pData,  uint32 nLength);

    ServerContextArgs& Args();
    

    /**
     * get the msg conn epoll
     */
    Epoll* GetMsgEpoll();

    /**
     * get the data block conn epoll
     */
    Epoll* GetBlockEpoll();
    
private:
    ServerContextMgr(ServerContextArgs& args);
    ServerContextMgr(const ServerContextMgr& rhs);
    ServerContextMgr& operator=(const ServerContextMgr& rhs);
    ~ServerContextMgr();
    
    static ServerContextMgr* m_pInstance;
    ServerContextArgs m_oArgs;
    MsgThread* m_pMsgThread;
    BlockThread* m_pBlockThread;
    map<int, ServerContext*> m_mapServers;

    Epoll* m_pMsgEpoll;
    Epoll* m_pBlockEpoll;
    Mutex m_pMsgEpollMutex;
    Mutex m_pBlockEpollMutex;
    static Mutex m_oClassMutex;
    Mutex m_oObjectMutex;
    static Condition m_oClassCondition;
    static bool m_bHasInit;
};

/**
 * 在ServerContextMgr上，一个Server和其他n个server进行连接
 * 每一组连接我们都抽象为ServerContext类
 */
class ServerContext
{
friend class BlockConnHandler;
friend class MsgConnHandler;
public:
    ServerContext(ServerInfo& item);

    ~ServerContext();

    /**
     * establish the connection between this server and peer server
     */
    void SetupConnection();

    /**
     * destroy the connection between this server and peer server
     */
    void DestroyConnection();
    

    void SendBlock(void* pData, uint32 nLength);

    void SendMsg(void* pData, uint32 nLength);

    void RecvBlock(void*& pData, uint32& length);

   
    MsgConnHandler* GetMsgConn();

    void SetMsgConn(MsgConnHandler* pConn);

    void ResetMsgConn();

    BlockConnHandler* GetBlockConn();

    void SetBlockConn(BlockConnHandler* pConn);

    void ResetBlockConn();

private:
    ServerContext(const ServerContext& rhs);
    ServerContext& operator=(const ServerContext& rhs);

    void InitMsgConnection();

    void InitBlockConnection();

private:
    /// server info
    ServerInfo m_oServerInfo;

    MsgConnHandler* m_pMsgConn;
    BlockConnHandler* m_pBlockConn;

    MsgQueue m_oSendQueue;

    Mutex m_oBlockDataMutex;
    Mutex m_oWriteMutex;
    Mutex m_oReadMutex;
    Mutex m_oMutex;
    Condition m_oBlockDataCondition;

    list<BlockData> m_lstBlockDataList;
};

class MsgThread : public Runnable
{
public:
    MsgThread();
    virtual ~MsgThread();
    
    virtual void run();
    void Stop();
private:
    bool m_bStopFlag;
    Epoll* m_pEpoll;
};

class BlockThread : public Runnable
{
public:
    BlockThread();
    virtual ~BlockThread();
    
    virtual void run();
    void Stop();
private:
    bool m_bStopFlag;
    Epoll* m_pEpoll;
};

class MsgDataListener : public ServerSocketHandler
{
public:
    MsgDataListener(ServerSocket* pSocket);
    /**
     * 当一个连接连上来的时候，会调用这个函数
     * @param pSocket 接收的连接
     */
    virtual int OnRead(Socket* pSocket);
private:
    /**
     * 接收一个用于消息通讯的Socket
     */
    void AcceptMsgConn(Socket* pSocket, BasePacket& pkt);

    /**
     * 接收一个用于数据通讯的Socket
     */
    void AcceptBlockConn(Socket* pSocket, BasePacket& pkt);
};


class BlockDataListener : public ServerSocketHandler
{
public:
    BlockDataListener(ServerSocket* pSocket);
    /**
     * 当一个连接连上来的时候，会调用这个函数
     * @param pSocket 接收的连接
     */
    virtual int OnRead(Socket* pSocket);
private:
    /**
     * 接收一个用于消息通讯的Socket
     */
    void AcceptMsgConn(Socket* pSocket, BasePacket& pkt);

    /**
     * 接收一个用于数据通讯的Socket
     */
    void AcceptBlockConn(Socket* pSocket, BasePacket& pkt);
};

/**
 * 消息通讯Socket处理器
 */
class MsgConnHandler : public SocketHandler
{
public:
    /**
     * 构造函数
     */
    MsgConnHandler(Socket* pSocket) : SocketHandler(pSocket){}

    /**
     * 有消息数据
     */
    virtual int OnRead();

    /**
     * 本socket有错误发生
     */
    virtual int OnError();

    /**
     * 处理从相邻Server来的消息,主要要对pPkt的内存处理
     */
    int HandleMessage(BasePacket& pPkt);

    bool HighPriorityThan(MsgConnHandler& conn);
    
    uint16 Source();
    void Source(uint16 source, bool peer = false);
    
    uint16 Target();
    void Target(uint16 target, bool peer = false);
    
    uint16 Peer();

private:
    uint16 m_nSource;
    uint16 m_nTarget;
    uint16 m_nPeer;     // Point to m_nSource or m_nTarget
};

class BlockConnHandler : public SocketHandler
{
public:

    /**
     * 构造函数
     */
    BlockConnHandler(Socket* pSocket) : SocketHandler(pSocket){m_bHasInit = false;}

    /**
     * 有消息数据,读到ServerContext的收消息队列里
     */
    virtual int OnRead();

    /**
     * 本socket有错误发生
     */
    virtual int OnError();

    /**
     * init the block socket
     */

    int InitBlockConn();

    bool HighPriorityThan(BlockConnHandler& conn);

    uint16 Source();
    void Source(uint16 source, bool peer = false);
    
    uint16 Target();
    void Target(uint16 target, bool peer = false);
    
    uint16 Peer();

private:

    uint16 m_nSource;
    uint16 m_nTarget;
    uint16 m_nPeer;     // Point to m_nSource or m_Target
    bool m_bHasInit;    // whether has been inited
};



#endif
