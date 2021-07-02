#include <SimpleSocket.h>

int main() {
	SimpleServer simple_server;
	SOCKET sock = simple_server.InitSocket(8080);
	simple_server.onRun();

	simple_server.Close();
	getchar();
	return 0;
}