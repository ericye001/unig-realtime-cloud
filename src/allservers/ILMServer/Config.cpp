#include <fstream>
#include <cstdlib>

#include "Global.h"
#include "Util.h"
#include "Config.h"

UniG_ILM_Config* UniG_ILM_Config::m_pInstance = NULL;

UniG_ILM_Config::UniG_ILM_Config(void)
{
}

UniG_ILM_Config::~UniG_ILM_Config(void)
{
}
/****************************************************************************************
 * 从配置文件里加载MapUnit信息,包括其所属zoneid,mapid,serverid还有名字
 * 存放在成员变量m_vecMapUnit里
 * 
 * 
 * 
 * 
 * 
 * 
 ***************************************************************************************/
bool UniG_ILM_Config::LoadConfigFile(std::string filePath)
{
  ifstream configFile;
  char buf[200];
  int count = 200;
  memset(buf, 0 ,200);
  if( filePath.empty() )
    {
      printf("shit!!\n");
      return false;
    }
  configFile.rdbuf()->open(filePath.c_str(), ios::in);
  if(!configFile)
    {
      printf("Open IVWLMM config file error,maybe the file doesn't exist\n");
      return false;
    }

  vector<string> vecToken;
  //MapUnitInfo tempMapUnit;
  uint32_t curLine = 0;
  while(configFile.getline(buf, count))
    {
      bool badLine = false;
      curLine++;
      // 被注释行或者标题行
      if(Util::StringFind((string)buf,"MapUnitID") || Util::StringFind((string)buf,"#"))
        {
          memset(buf, 0 ,200);
          continue;
        }
      // MapTile信息行
      else if(Util::StringFind((string)buf,"[map"))
        {
          MapTile tempMapTile;
          int pos = 0;
          while(buf[pos])
            {
              if(buf[pos] == ']' || buf[pos] == '[')
                {
                  buf[pos] = ' ';
                }
              pos ++;
            }
          vecToken.clear();
          Util::StringToken((string)buf, (string)" \t",vecToken);
          if(vecToken.size() != 3)
            {
              printf("invalid line in the config file,line:%d --%d\n",curLine,vecToken.size());
              m_vecMapUnit.clear();
              return false;
            }
          uint32_t mapID = atoi(vecToken[1].c_str());
          
          if(0 == mapID /*|| errno == ERANGE */)
            {
              printf("invalid line in the config file,line:%d\n",curLine);
              return false;
            }
          tempMapTile.mapTileID = mapID;
          vector<string> tok;
          Util::StringToken( (string)vecToken[2], (string)"*", tok );
          uint32_t x, y;
          x = atoi( tok[0].c_str() );
          y = atoi( tok[1].c_str() );
          if( x == 0 || y == 0 )
            {
              printf( "size = 0?? shit!!, line:%d\n", curLine );
              return false;
            }
          tempMapTile.sizeX = x;
          tempMapTile.sizeY = y;
          
          m_mapMapTile.push_back( tempMapTile );
          // printf("MapTile Size()=%d\nx:%d\n%d: ",m_mapMapTile.size(),tempMapTile.sizeX,tempMapTile.sizeY);;
          memset(buf, 0 ,200);
          continue;
        }
      // MapUnit信息行。
      else
        {

          if( m_mapMapTile.size() == 0 )
            {
              printf(" read cfg failed, line:%d\n", curLine );
            }
          
          MapTile &tempMapTile = m_mapMapTile[m_mapMapTile.size()-1];
          
          MapUnitInfo tempMapUnit;
          
          vecToken.clear();
          Util::StringToken((string)buf, (string)" \t",vecToken);
          if(vecToken.size() < LINE_TOKEN)
            {
              printf("invalid line in the config file,line:%d\n",curLine);
              return false;
            }
          tempMapUnit.mapUnitID = atoi(vecToken[0].c_str());
          // 如果指定了0或者调用atoi失败,该行是坏行
          if(0 == tempMapUnit.mapUnitID /*|| errno == ERANGE*/ )
            {
              badLine = true;
            }
          //  printf("MapUnit id:%d\t",atoi(vecToken[0].c_str()));
          memset(tempMapUnit.mapUnitName, 0, MAX_MAPUNIT_NAME_LENGTH);
          strcpy(tempMapUnit.mapUnitName, vecToken[1].c_str());
          // printf("MapUnit Name:%s\t",vecToken[1].c_str());
          // printf("MapUnit ZoneID:%d\t",atoi(vecToken[2].c_str()));
          // printf("MapUnit ServerID:%d\t",atoi(vecToken[3].c_str()));
          tempMapUnit.zoneID = atoi(vecToken[2].c_str());
          if(0 == tempMapUnit.mapID /*|| errno == ERANGE*/)
            {
              badLine = true;
            }
          tempMapUnit.serverID = atoi(vecToken[3].c_str());
          if(0 == tempMapUnit.serverID /*|| errno == ERANGE*/)
            {
              badLine = true;
            }
          tempMapUnit.isIsland = false;
          if(badLine)
            {
              printf("invalid line in the config file,line:%d\n",curLine);
              m_vecMapUnit.clear();
              return false;
            }
          tempMapTile.vecMapUnit.push_back(tempMapUnit);
          memset(buf, 0 ,200);
        }
    }
  
  // 进行错误检测。
  vector<MapTile>::iterator iterMapTile;
  for( iterMapTile = m_mapMapTile.begin() ; iterMapTile != m_mapMapTile.end() ; iterMapTile++ )
    {
      uint32_t x = iterMapTile->sizeX;
      uint32_t y = iterMapTile->sizeY;

      if( ( x * y ) != ( iterMapTile->vecMapUnit.size() ) )
        {
          return false;
		}
    }
  
  AssignNeighbourState();
  bool result = AssignZoneID();
  
  LoadZoneGroup();

  if(result)
    {
      return true;
	}
  else
    {
      return false;
    }
}



