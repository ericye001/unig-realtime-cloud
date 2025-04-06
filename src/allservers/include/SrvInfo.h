#ifndef SRV_INFO_HPP
#define SRV_INFO_HPP

#include <inttypes.h>

#pragma pack (1)
typedef struct SrvInfo
{
    uint16_t _id;
    uint32_t _ip;
    uint16_t _gamePort;             // Server listening on for clients
    uint16_t _infoBusPort;      // InfoBus listening on
}SrvInfo_T;

#pragma pack()

#endif
