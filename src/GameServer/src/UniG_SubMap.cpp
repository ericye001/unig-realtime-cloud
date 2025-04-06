#include <limits>
#include "UniG.h"

#ifdef max
#undef max
#endif 


#ifdef min
#undef min
#endif 


#define MAX_SYNC_REGION_LEVEL_NUM 1
//=====================================================================================================
// sub map 

UniG_SubMap::UniG_SubMap()
:m_pRegionArrPtr(NULL)
{}

UniG_SubMap::~UniG_SubMap()
{
	if(m_pRegionArrPtr)
	{
		delete[] m_pRegionArrPtr;
	}
}

void UniG_SubMap::Clean()
{
	if( m_pRegionArrPtr )
	{
		for( int i = 0; i < m_nColRegionNum * m_nRowRegionNum; ++i )
		{
			m_pRegionArrPtr[i].Clean();
		}
		delete [] m_pRegionArrPtr;
		m_pRegionArrPtr = NULL;
	}
	
}

bool UniG_SubMap::LoadMap( const std::string& filename )
{
	//std::ifstream inbuff;
	//MapFileHeader header;

	//inbuff.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);

	//inbuff.read((char*)&header, sizeof( header) );
	//m_fXRegionUnit = header.fTileLen * MAXNUMCELLOFTILE;
	//m_fZRegionUnit = header.bZPlus ? m_fXRegionUnit : -m_fXRegionUnit;
	//m_fXRegionUnit = header.bXPlus ? m_fXRegionUnit : -m_fXRegionUnit;


	//m_nColRegionNum	= (int)header.dwCol;
	//m_nRowRegionNum	= (int)header.dwRow;

	//m_vMapBegin.x = 0.0f;
	//m_vMapBegin.y = std::numeric_limits<float>::min();
	//m_vMapBegin.z = 0.0f;

	//m_vMapEnd.x = m_fXRegionUnit * m_nColRegionNum;
	//m_vMapEnd.z = m_fZRegionUnit * m_nRowRegionNum;
	//m_vMapEnd.y = std::numeric_limits<float>::max();

	//// all regions
	//m_pRegionArrPtr = new MapRegion[ m_nColRegionNum * m_nRowRegionNum ];
	//float x,z;
	//int nInd = 0;
	//for( int i = 0; i < m_nRowRegionNum; ++i )
	//{
	//	z = m_vMapBegin.z + m_fZRegionUnit * i;

	//	for( int j = 0; j < m_nColRegionNum; j++ )
	//	{
	//		x = m_vMapBegin.x + m_fXRegionUnit * j;
	//		m_pRegionArrPtr[nInd].Setup( i, j, x, z, m_fXRegionUnit, m_fZRegionUnit );

	//		inbuff.read( (char*)& (m_pRegionArrPtr[i].m_RegionInfo), sizeof( RegionInfo ) );
	//		nInd++;
	//	}
	//}
    return true;
}

void UniG_SubMap::CreateMap( int nRow, int nCol, bool bRandomHeight )
{
	m_fXRegionUnit = 18000;
	m_fZRegionUnit = 18000;


	m_nColRegionNum	= (int)nCol;
	m_nRowRegionNum	= (int)nRow;

	m_vMapBegin.x = -( m_fXRegionUnit * m_nColRegionNum * 0.5f );
	m_vMapBegin.y = std::numeric_limits<float>::min();
	m_vMapBegin.z = -( m_fZRegionUnit * m_nRowRegionNum * 0.5f );

	m_vMapEnd.x = m_fXRegionUnit * m_nColRegionNum * 0.5f;
	m_vMapEnd.z = m_fZRegionUnit * m_nRowRegionNum * 0.5f;
	m_vMapEnd.y = std::numeric_limits<float>::max();

	// all regions
	m_pRegionArrPtr = new MapRegion[ m_nColRegionNum * m_nRowRegionNum ];
	float x,z;
	int nInd = 1;
	for( int i = 0; i < m_nRowRegionNum; ++i )
	{
		z = m_vMapBegin.z + m_fZRegionUnit * i;

		for( int j = 0; j < m_nColRegionNum; j++ )
		{
			x = m_vMapBegin.x + m_fXRegionUnit * j;
			m_pRegionArrPtr[nInd-1].Setup( i, j, x, z, m_fXRegionUnit, m_fZRegionUnit );
            m_pRegionArrPtr[nInd-1].RegionID(nInd);
			nInd++;
		}
	}
}


