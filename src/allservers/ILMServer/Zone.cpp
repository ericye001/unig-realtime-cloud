#include "Zone.h"
#include "ZoneMgr.h"
/************************************************************************/
/* ���캯��,                                                            */
/* zoneID ��zone ID��ʶ                                                 */
/* mapID  ��zone����map                                                 */
/* isIsland �Ƿ��ǹµ�                                                  */
/************************************************************************/
UniG_ILM_Zone::UniG_ILM_Zone(uint16_t zoneID, uint32_t mapID, bool isIsland)
{
	m_nZoneID = zoneID;
	m_nMapID = mapID;
	m_bIsIsland = isIsland;
	m_nZoneLoad = 0;
	m_nServerID = 0;
	m_bHasInit = false;
}

/************************************************************************/
/* ��������                                                             */
/************************************************************************/
UniG_ILM_Zone::~UniG_ILM_Zone(void)
{

}
/************************************************************************/
/* �õ�����ͼ������ĵ�ͼ��Ԫ                                           */
/************************************************************************/
void UniG_ILM_Zone::GetMapUnit(list<uint32_t>& lstMapUnit)
{
    list<uint32_t>::iterator iter;
    for(iter = m_lstMapUnit.begin(); iter != m_lstMapUnit.end(); iter++)
    {
        lstMapUnit.push_back(*iter);
    }
}

/************************************************************************/
/* ��ӱ���ͼ������ĵ�ͼ��Ԫ                                           */
/************************************************************************/
void UniG_ILM_Zone::AddMapUnit(uint32_t mapID)
{
	m_lstMapUnit.push_back(mapID);
}

/************************************************************************/
/* ��ձ���ͼ������ĵ�ͼ��Ԫ                                           */
/************************************************************************/
void UniG_ILM_Zone::ClearMapUnit()
{
	m_lstMapUnit.clear();
}

/************************************************************************/
/* �ж�һ��zone�ǲ���һ����                                             */
/* ע��,���뱾zone��������server֮������ж��Ƿ�����                    */
/************************************************************************/
bool  UniG_ILM_Zone::IsBridge()
{

 
  //cout<<"Entering  bool  UniG_ILM_Zone::IsBridge() ...."<<endl;
  // ����ǹµ��Ļ��Ͳ�����
  if(IsIsland())
    {
      return false;
    }
  // �õ���ǰServer�ϵ�����mapunit,�����Լ�zone��MapUnit
  GameServerListener* pServer =  ServerManager::instance().findGameServerById(m_nServerID);
  if(pServer == NULL)
    {
      printf("Get server by id failed. ---UniG_ILM_Zone::IsBridge\n");
    }
  list<uint32_t> lstMapUnitOnServer;        // �ڱ�server�ϵ�����zone
  list<uint16_t> lstZoneOnServer;           // �ڱ�server�ϵ�����map Unit
  list<uint16_t>::iterator iterZone; 
  pServer->getZoneButIslandOnServer(lstZoneOnServer); // �õ���server�ϲ��ǹµ�������zone
  for(iterZone = lstZoneOnServer.begin(); iterZone != lstZoneOnServer.end(); iterZone++)
    {
      // cout<<" Server "<< m_nServerID <<" Has Zone " <<  *iterZone << " And Zone "<<   m_nZoneID   <<endl;
      // �Ѳ��Ǳ�zone������map unit�ŵ�������
      if(*iterZone != m_nZoneID)
        {
          UniG_ILM_Zone* pZone ;
          pZone= UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone);
          pZone->GetMapUnit(lstMapUnitOnServer);
        }
    }

  // ���濪ʼ�ŵ��ж�,������lstMapUnitOnServer�𽥲��õ�lstReachableMapUnit��
  // ���ȫ��Ų��lstReachableMapUnit,˵��������
  list<uint32_t> lstReachableMapUnit;
  lstReachableMapUnit.push_back(lstMapUnitOnServer.back());
  lstMapUnitOnServer.pop_back();
  list<uint32_t>::iterator iterAll, iterReach;
  int newReach = 1;
  while(newReach)
    {
      newReach = 0;
      for(iterAll = lstMapUnitOnServer.begin(); iterAll != lstMapUnitOnServer.end(); iterAll++)
        {
          for(iterReach = lstReachableMapUnit.begin(); iterReach != lstReachableMapUnit.end(); iterReach++)
            {
              if(UniG_ILM_Config::Instance()->IsNeighbour(*iterAll, *iterReach))
                {
                  lstReachableMapUnit.push_back(*iterAll);
                  lstMapUnitOnServer.erase(iterAll);
                  newReach++;
                  break;
                }
            }
          if(newReach)
            {
              break;
            }
        }
    }
  
  // �����server�ϵ����е�ͼ��Ԫ(���˱�zone)��ͨ�����ڹ�ϵ����,��ô��zone������
  if(lstMapUnitOnServer.empty())
    {
      //printf("\n%d is NOT Bridge\n",m_nZoneID);
      return false;
    }
  else
    {
      //printf("\n%d is Bridge\n",m_nZoneID);
      return true;
    }
}

