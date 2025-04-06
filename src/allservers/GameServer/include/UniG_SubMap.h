/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_SubMap.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 地图系统的封装

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef __UNIG_SUB_MAP_H__
#define __UNIG_SUB_MAP_H__

class ObjectMessage;
typedef UNIG_GUID ObjKey;
typedef int ObjTypeKey;
typedef CCell<Vector3, float, 16> MapCell;
typedef CRegion<Vector3, MapCell, 16, UniG_GameObject, ObjKey, ObjTypeKey, 4> MapRegion;

class UniG_SubMap
{
public:
	UniG_SubMap();
	~UniG_SubMap();

    const std::string& GetSubMapName()				const	{ return m_SubMapName;}    
	void SetSubMapName(const std::string& smapname)			{ m_SubMapName = smapname;}

	void Clean();
	bool LoadMap( const std::string& filename );
	void CreateMap( int nRow, int nCol, bool bRandomHeight );

	// Movable Objects Management
	MapRegion*	GetRegion(const Vector3& pos);							// only region
	MapRegion*	GetRegion(const Vector3& pos, float* pHeight);			// region & cell height
	MapRegion*	GetRegion(int nRow, int nCol ); 

	const MapRegion*	GetRegion(const Vector3& pos)					const { return GetRegion(pos); }					// only region
	const MapRegion*	GetRegion(const Vector3& pos, float* pHeight)	const { return GetRegion(pos, pHeight);} 			// region & cell height
	const MapRegion*	GetRegion(int nRow, int nCol )					const { return GetRegion(nRow, nCol); }

	// dynamic object manager
	void ObjectEnter( const ObjTypeKey& type, const Vector3& vPos , const ObjKey& strObjectName, UniG_GameObject* pObj);
	void ObjectLeave( const ObjTypeKey& type, const Vector3& vPos , const ObjKey& strObjectName, UniG_GameObject* pObj);
	void ObjectMove( const ObjTypeKey& type, const Vector3& vLastPos, const Vector3& vPos, const ObjKey& strObjectName, UniG_GameObject* pObj);

	// look for object, if finded, return true
	UniG_GameObject* FindObject( const ObjTypeKey& type, const Vector3& vCenter, float fRadius, const ObjKey& strObjectName);

    UniG_GameObject* FindObject(const ObjTypeKey& type, const ObjKey& strObjectName);

	// broadcast
    void Area_For_Each(const ObjTypeKey& type, const Vector3& vCenter, float fRadius, UniG_GameObject* pSrc, void* pParam, int nFlag, void (*pfnCallback)(UniG_GameObject*, UniG_GameObject*, void* pParam, int nFlag));
    void Sync_For_Each(const ObjTypeKey& type, UniG_GameObject* pSrc, void* pParam, int nFlag, void (*pfnCallback)(UniG_GameObject*, UniG_GameObject*, void* pParam, int nFlag));
    void Sync_For_Each(const ObjTypeKey& type, const Vector3& vPos, UniG_GameObject* pSrc, void* pParam, int nFlag, void (*pfnCallback)(UniG_GameObject*, UniG_GameObject*, void* pParam, int nFlag) );

    void BroadcastMsg( const ObjTypeKey& type, UniG_GameObject* pSrc, ObjectMessage* pMsg, bool bToSelf );
	void BroadcastMsg( const ObjTypeKey& type, const Vector3& vPos, UniG_GameObject* pSrc, ObjectMessage* pMsg, bool bToSelf );

    //void NotifyLaterPlayer(const ObjTypeKey& type, ObjectMessage* pMsg)

    // get the regions around the given region
    int GetAroundRegion(MapRegion* pRegion, vector<MapRegion*>& regionList);

    // set region info , the single unit info is 4 bytes
    void SetRegionInfo(uint32* pRegionInfo, uint32 nRegionNum)
    {
        if(nRegionNum >0 && nRegionNum == m_nRowRegionNum * m_nColRegionNum)
        {
            m_pRegionInfo = new uint32[nRegionNum];
            PtrMemCpy(m_pRegionInfo, pRegionInfo, nRegionNum * sizeof(uint32));
        }
        for(int i=0; i< m_nRowRegionNum * m_nColRegionNum; i++)
        {
            m_pRegionArrPtr[i].ServerID((uint16)(m_pRegionInfo[i] & 0x11110000));
            m_pRegionArrPtr[i].ZoneID((uint16)(m_pRegionInfo[i] & 0x00001111));
        }
    }

	// Update
	void Update();

public:
	std::string m_SubMapName;

	// the two vectors setup the map coordination
	Vector3 m_vMapBegin;
	Vector3 m_vMapEnd;

	float m_fXRegionUnit;
	float m_fZRegionUnit;
	// the number of tile rows and columns will be set from the map file
	int m_nRowRegionNum;
	int m_nColRegionNum;

    // the submap's global coordinate in the UniG_Map(left top)
    int m_nStartPosGlobalX;
    int m_nStartPosGlobalY;

    uint32 GetKey()
    {
        return ((uint32)m_nStartPosGlobalX << 16) + (uint32)m_nStartPosGlobalY;
    }
    uint32* m_pRegionInfo;
	MapRegion* m_pRegionArrPtr;
};

#endif
