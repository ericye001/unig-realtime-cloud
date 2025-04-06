#include "UniG.h"

UniG_Map* UniG_Map::m_pInstance = 0;

UniG_Map::UniG_Map()
{

}

UniG_Map::~UniG_Map()
{
    Clean();
}

UniG_Map* UniG_Map::Instance()
{
    if(m_pInstance == 0)
    {
        m_pInstance = new UniG_Map();
    }
    return m_pInstance;
}

void UniG_Map::Clean()
{
	
}


void UniG_Map::RegisterSubMap(UniG_SubMap* pSubMap)
{
    if(pSubMap)
    {
        if(m_mapSubMaps.find(pSubMap->GetSubMapName()) == m_mapSubMaps.end())
        {
            m_mapSubMaps.insert(pair<string, UniG_SubMap*>(pSubMap->GetSubMapName(), pSubMap));
        }
    }
}

int UniG_Map::RemoveSubMap(UniG_SubMap* pSubMap)
{
    //if(pSubMap == 0)
    //{
    //    return -1;
    //}
    //map<uint32, UniG_SubMap*>::iterator iter;
    //iter = m_mapSubMaps.find(pSubMap->GetKey());
    //if(iter == m_mapSubMaps.end())
    //{
    //    return -1;
    //}
    //else
    //{
    //  
    //    m_mapSubMaps.erase(pSubMap->GetKey());
    //    return 1;
    //}
    return 1;
}



UniG_SubMap* UniG_Map::GetSubMap(string strSubMapKey)
{

	map<string, UniG_SubMap*>::iterator it = m_mapSubMaps.find(strSubMapKey);
	if( it != m_mapSubMaps.end() )
    {
        return it->second;
    }
	return NULL;
}

void UniG_Map::Update()
{

	map<string, UniG_SubMap*>::iterator mapIter;

	for(mapIter=m_mapSubMaps.begin(); mapIter!=m_mapSubMaps.end(); mapIter++)
	{
		// update all submaps
		mapIter->second->Update();
	}

}

int UniG_Map::GetAllSubMaps(vector<UniG_SubMap*>& vecMaps)
{
    int nResult = 0;
    map<string, UniG_SubMap*>::iterator iter;
    for(iter = m_mapSubMaps.begin(); iter != m_mapSubMaps.end(); iter++)
    {
        vecMaps.push_back(iter->second);
        nResult++;
    }
    return nResult;
}