/************************************************************************/
/* ��zone�Ƿ�ֻ��һ��mapUnit���                                        */
/************************************************************************/
bool UniG_ILM_Zone::IsSigleMapUnit()
{
	return m_lstMapUnit.size() == 1? true:false;
}


/************************************************************************/
/* �������ڵĸ���zone��״̬                                             */
/************************************************************************/
void UniG_ILM_Zone::LoadNeighbourState()
{
	if(!m_bHasInit)
	{
		return;
	}
	// �����zone����,�����κΰ�Ҵ���,���Բ��ÿ��Ǻ���Χ�����ӹ�ϵ
	if(IsBridge())
	{
		return;
	}
	list<uint32_t> lstNeighbourMapUnit;
	list<uint32_t>::iterator iter;
    // �õ��ͱ�zone��ͼ���ڵ����е�ͼ��Ԫ
	for(iter = m_lstMapUnit.begin(); iter != m_lstMapUnit.end(); iter++)
	{
		UniG_ILM_Config::Instance()->GetNeighbourMapUnit(*iter,lstNeighbourMapUnit);
	}
	// ������Щ���ڵĵ�ͼ��Ԫ,�Դ˵õ���zone���ܱ�����server�����ӽ��̶ܳ�
	// �Ƚ���һ��map,����server id�����ӽ��ܹ�ϵ��ӳ��,Ҳ���Ǻ����Server�ϵ�mapUnit
	// ��������Խ��,�ʹ�������Խ����
	map<uint16_t, uint32_t> mapServerAdjoining;     // serverID�������ӳ̶ȵ�ӳ��
	map<uint16_t, uint32_t>::iterator itermap;
    // ������Щ���ڵ�MapUnit
	for(iter = lstNeighbourMapUnit.begin(); iter != lstNeighbourMapUnit.end(); iter++)
	{

         
		// ��Ҫ���Լ�zone���mapunit�Ž�ȥ
		if(find(m_lstMapUnit.begin(), m_lstMapUnit.end(),*iter) != m_lstMapUnit.end())
		{
			continue;
		}
        // �õ�map unit����zone
		uint16_t zoneID = UniG_ILM_Config::Instance()->GetZoneIDByMapUnitID(*iter);
        // �õ� zone��serverID
		uint16_t serverID = UniG_ILM_ZoneMgr::Instance()->GetServerIDByZoneID(zoneID);
		itermap = mapServerAdjoining.find(serverID);
		if(itermap == mapServerAdjoining.end())
		{
			mapServerAdjoining.insert(pair<uint16_t,uint32_t>(serverID,1));
		}
		else
		{
			(itermap->second)++;
		}
	}
	// ����NeighbourState����,��Plan��ע��,��server�Ĳ��ð�������,��Ϊ������ǿ���һ��server���ܱߵ�server��
	for(itermap = mapServerAdjoining.begin(); itermap != mapServerAdjoining.end(); itermap++)
	{
		if(itermap->first == m_nServerID)
		{
			continue;
		}
		NeighbourState temp;
		temp.neighbourServerID = itermap->first;
		temp.junctureLevel = itermap->second;
		temp.neighbourServerLoad = ServerManager::instance().getServerLoad(itermap->first);
		temp.zoneID = GetZoneID();
		temp.zoneLoad = GetZoneLoad();
		temp.serverID = GetServerID();
		//UniG_ILM_PlanMgr::Instance()->AddNeighbour(temp);
	}
}

/************************************************************************/
/* �õ���zone�ĸ���                                                     */
/************************************************************************/
uint32_t UniG_ILM_Zone::GetZoneLoad()
{
	return m_nZoneLoad;
}

/************************************************************************/
/* ���ñ�zone����                                                       */
/************************************************************************/
void UniG_ILM_Zone::SetZoneLoad(uint32_t load)
{
    m_nZoneLoad = load;
}

/************************************************************************/
/* �õ���zone ID                                                        */
/************************************************************************/
uint16_t UniG_ILM_Zone::GetZoneID()
{
	return m_nZoneID;
}

/************************************************************************/
/* ���ñ�zone����server ID                                              */
/************************************************************************/
void UniG_ILM_Zone::SetServerID(uint16_t serverID)
{
	m_nServerID = serverID;
	m_bHasInit = true;
}

/************************************************************************/
/* �õ���zone����server                                                 */
/************************************************************************/
uint16_t UniG_ILM_Zone::GetServerID()
{
	return m_nServerID;
}


/************************************************************************/
/* �õ���zone�Ƿ��ǹµ�                                                 */
/************************************************************************/
bool   UniG_ILM_Zone::IsIsland()
{
	return m_bIsIsland;
}





