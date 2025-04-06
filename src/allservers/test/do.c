
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <pthread.h>
#ifndef INADDR_NONE
#define INADDR_NONE     0xffffffff
#endif
#define MAX_STRING_LEN  1024
#define BUFSIZE  2048

#ifndef WIN32
#define SOCKET int
#else
#define errno WSAGetLastError()
#define close(a) closesocket(a)
#define write(a, b, c) send(a, b, c, 0)
#define read(a, b, c) recv(a, b, c, 0)
#endif



static char i_host[MAX_STRING_LEN];  
static char i_port[MAX_STRING_LEN]; 
static char i_ip[MAX_STRING_LEN];
static char i_from[MAX_STRING_LEN]; 
static char i_to[MAX_STRING_LEN]; 
static char i_zone[MAX_STRING_LEN]; 
typedef uint16_t server_id_t;
typedef uint16_t map_id_t;
typedef uint16_t zone_id_t;


int GenMoveZone_NetBuf(char* pOut,server_id_t fserver,server_id_t tserver,zone_id_t zone)
{

  int len = 4* sizeof(uint16_t);

  uint16_t type=0x9;
  memset(pOut,'\0',len);

  memcpy(pOut,&len ,sizeof(len));
  pOut += sizeof(len);
  memcpy(pOut,&type ,sizeof(type));
  pOut += sizeof(type);
  memcpy(pOut, &fserver, sizeof(fserver));
  pOut += sizeof(fserver);
  memcpy(pOut, &tserver, sizeof(tserver));
  pOut += sizeof(tserver);
  memcpy(pOut, &zone, sizeof(zone));
  return len+sizeof(len);
};





struct MapUnitInfo
{
	int		nID;
	char	szName[64];
};


int tcp_connect(const char *host, const unsigned short port)
{
    unsigned long non_blocking = 1;
    unsigned long blocking = 0;
    char * transport = "tcp";
    struct hostent      *phe;   /* pointer to host information entry    */
    struct protoent *ppe;       /* pointer to protocol information entry*/
    struct sockaddr_in sin;     /* an Internet endpoint address  */
    SOCKET s;                    /* socket descriptor and socket type    */
    int error;

#ifdef WIN32
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;
 
        wVersionRequested = MAKEWORD( 2, 0 );
 
        err = WSAStartup( wVersionRequested, &wsaData );
        if ( err != 0 ) {
            /* Tell the user that we couldn't find a usable */
            /* WinSock DLL.                               */
            printf("can't initialize socket library\n");
            exit(0);
        }
    }
#endif    
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    
    if ((sin.sin_port = htons(port)) == 0)
        printf("invalid port \"%d\"\n", port);
    
    /* Map host name to IP address, allowing for dotted decimal */
    if ( ( phe = gethostbyname(host) ) )
      {
        memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
      }
    else
      if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
        printf("can't get \"%s\" host entry\n", host);
    
    /* Map transport protocol name to protocol number */
    if ( (ppe = getprotobyname(transport)) == 0)
      printf("can't get \"%s\" protocol entry\n", transport);
    
    /* Allocate a socket */
    s = socket(PF_INET, SOCK_STREAM, ppe->p_proto);
    if (s < 0)
        printf("can't create socket: %s\n", strerror(errno));
    
    /* Connect the socket with timeout */
#ifdef WIN32
    ioctlsocket(s,FIONBIO,&non_blocking);
#else
    ioctl(s,FIONBIO,&non_blocking);
#endif
    //fcntl(s,F_SETFL, O_NONBLOCK);
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == -1){
        struct timeval tv; 
        fd_set writefds;
        tv.tv_sec = 10; 
        tv.tv_usec = 0; 
        FD_ZERO(&writefds); 
        FD_SET(s, &writefds); 
        if(select(s+1,NULL,&writefds,NULL,&tv) != 0){ 
            if(FD_ISSET(s,&writefds)){
                int len=sizeof(error); 
                //下面的一句一定要，主要针对防火墙 
                if(getsockopt(s, SOL_SOCKET, SO_ERROR,  (char *)&error, &len) < 0)
                    goto error_ret; 
                if(error != 0) 
                    goto error_ret; 
            }
            else
                goto error_ret; //timeout or error happen 
        }
        else goto error_ret; ; 

#ifdef WIN32
        ioctlsocket(s,FIONBIO,&blocking);
#else
        ioctl(s,FIONBIO,&blocking);
#endif

    }
    else{
error_ret:
        close(s);
        printf("can't connect to %s:%d\n", host, port);
       
    }
    return s;
}





/********************************************************/


#ifdef WIN32
char *optarg;

char getopt(int c, char *v[], char *opts)
{
    static int now = 1;
    char *p;

    if (now >= c) return EOF;

    if (v[now][0] == '-' && (p = strchr(opts, v[now][1]))) {
        optarg = v[now+1];
        now +=2;
        return *p;
    }

    return EOF;
}

#else
extern char *optarg;
#endif

#define required(a) if (!a) { return -1; }

int init(int argc, char *argv[])
{
    char c;
    //int i,optlen;
    //int slashcnt;

    i_host[0]  =  '\0';
    i_port[0]  =  '\0';
    i_ip[0]    =  '\0';
    i_from[0]  =  '\0';
    i_to[0]    =  '\0';
    i_zone[0]  =  '\0';
    while ((c = getopt(argc, argv, "i:p:f:t:z:?")) != EOF) {
        if (c == '?')
            return -1;
        switch (c) { 
          
        case 'f':
            required(optarg);
            strcpy(i_from, optarg);
            break;

        case 't':
          required(optarg);
          strcpy(i_to, optarg);
          break;

        case 'z':
          required(optarg);
          strcpy(i_zone, optarg);
          break;
          
        case 'p':
            required(optarg);
            strcpy(i_port, optarg);
            break;
         case 'i':
            required(optarg);
            strcpy(i_ip, optarg);
            break; 
 
          
        default:
            return -1;
        }
    }

    /* 
     * there is no default value for hostname, port number, 
     * password or uri
     */
     if (i_ip[0] == '\0' || i_port[0] == '\0'|| i_from[0]=='\0' ||i_to[0]=='\0' ||i_zone[0]=='\0')
     return -1;

    return 1;
}





int main(int argc, char *argv[])
{



  if (init(argc, argv) < 0)
    {
      printf("Useage:./ccc -p 4529 -i 192.168.0.106 -f 1 -t 2 -z 4\n\n\n");
      exit(1);
    }

  int fd;
  fd=tcp_connect(i_ip,atoi(i_port));
  char*  _read;;
  uint16_t from=atoi(i_from);
  uint16_t to=atoi(i_to);
  uint16_t zone=atoi(i_zone);
  uint32_t len=4*sizeof( uint16_t );
  
  len= GenMoveZone_NetBuf(_read,from,to,zone);

  // printf("write len : %d\n ", write(fd,&len, sizeof(len)));
  printf("write len : %d\n ", write(fd,_read, len));
 

  sleep(6);
 
  return 0;
} 


