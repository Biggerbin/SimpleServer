#ifndef _CELLCOMMON_H_
#define _CELLCOMMON_H_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#define SOCKET int
#define INVALID_SOCKET SOCKET(~0)
#define SOCKET_ERROR      (-1)

#endif //_WIN32



#define RECV_BUF_SIZE 10240
#define SEND_BUFF_SZIE 10240

#include "ObjectPool.h"
#include "MemoryPool.h"
#include "DataHeader.h"




#endif //_CELLCOMMON_H_
