#define WIN32_LEAN_AND_MEAN
#include <stdio.h>

#include <Windows.h>
#include <WinSock.h>

//#pragma comment(lib, "ws2_32_lib")

int main() {
	WORD ver = MAKEWORD(2, 2); //typedef unsigned short WORD 
	WSADATA dat;
	WSAStartup(ver, &dat);

	//建立服务端六个步骤
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf("build success\n");
	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(8080);
	ser_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(sock, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr)) == SOCKET_ERROR) {
		printf("bind fail");
		return 0;
	}

	if (listen(sock, 5) == SOCKET_ERROR) {
		printf("listen fail");
		return 0;
	}
	struct sockaddr_in cli_addr;
	SOCKET cli_sock = INVALID_SOCKET;
	int addlen = sizeof(struct sockaddr);
	char recv_buf[BUFSIZ];
	while (1) {
		cli_sock = accept(sock, (struct sockaddr *)&cli_addr, &addlen);
		if (cli_sock == INVALID_SOCKET) {
			printf("accept fail");
			return 0;
		}
		printf("clint ip = %s, port = %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
		while (1) {
			int len = recv(cli_sock, recv_buf, strlen(recv_buf)+1, 0);
			printf("%s, %d\n", recv_buf, len);
			if (len <= 0) break;
			
			if (0 == strcmp(recv_buf, "getName")) {
				char tmp[] = "SimpleServer";
				send(cli_sock, tmp, strlen(tmp)+1, 0);
			}
			else {
				char tmp[] = "again ask";
				send(cli_sock, tmp, strlen(tmp)+1, 0);
			}
		}
		printf("clint conn end");
		closesocket(cli_sock);
	}
	closesocket(sock);

	WSACleanup();
	return 0;
}