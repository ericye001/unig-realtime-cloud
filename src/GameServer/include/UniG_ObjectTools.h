/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_ObjectTools.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: ��Ϸ���󹤾ߣ����ڲ���ɾ���Ȳ���

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef UNIG_OBJECT_TOOLS_H__
#define UNIG_OBJECT_TOOLS_H__

#define TESTSUMAPREGION 3
/**
 * ��Ϸ�������������
 */
class ObjectTools
{
public:
    /**
     * ��������
     */
    virtual ~ObjectTools();

    /**
     * ȡ��һ��ʵ��
     */
    static ObjectTools* Instance();

    UNIG_GUID AllocateGUID();

    void TransfromRegionID(int nSource, int& nRow, int& nCol);

    int TransfromRegionID(int nRow, int nCol);

private:
    ObjectTools();
    static ObjectTools* m_pInstance;
    static UNIG_GUID m_nCurID;
};
#endif



