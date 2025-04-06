#ifndef _UNIG_ILM_PLANMGR_H__
#define _UNIG_ILM_PLANMGR_H__

#include "ZoneMgr.h"
#include "Zone.h"

class UniG_ILM_PlanMgr
{
private:
    // 单态类,避免新建对象
	UniG_ILM_PlanMgr(void);
	UniG_ILM_PlanMgr(UniG_ILM_PlanMgr& that);
	UniG_ILM_PlanMgr operator = (UniG_ILM_PlanMgr& that);
private:
    // 岛搬家,因为岛搬家有一定的优先级
    bool IslandMigration(uint16_t serverID, uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer);
private:
	static UniG_ILM_PlanMgr* m_pInstance;
	list<NeighbourState> m_lstAllNeighbour;                 // 所有zone和周围server的相邻关系
	list<NeighbourState> m_lstCurServerNeighbour;           // 指定server和周围server的相邻关系
public:
    // 析构函数
    ~UniG_ILM_PlanMgr(void);

    // 得到本server的实例
	static UniG_ILM_PlanMgr* Instance();

    // 添加一个相邻关系
	void AddNeighbour(NeighbourState state);

    // 清空相邻关系列表
	void ClearNeighbourList();

    // 打印搬家计划列表
	void PrintPlanList();

    // 得到属于当前server的zone和其他server的相邻关系
	bool GetCurServerNeighbour(uint16_t serverID, bool out = true);

    // 形成忙server向外搬家的计划
	int32_t FormBusyPlanList(uint16_t serverID);

    // 形成闲server向搬的计划
	int32_t FormIdlePlanList(uint16_t serverID);

    // 选择一个计划
	bool ChoosePlan(uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer);
    
    // 指定计划,得到搬家涉及到的zone,from, to
	bool MakePlan(uint16_t serverID, uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer);
};

#endif
