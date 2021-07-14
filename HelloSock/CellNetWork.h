#ifndef _CELL_NET_WORK_H_
#define _CELL_NET_WORK_H_

#include "CellCommon.h"

class CELLNetWork
{
private:
	CELLNetWork()
	{
#ifdef _WIN32
		//����Windows socket 2.x����
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif

#ifndef _WIN32
		//if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		//	return (1);
		//�����쳣�źţ�Ĭ������ᵼ�½�����ֹ
		signal(SIGPIPE, SIG_IGN);
#endif
	}

	~CELLNetWork()
	{
#ifdef _WIN32
		//���Windows socket����
		WSACleanup();
#endif
	}
public:
	static void Init()
	{
		static  CELLNetWork obj;
	}
};

#endif // !_CELL_NET_WORK_H_
