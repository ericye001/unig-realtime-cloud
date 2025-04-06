#ifndef CLIENT_INFO_HPP
#define CLIENT_INFO_HPP

#include <inttypes.h>

#pragma pack (1)
typedef struct ClientInfo
{
  uint16_t _port;
  uint32_t _id;
  uint32_t _ip;   
   
};
#pragma pack ()
#endif
