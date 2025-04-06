/*
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 
* 文件名称：
* 文件标识：
* 摘    要：
* 
*
* 修订记录：
*       修订日期    修订人    修订内容
*/

#include "common/Utils.h"


LogWriter_T* Logger_T::_pLogger = 0;
list<LogRecord_T*> Logger_T::_logRecords;
pthread_cond_t Logger_T::_incoming;


Mutex_T::Mutex_T()
{
    pthread_mutex_init(&_mutex, NULL);
}

MutexLock_T::MutexLock_T(Mutex_T& mutex)
    : _mutex(mutex)
{
    pthread_mutex_lock(&_mutex._mutex);
}

MutexLock_T::~MutexLock_T()
{
    pthread_mutex_unlock(&_mutex._mutex);
}

Condition_T::Condition_T()
{
    pthread_cond_init(&_condition, NULL);
}
Condition_T::~Condition_T()
{
    
}
void Condition_T::notifyOne()
{
    pthread_cond_signal(&_condition);
}
void Condition_T::notifyAll()
{
    pthread_cond_broadcast(&_condition);
}

void Condition_T::wait(Mutex_T& mutex)
{
    pthread_cond_wait(&_condition, &mutex._mutex);
}

void Condition_T::timedWait(Mutex_T& mutex)
{
    
}

//---------------------------------------------------------------------------
char* FormattedDate_T::getDate(char* pDate, size_t size, char* pFormat)
{
  time_t now;
  now = time(NULL);
  
  strftime(pDate, size, pFormat, localtime(&now));
  
  return pDate;
}

//---------------------------------------------------------------------------
void Logger_T::initialize(const char* pPath, const char* prefix)
{  
  Lock_T lock;
  if (!_pLogger) 
  {
    pthread_cond_init(&_incoming, NULL);
    _pLogger = new LogWriter_T(pPath, prefix);    
    _pLogger->start();    
    _pLogger->detach();
  }
}
void Logger_T::initialize(const string& path, const string& prefix)
{  
  Logger_T::initialize(path.c_str(), prefix.c_str());
}
void Logger_T::logIt(const char* pLevel, const char* pAction,
     const char* pContent, ...)
{  
  Lock_T lock;
  
  char mTime[10];
	FormattedDate_T::getDate(mTime, sizeof(mTime), "%H:%M:%S");
	va_list ap;
	va_start(ap, pContent);
	char buffer[1024];
	vsprintf(buffer, pContent, ap);
	va_end(ap);	
	
	LogRecord_T* pLogRec = new LogRecord_T(mTime, pLevel, pAction, buffer);
	
	if (!pLogRec)
	{
	  return;
	}
	
  _logRecords.push_back(pLogRec);
  ConditionNotifier_T notifier(_incoming);  
}

int Logger_T::size()
{
  Lock_T lock;
  return _logRecords.size();
}

LogRecord_T* Logger_T::get()
{
  Lock_T lock;
  if (_logRecords.size() == 0)
  {
    return NULL;
  }
  return *_logRecords.begin();
}

bool Logger_T::remove(LogRecord_T* pLogRec)
{
  Lock_T lock;
  list<LogRecord_T*>::iterator it;
  for (it = _logRecords.begin(); it != _logRecords.end(); ++it)
  {
    if (*it == pLogRec)
    {
      _logRecords.erase(it);
      delete pLogRec;
      return true;
    }
  }
  return false;
}

void Logger_T::requestIncoming()
{
  Lock_T lock;
  ConditionObserver_T observer(_incoming);
}

void Logger_T::stop()
{  
  while (size() > 0)
  {
    ConditionNotifier_T notifier(_incoming); 
    usleep(100);
  }  
  _pLogger->setCancelFlag(true); 
  ConditionNotifier_T notifier(_incoming); 
  while (_pLogger->isRunning())
  {    
    usleep(100);
  }
  
  delete _pLogger;
}

LogWriter_T::LogWriter_T(const char* pPath, const char* prefix)
  : _path(pPath), _prefix(prefix), _fileName(""), _pf(0)
{
  // Nothing
}

