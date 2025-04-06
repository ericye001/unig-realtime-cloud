
#include "PlanMgr.h"


#include "GameServerListener.h"
#include "GlobalArgs.h"
#include "ServerManager.h"
#include "Global.h"
#include "Config.h"

UniG_ILM_PlanMgr* UniG_ILM_PlanMgr::m_pInstance = NULL;

/************************************************************************/
/* 构造函数                                                             */
/************************************************************************/
UniG_ILM_PlanMgr::UniG_ILM_PlanMgr(void)
{
}

/************************************************************************/
/* 析构函数                                                             */
/************************************************************************/

UniG_ILM_PlanMgr::~UniG_ILM_PlanMgr(void)
{

}

/************************************************************************/
/* 得到本类所实例                                                       */
/************************************************************************/
UniG_ILM_PlanMgr* UniG_ILM_PlanMgr::Instance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new UniG_ILM_PlanMgr();
	}
	return m_pInstance;
}

/************************************************************************/
/* 添加一个相邻关系                                                     */
/************************************************************************/
void UniG_ILM_PlanMgr::AddNeighbour(NeighbourState state)
{
	m_lstAllNeighbour.push_back(state);
}

/************************************************************************/
/* 清空相邻关系                                                         */
/************************************************************************/
void UniG_ILM_PlanMgr::ClearNeighbourList()
{
	m_lstAllNeighbour.clear();
}

/************************************************************************/
/* 得到与当前Server搬家相关的相邻信息                                   */
/* 这里要区分是向外搬还是向里搬                                         */
/************************************************************************/
bool UniG_ILM_PlanMgr::GetCurServerNeighbour(uint16_t serverID, bool out)
{
	list<NeighbourState>::iterator iter;
	m_lstCurServerNeighbour.clear();
	for(iter = m_lstAllNeighbour.begin(); iter != m_lstAllNeighbour.end(); iter++)
	{
		// 被搬地图不再本server管理范围内要删掉,目的server不合法也要删掉
		if(iter->serverID == serverID && out )
		{
			m_lstCurServerNeighbour.push_back(*iter);
		}
		else if(iter->neighbourServerID == serverID && !out )
		{
			m_lstCurServerNeighbour.push_back(*iter);
		}
	}
	if(m_lstCurServerNeighbour.empty())
	{
		return false;
	}
	return true;
}



/************************************************************************/
/* 形成闲server向搬的计划                                               */
/************************************************************************/
int32_t UniG_ILM_PlanMgr::FormIdlePlanList(uint16_t serverID)
{
  //printf("考虑闲server周边忙的server的地图\n");
  //printf("Another Server'Maps\n");	
  //同组的其他Server
  list<uint16_t> lstServer;
  list<uint16_t>::iterator iterServer;
  ServerManager::instance().getServersInSameGroup(serverID, lstServer);
     
  if(lstServer.empty() || lstServer.size() == 1)
    {
      // printf("同组没有其他server\n");
      //printf("No other server in same group\n");
      return NO_OTHER_SERVER_IN_GROUP;
    }
	// 找到负担最小的Server
	list<uint16_t>::iterator iter;
	uint16_t minLoadServerID = lstServer.front();

	GameServerListener* minLoadServer =  ServerManager::instance().findGameServerById(minLoadServerID);

        if (minLoadServer == NULL)
          {
            return -1;
          }

	uint32_t minLoad = minLoadServer->playerNum();
	lstServer.pop_front();
	for(iter = lstServer.begin(); iter != lstServer.end(); iter++)
	{
		GameServerListener* pCurServer = ServerManager::instance().findGameServerById(*iter);
		if(pCurServer)
		{
			uint32_t curLoad = pCurServer->playerNum();
			if(curLoad < minLoad)
			{
				minLoad = curLoad;
				minLoadServer =pCurServer;
				minLoadServerID = pCurServer->srvId();
			}
		}
	}
    // 形成搬家方案
	if(!GetCurServerNeighbour(minLoadServerID, false) || m_lstCurServerNeighbour.empty())
	{
		//printf("没有备选方案\n");
		printf("No Plan To be Made\n");
		return NO_CANDIDATE_PLAN;
	}
	return 0;
}

