/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: MSTInfo.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: MST表信息

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _MST_INFO_H__
#define _MST_INFO_H__

/** 
 * MST表信息，包含所有地图信息，Server信息以及地图和Server映射信息
 */
class MSTInfo
{
public:
    /** 
     * 实例化创建一个对象
     */
    static MSTInfo& Instance();
    
    /**
     * 取得本Serve上MST信息
     * @return MST信息
     */
    vector<Map2SrvInfo>& AllMST();

    /**
     * 设置MST信息
     * @param mst MST信息，从ILMServer那里读取到
     */
    void AllMST(vector<Map2SrvInfo>& mst);
    
    /**
     * 取得所有GameServer的信息
     */
    vector<ServerInfo>& AllSrvs();

    /**
     * 把从ILM Server那里获取到的server列表信息保存在成员变量里
     * @param allSrvs server信息集合
     */
    void AllSrvs(vector<ServerInfo>& allSrvs);

    /**
     * 得到所有的map信息
     */
    vector<MapInfo>& AllMaps();

    /**
     * 把从ILM Server那里获取到的map列表信息保存在成员变量里
     * @param allMaps 地图信息列表
     */
    void AllMaps(vector<MapInfo>& allMaps);
    
    /**
     * 根据名字取得mapID
     * @param pName 地图名字
     * @return 地图id
     */
    uint16 MapID(const char* pName);

    /**
     * 取得一个map对应的server
     * @param nMapID 地图id
     * @return server id
     */
    uint16 MapOwner(uint16 nMapId);
    
    /**
     * 根据map id 得到MST映射信息
     * @param nMapID 地图id
     * @return 一组映射信息
     */
    Map2SrvInfo& GetMSTPairByMap(uint16 nMapId);

    /**
     * 根据ServerID得到server 信息
     * @nSrvID server id
     * @return server信息
     */
    ServerInfo& GetServerInfo(uint16 nSrvId);

    /**
     * 根据Map ID得到map信息
     * @nMapID map id
     * @return map信息
     */
    MapInfo& GetMapInfo(uint16 nMapId);
    
private:
    MSTInfo();
    
private:
    static MSTInfo* m_pInstance;
    
    // global MST
    vector<Map2SrvInfo> m_vecAllMST;

    // all servers' infomation
    vector<ServerInfo> m_vecAllSrvs;

    // all maps's infomation
    vector<MapInfo> m_vecAllMaps;
};


#endif

