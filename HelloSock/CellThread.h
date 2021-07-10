#ifndef _CELLTHREAD_H_
#define _CELLTHREAD_H_

#include <functional>
#include <thread>
#include "CELLSemaphore.h"

class CellThread
{
private:
	typedef std::function<void(CellThread*)> EventCall;
public:
	//�����߳�
	void Start(EventCall onCreate = nullptr, EventCall onRun = nullptr, EventCall onDestory = nullptr);

	//�ر��߳�
	void Close();
	//�ڹ����������˳�
	//����Ҫʹ���ź����������ȴ�
	//���ʹ�û�����
	void Exit();

	//�߳��Ƿ���������״̬
	bool isRun();
protected:
	//�߳�����ʱ�Ĺ�������
	void OnWork();
private:
	EventCall _onCreate;
	EventCall _onRun;
	EventCall _onDestory;
	//��ͬ�߳��иı�����ʱ��Ҫ����
	std::mutex _mutex;
	//�����̵߳���ֹ���˳�
	CellSemaphore _sem;
	//�߳��Ƿ�����������
	bool	_isRun = false;
};
#endif // !_CELLTHREAD_H_



