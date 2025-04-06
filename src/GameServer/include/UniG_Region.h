/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_Region.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: Region encapsulate

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _UNIG_REGION_H__
#define _UNIG_REGION_H__


#include "UniG.h"

#define MAX_MANAGER_GRID_NUM 1

/**
 * 一个Region格子的信息
 */
struct CellInfo	
{
    int length;
    void* data;
};

#define MAX_OBJECT_TYPE 4               ///< 最大对象类型数目

typedef CObjectContainer< UNIG_GUID, UniG_GameObject, uint32, MAX_OBJECT_TYPE > ObjectPool;

template<class Vector, class InfoType, int n>
class CLevelGrid
{

 public:
	int	  m_nRow, m_nCol;   ///< 在上一层容器里的位置
	float m_X, m_Z;         ///< 本层容器的绝对坐标

	float m_XLen;			///< 本容器X方向的长度
	float m_ZLen;           ///< 本容器Z方向的长度

	InfoType m_Info[n][n];
public:
	virtual void Setup( int row, int col, float x, float z, float xlen, float zlen )
	{
		m_nRow = row;
		m_nCol = col;
		m_X = x;
		m_Z = z;
		m_XLen = xlen;
		m_ZLen = zlen;
	}

	void SetupChildLevelGrid()			// InfoType同样继承于CLevelGrid才能使用
	{
		float fxu = m_XLen / n;
		float fzu = m_ZLen / n;
		float fx = m_X;
		float fz = m_Z;
		// child levelgrid setup
		for( int i = 0; i < n; ++i )
		{
			fz += i * fzu;
			for( int j = 0; j < n; ++j)
			{
				fx += j * fxu;
				GetIndexInfo(i,j)->Setup(i, j, fx, fz, fxu, fzu);
			}
		}
	}

	InfoType* GetIndexInfo(int row, int col)
	{
		return &(m_Info[row][col]);
	}

	InfoType* GetPosInfo( const Vector& vPos )
	{
		float fXRegionOff = vPos.x - m_X;
		float fZRegionOff = vPos.z - m_Z;

		if( ((fXRegionOff * (m_XLen - fXRegionOff)) < 0) || ((fZRegionOff *(m_ZLen - fZRegionOff)) < 0))
			return 0;

		int row, col;

		row = (int)( n * fZRegionOff / m_ZLen );
		col = (int)( n * fXRegionOff / m_XLen );
	
		return &(m_Info[row][col]);
	}
};


template<class Vector, class HeightInfoType, int n>
class CCell : public CLevelGrid<Vector, HeightInfoType, n>
{
public:
    CellInfo* m_pCellInfo;

	HeightInfoType* GetHeight(const Vector& vPos)
	{
		return GetPosInfo(vPos);					// example
	}

    void SetCellInfo(CellInfo* pInfo)
    {
        m_pCellInfo->length = pInfo->length;
        if(m_pCellInfo->length > 0)
        {
            m_pCellInfo->data = new char[m_pCellInfo->length];
            memcpy(m_pCellInfo->data, pInfo->data, m_pCellInfo->length);
        }
        else
        {
            m_pCellInfo->data = 0;
        }
    }

    CellInfo* GetCellInfo()
    {
        return m_pCellInfo;
    }
    
};

template<class Vector, class Cell, int nCell, class Object, class Key, class TypeKey, int nType>
class CRegion : 
	public CLevelGrid<Vector, Cell, nCell>, 
	public CManagerGrid< MAX_MANAGER_GRID_NUM, Vector,
						CRegion<Vector, Cell, nCell, Object, Key, TypeKey, nType>, 
						CObjectContainer<Key, Object, TypeKey, nType> >
{
	typedef CObjectContainer<Key, Object, TypeKey, nType> MAPOBJECTPOOL;
    typedef CRegion<Vector, Cell, nCell, Object, Key, TypeKey, nType> _Myt;
    typedef CManagerGrid<MAX_MANAGER_GRID_NUM, Vector, _Myt, MAPOBJECTPOOL> _MyMgr;
    
public:
	CRegion() 
    { 
        SetParent(this);
        m_bLocked = false;
    }

	Cell* GetCell(const Vector& vPos){ return GetPosInfo(vPos);}
	
	void Setup( int row, int col, float x, float z, float xlen, float zlen )
	{
		CLevelGrid<Vector, Cell, nCell>::Setup(row, col, x, z, xlen, zlen);
		SetupChildLevelGrid();          // 设置所有的Cell层级网格信息
	}

    void Lock()
    {
        m_bLocked = true;
    }

    void UnLock()
    {
        m_bLocked = false;
    }

    bool IsLocked()
    {
        return m_bLocked;
    }

    uint32 GetID()
    {
        return 0;
    }

    uint16 ServerID()
    {
        return m_nServerID;
    }

    void ServerID(uint16 nServerID)
    {
        m_nServerID = nServerID;
    }

    uint16 ZoneID()
    {
        return m_nZoneID;
    }

    void ZoneID(uint16 nZoneID)
    {
        m_nZoneID = nZoneID;
    }

    uint32 RegionID()
    {
        return m_nRegionID;
    }

    void RegionID(uint32 nRegionID)
    {
        m_nRegionID = nRegionID;
    }

    uint32 GetRegionLoad(const TypeKey& type )
    {
        return size(type);
    }
public:
	// object manager	
	void ObjectEnter(Object* pObj, const Vector& vPos, const Key& keyObj, const TypeKey& type );
	void ObjectLeave(const Vector& vPos, const Key& keyObj, const TypeKey& type );

	void	For_Each( const TypeKey& type, void* pParam, void (*pfnCallback)(Object*, void* pParam) );
    void	For_Each( const TypeKey& type, void* pParam, void (*pfnCallback)(Object*, void* pParam), Object*);
	Object* FindObject(const Key& keyObj, const TypeKey& type);

	void Update();
private:
    bool m_bLocked;
    uint16 m_nServerID;
    uint16 m_nZoneID;
    uint32 m_nRegionID;
};

