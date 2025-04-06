#include "ServerManager.h"
#include "stacktrace.h"
#include "signal.h"
#include "Config.h"
#include "ZoneMgr.h"
#include "GlobalArgs.h"
void CrashHandler(int sig)
{
  /* Reinstall default handler to prevent race conditions */
  signal(sig, SIG_DFL);
  /* Print the stack trace */
  StackTrace();
  /* And exit because we may have corrupted the internal
   * memory allocation lists. Use abort() if we want to
   * generate a core dump. */
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
  if(!UniG_ILM_Config::Instance()->LoadConfigFile("config.cfg"))
    {
      printf("load config file failure\n");
      exit(1);
    }
  // UniG_ILM_Config::Instance()->PrintMapUnitInfo();
  UniG_ILM_ZoneMgr::Instance()->LoadZoneInfo();
  //UniG_ILM_Config::Instance()->PrintMapUnitInfo();

  ServerEpoll::init(CommonArgs_T::instance()._maxGameServer+1+1+1);
  ServerEpoll::instance().clear();
  ServerEpoll::instance().add(&GameServerAcceptor::instance());
  ServerEpoll::instance().add(&RoleServerAcceptor::instance());
  ServerEpoll::instance().add(&ProxyMgrServerAcceptor::instance());
  ServerEpoll::instance().add(&ConsoleAcceptor::instance());
  while (true)
    {
      ServerEpoll::instance().wait(1);
      ServerManager::instance().acceptGameServers();
      ServerManager::instance().acceptRoleServers();
      ServerManager::instance().timedQueryServerStat();      
      ServerManager::instance().makePlanWhileRequired();
      ServerManager::instance().doPlan();
    }

    /**
    for(int ii=1;ii<10;ii++){
      list<uint32_t> mapUnit;
      UniG_ILM_Config::Instance()->GetNeighbourMapUnit(ii,mapUnit);
      cout<<"map id :"<<ii<<endl;
      list<uint32_t>::iterator iter;
      for(iter=mapUnit.begin();iter!=mapUnit.end();iter++)
        cout<<" Adjoin mapunit is "<<*iter<<endl;
    }
    **/
}