/************************************************************************/
/* 形成以忙为中心搬家计划，自己的管辖地图分配给周边的Server             */
/************************************************************************/
int32_t UniG_ILM_PlanMgr::FormBusyPlanList(uint16_t serverID)
{
	bool flag = true;
	list<NeighbourState>::iterator iter;
	//同组的其他Server
	list<uint16_t> lstServer;
	list<uint16_t>::iterator iterServer;
	ServerManager::instance().getServersInSameGroup(serverID, lstServer);
	if(lstServer.empty() || lstServer.size() == 1)
	{
		// 同组没有其他server
		//printf("没有其他server和本server同组\n");
	        //printf("No other server in same group\n");
		return NO_OTHER_SERVER_IN_GROUP;
    }
	
	// 得到当前server可搬家的备选方案
	if(!GetCurServerNeighbour(serverID))
	{
		//printf("没有被选方案\n");
		printf("No Plan To be Made\n");
		return NO_CANDIDATE_PLAN;
	}
	if(ServerManager::instance().hasIdleServer())
	{
        // 如果有空闲server, 则把地图的目的server设置成空闲server
		for(iter = m_lstCurServerNeighbour.begin(); iter != m_lstCurServerNeighbour.end(); iter++)
		{
                  iter->neighbourServerID = ServerManager::instance().getIdleServer();
                  iter->neighbourServerLoad = 0;
                  iter->junctureLevel = MAX_JUNCTURE_LEVEL;
		}
		//printf("有空闲Server\n");
		printf("Has one Ideal Server\n");
		return HAS_IDLE_SERVER;
	}
	// 没有空闲server的情况,要看目的server是否合适
	flag = true;
	while(flag)
	{
		if(m_lstCurServerNeighbour.empty())
		{
			break;
		}
		for(iter = m_lstCurServerNeighbour.begin(); iter != m_lstCurServerNeighbour.end(); )
		{
			// 目的server不合法也要删掉(目的server应该在同组server内, 目的server负载+本zone负载不应该大于当前server负载)
                  if(find(lstServer.begin(), lstServer.end(), iter->neighbourServerID) == lstServer.end() 
                     || (iter->zoneLoad + iter->neighbourServerLoad) > (ServerManager::instance().getServerLoad(iter->serverID))
                     || iter->neighbourServerLoad >CommonArgs_T::instance()._maxServerLoad)
			{
				m_lstCurServerNeighbour.erase(iter);
				break;
			}
			else
			{
				iter++;
				if(iter == m_lstCurServerNeighbour.end())
				{
					flag = false;
					break;
				}
			}
		}
	}

	if(m_lstCurServerNeighbour.empty())
	{
		//printf("空闲的Server都不能接受这片地图\n");
		printf(" No Ideal Server can accept this Map\n");
		return NO_CANDIDATE_PLAN;
	}
	return 0;

}


/************************************************************************/
/* 选择一个搬家方案                                                     */
/************************************************************************/
bool UniG_ILM_PlanMgr::ChoosePlan(uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer)
{
	list<NeighbourState>::iterator iter;
	if(m_lstCurServerNeighbour.size() <= 0)
	{
		return false;
	}
	// 这里要加上怎么权衡,找到一个合适的server接受他
	else if(m_lstCurServerNeighbour.size() == 1)
          {
            zoneID = m_lstCurServerNeighbour.front().zoneID;
            fromServer = m_lstCurServerNeighbour.front().serverID;
            toServer = m_lstCurServerNeighbour.front().neighbourServerID;
            return true;
          }
	else            // 找连接最紧密的搬家
          {
            uint32_t maxJunctureLevel = 1;
            list<NeighbourState>::iterator iterResult = m_lstCurServerNeighbour.begin();
            for(iter = m_lstCurServerNeighbour.begin(); iter != m_lstCurServerNeighbour.end(); iter++)
              {
                // 属于有空闲server的情况
                if (iter->junctureLevel == MAX_JUNCTURE_LEVEL)
                  {
                      iterResult = iter;
                      break;
                  }
                if(iter->junctureLevel > maxJunctureLevel)
                  {
                    maxJunctureLevel = iter->junctureLevel;
                    iterResult = iter;
                  }
              }
            zoneID = iterResult->zoneID;
            fromServer = iterResult->serverID;
            toServer = iterResult->neighbourServerID;
          }
	return true;
	
}/************************************************************************/
/* 打印搬家计划列表                                                      */
/*************************************************************************/
void UniG_ILM_PlanMgr::PrintPlanList()
{
  list<NeighbourState>::iterator iter;
  for(iter = m_lstCurServerNeighbour.begin(); iter != m_lstCurServerNeighbour.end(); iter++)
    {
      //printf("相邻ServerID\t连接程度\t相邻Server负载\t本ZoneID\t本Zone负载\n");
     // printf("%12d\t%8d\t%14d\t%8d\t%10d\n", iter->neighbourServerID, iter->junctureLevel, iter->neighbourServerLoad, iter->zoneID, iter->zoneLoad);
    }
}

