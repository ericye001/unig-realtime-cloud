#ifndef UniG_ILM_Zone_MGR_H__
#define UniG_ILM_Zone_MGR_H__
#include <time.h>

#include "Global.h"
#include "Config.h"
#include "Zone.h"

class UniG_ILM_ZoneMgr
{
private:
	UniG_ILM_ZoneMgr(void);
	UniG_ILM_ZoneMgr(const UniG_ILM_ZoneMgr& that);
	UniG_ILM_ZoneMgr operator = (const UniG_ILM_ZoneMgr& that);

private:
	list<UniG_ILM_Zone* > m_lstZone;
	static UniG_ILM_ZoneMgr* m_pInstance;

public:
	static UniG_ILM_ZoneMgr* Instance();
	void LoadZoneInfo();
	void AddZone(UniG_ILM_Zone* zone);

	UniG_ILM_Zone* GetZoneByID(uint16_t zoneID);

	uint16_t GetServerIDByZoneID(uint16_t zoneID);
	void UpdateZoneInfo(void);
	~UniG_ILM_ZoneMgr(void);
	void SetRandomLoad(uint32_t maxLoad);
};

#endif
