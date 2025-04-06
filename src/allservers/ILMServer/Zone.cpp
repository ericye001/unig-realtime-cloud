#include "Zone.h"
#include "ZoneMgr.h"
/************************************************************************/
/* 构造函数,                                                            */
/* zoneID 本zone ID标识                                                 */
/* mapID  本zone所属map                                                 */
/* isIsland 是否是孤岛                                                  */
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
/* 析构函数                                                             */
/************************************************************************/
UniG_ILM_Zone::~UniG_ILM_Zone(void)
{

}
/************************************************************************/
/* 得到本地图所管理的地图单元                                           */
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
/* 添加本地图所管理的地图单元                                           */
/************************************************************************/
void UniG_ILM_Zone::AddMapUnit(uint32_t mapID)
{
	m_lstMapUnit.push_back(mapID);
}

/************************************************************************/
/* 清空本地图所管理的地图单元                                           */
/************************************************************************/
void UniG_ILM_Zone::ClearMapUnit()
{
	m_lstMapUnit.clear();
}

/************************************************************************/
/* 判断一个zone是不是一个桥                                             */
/* 注意,必须本zone设置所属server之后才能判断是否是桥                    */
/************************************************************************/
bool  UniG_ILM_Zone::IsBridge()
{

 
  //cout<<"Entering  bool  UniG_ILM_Zone::IsBridge() ...."<<endl;
  // 如果是孤岛的话就不是桥
  if(IsIsland())
    {
      return false;
    }
  // 得到当前Server上的所有mapunit,除了自己zone的MapUnit
  GameServerListener* pServer =  ServerManager::instance().findGameServerById(m_nServerID);
  if(pServer == NULL)
    {
      printf("Get server by id failed. ---UniG_ILM_Zone::IsBridge\n");
    }
  list<uint32_t> lstMapUnitOnServer;        // 在本server上的所有zone
  list<uint16_t> lstZoneOnServer;           // 在本server上的所有map Unit
  list<uint16_t>::iterator iterZone; 
  pServer->getZoneButIslandOnServer(lstZoneOnServer); // 得到本server上不是孤岛的所有zone
  for(iterZone = lstZoneOnServer.begin(); iterZone != lstZoneOnServer.end(); iterZone++)
    {
      // cout<<" Server "<< m_nServerID <<" Has Zone " <<  *iterZone << " And Zone "<<   m_nZoneID   <<endl;
      // 把不是本zone的所有map unit放到链表里
      if(*iterZone != m_nZoneID)
        {
          UniG_ILM_Zone* pZone ;
          pZone= UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterZone);
          pZone->GetMapUnit(lstMapUnitOnServer);
        }
    }

  // 下面开始桥的判断,我们拿lstMapUnitOnServer逐渐拆开拿到lstReachableMapUnit里
  // 如果全能挪倒lstReachableMapUnit,说明不是桥
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
  
  // 如果本server上的所有地图单元(除了本zone)能通过相邻关系到达,那么本zone不是桥
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
/* 本zone是否只由一个mapUnit组成                                        */
/************************************************************************/
bool UniG_ILM_Zone::IsSigleMapUnit()
{
	return m_lstMapUnit.size() == 1? true:false;
}


/************************************************************************/
/* 加载相邻的各个zone的状态                                             */
/************************************************************************/
void UniG_ILM_Zone::LoadNeighbourState()
{
	if(!m_bHasInit)
	{
		return;
	}
	// 如果本zone是桥,则不做任何搬家处理,所以不用考虑和周围的连接关系
	if(IsBridge())
	{
		return;
	}
	list<uint32_t> lstNeighbourMapUnit;
	list<uint32_t>::iterator iter;
    // 得到和本zone地图相邻的所有地图单元
	for(iter = m_lstMapUnit.begin(); iter != m_lstMapUnit.end(); iter++)
	{
		UniG_ILM_Config::Instance()->GetNeighbourMapUnit(*iter,lstNeighbourMapUnit);
	}
	// 遍历这些相邻的地图单元,以此得到本zone和周边其他server的连接紧密程度
	// 先建立一个map,保存server id和连接紧密关系的映射,也就是和这个Server上的mapUnit
	// 相连个数越多,就代表连接越紧密
	map<uint16_t, uint32_t> mapServerAdjoining;     // serverID和其连接程度的映射
	map<uint16_t, uint32_t>::iterator itermap;
    // 遍历这些相邻的MapUnit
	for(iter = lstNeighbourMapUnit.begin(); iter != lstNeighbourMapUnit.end(); iter++)
	{

         
		// 不要把自己zone里的mapunit放进去
		if(find(m_lstMapUnit.begin(), m_lstMapUnit.end(),*iter) != m_lstMapUnit.end())
		{
			continue;
		}
        // 得到map unit所在zone
		uint16_t zoneID = UniG_ILM_Config::Instance()->GetZoneIDByMapUnitID(*iter);
        // 得到 zone的serverID
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
	// 构造NeighbourState对象,向Plan里注册,本server的不用包含在内,因为搬家总是考虑一个server往周边的server搬
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
/* 得到本zone的负载                                                     */
/************************************************************************/
uint32_t UniG_ILM_Zone::GetZoneLoad()
{
	return m_nZoneLoad;
}

/************************************************************************/
/* 设置本zone负载                                                       */
/************************************************************************/
void UniG_ILM_Zone::SetZoneLoad(uint32_t load)
{
    m_nZoneLoad = load;
}

/************************************************************************/
/* 得到本zone ID                                                        */
/************************************************************************/
uint16_t UniG_ILM_Zone::GetZoneID()
{
	return m_nZoneID;
}

/************************************************************************/
/* 设置本zone所属server ID                                              */
/************************************************************************/
void UniG_ILM_Zone::SetServerID(uint16_t serverID)
{
	m_nServerID = serverID;
	m_bHasInit = true;
}

/************************************************************************/
/* 得到本zone所属server                                                 */
/************************************************************************/
uint16_t UniG_ILM_Zone::GetServerID()
{
	return m_nServerID;
}


/************************************************************************/
/* 得到本zone是否是孤岛                                                 */
/************************************************************************/
bool   UniG_ILM_Zone::IsIsland()
{
	return m_bIsIsland;
}





