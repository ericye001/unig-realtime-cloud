#ifndef _PROTOCAL_H
#define _PROTOCAL_H

#include "MsgCode.h"

using namespace MsgType;

enum {
    _MSG_C2S_USER_LOGIN = 2000,
    _MSG_S2C_USER_LOGIN,
    _MSG_S2C_PROXY_INFO,

    //ADD_CLIENT_TO_PM_REQUEST = 0x5001,
    //ADD_CLIENT_TO_PM_RESPONSE = 0x5002,
};

struct stProtocalHeader {
    unsigned int usSize;
    unsigned short usType;
    // unsigned short ucExt;
};

struct stC2SLogin {
    unsigned int usSize;
    unsigned short usType;
    // unsigned short ucExt;
    char szUser[16];
    char szPassword[16];
};

struct stS2CLogin {
    unsigned int usSize;
    unsigned short usType;
    // unsigned short ucExt;
    unsigned int nUserId;
};

struct stS2CProxyInfo {
    unsigned int usSize;
    unsigned short usType;
    // unsigned short ucExt;
    unsigned long ulProxyIP;
    unsigned short usPort;
	unsigned int nRSID;
    unsigned int nSession;
};

struct ProxySrvInfo {

    unsigned int _ip;
    unsigned short _port;    
};

struct ClientInfo
{
    unsigned int _id;
    unsigned int _ip;   
    unsigned short _port;
};



struct stAddClientToPMResponse {
    unsigned int usSize;
    unsigned short usType;
    // unsigned short ucExt;
    unsigned int nUserID;
    unsigned int nSession;
    ProxySrvInfo pProxySvrInfo;
    unsigned int RoleServerID;
};

struct stAddClientToPMRequest {
    unsigned int usSize;
    unsigned short usType;
    // unsigned short ucExt;
    unsigned int nUserID;
    unsigned int nSession;
    ClientInfo pClientInfo;
};


#endif 