LogWriter_T::~LogWriter_T() 
{
  fclose(_pf);
}

void LogWriter_T::run()
{
  while (true)
  { 
    if (Logger_T::size() == 0)
    {
      Logger_T::requestIncoming();
    }    
    
    if (getCancelFlag())
    { 
      return;
    }    
    
    LogRecord_T* pLogRec;
    while ((pLogRec = Logger_T::get()))
    {
      if (!write(pLogRec))
      {
        printf("Fail to write log\n");
        continue;
      }
      if (!Logger_T::remove(pLogRec))
      {
        printf("Fail to remove logrecord\n");
      }   
    }
  }
}
void LogWriter_T::setCancelFlag(bool cancelFlag)
{  
  _cancelFlag = cancelFlag;
}

bool LogWriter_T::getCancelFlag()
{
  return _cancelFlag;
}

bool LogWriter_T::write(LogRecord_T* pLogRec)
{
  char date[10];
	FormattedDate_T::getDate(date, sizeof(date), "%Y%m%d");
  
  char fileName[100];
	sprintf(fileName, 
	    "%s/%s.%s.log", 
	    _path.c_str(), 
	    _prefix.c_str(), 
	    date);
	
	char line[1120];
  /*
	sprintf(line, "%-12s%-4s%s\n", pLogRec->time().c_str(), 
	    pLogRec->level().c_str(),  
	    pLogRec->content().c_str());
	*/
	sprintf(line, 
					"%-12s%-4s%-30s%s\n", 
					pLogRec->_time, 
					pLogRec->_level,  
					pLogRec->_action,
					pLogRec->_content);
	
	if (strcmp(_fileName.c_str(), fileName) != 0)
	{
	  if (_pf)
	  {
	    fclose(_pf);	  
	  }
	  
	  _fileName = fileName;
	  _pf = fopen(_fileName.c_str(), "a");
	  if (!_pf)
	  {
	    cout << "Fail to open " << _fileName << " file" << endl;	  
	    return false;
	  }	  
	}
	if (!_pf)
	{
	  _pf = fopen(_fileName.c_str(), "a");
	  if (!_pf)
	  {
	    cout << "Fail to open " << _fileName << " file" << endl;	  
	    return false;
	  }
	}
	
	if (fputs(line, _pf) == EOF)
	{
	  fclose(_pf);
	  _pf = NULL;
	  return false;
	}
	fflush(_pf);
	return true;
}
//---------------------------------------------------------------------------
LogRecord_T::LogRecord_T(const char* pTime, const char* pLevel, const char* pAction,
    const char* pContent)
//  : _time(pTime), _level(pLevel), _content(pContent)
{		
	memset(_time, 0, sizeof(_time));
  strncpy(_time, 
					pTime, 
					strlen(pTime) > sizeof(_time)-1 ? sizeof(_time)-1 : strlen(pTime));
	memset(_level, 0, sizeof(_level));
  strncpy(_level, 
					pLevel, 
					strlen(pLevel) > sizeof(_level)-1 ? sizeof(_level)-1 : strlen(pLevel));
	memset(_action, 0, sizeof(_action));	
	strncpy(_action, 
					pAction, 
					strlen(pAction) > sizeof(_action)-1 ? sizeof(_action)-1 : strlen(pAction));
  memset(_content, 0, sizeof(_content));
	strncpy(_content, 
					pContent,
					strlen(pContent) > sizeof(_content)-1 ? sizeof(_content)-1 : strlen(pContent));
}
/*
string& LogRecord_T::time() 
{
  return _time;
}
string& LogRecord_T::level() 
{
  return _level;
}
string& LogRecord_T::content() 
{
  return _content;
}
*/
//---------------------------------------------------------------------------
//  initDaemon()-do
//---------------------------------------------------------------------------
//  Description:  Turn to daemon process
//  Parameters:   None
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
void initDaemon( void )
{
  pid_t pid;
  if ((pid = fork()) < 0)
  {
    cout << "fork error: " << strerror(errno) << endl;
    exit(0);
  }
  else if (pid != 0)
  { // Parent process, exit
    exit(0);
  }  
  setsid( );
}
//---------------------------------------------------------------------------
//  getSubStr()-
//---------------------------------------------------------------------------
//  Description:  Split the p_strIn string delimed by p_delimer, then return
//                the number pos sub-string in p_strOut
//  Parameters:   p_strIn:    input string
//                pos:        the position of sub-string to return
//                p_delimer:  the delimer of input string
//                p_strOut:   output string
//  Return:       None
//  Exception:    None
//  MT Safe:      False
//  Note:         None
//---------------------------------------------------------------------------
char* getSubStr( const char* p_strIn,int pos, const char* p_delimer,
        char* p_strOut
)
{
  int i;
  char *ps;
  char tmpStr[1024];

  if ( strlen( p_strIn ) >= 1024 )
  {
    return NULL;
  }

  memset( tmpStr, 0, sizeof( tmpStr ));
  strcpy( tmpStr, p_strIn );

  if ((ps = strtok(tmpStr, p_delimer)) == NULL)
  {
    return NULL;
  }
  for (i = 1; i <= pos; ++i)
  {
    if ((ps = strtok(NULL, p_delimer)) == NULL)
    {
      return NULL;
    }
  }
  strcpy( p_strOut, ps );

  return p_strOut;
}

