#define _CRT_SECURE_NO_WARNINGS
#include <SimpleClient.hpp>
#include <thread>
#include <atomic>
int e = 1;

bool g_bRun = true;
void cmdThread()
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("退出cmdThread线程\n");
			break;
		}
		else {
			printf("不支持的命令。\n");
		}
	}
}
//客户端数量
const int cCount = 100;
//发送线程数量
const int tCount = 2;
//客户端数组
SimpleClient* client[cCount];
std::atomic_int sendCount = 0;
std::atomic_int readyCount = 0;
void sendThread(int id)
{
	printf("thread<%d>,start\n", id);
	//4个线程 ID 1~4
	int c = cCount / tCount;
	int begin = (id - 1) * c;
	int end = id * c;

	for (int n = begin; n < end; n++)
	{
		client[n] = new SimpleClient();
	}
	for (int n = begin; n < end; n++)
	{
		client[n]->InitSocket();
		client[n]->Connect("127.0.0.1", 8080);
	}

	printf("thread<%d>,Connect<begin=%d, end=%d>\n", id, begin, end);

	readyCount++;
	while (readyCount < tCount)
	{//等待其它线程准备好发送数据
		std::chrono::milliseconds t(10);
		std::this_thread::sleep_for(t);
	}

	Login login[10];
	for (int n = 0; n < 10; n++)
	{
		strcpy(login[n].username, "lyd");
		strcpy(login[n].password, "lydmm");
	}
	const int nLen = sizeof(login);
	while (g_bRun)
	{
		for (int n = begin; n < end; n++)
		{
			if (SOCKET_ERROR != client[n]->sendData(login[3]));
			{
				sendCount++;
			}
			//client[n]->onRun();
		}
	}

	for (int n = begin; n < end; n++)
	{
		client[n]->close();
		delete client[n];
	}

	printf("thread<%d>,exit\n", id);
}

void process(SimpleClient* simple_client) {
	while (true) {
		Login login;
		strcpy((char*)&login.username, "sada");
		strcpy((char*)&login.password, "12345");
		simple_client->sendData(login);
		/*char send_buf[128];
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
			printf("重新输入...\n");
		}*/
	}
}

int main() {
	std::thread t1(cmdThread);
	t1.detach();

	//启动发送线程
	for (int n = 0; n < tCount; n++)
	{
		std::thread t2(sendThread, n + 1);
		t2.detach();
	}
	while (g_bRun);

	printf("已退出。\n");
	return 0;
}