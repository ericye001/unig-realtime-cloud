/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: MSTInfo.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: MST����Ϣ

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _MST_INFO_H__
#define _MST_INFO_H__

/** 
 * MST����Ϣ���������е�ͼ��Ϣ��Server��Ϣ�Լ���ͼ��Serverӳ����Ϣ
 */
class MSTInfo
{
public:
    /** 
     * ʵ��������һ������
     */
    static MSTInfo& Instance();
    
    /**
     * ȡ�ñ�Serve��MST��Ϣ
     * @return MST��Ϣ
     */
    vector<Map2SrvInfo>& AllMST();

    /**
     * ����MST��Ϣ
     * @param mst MST��Ϣ����ILMServer�����ȡ��
     */
    void AllMST(vector<Map2SrvInfo>& mst);
    
    /**
     * ȡ������GameServer����Ϣ
     */
    vector<ServerInfo>& AllSrvs();

    /**
     * �Ѵ�ILM Server�����ȡ����server�б���Ϣ�����ڳ�Ա������
     * @param allSrvs server��Ϣ����
     */
    void AllSrvs(vector<ServerInfo>& allSrvs);

    /**
     * �õ����е�map��Ϣ
     */
    vector<MapInfo>& AllMaps();

    /**
     * �Ѵ�ILM Server�����ȡ����map�б���Ϣ�����ڳ�Ա������
     * @param allMaps ��ͼ��Ϣ�б�
     */
    void AllMaps(vector<MapInfo>& allMaps);
    
    /**
     * ��������ȡ��mapID
     * @param pName ��ͼ����
     * @return ��ͼid
     */
    uint16 MapID(const char* pName);

    /**
     * ȡ��һ��map��Ӧ��server
     * @param nMapID ��ͼid
     * @return server id
     */
    uint16 MapOwner(uint16 nMapId);
    
    /**
     * ����map id �õ�MSTӳ����Ϣ
     * @param nMapID ��ͼid
     * @return һ��ӳ����Ϣ
     */
    Map2SrvInfo& GetMSTPairByMap(uint16 nMapId);

    /**
     * ����ServerID�õ�server ��Ϣ
     * @nSrvID server id
     * @return server��Ϣ
     */
    ServerInfo& GetServerInfo(uint16 nSrvId);

    /**
     * ����Map ID�õ�map��Ϣ
     * @nMapID map id
     * @return map��Ϣ
     */
    MapInfo& GetMapInfo(uint16 nMapId);
    
private:
    MSTInfo();
    
private:
    static MSTInfo* m_pInstance;
    
    // global MST
    vector<Map2SrvInfo> m_vecAllMST;

    // all servers' infomation
    vector<ServerInfo> m_vecAllSrvs;

    // all maps's infomation
    vector<MapInfo> m_vecAllMaps;
};


#endif