UniG_ILM_Config* UniG_ILM_Config::Instance()
{
  if(NULL == m_pInstance)
    {
      m_pInstance = new UniG_ILM_Config();
	}
  return m_pInstance;
}


bool UniG_ILM_Config::AssignZoneID() 
{
  uint16_t nMaxZoneID = 0;
  uint32_t nSizeUnit = 0;
  vector<MapTile>::iterator iterMapTile;
  for( iterMapTile = m_mapMapTile.begin() ; iterMapTile != m_mapMapTile.end() ; iterMapTile++ )
    {
      vector<MapUnitInfo>::iterator iterMapUnitInfo;
      for( iterMapUnitInfo = iterMapTile->vecMapUnit.begin() ; iterMapUnitInfo != iterMapTile->vecMapUnit.end() ; iterMapUnitInfo++ )
        {
          if( iterMapUnitInfo->zoneID > nMaxZoneID )
            {
              nMaxZoneID = iterMapUnitInfo->zoneID;
            }
        }
      nSizeUnit += iterMapTile->vecMapUnit.size(); 
    }
  
  // 指定的最大zoneID比MapUnit个数还要多,不允许这样
  if( nMaxZoneID > nSizeUnit )
    {
      return false;
    }
  
  for( iterMapTile = m_mapMapTile.begin() ; iterMapTile != m_mapMapTile.end() ; iterMapTile++ )
    {
      vector<MapUnitInfo>::iterator iterMapUnitInfo;
      for( iterMapUnitInfo = iterMapTile->vecMapUnit.begin() ; iterMapUnitInfo != iterMapTile->vecMapUnit.end() ; iterMapUnitInfo++ )
        {
          if( iterMapUnitInfo->zoneID == 0 )
            {
              iterMapUnitInfo->zoneID = ++nMaxZoneID;
            }
        }
      
    }
  return true;
}



