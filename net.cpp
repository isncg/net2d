#include "net.h"
#include "message.h"

#pragma comment(lib, "Ws2_32.lib")

extern HWND hWnd;



struct TESTINTERFACE {
	float x;
	float y;
};



#include<vector>
#include<string>



#include<iostream>
DWORD WINAPI RunUDP(LPVOID params) {
	Winsock ws;
	auto socket = ws.UdpServerSocket(7777);
	std::vector<char> buffer(4096);
	std::vector<TESTINTERFACE> history;


	while (true) {
		int len = recv(socket, &buffer[0], 4096, 0);
		if (len <= 0) {
			break;
		}
		if (errno) {
			std::cout << "socket error " << errno << std::endl;
		}

		TESTINTERFACE data = *((TESTINTERFACE*)&buffer[0]);
		std::cout << "data " << data.x << "  " << data.y << std::endl;
		history.push_back(data);

		POINT_MESSAGE pm;
		pm.x = data.x;
		pm.y = data.y;
		pm.size = 5;
		SendNetPointMessage(hWnd, 0, pm);

	}
	return 0;
}




void RunUDPThread() {
	HANDLE hThread = CreateThread(NULL, 0, RunUDP, NULL, 0, NULL);
}

NetThread::NetThread(LPTHREAD_START_ROUTINE entry)
{
	hThread = CreateThread(NULL, 0, entry, NULL, 0, NULL);
}

NetThread::~NetThread()
{
	TerminateThread(hThread, 0);
}



//class UdpServerThread {
//	HANDLE hThread = 0;
//public:
//	UdpServerThread(LPTHREAD_START_ROUTINE entry) {
//		hThread = CreateThread(NULL, 0, entry, NULL, 0, NULL);
//	}
//	~UdpServerThread() {
//		TerminateThread(hThread, 0);
//	}
//};