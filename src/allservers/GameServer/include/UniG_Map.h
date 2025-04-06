/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_Map.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: ��ͼ������

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 
#ifndef __UNIG_GAMEMAP_H__
#define __UNIG_GAMEMAP_H__
/**
 * ��ͼ������,��̬��
 */
class UniG_Map
{
private:
	UniG_Map(const UniG_Map&);
	UniG_Map& operator=(const UniG_Map&);
    UniG_Map();

public:
    /**
     * ʵ����һ��UniG_Map
     */
    static UniG_Map* Instance();

    /**
     * ��������
     */
	~UniG_Map();

    /**
     * ������е�ͼ
     */ 
	void Clean();


    /**
     * ����ӵ�ͼ
     * @param pSubMap �ӵ�ͼ
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
     * ���µ�ͼ����ͼ�ϵ�����
     */
	void Update();

    int GetAllSubMaps(vector<UniG_SubMap*>& vecMaps);

private:
    static UniG_Map*  m_pInstance;
    map<string, UniG_SubMap*> m_mapSubMaps;

}; 

#endif