MapRegion* UniG_SubMap::GetRegion(const Vector3& pos)
{
	// check pos x z
	if( ( ( pos.x - m_vMapBegin.x) * ( m_vMapEnd.x - pos.x ) < 0 ) ||
		( ( pos.z - m_vMapBegin.z) * ( m_vMapEnd.z - pos.z ) < 0 ) )
		return NULL;
	// x is the x-coord, y is the y-coord of the virtual region two-dimensional index
	int x, y;

	x = (int)( ( pos.x - m_vMapBegin.x ) / m_fXRegionUnit );
	y = (int)( ( pos.z - m_vMapBegin.z ) / m_fZRegionUnit );

	return GetRegion( y, x );
}

MapRegion* UniG_SubMap::GetRegion(const Vector3& pos, float* pHeight)			// region & cell height
{
	MapRegion* pRe = GetRegion(pos);
	if( pRe && pHeight)
		*pHeight = *(pRe->GetCell(pos)->GetHeight(pos));
	return pRe;	
}

MapRegion* UniG_SubMap::GetRegion(int nRow, int nCol )
{
	if( nRow < 0 || nRow >= m_nRowRegionNum || nCol < 0 || nCol >= m_nColRegionNum )
	{
		return NULL;
	}
	return & m_pRegionArrPtr[ nRow * m_nColRegionNum + nCol ]; 
}
// dynamic object manager
void UniG_SubMap::ObjectEnter( const ObjTypeKey& type, const Vector3& vPos , const ObjKey& strObjectName, UniG_GameObject* pObj)
{
	MapRegion* curRegion = GetRegion(vPos);
	curRegion->ObjectEnter(pObj, vPos, strObjectName, type );

    pObj->OnEnterSubMap(this, curRegion, vPos);
}

void UniG_SubMap::ObjectLeave( const ObjTypeKey& type, const Vector3& vPos , const ObjKey& strObjectName, UniG_GameObject* pObj)
{
	MapRegion* curRegion = GetRegion(vPos);
	curRegion->ObjectLeave( vPos, strObjectName, type);

    pObj->OnLeaveSubMap(this, curRegion, vPos);
}

void UniG_SubMap::ObjectMove( const ObjTypeKey& type, const Vector3& vLastPos, const Vector3& vPos, const ObjKey& strObjectName, UniG_GameObject* pObj)
{
	MapRegion* pLastRegion = GetRegion(vLastPos);
	MapRegion* pDestRegion =  GetRegion(vPos);

	if( !pLastRegion)
		return;			

    if( !pDestRegion )
    {
	    pObj->OnMove(vLastPos, true);
    }
    else
    {
        pObj->OnMove(vPos, false);

	    if( pLastRegion != pDestRegion )
	    {
		    pLastRegion->ObjectLeave(vLastPos, strObjectName, type);
		    pDestRegion->ObjectEnter(pObj, vPos, strObjectName, type );

		    pObj->OnRegionChange(pLastRegion, pDestRegion);
	    }
    }
}

static bool IsWithinCircle(const Vector3& vCenter, float fRadius, const Vector3& vPos)
{
	// the circle function expression
	float lefthandside = (vPos.x - vCenter.x) * (vPos.x - vCenter.x) + (vPos.z - vCenter.z) * (vPos.z - vCenter.z);
	if(lefthandside <= fRadius * fRadius)
		return true;
	else
		return false;	
}