bool UniG_ILM_Config::AssignNeighbourState()
{
  // 一个地图上有一个地图单元,如果有一个就是孤岛
  vector<MapUnitInfo>::iterator iterUnit;
  list<uint32_t> lstMapID;
  list<uint32_t>::iterator iterMapID;
  
  vector<uint32_t> vecUnitOnMap;
  vector<MapTile>::iterator iterMapTile;
  for( iterMapTile = m_mapMapTile.begin() ; iterMapTile != m_mapMapTile.end() ; iterMapTile++ )
    {
      // 孤岛
      if( iterMapTile->sizeX * iterMapTile->sizeY == 1 )
        {
                  if( iterMapTile->vecMapUnit.size() )
                    {
                      iterMapTile->vecMapUnit.front().isIsland = true;			
                    }
                  continue;
        }
      vecUnitOnMap.clear();
      vector<MapUnitInfo>::iterator iterMapUnitInfo;
      for( iterMapUnitInfo = iterMapTile->vecMapUnit.begin() ; iterMapUnitInfo != iterMapTile->vecMapUnit.end() ; iterMapUnitInfo++ )
        {
          vecUnitOnMap.push_back( iterMapUnitInfo->mapUnitID );
        }
      
      // 非孤岛
      int m, n;
      n = (int)iterMapTile->sizeX;
      m = (int)iterMapTile->sizeY;
      for(int i=0; i<m*n; i++)
        {
          // left map
          if(i-1 >= 0 && i%m != 0) 
            {
              m_mapNeighbourMaps.insert(pair<uint32_t, uint32_t>(vecUnitOnMap[i] , vecUnitOnMap[i-1]));
              
              //upper left map
              if(i>m)
                {
                  m_mapNeighbourMaps.insert(pair<uint32_t, uint32_t>(vecUnitOnMap[i] , vecUnitOnMap[i-1-m]));

                }

              //under left map

              if(i+m<m*n)
                {
                  m_mapNeighbourMaps.insert(pair<uint32_t, uint32_t>(vecUnitOnMap[i] , vecUnitOnMap[i-1+m]));
                }
            }
          // upper map
          if(i- m >= 0)
            {
              m_mapNeighbourMaps.insert(pair<uint32_t, uint32_t>(vecUnitOnMap[i],vecUnitOnMap[i-m]));

            }
          // right map
          if(i+1 < m*n && (i+1)%m != 0)
            {
              m_mapNeighbourMaps.insert(pair<uint32_t, uint32_t>(vecUnitOnMap[i], vecUnitOnMap[i+1]));

              //upper right map
               if(i>m)
                {
                  m_mapNeighbourMaps.insert(pair<uint32_t, uint32_t>(vecUnitOnMap[i] , vecUnitOnMap[i+1-m]));

                }
               //under right map
                 if(i+m+1<=m*n)
                {
                  m_mapNeighbourMaps.insert(pair<uint32_t, uint32_t>(vecUnitOnMap[i] , vecUnitOnMap[i+1+m]));
                }

            }
          // under map
          if(i+m < m*n)
            {
              m_mapNeighbourMaps.insert(pair<uint32_t, uint32_t>(vecUnitOnMap[i], vecUnitOnMap[i+m]));
            }

          
        }
    }
  
  return true;
  
}


bool UniG_ILM_Config::GetServeGroupByMapID(uint32_t mapID, list<uint16_t>& lstServer)
{
  map<uint32_t, list<uint16_t> >::iterator iter;
  for( iter = m_mapMapServerGroup.begin(); iter != m_mapMapServerGroup.end(); iter++)
    {
      if(iter->first == mapID)
        {
          lstServer = iter->second;
          return true;
        }
    }
  return false;
}


void UniG_ILM_Config::GetAllZoneID( list<uint16_t>& lstZone )
{
  list<uint16_t>::iterator iter;
  vector<MapTile>::iterator iterMapTile;
  for( iterMapTile = m_mapMapTile.begin() ; iterMapTile != m_mapMapTile.end() ; iterMapTile++ )
    {
      vector<MapUnitInfo>::iterator iterMapUnitInfo;
      for( iterMapUnitInfo = iterMapTile->vecMapUnit.begin() ; iterMapUnitInfo != iterMapTile->vecMapUnit.end() ; iterMapUnitInfo++ )
        {
          iter = find(lstZone.begin(), lstZone.end(), iterMapUnitInfo->zoneID );
          if(iter != lstZone.end())
            {
              continue;
            }
          else
            {
              lstZone.push_back(iterMapUnitInfo->zoneID);
            }
        }		
    }
}

