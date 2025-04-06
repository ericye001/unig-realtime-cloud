#ifndef _UNIG_ILM_CONFIG_H__
#define _UNIG_ILM_CONFIG_H__
#include "Global.h"
#pragma pack (1)
struct MapUnitInfo
{
	uint32_t mapUnitID;
	char   mapUnitName[MAX_MAPUNIT_NAME_LENGTH];
	uint16_t zoneID;
	uint32_t mapID;
	uint16_t serverID;
	bool   isIsland;
};

struct MapTile
{
	uint32_t mapTileID;
	uint32_t sizeX;
	uint32_t sizeY;
	vector<MapUnitInfo> vecMapUnit;
};

struct ZoneGroup
{	
	uint16_t nServerGroup;
	uint16_t nServerID;
	bool   bDispatched;
	list<uint16_t> ZoneID;

	ZoneGroup()
	{
		nServerGroup = 0;
		nServerID = 0;
		bDispatched = false;
		ZoneID.clear();
	}

	~ZoneGroup()
	{
	}
};


struct MapUnitOnServer
{
	uint16_t serverID;
	uint32_t mapID;
	list<uint32_t> lstMapUnit;
};

class UniG_ILM_Config
{
private:
	UniG_ILM_Config(void);
	bool AssignZoneID();
	bool AssignNeighbourState();
	bool LoadServerGroupOnMap();

private:
	vector<MapUnitInfo> m_vecMapUnit;
	static UniG_ILM_Config* m_pInstance;
	map<uint32_t, string> m_mapMapSize;
	map<uint32_t, list<uint16_t> > m_mapMapServerGroup;
	multimap<uint32_t, uint32_t> m_mapNeighbourMaps;
	vector<MapUnitOnServer> m_vecMapUnitOnServer;
	vector<MapTile> m_mapMapTile;
	vector<ZoneGroup> m_vZoneGroup;

public:

	~UniG_ILM_Config(void);
	static UniG_ILM_Config* Instance();
        
	bool LoadConfigFile(string configFilePath);
	bool GetServeGroupByMapID(uint32_t mapID, list<uint16_t>& lstServer);
	// 根据Server得到同组的其他server
	void GetAllZoneID(list<uint16_t>& lstZone);
	void GetAllServerID(list<uint16_t>& lstServer);
	void GetAllMapID(list<uint32_t>& lstMap);
	void GetMapUnitByZone(uint16_t zoneID, list<MapUnitInfo>& lstMap);
	void PrintMapUnitInfo();
	void GetNeighbourMapUnit(uint32_t mapUnitID, list<uint32_t>& mapUnit);
	uint32_t GetZoneIDByMapUnitID(uint32_t mapUnitID);
	bool IsNeighbour(uint32_t map1, uint32_t map2);
	bool GetManagedZone(list<uint16_t>& lstZone, uint16_t &ServerGroupID );
	void LoadZoneGroup();
        bool RecoveryManagedZone(list<uint16_t> lstZone, uint16_t ServerGroupID );

        uint32_t GetNeighbourMapUnit(multimap<uint32_t, uint32_t>& mapNeighbourMaps);
        uint32_t GetMapUnitInfo(vector<MapUnitInfo>& vecMapUnit);
};

#pragma pack ()
typedef multimap<uint32_t, uint32_t>::iterator iterator_map;


#endif