UniG_GameObject* UniG_SubMap::FindObject( const ObjTypeKey& type, const Vector3& vCenter, float fRadius, const ObjKey& strObjectName)
{
	int MinColNum = (int)((vCenter.x - fRadius) / m_fXRegionUnit);
	int MaxColNum = (int)((vCenter.x + fRadius) / m_fXRegionUnit);
	int MinRowNum = (int)((vCenter.z + fRadius) / m_fZRegionUnit);
	int MaxRowNum = (int)((vCenter.z - fRadius) / m_fZRegionUnit);

	int i, j;
	for(i=MinRowNum; i<=MaxRowNum; i++)
	{
		for(j=MinColNum; j<=MaxColNum; j++)
		{
			UniG_GameObject* findresult = GetRegion(i,j)->FindObject(strObjectName, type);
            return findresult;
		}
	}
	return NULL;
}

UniG_GameObject* UniG_SubMap::FindObject(const ObjTypeKey& type, const ObjKey& strObjectName)
{
	int i, j;
    for(i=0; i< m_nRowRegionNum; i++)
	{
        for(j=0; j< m_nColRegionNum; j++)
		{
			UniG_GameObject* findresult = GetRegion(i,j)->FindObject(strObjectName, type);
            if( findresult )
                return findresult;
		}
	}
	return NULL;
}

void UniG_SubMap::Area_For_Each( const ObjTypeKey& type, const Vector3& vCenter, float fRadius, void* pParam, void (*pfnCallback)(UniG_GameObject*, void* pParam))
{
	int MinColNum = (int)((vCenter.x - fRadius) / m_fXRegionUnit);
	int MaxColNum = (int)((vCenter.x + fRadius) / m_fXRegionUnit);
	int MinRowNum = (int)((vCenter.z + fRadius) / m_fZRegionUnit);
	int MaxRowNum = (int)((vCenter.z - fRadius) / m_fZRegionUnit);

	int i, j;
	for(i=MinRowNum; i<=MaxRowNum; i++)
	{
		for(j=MinColNum; j<=MaxColNum; j++)
		{
			MapRegion* pR = GetRegion(i, j);
			pR->For_Each(type, pParam, pfnCallback);
		}
	}
}

void UniG_SubMap::Sync_For_Each(const ObjTypeKey& type, void* pParam, void (*pfnCallback)(UniG_GameObject*, void* pParam))
{
	for(int i=0; i < m_nColRegionNum*m_nRowRegionNum; i++)
	{
		m_pRegionArrPtr[i].For_Each(type, pParam, pfnCallback);
	}
}

void UniG_SubMap::Sync_For_Each( const ObjTypeKey& type, const Vector3& vPos, void* pParam, void (*pfnCallback)(UniG_GameObject*, void* pParam) )
{
	MapRegion* pRegion = GetRegion(vPos);
	if( !pRegion )
		return;

	MapRegion* pField = 0;

	for( int i = -MAX_SYNC_REGION_LEVEL_NUM; i <= MAX_SYNC_REGION_LEVEL_NUM; ++i )
		for( int j = -MAX_SYNC_REGION_LEVEL_NUM; j <= MAX_SYNC_REGION_LEVEL_NUM; ++j )
		{
			pField = GetRegion(pRegion->m_nRow + i, pRegion->m_nCol + j );
			pField->For_Each( type, pParam, pfnCallback );
		}
}

void RegionBroadcast(UniG_GameObject* pObj, void* pMsg)
{
    UniG_Player* pPlayer = (UniG_Player*)pObj;
    if( pPlayer && !pPlayer->IsAgent() )
        pPlayer->GetSession()->EnqueMsg((ObjectMessage*)pMsg);
}

void UniG_SubMap::BroadcastMsg( const ObjTypeKey& type, ObjectMessage* pMsg )
{
	for(int i=0; i < m_nColRegionNum*m_nRowRegionNum; i++)
	{
        m_pRegionArrPtr[i].For_Each(type, pMsg, RegionBroadcast);
	}
}

