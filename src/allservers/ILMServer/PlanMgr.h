#ifndef _UNIG_ILM_PLANMGR_H__
#define _UNIG_ILM_PLANMGR_H__

#include "ZoneMgr.h"
#include "Zone.h"

class UniG_ILM_PlanMgr
{
private:
    // ��̬��,�����½�����
	UniG_ILM_PlanMgr(void);
	UniG_ILM_PlanMgr(UniG_ILM_PlanMgr& that);
	UniG_ILM_PlanMgr operator = (UniG_ILM_PlanMgr& that);
private:
    // �����,��Ϊ�������һ�������ȼ�
    bool IslandMigration(uint16_t serverID, uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer);
private:
	static UniG_ILM_PlanMgr* m_pInstance;
	list<NeighbourState> m_lstAllNeighbour;                 // ����zone����Χserver�����ڹ�ϵ
	list<NeighbourState> m_lstCurServerNeighbour;           // ָ��server����Χserver�����ڹ�ϵ
public:
    // ��������
    ~UniG_ILM_PlanMgr(void);

    // �õ���server��ʵ��
	static UniG_ILM_PlanMgr* Instance();

    // ���һ�����ڹ�ϵ
	void AddNeighbour(NeighbourState state);

    // ������ڹ�ϵ�б�
	void ClearNeighbourList();

    // ��ӡ��Ҽƻ��б�
	void PrintPlanList();

    // �õ����ڵ�ǰserver��zone������server�����ڹ�ϵ
	bool GetCurServerNeighbour(uint16_t serverID, bool out = true);

    // �γ�æserver�����ҵļƻ�
	int32_t FormBusyPlanList(uint16_t serverID);

    // �γ���server���ļƻ�
	int32_t FormIdlePlanList(uint16_t serverID);

    // ѡ��һ���ƻ�
	bool ChoosePlan(uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer);
    
    // ָ���ƻ�,�õ�����漰����zone,from, to
	bool MakePlan(uint16_t serverID, uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer);
};

#endif
