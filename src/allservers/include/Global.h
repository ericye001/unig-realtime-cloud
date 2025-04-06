#ifndef _UNIG_ILM_GLOBAL_H__
#define _UNIG_ILM_GLOBAL_H__

#include <list>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

#define MAX_MAPUNIT_NAME_LENGTH 20
#define DEFAULT_ZONEID_VALUE    0
#define LINE_TOKEN			    4
#define FILE_LINE_BUF_LEN       200

#define MAX_JUNCTURE_LEVEL      10

#define NO_OTHER_SERVER_IN_GROUP   0x00000010
#define NEIGHBOUR_SERVER_ALL_BUSY  0x00000011
#define NO_CANDIDATE_PLAN		   0x00000012
#define HAS_IDLE_SERVER			   0x00000013


typedef unsigned int   UINT32;
typedef int	       int32_t;
//typedef char           char;
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef short          int16_t;
typedef bool           BOOL;

extern uint32_t g_nAllowedMaxOdds;			//两个server允许的最大差值
extern uint32_t g_nMinCanMigValue;			//可搬家的最小值,小于这个值不搬
extern uint32_t g_nMaxServerLoad;				// Server能容忍的最大值

#endif
