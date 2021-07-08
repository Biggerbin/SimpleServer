#include "CellClient.h"


CellClient::CellClient(SOCKET cliSock) {
	_recvBuf1[RECV_BUF_SIZE] = {};
	_szMsgBuf2[RECV_BUF_SIZE * 10] = {};
	_lastPos = 0;
	_cli_sock = cliSock;
}

int CellClient::SendData(dataHeaderPtr header)
{
	int ret = SOCKET_ERROR;
	int nSendLen = header->data_length;
	const char* pSendData = (char*)header.get();
	while (true) {
		if (_lastSendPos + nSendLen >= SEND_BUFF_SZIE) {
			int nCopyLen = SEND_BUFF_SZIE - _lastSendPos;
			memcpy(_szSendBuf + _lastSendPos, pSendData, nCopyLen);
			pSendData += nCopyLen;
			nSendLen -= nCopyLen;
			ret = send(_cli_sock, _szSendBuf, SEND_BUFF_SZIE, 0);
			_lastSendPos = 0;
			if (SOCKET_ERROR == ret) {
				return ret;
			}
		}
		else {
			memcpy(_szSendBuf + _lastSendPos, pSendData, nSendLen);
			_lastSendPos += nSendLen;
			break;
		}
	}

}
