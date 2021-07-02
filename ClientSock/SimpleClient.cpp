#include "SimpleClient.hpp"

SimpleClient::SimpleClient()
{
	_sock = INVALID_SOCKET;
	FD_ZERO(&read_set);
	_recvBuf = new RecvBuf();
}

SOCKET SimpleClient::InitSocket()
{
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2); //typedef unsigned short WORD 
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif // _WIN32
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!isRun()) {
		printf("����socketʧ��...\n");
		return INVALID_SOCKET;
	}
	printf("�ɹ�����socket...\n");
	FD_SET(_sock, &read_set);
	return _sock;
}

void SimpleClient::Connect(std::string ip_addr, int port)
{
	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);

#ifdef _WIN32
	ser_addr.sin_addr.S_un.S_addr = inet_addr(ip_addr.c_str());
#else
	ser_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
#endif

	int ret = connect(_sock, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr));
	if (ret == SOCKET_ERROR) {
		printf("���ӷ�����ʧ��...\n");
		return;
	}
	printf("�ɹ����ӷ�����...\n");

}

void SimpleClient::close()
{
if (isRun()) {
#ifdef WIN32
		closesocket(_sock);
		WSACleanup();
#else
		close(_sock);
#endif // WIN32
		_sock = INVALID_SOCKET;
}

}


int SimpleClient::onRun()
{
	
	if (isRun()) {
		fd_set tmp_set = read_set;
		timeval t = { 0, 0 };
		int num = (int)select(_sock + 1, &tmp_set, NULL, NULL, &t);
		if (num < 0) {
			close();
			printf("����������ӽ���...\n");
			return -1;
		}
		if (FD_ISSET(_sock, &tmp_set)) {
			if (-1 == recvData()) {
				printf("<socket %d>����������ӽ���...\n", _sock);
				return -1;
			}
		}
		return 0;
	}
	return -1;
}

bool SimpleClient::isRun()
{
	return _sock != INVALID_SOCKET;
}

int SimpleClient::recvData()
{
	//���ճ������
	
	int len = recv(_sock, (char*)_recvBuf->_recvBuf1, sizeof(RECV_BUF_SIZE), 0);
	if (len <= 0) {
		return -1;
	}
	memcpy(_recvBuf->_szMsgBuf2+ _recvBuf->_lastPos, _recvBuf->_recvBuf1, len);
	_recvBuf->_lastPos += len;
	while (_recvBuf->_lastPos >= sizeof(struct DataHeader)) {
		DataHeader* dataheader = (DataHeader *)_recvBuf->_szMsgBuf2;
		
		if (_recvBuf->_lastPos >= dataheader->data_length) {

			int siz = _recvBuf->_lastPos - dataheader->data_length;
			onNetMsg(dataheader);
			memcpy(_recvBuf->_szMsgBuf2, _recvBuf->_szMsgBuf2 + dataheader->data_length, siz);
			_recvBuf->_lastPos = siz;
		}
		else {
			break;
		}
	}
	//...

	return 0;
}

int SimpleClient::onNetMsg(DataHeader* dataheader)
{
	if (CMD_LOGIN_RESULT == dataheader->cmd) {
		LoginResult* result = (LoginResult *)dataheader;
		printf("�յ���½�������: %d�� �����Ϊ%d\n", result->result, result->data_length);
	}
	else if (CMD_LOGOUT_RESULT == dataheader->cmd) {
		LogoutResult* result = (LogoutResult *)dataheader;
		printf("�յ��˳��������: %d�� �����Ϊ%d\n", result->result, result->data_length);
	}
	else if (CMD_NEW_USER == dataheader->cmd) {
		NewUser* result = (NewUser *)dataheader;
		if (result->result) {
			printf("���û���¼<%d>�� �����Ϊ%d\n", result->sock, result->data_length);
		}
		else {
			printf("�û��˳�<%d>�� �����Ϊ%d\n", result->sock, result->data_length);
		}	
	}
	else {
		printf("�޷�ʶ����...\n");
	}

	return 0;
}

int SimpleClient::sendData(DataHeader &dataheader)
{
	if (isRun()) {
		send(_sock, (const char*)&dataheader, dataheader.data_length, 0);
	}
	return 0;
}





