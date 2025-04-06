#include "UniG.h"

ObjectTools* ObjectTools::m_pInstance = 0;
UNIG_GUID ObjectTools::m_nCurID = 0;

ObjectTools::ObjectTools()
{

}

ObjectTools::~ObjectTools()
{

}

ObjectTools* ObjectTools::Instance()
{
    if(!m_pInstance)
    {
        m_pInstance = new ObjectTools;
    }
    return m_pInstance;
}

UNIG_GUID ObjectTools::AllocateGUID()
{
    return ++m_nCurID;
}

void ObjectTools::TransfromRegionID(int nSource, int& nRow, int& nCol)
{
    //if(nSource == 0)
    //{
    //    nRow = 0;
    //    nCol = 0;
    //}
    //else if(nSource % TESTSUMAPREGION == 0)
    //{
    //    nRow = nSource/(TESTSUMAPREGION - 1);
    //    nCol = TESTSUMAPREGION - 1;
    //}
    //else
    //{
    //    nRow = nSource/TESTSUMAPREGION;
    //    nCol = (nSource-1)%TESTSUMAPREGION;
    //}
    assert(nSource > 0 && nSource < TESTSUMAPREGION * TESTSUMAPREGION);
    nSource -= 1;
    nRow = nSource / TESTSUMAPREGION;
    nCol = nSource % TESTSUMAPREGION;
}

int ObjectTools::TransfromRegionID(int nRow, int nCol)
{
    return nRow*10 + nCol + 1;
}


