#pragma once

namespace MsgDir
{
    const unsigned short
    MSG_IN  = 1,
    MSG_OUT = 0,
    MSG_SYN = 2;
}

namespace MsgType
{
const unsigned short
    ////////////////////////////////////////////////////////////////
    /*ILM vs GameServer*/
    SERVER_CONNECT_ILM_REQUEST      =   0x0001,
    SERVER_CONNECT_ILM_RESPONSE     =   0x0002,
    
    UPDATE_MST_REQUEST              =   0x0003,
    UPDATE_MST_RESPONSE             =   0x0004,
    
    SERVER_LEAVE_REQUEST            =   0x0005,
    SERVER_LEAVE_RESPONSE           =   0x0006,

    SERVER_STATUS_QUERY             =   0x0009,
    SERVER_STATUS_REPLY             =   0x000A,


    START_PARTITION_REQUEST         =   0x000B,
    START_PARTITION_RESPONSE        =   0x000C,

    LOCK_MAP_REQUEST                =   0x000D,
    LOCK_MAP_RESPONSE               =   0x000E,

    UNLOCK_MAP_REQUEST              =   0x000F,
    UNLOCK_MAP_RESPONSE             =   0x0010,

    UNDO_START_PARTITION_REQUEST    =   0x0011,

    CLEAR_AGENT_REQUEST             =   0x0012,
    CLEAR_AGENT_RESPONSE            =   0x0013,

    LOAD_SUBMAP_REQUEST             =   0x0014,
    LOAD_SUBMAP_RESPONSE            =   0x0015,
    //ADD
    MAP_UNIT_RESPONSE               =   0x0016,
    MAP_UNIT_RESPONSE1              =   0x0017,

    /*GameServerµ½Proxy*/
    MESSAGE_FROM_PROXY              =   0x0018,
    ////////////////////////////////////////////////////////////////
    // proxy 1+++
    PROXY_CONNECT_LOGIN_REQUEST     =   0x1005,
    PROXY_CONNECT_LOGIN_RESPONSE    =   0x1006,
    PROXY_CONNECT_SERVER_REQUEST    =   0x1007,
    PROXY_CONNECT_SERVER_RESPONSE   =   0x1008,
    PROXY_CONNECT_LOGIN_COMPLETE    =   0x1009,
    PROXY_DISCONNECT_LOGIN_REQUEST  =   0x100A,
    PROXY_DISCONNECT_LOGIN_RESPONSE  =   0x100B,
    PROXY_DISCONNECT_LOGIN_COMPLETE  =   0x100C,
    PROXY_DISCONNECT_LOGIN_COMPLETE_RESPONSE  =   0x100D,
    UPDATE_SERVER_IP_TABLE          =   0x100E,
    PROXY_CONNECT_GAMESERVER_REQUEST     =   0x100F,
    PROXY_CONNECT_GAMESERVER_RESPONSE    =   0x1010,

    ////////////////////////////////////////////////////////////////
    // rss 2+++
    ROLESERVER_CONNECT_IGO_REQUEST  =   0x2001,
    ROLESERVER_CONNECT_IGO_RESPONSE =   0x2002,

    ////////////////////////////////////////////////////////////////
    CHANGE_SERVERID                 =   0x3005,
    NEW_USER_LOGIN                  =   0x3006,
    USER_LOGOUT                     =   0x3007,
    OBJECT_ENTER_MAP                =   0x3008,
    OBJECT_LEAVE_MAP                =   0x3009,

    START_ZONE_MOVE_REQUEST         =   0x3011,
    UNDO_START_ZONE_MOVE_REQUEST    =   0x3012,
    START_ZONE_MOVE_RESPONSE        =   0x3013,
    PACK_MAP_REQUEST                =   0x3014,
    PACK_MAP_RSPONSE                =   0x3015,

    UNPACK_MAP_REQUEST              =   0x3016,
    UNPACK_MAP_RSPONSE              =   0x3017,

    OBJECT_MESSAGE                  =   0x3018,
    SYSTEM_MESSAGE                  =   0x3019,
   

    //////////////////////////////////////////////////////////////////
    AGENT_CREATE_REQUEST            =   0x301A,
    AGENT_CREATE_RESPONSE           =   0x301B,

    SYN_AGENT_MESSAGE               =   0x301C,

    AGENT_DELETE_REQUEST            =   0x301D,
    AGENT_DELETE_RESPONSE           =   0x301E,


    REALEASE_OBJECT_REQUEST         =   0x3020,
    REALEASE_OBJECT_RESPONSE        =   0x3021,

    LOCK_OBJECT_REQUEST             =   0x3022,
    LOCK_OBJECT_RESPONSE            =   0x3023,

    MSG_CONN_REQUEST                =   0x3024,
    MSG_CONN_RESPONSE               =   0x3025,
        
    BLOCK_CONN_REQUEST              =   0x3026,
    BLOCK_CONN_RESPONSE             =   0x3027,

    SESSION_CREATE_REQUEST          =   0x3028,
    SESSION_CREATE_RESPONSE         =   0x3029,

    AGENT_AND_SESSION_CREATE_REQUEST=   0x302A,

    ////////////////////////////////////////////////////////////////
    // 4+++
    PROXY_STATUS_QUERY              =   0x4001,
    PROXY_STATUS_REPLY              =   0x4002,
    UPDATE_SERVER_IPTABLE_REQUEST   =   0x4003,
    UPDATE_SERVER_IPTABLE_RESPONSE  =   0x4004,

    PM_CONNECT_ILM_REQUEST          =   0x4005,
    PM_CONNECT_ILM_RESPONSE         =   0x4006,
    ADD_SERVER_REQUEST              =   0x4007,
    ADD_SERVER_RESPONSE             =   0x4008,  
    
    SERVER_LOAD_PM                  =   0x4009,
    SERVER_LOAD_PM_REPLAY           =   0x400A,



    REMOVE_SERVER_REQUEST           =   0x400B,
    REMOVE_SERVER_RESPONSE          =   0x400C,
    ////////////////////////////////////////////////////////////////
    // 5+++
    CONSOLE_COMMAND                 =   0x5000,
    ADD_CLIENT_TO_PM_REQUEST        =   0x5001,
    ADD_CLIENT_TO_PM_RESPONSE       =   0x5002,
    SEND_SESSION_TO_PROXY_REQUEST   =   0x5003,
    SEND_SESSION_TO_PROXY_RESPONSE  =   0x5004,

    /////////////////////////////////////////////////////////////////
    //9+++
    /*Loginµ½ILM*/
    LOGIN_CONNECT_ILM_REQUEST       =   0x6001,
    LOGIN_CONNECT_ILM_RESPONSE      =   0x6002,
    ////////////////////////////////////////////////////////////////
    /*Proxyµ½RSS*/
    USER_SEARCH_ALL_ROLE            =   0x9001,
    USER_ADD_ROLE                   =   0x9002,
    USER_DELETE_ROLE                =   0x9003,
    USER_ENTER_GAME                 =   0x9004,
    ////////////////////////////////////////////////////////////////
    SYSTEM_HEART_TICK               =   0xFFFF;
////////////////////////////////////////////////////////////////////
};
