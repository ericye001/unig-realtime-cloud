#include "ZoneMgr.h"

UniG_ILM_ZoneMgr* UniG_ILM_ZoneMgr::m_pInstance = NULL;

UniG_ILM_ZoneMgr::UniG_ILM_ZoneMgr()
{

}

UniG_ILM_ZoneMgr::~UniG_ILM_ZoneMgr(void)
{

}

UniG_ILM_ZoneMgr* UniG_ILM_ZoneMgr::Instance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new UniG_ILM_ZoneMgr();
	}
	return m_pInstance;
}

void UniG_ILM_ZoneMgr::LoadZoneInfo()
{
  list<uint16_t> lstZone;
  list<MapUnitInfo> lstMapUnit;
  list<MapUnitInfo>::iterator iterMapUnit;
  list<uint16_t>::iterator iterZone;
  // 得到所有的zone编号
  UniG_ILM_Config::Instance()->GetAllZoneID(lstZone);
  
  for(iterZone = lstZone.begin(); iterZone !=lstZone.end(); iterZone++)
    {
      lstMapUnit.clear();
      UniG_ILM_Config::Instance()->GetMapUnitByZone(*iterZone, lstMapUnit);
      UniG_ILM_Zone* pZone = new UniG_ILM_Zone(*iterZone, lstMapUnit.begin()->mapID,lstMapUnit.begin()->isIsland);
      for(iterMapUnit = lstMapUnit.begin(); iterMapUnit != lstMapUnit.end(); iterMapUnit++)
        {
          pZone->AddMapUnit(iterMapUnit->mapUnitID);
          // printf("Mapuint [%d] in Zone [%d]\n",iterMapUnit->mapUnitID,*iterZone);
        }
      m_lstZone.push_back(pZone);
    }
}

void UniG_ILM_ZoneMgr::AddZone(UniG_ILM_Zone* zone)
{
	m_lstZone.push_back(zone);
}





UniG_ILM_Zone* UniG_ILM_ZoneMgr::GetZoneByID(uint16_t zoneID)

{
	list<UniG_ILM_Zone*>::iterator iter;
	for(iter = m_lstZone.begin(); iter != m_lstZone.end(); iter++)
	{
		if((*iter)->GetZoneID() == zoneID)
		{
			return *iter;
		}
	}
	return NULL;
}


uint16_t UniG_ILM_ZoneMgr::GetServerIDByZoneID(uint16_t zoneID)
{
	return GetZoneByID(zoneID)->GetServerID();
}

void UniG_ILM_ZoneMgr::UpdateZoneInfo(void)
{
  //	UniG_ILM_PlanMgr::Instance()->ClearNeighbourList();
	list<UniG_ILM_Zone*>::iterator iter;
	for(iter = m_lstZone.begin(); iter != m_lstZone.end(); iter++)
	{
		(*iter)->LoadNeighbourState();
	}
}

void UniG_ILM_ZoneMgr::SetRandomLoad(uint32_t maxLoad)
{
	list<UniG_ILM_Zone*>::iterator iter;
	srand( (unsigned)time( NULL ) );
	for(iter = m_lstZone.begin(); iter != m_lstZone.end(); iter++)
	{
		(*iter)->SetZoneLoad(rand()%maxLoad);
	}
}
