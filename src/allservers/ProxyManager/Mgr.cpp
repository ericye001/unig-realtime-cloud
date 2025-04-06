#include "ProxyManager.h"
#include "stacktrace.h"
#include "signal.h"

void CrashHandler(int sig)
{
  signal(sig, SIG_DFL);
  StackTrace();
  exit(-1);
}


int main(int argc, char *argv[])
{
  struct sigaction sact;

  StackTraceInit(argv[0], -1);
  
  sigemptyset(&sact.sa_mask);
  sact.sa_flags = 0;
  sact.sa_handler = CrashHandler;
  sigaction(SIGSEGV, &sact, NULL);
  sigaction(SIGBUS, &sact, NULL);


  ProxyManager::instance().connectILM();
  ProxyManager::instance().startAcceptProxy();
  ProxyManager::instance().startAcceptLogin();
  time_t  _lastQueryTime = time(NULL);
  while(true)
    {
      if(!ProxyManager::instance()._ILMConnected)
        {
          time_t curTime = time(NULL);
          if(curTime - _lastQueryTime < 40)
            {
              continue;

             }
           _lastQueryTime = time(NULL);

           ProxyManager::instance().connectILM();      
         }
       else
         {

           
           ProxyManager::instance().queryProxies();
           ProxyManageEpoll::instance().wait(5);
        }
    }
    return 0;
}

