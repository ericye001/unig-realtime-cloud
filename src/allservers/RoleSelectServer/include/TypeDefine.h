/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: TypeDefine.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: 定义一些基本的类型

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _TYPE_DEFINE_H__
#define _TYPE_DEFINE_H__

#include "./../../include/MsgCode.h"

namespace Command
{
    const uint16
        MSG_CONN_REQUEST = 0x0010,
        MSG_CONN_RESPONSE = 0x0020,

        BLOCK_CONN_REQUEST = 0x0030,
        BLOCK_CONN_RESPONSE = 0x0040,

        // 下边基本没用 TODO
        BREAK_MSG_CONN_REQUEST = 0x0050,
        BREAK_MSG_CONN_RESPONSE = 0x0060,
        BREAK_BLOCK_CONN_REQUEST = 0x0070,
        BREAK_BLOCK_CONN_RESPONSE = 0x0080,

        BEGIN_BLOCK_TRANSFER = 0x0100,
        BLOCK_DATA = 0x0200,
        END_BLOCK_TRANSFER = 0x0300,

        OPTION_REQUEST = 0x0400;
};


struct ILMArgs
{
    uint32 nILMIp;                          // the ILM server's ip
    uint16 nILMPort;                        // the ILM server's port for server
    uint32 nSrvIp;                          // the server's ip
    uint16 nSrvPort;                        // the server's port for proxy
    uint16 nMaxSrvNum;                      // the max server number for initialize the connections
};

struct DOGE_PlayerNumOnMap
{
    char* mMapName;                         // the map name
    int   mNum;                             // the number of players on the map
};

struct DOGE_ServerLoad
{
    int m_nMapNum;                            // the number of maps
    DOGE_PlayerNumOnMap* m_pPlayerNumOnMap;   // an array of DOGE_PlayerNumOnMap_T
};


namespace CommandResult
{
    const uint8
        SUCCESS = 0x01,
        FAILURE = 0x00,
        CONN_EXIST = 0x01;
};

namespace MsgResult
{
    const int
        SUCCESS = 1,
        FAILURE = 0;
};

struct Option
{
    uint16 nKey;
    uint16 nValue;
};

typedef unsigned int UNIG_GUID;
const float32 PI = 3.1415926f;

struct Vector2
{
    float32 x;
    float32 y;
};

struct Vector3
{
    float32 x;
    float32 y;
    float32 z;
};

struct Quaternion
{
    float32 w;
    float32 x;
    float32 y;
    float32 z;
};

struct MapInfo
{
    uint16 nMapID;
    char szName[50];
    int  nPlayerNum;
    //uint32 nWidth;
    //uint32 nHeight;
    //int nX;
    //int nY;
    //vector<uint16> adjoinningMaps;
};

#pragma pack(1)
struct ServerInfo
{
    uint16 nServerID;
    uint32 nIP;
    uint16 nPort; 
    uint16 nMsgBusPort; 
};

struct Map2SrvInfo
{
    uint32 nMapID;
    uint16 nServerID;
};
#pragma pack()
#endif

