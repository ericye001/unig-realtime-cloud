// ---------------------------------------------------------------------------
//  Includes for all the program files
// ---------------------------------------------------------------------------
#include "GlobalArgs.h"

#define IGO_CONFIG_PATH "../etc/IGO_config.ini"


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

        _parser->getKeyVal(*it, "LogPath", args._logPath);
        _parser->getKeyVal(*it, "LogPrefix", args._logPrefix);
        _parser->getKeyVal(*it, "MaxGameServer", args._maxGameServer);
        _parser->getKeyVal(*it, "AcceptLoginServerPort", args. _acceptLoginServerPort);
        _parser->getKeyVal(*it, "AcceptRoleServerPort", args. _acceptRoleServerPort);
        _parser->getKeyVal(*it, "AcceptGameServerPort", args._acceptGameServerPort);
        _parser->getKeyVal(*it, "AcceptConsolePort", args._acceptConsolePort);
        _parser->getKeyVal(*it, "GameServerStatQueryT", args. _gameServerStatQueryT);
        _parser->getKeyVal(*it, "PlayerNumBalanceValue", args._playerNumBalanceValue);
        _parser->getKeyVal(*it, "BaseInfoBusPort", args._baseInfoBusPort);
        _parser->getKeyVal(*it, "InfoBusMaxMsgNum", args._infoBusMaxMsgNum);
        _parser->getKeyVal(*it, "InfoBusBatchNum", args._infoBusBatchNum);
        _parser->getKeyVal(*it, "MaxServerLoad", args._maxServerLoad);


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

