#ifndef __EPOLL_H__
#define __EPOLL_H__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include "IGOStateDef.h"
typedef unsigned short		WORD;
typedef unsigned long		DWORD;
#define MAX_READ_BUFF	1024
#define MAX_CONNECT_NUM	200

inline void setNonblock(int sock)
{
	int opts;
	opts = fcntl(sock, F_GETFL);
	if(opts<0)
	{
		perror("fcntl(sock,GETFL)");
		exit(1);
	}
	opts = opts | O_NONBLOCK;
	if( fcntl(sock, F_SETFL, opts) < 0 )
	{
		perror("fcntl(sock,SETFL,opts)");
		exit(1);
	}
}

int dopoll();
#endif


