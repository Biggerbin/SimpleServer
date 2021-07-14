#include "CellClient.h"


CellClient::CellClient(SOCKET cliSock) :
	_recvBuff(RECV_BUF_SIZE),
	_sendBuff(SEND_BUFF_SZIE)
{
	static int n = 1;
	id = n++;
	_cli_sock = cliSock;

	resetDTHeart();
	resetDTSend();
}

CellClient::~CellClient()
{
	CellLog::Info("s=%d CELLClient%d.~CELLClient\n", serverId, id);
	if (INVALID_SOCKET != _cli_sock)
	{
#ifdef _WIN32
		closesocket(_cli_sock);
#else
		close(_cli_sock);
#endif
		_cli_sock = INVALID_SOCKET;
	}
}

bool CellClient::hasMsg()
{
	return _recvBuff.hasMsg();
}

DataHeader * CellClient::front_msg()
{
	return (DataHeader*)_recvBuff.getData();
}

void CellClient::pop_front_msg()
{
	if (hasMsg()) {
		_recvBuff.pop(front_msg()->data_length);
	}
}

int CellClient::recvData()
{
	return _recvBuff.read4socket(_cli_sock);
}

int CellClient::SendReal()
{
	resetDTSend();
	return _sendBuff.write2socket(_cli_sock);
}


int CellClient::SendData(dataHeaderPtr header)
{
	if (_sendBuff.push((const char*)&(*header), header->data_length))
	{
		return header->data_length;
	}
	return SOCKET_ERROR;

}

bool CellClient::checkHeart(time_t dt)
{
	_dtHeart += dt;
	if (_dtHeart >= CLIENT_HREAT_DEAD_TIME) {
		CellLog::Info("checkHeart dead:s=%d,time=%ld\n", _cli_sock, _dtHeart);
		return true;
	}
	return false;
}

void CellClient::resetDTHeart()
{
	_dtHeart = 0;
}

bool CellClient::checkSend(time_t dt)
{
	_dtSend += dt;
	if (_dtSend >= CLIENT_SEND_MSG_TIEM) {
		CellLog::Info("checkSend:s=%d,time=%d\n", _cli_sock, _dtSend);
		SendReal();
		resetDTSend();
		return true;
	}
	return false;
}

void CellClient::resetDTSend()
{
	_dtSend = 0;
}
