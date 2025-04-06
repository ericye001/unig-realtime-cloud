#ifndef MAP_TO_SRV_INFO_HPP
#define MAP_TO_SRV_INFO_HPP
#include <inttypes.h>

#pragma pack (1)


typedef struct Map2SrvInfo
{
    uint32_t _mapId;
    uint16_t _srvId;
}Map2SrvInfo_T;

#pragma pack ()
#endif
