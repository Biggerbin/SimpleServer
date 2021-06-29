#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <Windows.h>
#include <WinSock.h>

#pragma comment(lib, "ws2_32.lib")



int main() {
	WORD ver = MAKEWORD(2, 2); //typedef unsigned short WORD 
	WSADATA dat;
	WSAStartup(ver, &dat);

	SOCKET cli_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(8080);
	ser_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	int ret = connect(cli_sock, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr));
	char recv_buf[100];
	char send_buf[100];
	while (1) {
		scanf_s("%s", send_buf, sizeof(send_buf));
		printf("%s\n", send_buf);
		if (0 == strcmp(send_buf, "quit")) {
			break;
		}
		send(cli_sock, send_buf, strlen(send_buf) + 1, 0);
		int len = recv(cli_sock, recv_buf, strlen(recv_buf) + 1, 0);
		printf("%d, %s\n", len, recv_buf);
		
	}
	printf("end");
	closesocket(cli_sock);
	getchar();

	WSACleanup();
	
	return 0;
}