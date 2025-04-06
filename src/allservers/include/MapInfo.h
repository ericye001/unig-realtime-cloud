#ifndef MAP_ITEM_HPP
#define MAP_ITEM_HPP
#include <inttypes.h>
//
// MapItem
//
#pragma pack (1)
typedef struct MapInfo
{
    uint32_t _id;
    // char _name[50];
    uint16_t _playerNum;
    //uint32_t _width;
    // uint32_t _height;
    // int _x;
    // int _y;
    //std::vector<uint16_t> _adjoinningMaps;
}MapInfo_T;

#pragma pack ()
#endif
