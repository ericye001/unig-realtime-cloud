#ifndef GLOBALARGS_HPP
#define GLOBALARGS_HPP

#include "common/Utils.h"
#include "common/IniParser.h"
#include <limits.h>
#include <vector>
#include <string>

class CommonArgs_T;
class ArgsHandler_T;

using namespace std;

class ArgsHandler_T : public ClassLevelLockable_T<ArgsHandler_T>
{
  public:
    static ArgsHandler_T& instance();
    
    void load(CommonArgs_T& args);    
    void save(CommonArgs_T& args);
    
  private:
    ArgsHandler_T();
    ArgsHandler_T(const ArgsHandler_T& rhs);
    ArgsHandler_T& operator=(const ArgsHandler_T& rhs);
        
    // Create a new SpReceiverArgs_T and store a pointer to is in _pInstance
    static void create();
    // Gets called if dead reference detected
    static void onDeadReference();
    virtual ~ArgsHandler_T();
    // Data
    static ArgsHandler_T* _pInstance;
    static bool _destroyed;
    static IniParser_T* _parser;
};

class CommonArgs_T : public ClassLevelLockable_T<CommonArgs_T>
{
  public:
    static CommonArgs_T& instance();

    string m_sLogPath;
    string m_sLogPrefix;
    string m_sILMServerIp;
    int m_iILMServerPort;
    int m_iLocalServerPort;
    int m_iMaxEpollSfd;
    int m_iProxyMgrPort;
    int m_iproxyQueryFreq;
    int m_iacceptLoginServerPort;

  private:
    CommonArgs_T() {};
    CommonArgs_T(const CommonArgs_T& rhs);
    CommonArgs_T& operator=(const CommonArgs_T& rhs);
    
    // Create a new CommonArgs_T and store a pointer to is in _pInstance
    static void create();
    // Gets called if dead reference detected
    static void onDeadReference();
    virtual ~CommonArgs_T();
    // Data
    static CommonArgs_T* _pInstance;
    static bool _destroyed;
};


#endif