//====================================================================================================
//

template<class Vector, class Cell, int nCell, class Object, class Key, class TypeKey, int nType>
void CRegion<Vector, Cell, nCell, Object, Key, TypeKey, nType>::Update()
{
	int i, j;
	
    std::list<Object*> objlist;
	for(i=0; i<MAX_MANAGER_GRID_NUM; i++)
	{
		for(j=0; j<MAX_MANAGER_GRID_NUM; j++)
		{
			//for(m=0; m<nType; m++)
			//{

   //             typename MAPOBJECTPOOL::KEY2OBJECT* pMap = _MyMgr::m_Pools[i][j].GetTypeMap(m);
			//	if( !pMap )
			//		continue;

			//	typename MAPOBJECTPOOL::KEY2OBJECT::iterator it = pMap->begin();
			//	while(it != pMap->end())
			//	{
			//		it->second->Update();
			//		it++;
			//	}
			//}
            typename std::list<Object*>::iterator it;
            _MyMgr::m_Pools[i][j].Clone2List(objlist);
            for( it = objlist.begin(); it != objlist.end(); it++)
            {
                (*it)->Update();
            }
            objlist.clear();            
		}
	}
}

template<class Vector, class Cell, int nCell, class Object, class Key, class TypeKey, int nType>
Object* CRegion<Vector, Cell, nCell, Object, Key, TypeKey, nType>::FindObject(const Key& keyObj, const TypeKey& type)
{
	Object* pobj = 0;

	for( int i = 0; i < MAX_MANAGER_GRID_NUM; ++i )
	{
		for( int j = 0; j < MAX_MANAGER_GRID_NUM; ++j )
		{
			typename MAPOBJECTPOOL::iterator it = _MyMgr::m_Pools[i][j].find(type, keyObj);
			if( it != _MyMgr::m_Pools[i][j].end(type) )
			{
				return it->second;
			}
		}
	}
	return 0;
}

template<class Vector, class Cell, int nCell, class Object, class Key, class TypeKey, int nType>
void CRegion<Vector, Cell, nCell, Object, Key, TypeKey, nType>::ObjectEnter(Object* pObj, const Vector& vPos, const Key& keyObj, const TypeKey& type )
{
	MAPOBJECTPOOL* pool = GetPool( vPos );
	if( !pool )
	{
		return;
	}
	pool->InsertObject( type, keyObj, pObj );
}

template<class Vector, class Cell, int nCell, class Object, class Key, class TypeKey, int nType>
void CRegion<Vector, Cell, nCell, Object, Key, TypeKey, nType>::ObjectLeave(const Vector& vPos, const Key& keyObj, const TypeKey& type )
{
	MAPOBJECTPOOL* pool = GetPool( vPos );
	if( !pool )
	{
		return;
	}
	pool->RemoveObject( type, keyObj);
}

template<class Vector, class Cell, int nCell, class Object, class Key, class TypeKey, int nType>
void CRegion<Vector, Cell, nCell, Object, Key, TypeKey, nType>::For_Each( const TypeKey& type, void* pParam, void (*pfnCallback)(Object*, void* pParam) )
{
    std::list<Object*> objlist;
	for( int i = 0; i < MAX_MANAGER_GRID_NUM; ++i )
		for( int j = 0; j < MAX_MANAGER_GRID_NUM; ++j )
		{
			//typename MAPOBJECTPOOL::iterator it = _MyMgr::m_Pools[i][j].begin( type );
			//while( it != _MyMgr::m_Pools[i][j].end(type) )
			//{
			//	pfnCallback(it->second, pParam);
			//	it++;
			//}

            typename std::list<Object*>::iterator it;
            _MyMgr::m_Pools[i][j].Clone2List(type, objlist);
            for( it = objlist.begin(); it != objlist.end(); it++)
            {
				pfnCallback(*it, pParam);
            }
			objlist.clear();
		}
}


template<class Vector, class Cell, int nCell, class Object, class Key, class TypeKey, int nType>
void CRegion<Vector, Cell, nCell, Object, Key, TypeKey, nType>::For_Each( const TypeKey& type, void* pParam, void (*pfnCallback)(Object*, void* pParam) , Object* pExcludeObj)
{
	for( int i = 0; i < MAX_MANAGER_GRID_NUM; ++i )
		for( int j = 0; j < MAX_MANAGER_GRID_NUM; ++j )
		{
			//typename MAPOBJECTPOOL::iterator it = _MyMgr::m_Pools[i][j].begin( type );
			//while( it != _MyMgr::m_Pools[i][j].end(type) )
			//{
			//	pfnCallback(it->second, pParam);
			//	it++;
			//}

            std::list<Object*> objlist;
            typename std::list<Object*>::iterator it;
            _MyMgr::m_Pools[i][j].Clone2List(type, objlist);
            for( it = objlist.begin(); it != objlist.end(); it++)
            {
                if(*it != pExcludeObj)
                {
				    pfnCallback(*it, pParam);
                }
            }
			
		}
}


#endif

