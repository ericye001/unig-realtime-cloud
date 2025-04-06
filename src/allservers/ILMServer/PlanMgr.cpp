
#include "PlanMgr.h"


#include "GameServerListener.h"
#include "GlobalArgs.h"
#include "ServerManager.h"
#include "Global.h"
#include "Config.h"

UniG_ILM_PlanMgr* UniG_ILM_PlanMgr::m_pInstance = NULL;

/************************************************************************/
/* ���캯��                                                             */
/************************************************************************/
UniG_ILM_PlanMgr::UniG_ILM_PlanMgr(void)
{
}

/************************************************************************/
/* ��������                                                             */
/************************************************************************/

UniG_ILM_PlanMgr::~UniG_ILM_PlanMgr(void)
{

}

/************************************************************************/
/* �õ�������ʵ��                                                       */
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
/* ���һ�����ڹ�ϵ                                                     */
/************************************************************************/
void UniG_ILM_PlanMgr::AddNeighbour(NeighbourState state)
{
	m_lstAllNeighbour.push_back(state);
}

/************************************************************************/
/* ������ڹ�ϵ                                                         */
/************************************************************************/
void UniG_ILM_PlanMgr::ClearNeighbourList()
{
	m_lstAllNeighbour.clear();
}

/************************************************************************/
/* �õ��뵱ǰServer�����ص�������Ϣ                                   */
/* ����Ҫ����������ỹ�������                                         */
/************************************************************************/
bool UniG_ILM_PlanMgr::GetCurServerNeighbour(uint16_t serverID, bool out)
{
	list<NeighbourState>::iterator iter;
	m_lstCurServerNeighbour.clear();
	for(iter = m_lstAllNeighbour.begin(); iter != m_lstAllNeighbour.end(); iter++)
	{
		// �����ͼ���ٱ�server����Χ��Ҫɾ��,Ŀ��server���Ϸ�ҲҪɾ��
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
/* �γ���server���ļƻ�                                               */
/************************************************************************/
int32_t UniG_ILM_PlanMgr::FormIdlePlanList(uint16_t serverID)
{
  //printf("������server�ܱ�æ��server�ĵ�ͼ\n");
  //printf("Another Server'Maps\n");	
  //ͬ�������Server
  list<uint16_t> lstServer;
  list<uint16_t>::iterator iterServer;
  ServerManager::instance().getServersInSameGroup(serverID, lstServer);
     
  if(lstServer.empty() || lstServer.size() == 1)
    {
      // printf("ͬ��û������server\n");
      //printf("No other server in same group\n");
      return NO_OTHER_SERVER_IN_GROUP;
    }
	// �ҵ�������С��Server
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
    // �γɰ�ҷ���
	if(!GetCurServerNeighbour(minLoadServerID, false) || m_lstCurServerNeighbour.empty())
	{
		//printf("û�б�ѡ����\n");
		printf("No Plan To be Made\n");
		return NO_CANDIDATE_PLAN;
	}
	return 0;
}

/************************************************************************/
/* �γ���æΪ���İ�Ҽƻ����Լ��Ĺ�Ͻ��ͼ������ܱߵ�Server             */
/************************************************************************/
int32_t UniG_ILM_PlanMgr::FormBusyPlanList(uint16_t serverID)
{
	bool flag = true;
	list<NeighbourState>::iterator iter;
	//ͬ�������Server
	list<uint16_t> lstServer;
	list<uint16_t>::iterator iterServer;
	ServerManager::instance().getServersInSameGroup(serverID, lstServer);
	if(lstServer.empty() || lstServer.size() == 1)
	{
		// ͬ��û������server
		//printf("û������server�ͱ�serverͬ��\n");
	        //printf("No other server in same group\n");
		return NO_OTHER_SERVER_IN_GROUP;
    }
	
	// �õ���ǰserver�ɰ�ҵı�ѡ����
	if(!GetCurServerNeighbour(serverID))
	{
		//printf("û�б�ѡ����\n");
		printf("No Plan To be Made\n");
		return NO_CANDIDATE_PLAN;
	}
	if(ServerManager::instance().hasIdleServer())
	{
        // ����п���server, ��ѵ�ͼ��Ŀ��server���óɿ���server
		for(iter = m_lstCurServerNeighbour.begin(); iter != m_lstCurServerNeighbour.end(); iter++)
		{
                  iter->neighbourServerID = ServerManager::instance().getIdleServer();
                  iter->neighbourServerLoad = 0;
                  iter->junctureLevel = MAX_JUNCTURE_LEVEL;
		}
		//printf("�п���Server\n");
		printf("Has one Ideal Server\n");
		return HAS_IDLE_SERVER;
	}
	// û�п���server�����,Ҫ��Ŀ��server�Ƿ����
	flag = true;
	while(flag)
	{
		if(m_lstCurServerNeighbour.empty())
		{
			break;
		}
		for(iter = m_lstCurServerNeighbour.begin(); iter != m_lstCurServerNeighbour.end(); )
		{
			// Ŀ��server���Ϸ�ҲҪɾ��(Ŀ��serverӦ����ͬ��server��, Ŀ��server����+��zone���ز�Ӧ�ô��ڵ�ǰserver����)
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
		//printf("���е�Server�����ܽ�����Ƭ��ͼ\n");
		printf(" No Ideal Server can accept this Map\n");
		return NO_CANDIDATE_PLAN;
	}
	return 0;

}


/************************************************************************/
/* ѡ��һ����ҷ���                                                     */
/************************************************************************/
bool UniG_ILM_PlanMgr::ChoosePlan(uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer)
{
	list<NeighbourState>::iterator iter;
	if(m_lstCurServerNeighbour.size() <= 0)
	{
		return false;
	}
	// ����Ҫ������ôȨ��,�ҵ�һ�����ʵ�server������
	else if(m_lstCurServerNeighbour.size() == 1)
          {
            zoneID = m_lstCurServerNeighbour.front().zoneID;
            fromServer = m_lstCurServerNeighbour.front().serverID;
            toServer = m_lstCurServerNeighbour.front().neighbourServerID;
            return true;
          }
	else            // ����������ܵİ��
          {
            uint32_t maxJunctureLevel = 1;
            list<NeighbourState>::iterator iterResult = m_lstCurServerNeighbour.begin();
            for(iter = m_lstCurServerNeighbour.begin(); iter != m_lstCurServerNeighbour.end(); iter++)
              {
                // �����п���server�����
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
/* ��ӡ��Ҽƻ��б�                                                      */
/*************************************************************************/
void UniG_ILM_PlanMgr::PrintPlanList()
{
  list<NeighbourState>::iterator iter;
  for(iter = m_lstCurServerNeighbour.begin(); iter != m_lstCurServerNeighbour.end(); iter++)
    {
      //printf("����ServerID\t���ӳ̶�\t����Server����\t��ZoneID\t��Zone����\n");
     // printf("%12d\t%8d\t%14d\t%8d\t%10d\n", iter->neighbourServerID, iter->junctureLevel, iter->neighbourServerLoad, iter->zoneID, iter->zoneLoad);
    }
}

/************************************************************************/
/* ָ����Ҽƻ�����ں���                                               */
/************************************************************************/
bool UniG_ILM_PlanMgr::MakePlan(uint16_t serverID, uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer)
{
  // ����µ���ҳɹ����򱾴μƻ��ɹ�
  if(IslandMigration(serverID, zoneID, fromServer, toServer))
    {
      //printf("�µ���ҷ���\n");
      //printf("IslandMigration\n");
      return  true;
    }
  // ����zone��Ϣ
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
/* �Ƿ�Ϊ�µ���ҷ���,�µ���ҷ�������                                  */
/************************************************************************/
bool UniG_ILM_PlanMgr::IslandMigration(uint16_t serverID, uint16_t& zoneID, uint16_t& fromServer, uint16_t& toServer)
{
  // �õ������server
  GameServerListener* pServer = ServerManager::instance().findGameServerById(serverID);
  GameServerListener* pMinLoadServer;
	if(pServer)
          {
            // �õ���server����Ĺµ�
            list<uint16_t> lstIsland;
            list<uint16_t>::iterator iterIsland;
            pServer->getIsland(lstIsland);
            uint16_t minLoadIsland=0;               // ��������Ĺµ�
            UniG_ILM_Zone* pMinLoadZone;
            uint32_t minLoad=0;
            // ������server����������һ���µ�

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
            // ���û�п��� server,��һ���Ƚ��е�server��
            pMinLoadServer = ServerManager::instance().getMinLoadServer();
            if(pMinLoadServer)
              {
                // ������е�server��������µ��ĸ��ر�ԭserver���ػ��󣬲���
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
