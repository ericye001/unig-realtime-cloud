/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: UniG_ObjectTools.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 游戏对象工具，用于查找删除等操作

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
 * 游戏对象操作工具类
 */
class ObjectTools
{
public:
    /**
     * 析构函数
     */
    virtual ~ObjectTools();

    /**
     * 取得一个实例
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



