#include "LoginServerListener.h"
// common definations
#include "Map2SrvInfo.h"
#include "MapInfo.h"
#include "SrvInfo.h"
#include "GlobalArgs.h"

// message definations
#include "message/Server2IGO.h"
#include "message/Login2IGO.h"

// local class definations
#include "Plan.h"
#include "MSTManager.h"
#include "ServerManager.h"
#include "GameServerListener.h"

LoginServerAcceptor_T* LoginServerAcceptor_T::_pInstance = 0;
LoginServerAcceptor_T& LoginServerAcceptor_T::instance()
{
    if (_pInstance)
    {
        return *_pInstance;
    }
    _pInstance = new LoginServerAcceptor_T;
    if (!_pInstance)
    {
        throw StrException_T(
            string("Fail to new LoginServerAcceptor_T\n"
                "\tat LoginServerAcceptor_T::instance")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    return *_pInstance;
}

void LoginServerAcceptor_T::free()
{
    if (_pInstance)
    {
        delete _pInstance;
        _pInstance = 0;
    }
}

LoginServerAcceptor_T::LoginServerAcceptor_T()
{
    SocketInterface_T* pSocket = new ServerSocket_T(
        CommonArgs_T::instance()._acceptLoginServerPort);
    if (!pSocket)
    {
        throw StrException_T(
            string("Fail to new ServerSocket_T\n"
                "\t at LoginServerAcceptor_T::LoginServerAcceptor_T()")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    socket(pSocket);
    event(EPOLLIN|EPOLLHUP|EPOLLERR);
    action(this);
}

LoginServerAcceptor_T::~LoginServerAcceptor_T()
{
    
}

void LoginServerAcceptor_T::execute(Connection_T* pConn)
{
    SocketInterface_T* pSocket = ((ServerSocket_T*)pConn->socket())->accept();
    cout << "IGO accept login server" <<pConn->fd()<< endl;

    if(!pSocket)
    {
        cout << "Failed to accept GameServer" << endl;
        return;
    }
    LoginServerListener_T* listener = new LoginServerListener_T(pSocket);
    if (!listener)
    {
        delete pSocket;
        throw StrException_T(
            string("Fail to new LoginServerListener_T(pSocket)\n"
                "\tat LoginServerAcceptor_T::execute")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    ServerManager_T::instance().loginServerListener(listener);
}

void LoginServerAction_T::execute(Connection_T* pConn)
{    
     cout << pConn->fd() << endl;
     LoginServerListener_T* listener = 
       static_cast<LoginServerListener_T*>(pConn);
        
    BasePacket_T pkg;
    try
    {        
        pkg.read(listener);
    }
    catch (exception& e)
    {
        cout << "exception: " << e.what() << endl
            << "\tat LoginServerAction_T::execute"
            << SrcLocation_T(__FILE__, __LINE__) << endl;
        ServerManager_T::instance().onLoginServerException(listener);
        return;
    }
        
    switch(BaseMessage_T::type(&pkg))
    {
        case MsgType::UPDATE_MST_RESPONSE:
        {
//cout << "----LoginServerAction_T::execute UPDATE_MST_RESPONSE" << endl;
            UpdateMSTResponseMsg_T updateMSTResp;
            updateMSTResp.unpack(&pkg);
            listener->onUpdateMSTReply(updateMSTResp);
        }
        break;
        default:
        // wrong message
cout << "receive unknown message from login server" << endl;
        break;
    }
}

//
//
//
LoginServerListener_T::LoginServerListener_T(SocketInterface_T* pSocket)
{

  
    socket(pSocket);
    Action_T* pAction = new LoginServerAction_T;
    if (!pAction)
    {
        throw StrException_T(
            string("Fail to new LoginServerAction_T\n"
                "\tat LoginServerListener_T::LoginServerListener_T")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    action(pAction);
    event(EPOLLIN|EPOLLHUP|EPOLLERR);
    LoginConnectIGORequestMsg_T request;
    request.read(this);
    LoginConnectIGOResponseMsg_T response;
    response.write(this);
}

LoginServerListener_T::~LoginServerListener_T()
{
    
}

bool LoginServerListener_T::gameServerJoin(GameServerListener_T* listener)
{
    try
    {
        AddServerRequestMsg_T request;
        request._newServer = listener->srvInfo();
        request._ext = 0;
        request.write(this);
        
        AddServerResponseMsg_T response;
        response.read(this);
        
        return response._addedServer._id;
    }
    catch (exception& e)
    {
        cout << "exception: " << e.what() << endl
            << "\tat LoginServerListener_T::join"
            << SrcLocation_T(__FILE__, __LINE__) << endl;
    }
    return false;
}



bool LoginServerListener_T::gameServerReJoin(GameServerListener_T* listener)
{
 
        
        return  listener->srvInfo()._id;
    
}







void LoginServerListener_T::updateMST(UpdateMSTRequestMsg_T& updateMSTMsg)
{
    updateMSTMsg.write(this);
}

void LoginServerListener_T::onUpdateMSTReply(UpdateMSTResponseMsg_T& reply)
{
    ServerEpoll_T::instance().remove(this);
}