void createPath( const string& path )
{
  char command[200];
  memset( command, 0, sizeof( command ) );
  sprintf( command, "mkdir -p %s", path.c_str( ));

  if( system( command ) < 0 )
  {
      cerr << "Fail to create " << path << " path." << endl;
			exit( -1 );
  }
}
string& ltrim(string& s)
{
  for (unsigned int i = 0; i < s.length(); i++) 
	{
		if (!isspace(s.at(i))) 
		{
			s.erase(0, i);
			return s;
		}		
	}	
	s.erase(0, s.length());
	
	return s;	
}


string& rtrim(string& s)
{
	for (int i = s.length()-1; i >= 0; i-- ) 
	{
		if (!isspace(s.at(i))) 
		{
		  s.erase(i+1);
		  return s;
		}
	}
	s.erase(0);
	
	return s;
}
void start(const char* process)
{
  char cmd[100];
  char line[20];
  sprintf(cmd, "ps -ef|grep %s|grep start|grep -v grep|cut -c10-16", process);
  FILE* pF = popen(cmd, "r");
  if (!pF)
  {
    cout << "Starting [" << process << "] process failed: " << strerror(errno) << endl;
    exit(0);
  }
  
  while (fgets(line, sizeof(line), pF))
  {    
    if (line[strlen(line)-1] == '\n')
    {
      line[strlen(line)-1] = '\0';
    }
    
    if (atoi(line) == getpid())
    {
      continue;
    }
    
    cout << "[" << process << "] process is active, starting canceled." << endl;    
    pclose(pF);
    exit(0);
  }
  cout << "[" << process << "] process started." << endl;    
  
  pclose(pF);
}

void stop(const char* process)
{
  char cmd[100];
  char line[20];
  sprintf(cmd, "ps -ef|grep %s|grep start|grep -v grep|cut -c10-16", process);
  FILE* pF = popen(cmd, "r");
  if (!pF)
  {
    cout << "Stopping [" << process << "] process failed: " << strerror(errno) << endl;
    exit(0);
  }
  
  if (fgets(line, sizeof(line), pF))
  {
    pid_t pid = atoi(line);
    kill(pid, SIGTERM);
    cout << "Stopping [" << process << "] process, please wait." << endl;
    //int i = 0;
    while (kill(pid, 0) >= 0)
    {   
      sleep(1);
      /*
      if (++i > 10)
      {
        cout << "kill " << pid << endl;
        kill(pid, SIGTERM);
      } 
      */
    }
    cout << "[" << process << "] process stopped." << endl;    
  }
  else
  {
    cout << "[ " << process << " ] process is inactive, stopping canceled." << endl;
  }
  
  pclose(pF);
  exit(0);
}