/************************************************************************/
/* 指定搬家计划的入口函数                                               */
/************************************************************************/
bool UniG_ILM_PlanMgr::MakePlan(uint16_t serverID, uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer)
{
  // 如果孤岛搬家成功，则本次计划成功
  if(IslandMigration(serverID, zoneID, fromServer, toServer))
    {
      //printf("孤岛搬家方案\n");
      //printf("IslandMigration\n");
      return  true;
    }
  // 更新zone信息
  UniG_ILM_ZoneMgr::Instance()->UpdateZoneInfo();
  //UniG_ILM_PlanMgr::Instance()->PrintPlanList();
  if(FormBusyPlanList(serverID) == 0)
    {
      ChoosePlan(zoneID, fromServer, toServer);
      return true;
    }
  else
    {
      if(FormIdlePlanList(serverID) == 0)
        {
          ChoosePlan(zoneID, fromServer, toServer);
          return true;
        }
      else
        {
          return false;
        }
    }
}

/************************************************************************/
/* 是否为孤岛搬家方案,孤岛搬家方案优先                                  */
/************************************************************************/
bool UniG_ILM_PlanMgr::IslandMigration(uint16_t serverID, uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer)
{
  // 得到待搬家server
  GameServerListener* pServer = ServerManager::instance().findGameServerById(serverID);
  GameServerListener* pMinLoadServer;
	if(pServer)
          {
            // 得到本server管理的孤岛
            list<uint16_t> lstIsland;
            list<uint16_t>::iterator iterIsland;
            pServer->getIsland(lstIsland);
            uint16_t minLoadIsland=0;               // 负载最轻的孤岛
            UniG_ILM_Zone* pMinLoadZone;
            uint32_t minLoad=0;
            // 如果这个server管理了至少一个孤岛

            if(lstIsland.size() == 0)
              {
                 pServer->getZoneOnServer(lstIsland);

              }
              

            if(lstIsland.size() > 0)
              {
                minLoadIsland = lstIsland.front();
                pMinLoadZone = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(minLoadIsland);
                if (pMinLoadZone == NULL)
                  {
                    return false;
                  }
                minLoad = pMinLoadZone->GetZoneLoad();
                for (iterIsland = lstIsland.begin(); iterIsland != lstIsland.end(); iterIsland++)
                  {
                    UniG_ILM_Zone* pZone = UniG_ILM_ZoneMgr::Instance()->GetZoneByID(*iterIsland);
                    if (pZone)
                      {
                        if (pZone->GetZoneLoad() < minLoad)
                          {
                            minLoad = pZone->GetZoneLoad();
                            pMinLoadZone = pZone;
                          }
                      }
                  }
              }

            
            if( ServerManager::instance().hasIdleServer())
              {
                zoneID = pMinLoadZone->GetZoneID();
                fromServer = pServer->srvId();
                toServer  =  ServerManager::instance().getIdleServer();
                return true;
              }
            // 如果没有空闲 server,找一个比较闲的server吧
            pMinLoadServer = ServerManager::instance().getMinLoadServer();
            if(pMinLoadServer)
              {
                // 如果最闲的server加上这个孤岛的负载比原server负载还大，不行
                if(pMinLoadServer->playerNum()+pMinLoadZone->GetZoneLoad() >= pServer->playerNum())
                  {
                    return false;
                  }
                zoneID = pMinLoadZone->GetZoneID();
                fromServer = pServer->srvId();
                toServer = pMinLoadServer->srvId();
                return true;
              }
          }
	return false;
}
