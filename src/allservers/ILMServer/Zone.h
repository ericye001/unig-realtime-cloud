#ifndef _UNIG_ILM_ZONE_H__
#define _UNIG_ILM_ZONE_H__
#include "GameServerListener.h"
#include "ServerManager.h"
#include "Config.h"

// 一个zone和其临接Server的关系
struct NeighbourState
{
	uint16_t neighbourServerID;               // 相邻serverID
	uint32_t neighbourServerLoad;             // 相邻server负载
	uint8_t  junctureLevel;                   // 连接程度,就是这个zone和相邻地图有几条边相邻
	uint16_t zoneID;                          // 本zone ID
	uint32_t zoneLoad;                        // 本zone 负载
	uint16_t serverID;                        // 本zone所属 server
};

// zone类
class UniG_ILM_Zone
{
private:
	list<uint32_t> m_lstMapUnit;              // 本zone包含的地图单元
	uint16_t m_nZoneID;                       // 本zoneID
	uint32_t m_nMapID;                        // 本zone所属的map
	uint16_t m_nServerID;                     // 本zone所属的server,一个zone只能属于一个server
	bool   m_bIsIsland;                     // 是否是孤岛
	uint32_t m_nZoneLoad;                     // 本zone浮躁
	bool   m_bHasInit;                      // 本zone是否初始化了
private:
	bool IsBridge();                        // 是否是桥
public:
    // 构造函数
	UniG_ILM_Zone(uint16_t zoneID, uint32_t mapID, bool isIsland);

    // 析构函数
	~UniG_ILM_Zone(void);

    // 添加本zone所管理的地图单元
	void AddMapUnit(uint32_t mapID);

    // 得到本地图所管理的地图单元
        void GetMapUnit(list<uint32_t>& lstMapUnit);

    // 清理本zone所包含的地图单元
	void ClearMapUnit();

    // 本zone是否只由一个mapUnit组成
	bool IsSigleMapUnit();

    // 加载相邻的各个zone的状态
	void LoadNeighbourState();

    // 得到zoneID
	uint16_t GetZoneID();

    // 得到zone负载
	uint32_t GetZoneLoad();

    // 设置本zone负载
        void SetZoneLoad(uint32_t load);

    // 得到本zone规谁管理
	uint16_t GetServerID();

    // 设置本zone的server ID
        void SetServerID(uint16_t serverID);

    //是否是孤岛
	bool   IsIsland();
};

#endif
