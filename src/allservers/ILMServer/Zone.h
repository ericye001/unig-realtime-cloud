#ifndef _UNIG_ILM_ZONE_H__
#define _UNIG_ILM_ZONE_H__
#include "GameServerListener.h"
#include "ServerManager.h"
#include "Config.h"

// һ��zone�����ٽ�Server�Ĺ�ϵ
struct NeighbourState
{
	uint16_t neighbourServerID;               // ����serverID
	uint32_t neighbourServerLoad;             // ����server����
	uint8_t  junctureLevel;                   // ���ӳ̶�,�������zone�����ڵ�ͼ�м���������
	uint16_t zoneID;                          // ��zone ID
	uint32_t zoneLoad;                        // ��zone ����
	uint16_t serverID;                        // ��zone���� server
};

// zone��
class UniG_ILM_Zone
{
private:
	list<uint32_t> m_lstMapUnit;              // ��zone�����ĵ�ͼ��Ԫ
	uint16_t m_nZoneID;                       // ��zoneID
	uint32_t m_nMapID;                        // ��zone������map
	uint16_t m_nServerID;                     // ��zone������server,һ��zoneֻ������һ��server
	bool   m_bIsIsland;                     // �Ƿ��ǹµ�
	uint32_t m_nZoneLoad;                     // ��zone����
	bool   m_bHasInit;                      // ��zone�Ƿ��ʼ����
private:
	bool IsBridge();                        // �Ƿ�����
public:
    // ���캯��
	UniG_ILM_Zone(uint16_t zoneID, uint32_t mapID, bool isIsland);

    // ��������
	~UniG_ILM_Zone(void);

    // ��ӱ�zone������ĵ�ͼ��Ԫ
	void AddMapUnit(uint32_t mapID);

    // �õ�����ͼ������ĵ�ͼ��Ԫ
        void GetMapUnit(list<uint32_t>& lstMapUnit);

    // ����zone�������ĵ�ͼ��Ԫ
	void ClearMapUnit();

    // ��zone�Ƿ�ֻ��һ��mapUnit���
	bool IsSigleMapUnit();

    // �������ڵĸ���zone��״̬
	void LoadNeighbourState();

    // �õ�zoneID
	uint16_t GetZoneID();

    // �õ�zone����
	uint32_t GetZoneLoad();

    // ���ñ�zone����
        void SetZoneLoad(uint32_t load);

    // �õ���zone��˭����
	uint16_t GetServerID();

    // ���ñ�zone��server ID
        void SetServerID(uint16_t serverID);

    //�Ƿ��ǹµ�
	bool   IsIsland();
};

#endif