void UniG_SubMap::BroadcastMsg( const ObjTypeKey& type, const Vector3& vPos, ObjectMessage* pMsg )
{
	MapRegion* pRegion = GetRegion(vPos);
	if( !pRegion )
		return;

	MapRegion* pField = 0;

	for( int i = -MAX_SYNC_REGION_LEVEL_NUM; i <= MAX_SYNC_REGION_LEVEL_NUM; ++i )
		for( int j = -MAX_SYNC_REGION_LEVEL_NUM; j <= MAX_SYNC_REGION_LEVEL_NUM; ++j )
		{
			pField = GetRegion(pRegion->m_nRow + i, pRegion->m_nCol + j );
            if( pField )
			    pField->For_Each(type, pMsg, RegionBroadcast);
		}
}

void UniG_SubMap::BroadcastMsg( const ObjTypeKey& type, const Vector3& vPos, ObjectMessage* pMsg, UniG_GameObject* pExclude)
{
	MapRegion* pRegion = GetRegion(vPos);
	if( !pRegion )
		return;

	MapRegion* pField = 0;

	for( int i = -MAX_SYNC_REGION_LEVEL_NUM; i <= MAX_SYNC_REGION_LEVEL_NUM; ++i )
		for( int j = -MAX_SYNC_REGION_LEVEL_NUM; j <= MAX_SYNC_REGION_LEVEL_NUM; ++j )
		{
			pField = GetRegion(pRegion->m_nRow + i, pRegion->m_nCol + j );
            if( pField )
			    pField->For_Each(type, pMsg, RegionBroadcast, pExclude);
		}
}

//
//void UniG_SubMap::NotifyLaterPlayer(const ObjTypeKey& type, ObjectMessage* pMsg)
//{
//    
//}

int UniG_SubMap::GetAroundRegion(MapRegion* pRegion, vector<MapRegion*>& regionList)
{
    int nRegionNum = 0;
    int nIndex = pRegion - m_pRegionArrPtr + 1;
    // if any on the top ?
    if(nIndex > m_nColRegionNum)
    {
        // top
        regionList.push_back(m_pRegionArrPtr + (nIndex - m_nColRegionNum - 1));
        nRegionNum++;
        // left top
        if ( (nIndex - m_nColRegionNum)%m_nColRegionNum != 1)
        {
            regionList.push_back(m_pRegionArrPtr + (nIndex - m_nColRegionNum - 2));
            nRegionNum++;
        }

        // right top
        if ( (nIndex - m_nColRegionNum)%m_nColRegionNum != 0)
        {
            regionList.push_back(m_pRegionArrPtr + (nIndex - m_nColRegionNum ));
            nRegionNum++;
        }
    }
    // if any in the bottom
    if(nIndex <= m_nColRegionNum * (m_nRowRegionNum - 1))
    {
        // bottom
        regionList.push_back(m_pRegionArrPtr + (nIndex + m_nColRegionNum) -1);
        nRegionNum++;

        // left bottom
        if( (nIndex + m_nColRegionNum)%m_nColRegionNum !=1)
        {
            regionList.push_back(m_pRegionArrPtr + (nIndex + m_nColRegionNum - 2));
            nRegionNum++;
        }
        // right bottom
        if((nIndex + m_nColRegionNum)%m_nColRegionNum != 0)
        {
            regionList.push_back(m_pRegionArrPtr + (nIndex + m_nColRegionNum));
            nRegionNum++;
        }
    }

    //left
    if(nIndex%m_nColRegionNum != 1)
    {
        regionList.push_back(m_pRegionArrPtr + nIndex - 2);
        nRegionNum++;
    }
    // right
    if(nIndex%m_nColRegionNum != 0)
    {
        regionList.push_back(m_pRegionArrPtr + nIndex);
        nRegionNum++;
    }
    return nRegionNum;
}

void UniG_SubMap::Update()
{
	// update all objects
	int i;
	
	for(i=0; i<m_nRowRegionNum*m_nColRegionNum; i++)
	{
		m_pRegionArrPtr[i].Update();
	}
}