void UniG_ILM_Config::GetAllServerID(list<uint16_t>& lstServer)
{
  list<uint16_t>::iterator iter;
  for(uint32_t i = 0; i< m_vecMapUnit.size(); i++)
    {
      iter = find(lstServer.begin(), lstServer.end(),m_vecMapUnit[i].serverID);
      if(iter != lstServer.end())
        {
          continue;
        }
      else
        {
          lstServer.push_back(m_vecMapUnit[i].serverID);
        }
    }
}

void UniG_ILM_Config::GetAllMapID(list<uint32_t>& lstMap)
{
  list<uint32_t>::iterator iter;
  for(uint32_t i = 0; i< m_vecMapUnit.size(); i++)
    {
      iter = find(lstMap.begin(), lstMap.end(),m_vecMapUnit[i].mapID);
      if(iter != lstMap.end())
        {
          continue;
        }
      else
        {
          lstMap.push_back(m_vecMapUnit[i].serverID);
        }
    }
}

void UniG_ILM_Config::GetMapUnitByZone(uint16_t zoneID, list<MapUnitInfo>& lstMap)
{
  vector<MapTile>::iterator iterMapTile;
  for( iterMapTile = m_mapMapTile.begin() ; iterMapTile != m_mapMapTile.end() ; iterMapTile++ )
    {
      vector<MapUnitInfo>::iterator iterMapUnitInfo;
      for( iterMapUnitInfo = iterMapTile->vecMapUnit.begin() ; iterMapUnitInfo != iterMapTile->vecMapUnit.end() ; iterMapUnitInfo++ )
        {
          if( iterMapUnitInfo->zoneID == zoneID )
            {
              lstMap.push_back( *iterMapUnitInfo );
            }
          else
            {
              continue;
            }
        }		
    }
}

uint32_t UniG_ILM_Config::GetZoneIDByMapUnitID(uint32_t mapUnitID)
{
  vector<MapTile>::iterator iterMapTile;
  for( iterMapTile = m_mapMapTile.begin() ; iterMapTile != m_mapMapTile.end() ; iterMapTile++ )
    {
      vector<MapUnitInfo>::iterator iterMapUnitInfo;
      for( iterMapUnitInfo = iterMapTile->vecMapUnit.begin() ; iterMapUnitInfo != iterMapTile->vecMapUnit.end() ; iterMapUnitInfo++ )
        {
          if( iterMapUnitInfo->mapUnitID == mapUnitID )
            {
              return iterMapUnitInfo->zoneID;
            }
        }		
    }
  
  return 0;
}
bool UniG_ILM_Config::IsNeighbour(uint32_t map1, uint32_t map2)
{
	//printf("\n%d   %d",map1, map2);
  list<uint32_t> lstNeighbour;
  GetNeighbourMapUnit(map1, lstNeighbour);
  if(find(lstNeighbour.begin(), lstNeighbour.end(), map2) == lstNeighbour.end())
    {
      //	printf(" is not adjoining\n");
      return false;
    }
  //printf(" is adjoining\n");
  return true;
}

void UniG_ILM_Config::GetNeighbourMapUnit(uint32_t mapUnitID, list<uint32_t>& mapUnit)
{
  
  iterator_map iter;
  pair<iterator_map, iterator_map> result;
  result = m_mapNeighbourMaps.equal_range(mapUnitID);
  for(iter = result.first; iter != result.second; iter++)
    {
      if(find(mapUnit.begin(), mapUnit.end(), iter->second) == mapUnit.end())
        {
          mapUnit.push_back(iter->second);
        }
    }
}

uint32_t UniG_ILM_Config::GetNeighbourMapUnit(multimap<uint32_t, uint32_t>& mapNeighbourMaps)
{

  mapNeighbourMaps.clear();
  iterator_map iter;
  for(iter =m_mapNeighbourMaps.begin() ; iter != m_mapNeighbourMaps.end(); iter++)
    {
      
      mapNeighbourMaps.insert(*iter);      
    }

  return mapNeighbourMaps.size();
  
}




 uint32_t UniG_ILM_Config::GetMapUnitInfo(vector<MapUnitInfo>& vecMapUnit)
{

  vecMapUnit.clear();

  vector<MapTile>::iterator iterMapTile;
  for( iterMapTile = m_mapMapTile.begin() ; iterMapTile != m_mapMapTile.end() ; iterMapTile++ )
    {
      vector<MapUnitInfo>::iterator iterMapUnitInfo;
      for( iterMapUnitInfo = iterMapTile->vecMapUnit.begin() ; iterMapUnitInfo != iterMapTile->vecMapUnit.end() ; iterMapUnitInfo++ )
        {
          vecMapUnit.push_back(*iterMapUnitInfo);
          // printf("%d\t\t%s\n", iterMapUnitInfo->mapUnitID, iterMapUnitInfo->mapUnitName);
         
        }		
    }
  
  return vecMapUnit.size();


}



