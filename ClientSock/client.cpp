#define _CRT_SECURE_NO_WARNINGS
#include <SimpleClient.hpp>
#include <thread>

int e = 1;

void process(SimpleClient* simple_client) {
	while (true) {
		char send_buf[128];
		scanf_s("%s", &send_buf, 128);
		if (0 == strcmp(send_buf, "quit")) {
			simple_client->close();
			printf("线程退出...");
			return;
		}
		else if (0 == strcmp(send_buf, "logout")) {
			Logout logout;
			strcpy((char*)&logout.username, "sada");
			simple_client->sendData(logout);

		}
		else if (0 == strcmp(send_buf, "login")) {
			Login login;
			strcpy((char*)&login.username, "sada");
			strcpy((char*)&login.password, "12345");
			simple_client->sendData(login);
		}
		else {
			printf("重新输入...");
		}
	}
}

int main() {
	SimpleClient simple_client;
	simple_client.InitSocket();
	simple_client.Connect("127.0.0.1", 8080);
	
	std::thread t(process, &simple_client);
	t.detach();

	while (simple_client.isRun()) {
		simple_client.onRun();
	}
	simple_client.close();
	while (1);
	return 0;
}