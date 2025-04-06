#ifndef __DOGE_H__
#define __DOGE_H__

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

/* The following defines all player's action we supported
 */
static const int DOGE_ACTION_JOIN = 1;          // player joining
static const int DOGE_ACTION_MOVE = 2;          // player moving
static const int DOGE_ACTION_BEAT = 3;          // player beating
static const int DOGE_ACTION_REMOVE = 4;        // removing player

#define DOGE_INITIALIZE_FN  "initialize"      // initialization
#define DOGE_ASSISTANT_FN  "assistant"        // game assistant
#define DOGE_MAP_POSSESSED_FN  "mapPossessed" // check whether the map is belong to the server
#define DOGE_SYNCHRONIZE_FN  "synchronize"    // flush all message between servers
#define DOGE_LOG_SYS_LOAD_FN  "logSysLoad"    // 

/* The player's infomation.
 * When game logic calls DOGE's assistant function,
 * this struct should be provided.
 */
typedef struct
{
    char* _name;                // player's name
    int _action;                // player's action
    int _direction;             // player's moving direction if necessary
    
    char* _viewableMaps[10];    // the viewable maps's name (max 10) 
    char* _prevMapName;         // the previouse map name before movement
    char* _currMapName;         // the current map name after movement

    size_t _paramSize;          // the size of parameter if needed, else 0
    void*  _param;              // the parameter
}DOGE_PlayerInfo_T;

/* player number on one map
 */
typedef struct
{
    char* _name;                // the map name
    int  _num;                  // the number of players on the map
}DOGE_PlayerNumOnMap_T;

/* Server load struct
 */
typedef struct
{
    int _mapNum;                // the number of maps
    DOGE_PlayerNumOnMap_T* _playerNumOnMap;  // an array of DOGE_PlayerNumOnMap_T
}DOGE_ServerLoad_T;


/* Player's id. As to crossfire, it is defined as a array of char,
 * indicates the name of the player
 */
typedef struct
{
    char _name[50];             // the player's name
}DOGE_PlayerId_T;

/* The arguments used by game engine to initialize, such as initialize
 * the connection with IGO, etc. it must be given.
 */
typedef struct
{
    uint32_t _igoIp;            // the IGO server's ip
    uint16_t _igoPort;          // the IGO server's port for server
    uint32_t _srvIp;            // the server's ip
    uint16_t _srvPort;          // the server's port for proxy
    uint16_t _maxSrvNum;        // the max server number for initialize the connections
}DOGE_Args_T;



/* The arguments used by game engine to initialize, including all game logic's
 * functions' address which will be called by game engine
 */
typedef struct
{
    /* Get the number of players on the map named mapName
     */
    int (*getPlayerNumOnMap_FP)(const char* mapName);
    
    /* Get the server's status, not implemented.
     */
    int (*getServerLoad_FP)(DOGE_ServerLoad_T* srvLoad);

    int (*setMigrationFlag_FP)(const char* mapName, int from, int to);
    int (*clearMigrationFlag_FP)();
    
    /* Get the partition data from game logic.
     * Return 0 if success.
     * Return -1 if failure.
     */
    int (*packMigratorsOnMap_FP)(const char* mapName, void** data, int* length);
    
    /* Tell game logic that accept the partition data to
     * response the partition request.
     * Return 0 if success.
     * Return -1 if failure.
     */
    int (*acceptMigrators_FP)(void* data, int length);
   
//    int (*removePlayersOnMap)(const char* mapName);
//    int (*redirectPlayersOnMap)(const char* mapName, int srvId);
    
    /* Release the map specified by mapName, tell all players standing on the map
     * to swith to other server with id srvId
     */
    int (*releaseMapTo_FP)(const char* mapName, int srvId);
    
    /* If Migration failed, undo it
     */
    int (*undoMigration_FP)(const char* mapName);
    

//     DOGE_PlayerId_T* (*getPlayerId)(int mapId, int* num);
    
    /* Get the player's infomation for creating, updating, transfering agent/player
     * from game logic
     */
    void* (*serializePlayer_FP)(DOGE_PlayerId_T* player, int* length);
    void* (*serializeAgent_FP)(DOGE_PlayerId_T* player, int* length);
    
    /* Tell game logic to add a player/agent
     */
    int (*addPlayer_FP)(void* playerInfo, int length);    
    int (*addAgent_FP)(void* playerInfo, int length);
    
    /* Tell game logic to delete a player/agent
     */
    int (*removePlayer_FP)(DOGE_PlayerId_T* player);
    int (*removeAgent_FP)(DOGE_PlayerId_T* player);
    
    /* Tell game logic to update a player/agent
     */
    int (*updatePlayer_FP)(void* playInfo, int length);
    int (*updateAgent_FP)(void* playInfo, int length);
    
    /* Tell game logic that an agent specified by object will
     * beat something.
     */
    int (*beatPlayer_FP)(void* object, int length);
    
    /* Tell game logic to send "changeServerId" to client(s)
     * if flag == 1 then name is a player's name
     * else name is a map's name, so all players on the map will be sent
     * that command.
     */
    int (*redirectPlayer_FP)(const char* name, int srvId);
}DOGE_LogicIntf_T;




/* The type of game engine's initializing function pointer,
 * used by game logic to dlsym in GameEngine.so.
 */
typedef int (*DOGE_Initialize_FT)(DOGE_Args_T* engineArgs, DOGE_LogicIntf_T* logicAgent);

/* The type of game engine's assistant function pointer,
 * used by game logic to dlsym in GameEngine.so.
 * When action is made by one player, game logic should call this function
 * to do additional work, such as create agent, update agent, delete agent, 
 * transfer player, etc.
 *
 * Return 0 if no addtiional action being done.
 * Return 1 if the player cross servers.
 */
typedef int (*DOGE_Assistant_FT)( DOGE_PlayerInfo_T* objectInfo);

/* Help game logic to know whether the map named name is out of
 * current server.
 *
 * Return 1 if the map is out of current server
 * Return 0 if the map is not out of current server
 */
typedef int (*DOGE_MapPossessed_FT)(const char* name);
/* The following 2 function do not be implemented.
 */
typedef int (*DOGE_RemoveServer_FT)();
typedef int (*DOGE_Synchronize_FT)();

typedef void (*DOGE_LogSysLoad_FT)();




#endif