void UniG_ILM_Config::PrintMapUnitInfo()
{
  printf("MapUintId\tmapUnitName\tZoneID\tServerID\n");
  vector<MapTile>::iterator iterMapTile;
  for( iterMapTile = m_mapMapTile.begin() ; iterMapTile != m_mapMapTile.end() ; iterMapTile++ )
    {
      vector<MapUnitInfo>::iterator iterMapUnitInfo;
      for( iterMapUnitInfo = iterMapTile->vecMapUnit.begin() ; iterMapUnitInfo != iterMapTile->vecMapUnit.end() ; iterMapUnitInfo++ )
        {
          printf("%d\t\t%s\t%d\t%d\n", iterMapUnitInfo->mapUnitID, iterMapUnitInfo->mapUnitName, iterMapUnitInfo->zoneID, iterMapUnitInfo->serverID);
        }		
    }
}


bool UniG_ILM_Config::GetManagedZone(list<uint16_t>& lstZone, uint16_t &ServerGroupID )
{
  if( m_vZoneGroup.empty() )
    {
      return false;
    }
  
  lstZone = m_vZoneGroup.front().ZoneID;
  ServerGroupID = m_vZoneGroup.front().nServerGroup;
  
  vector<ZoneGroup>::iterator iter = m_vZoneGroup.begin();
  m_vZoneGroup.erase( iter );
  return true;
}


bool UniG_ILM_Config::RecoveryManagedZone(list<uint16_t> lstZone, uint16_t ServerGroupID )
{

 
  ZoneGroup _zoneG;
  _zoneG.ZoneID=lstZone;
  _zoneG.nServerGroup=ServerGroupID;
  m_vZoneGroup.push_back(_zoneG);
  return true;
}





// 孤岛 虽然取出来，但没有设置正确的MapID
void UniG_ILM_Config::LoadZoneGroup()
{
  vector<MapTile>::iterator iterMapTile;
  for( iterMapTile = m_mapMapTile.begin() ; iterMapTile != m_mapMapTile.end() ; iterMapTile++ )
    {
      vector<MapUnitInfo>::iterator iterMapUnitInfo;
      for( iterMapUnitInfo = iterMapTile->vecMapUnit.begin() ; iterMapUnitInfo != iterMapTile->vecMapUnit.end() ; iterMapUnitInfo++ )
        {
          bool bIsExist = false;
          vector<ZoneGroup>::iterator iterZoneGroup;
          for( iterZoneGroup = m_vZoneGroup.begin() ; iterZoneGroup != m_vZoneGroup.end() ; iterZoneGroup++ )
            {
              if( iterZoneGroup->nServerID == iterMapUnitInfo->serverID )
                {
                  if(find(iterZoneGroup->ZoneID.begin(),iterZoneGroup->ZoneID.end(), iterMapUnitInfo->zoneID) ==iterZoneGroup->ZoneID.end())
                    {
                      iterZoneGroup->ZoneID.push_back( iterMapUnitInfo->zoneID );
                    }
                  bIsExist = true;
                  break;
                }
            }
          if( bIsExist == false )
            {
              ZoneGroup zg;
              zg.nServerID = iterMapUnitInfo->serverID;
              printf("zg.nServerID : %d\n",zg.nServerID);
              zg.nServerGroup = iterMapTile->mapTileID;
              zg.bDispatched = false;
              zg.ZoneID.push_back( iterMapUnitInfo->zoneID );
              m_vZoneGroup.push_back( zg );
            }
        }		
    }

  printf("m_vZoneGroup size() : %d\n",m_vZoneGroup.size());
}
