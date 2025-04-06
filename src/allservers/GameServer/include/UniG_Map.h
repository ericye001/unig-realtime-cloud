/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_Map.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 地图管理器

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 
#ifndef __UNIG_GAMEMAP_H__
#define __UNIG_GAMEMAP_H__
/**
 * 地图管理器,单态类
 */
class UniG_Map
{
private:
	UniG_Map(const UniG_Map&);
	UniG_Map& operator=(const UniG_Map&);
    UniG_Map();

public:
    /**
     * 实例化一个UniG_Map
     */
    static UniG_Map* Instance();

    /**
     * 析构函数
     */
	~UniG_Map();

    /**
     * 清空所有地图
     */ 
	void Clean();


    /**
     * 添加子地图
     * @param pSubMap 子地图
     * @return -1:failure 0:success
     */
    void RegisterSubMap(UniG_SubMap* pSubMap);

    /**
     * remove a submap from umap
     * @param pSubMap the submap to be deleted
     * @return -1:failure  0:success
     */
    int RemoveSubMap(UniG_SubMap* pSubMap);

    /**
     * get the submap pointer from submap's name
     * @param subMapName
     */
	UniG_SubMap* GetSubMap(string strSubMapKey);

	/**
     * 更新地图及地图上的物体
     */
	void Update();

    int GetAllSubMaps(vector<UniG_SubMap*>& vecMaps);

private:
    static UniG_Map*  m_pInstance;
    map<string, UniG_SubMap*> m_mapSubMaps;

}; 

#endif