void kill(const char* process)
{
  char cmd[100];
  char line[20];
  sprintf(cmd, "ps -ef|grep %s|grep start|grep -v grep|cut -c10-16", process);
  FILE* pF = popen(cmd, "r");
  if (!pF)
  {
    cout << "Killing [" << process << "] process failed: " << strerror(errno) << endl;
    exit(0);
  }
  
  if (fgets(line, sizeof(line), pF))
  {
    pid_t pid = atoi(line);
    kill(pid, SIGKILL);
    cout << "Killing [" << process << "] process, please wait." << endl;
    //int i = 0;
    while (kill(pid, 0) >= 0)
    {   
      sleep(1);
      /*
      if (++i > 10)
      {
        cout << "kill " << pid << endl;
        kill(pid, SIGTERM);
      } 
      */
    }
    cout << "[" << process << "] process killed." << endl;    
  }
  else
  {
    cout << "[ " << process << " ] process is inactive, killing canceled." << endl;
  }
  
  pclose(pF);
  exit(0);
}

void doSignalSIGTERM(sighandler_t handler)
{
	sigset_t newmask;	
	sigemptyset(&newmask);	
	sigaddset(&newmask, SIGTERM);	
	signal(SIGTERM, handler);
}

void setSignalHandler(int sig, sighandler_t handler)
{
  struct sigaction act, oldact;

  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_handler = handler;
  sigaction(sig, &act, &oldact);
}


SrcLocation_T::SrcLocation_T(char* file, int line)
    : string("")
{
    *this += string("(") + file + ":";
    char l[10];
    sprintf(l, "%d", line);
    *this += string(l) + ")";
}

StrException_T::StrException_T(const char* what) throw()
    : _what(what)
{
    // No body
}

StrException_T::StrException_T(const string& what) throw()
    : _what(what)
{
    // No body
}

const char* StrException_T::what() const throw()
{
    return _what.c_str();
}

/* This must be called before each Getopt. */
void
getoptReset(GetoptInfo *const opt)
{
	memset(opt, 0, sizeof(GetoptInfo));
	opt->ind = 1;
	opt->init = 0xF123456F;
}	/* GetoptReset */



int
getopt(GetoptInfo *const opt, int nargc, char **const nargv, const char *const ostr)
{
	const char *oli;				   /* Option letter list index */
	if ((opt == NULL) || (nargc == 0) || (nargv == (char **) 0) || (ostr == NULL))
		return (EOF);

	if (opt->init != 0xF123456F) {
		/* We can do it for them. */
		getoptReset(opt);
	}

	if ((opt->place == NULL) || (opt->place[0] == '\0')) {
		/* update scanning pointer */
		if (opt->ind >= nargc || *(opt->place = nargv[opt->ind]) != '-')
			return (EOF);
		if (opt->place[1] && *++opt->place == '-') {	/* found "--" */
			++opt->ind;
			return (EOF);
		}
	}								   /* Option letter okay? */

	if (opt->place == NULL)
		oli = NULL;
	else if ((opt->opt = (int) *opt->place++) == (int) ':')
		oli = NULL;
	else
		oli = strchr(ostr, opt->opt);

	if (oli == NULL) {
		if ((opt->place == NULL) || (opt->place[0] == '\0'))
			++opt->ind;
		if (opt->err)
			(void) fprintf(stderr, "%s%s%c\n", *nargv, ": illegal option -- ", opt->opt);
		return((int) '?');
	}
	if (*++oli != ':') {			   /* don't need argument */
		opt->arg = NULL;
		if ((opt->place == NULL) || (opt->place[0] == '\0'))
			++opt->ind;
	} else {						   /* need an argument */
		if ((opt->place != NULL) && (opt->place[0] != '\0'))
			opt->arg = (char *) opt->place;
		else if (nargc <= ++opt->ind) {  /* no arg */
			opt->place = NULL;
			if (opt->err) 
				(void) fprintf(stderr, "%s%s%c\n", *nargv, ": option requires an argument -- ", opt->opt);
			return((int) '?');
		} else						   /* white space */
			opt->arg = (char *) nargv[opt->ind];
		opt->place = NULL;
		++opt->ind;
	}
	return (opt->opt);				   /* dump back Option letter */
}									   /* Getopt */
