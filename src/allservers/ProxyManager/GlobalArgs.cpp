// ---------------------------------------------------------------------------
//  Includes for all the program files
// ---------------------------------------------------------------------------
#include "GlobalArgs.h"

#define IGO_CONFIG_PATH "../etc/pm_config.ini"


CommonArgs_T* CommonArgs_T::_pInstance = 0;
bool CommonArgs_T::_destroyed = false;

ArgsHandler_T* ArgsHandler_T::_pInstance = 0;
IniParser_T* ArgsHandler_T::_parser = 0;
bool ArgsHandler_T::_destroyed = false;

ArgsHandler_T& ArgsHandler_T::instance() 
{
    if (!_pInstance) 
    {
        Lock_T lock;
        if (!_pInstance)
        {
            // Check for dead reference
            if (_destroyed)
            {
                onDeadReference();
            }
            else
            {
               // First call--initialize
               create();
            }
        }
    }

    return *_pInstance;
} 

ArgsHandler_T::ArgsHandler_T()
{
    // No body
}

// Create a new SpReceiverArgs_T and store a pointer to is in _pInstance
void ArgsHandler_T::create() 
{
    // Task: initialize _pInstance
    static ArgsHandler_T theInstance;
    _pInstance = &theInstance;
    try {
        _parser = new IniParser_T(IGO_CONFIG_PATH);
        _parser->load();
    }
    catch (const IniParserException_T& toCatch) {
        cout << toCatch.what() << endl;
        exit(1);
    }
}
// Gets called if dead reference detected
void ArgsHandler_T::onDeadReference() 
{
    //throw std::runtime_error("Dead Reference Detected");
    throw string("Dead Reference Detected");
}
ArgsHandler_T::~ArgsHandler_T() 
{
    _pInstance = 0;
    _destroyed = true;
    delete _parser;
}
void ArgsHandler_T::load(CommonArgs_T& args)
{
    try
    {
        vector<Section_T>::iterator it;
        _parser->getSection("Common", it);
        _parser->getKeyVal(*it, "LogPath", args.m_sLogPath);
        _parser->getKeyVal(*it, "LogPrefix", args.m_sLogPrefix);
        _parser->getKeyVal(*it, "MaxEpollSfd", args.m_iMaxEpollSfd);
        _parser->getKeyVal(*it, "ProxyMgrPort", args.m_iProxyMgrPort);
        _parser->getKeyVal(*it, "ProxyQueryFreq", args.m_iproxyQueryFreq);
        _parser->getKeyVal(*it, "ILMServerIp", args.m_sILMServerIp);
        _parser->getKeyVal(*it, "ILMServerPort", args.m_iILMServerPort);
        _parser->getKeyVal(*it, "LoginServerPort", args.m_iacceptLoginServerPort);
    }
    catch (IniParserException_T& e)
    {
        cout << e.what() << endl;
        exit(1);
    }
    catch (...)
    {
        cout << "Unkown error." << endl;
        exit(1);
    }
}

void ArgsHandler_T::save(CommonArgs_T& args)
{
  
}

CommonArgs_T& CommonArgs_T::instance() 
{
    if (!_pInstance) 
    {
        Lock_T lock;
    
        if (!_pInstance)
        {
            // Check for dead reference
            if (_destroyed)
            {
                onDeadReference();
            }
            else
            {
                // First call--initialize
                create();
            }
        }
    }
    return *_pInstance;
}
// Create a new CommonArgs_T and store a pointer to is in _pInstance
void CommonArgs_T::create() 
{
    // Task: initialize _pInstance
    static CommonArgs_T theInstance;
    _pInstance = &theInstance;   
    ArgsHandler_T& handler = ArgsHandler_T::instance();
    handler.load(*_pInstance);
}
// Gets called if dead reference detected
void CommonArgs_T::onDeadReference() 
{
    //throw std::runtime_error("Dead Reference Detected");
    throw string("Dead Reference Detected");
}
CommonArgs_T::~CommonArgs_T() 
{
    _pInstance = 0;
    _destroyed = true;
}

