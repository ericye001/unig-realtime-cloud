#include "UniG.h"

int main(int argc, char *argv[])
{
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD( 2, 2 );
    WSAStartup( wVersionRequested, &wsaData );
#endif

    if(!UniG_RSS::Instance()->Initialize())
    {
        return 1;
    }
    try 
    {
        UniG_RSS::Instance()->MainRun();
    } 
    catch (...) 
    {
    }
    Thread::sleep(10);
    UniG_RSS::Instance()->Finalize();

#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}
