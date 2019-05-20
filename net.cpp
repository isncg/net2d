#include "net.h"
#include "message.h"
#include "ui.h"
#pragma comment(lib, "Ws2_32.lib")
//
//extern HWND hWnd;



struct TESTINTERFACE {
	float x;
	float y;
};


struct RADARTARGET {
	float id;
	float x;
	float y;
	float v;
	float acc;
};


struct RADARFRAME {
	float flag;
	float type;
	float version;
	float curtime;
	RADARTARGET targets[32];
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
		SendNetPointMessage(UI::GraphicsArea, 0, pm);

	}
	return 0;
}

DWORD WINAPI RunRadar(LPVOID params) {
	Winsock ws;
	auto socket = ws.UdpServerSocket(7777);
	std::vector<char> buffer(4096);
	std::vector<TESTINTERFACE> history;
	int frame_id = 0;

	while (true) {
		int len = recv(socket, &buffer[0], 2048, 0);
		if (len <= 0) {
			break;
		}
		if (errno) {
			std::cout << "socket error " << errno << std::endl;
		}

		//check flags

		if (buffer[0] == 1 && buffer[1] == 0 && buffer[2] == 1 && buffer[3] == 0) {

		}
		else {
			continue;
		}

		len = recv(socket, &buffer[2048], 2048, 0);
		if (len <= 0) {
			break;
		}
		if (errno) {
			std::cout << "socket error " << errno << std::endl;
		}


		//check flags

		if (buffer[0] == 2 && buffer[1] == 0 && buffer[2] == 2 && buffer[3] == 0) {

		}
		else {
			continue;
		}

		memcpy_s(&buffer[0], 1200, &buffer[4], 1200);
		memcpy_s(&buffer[1200], 1200, &buffer[1208], 1200);

		RADARFRAME* frame = (RADARFRAME*)&buffer[0];


		std::list<POINT_MESSAGE> points;

		for (int i = 0; i < 32; i++) {
			int target_id = frame->targets[i].id;
			if (target_id <= 0) {
				break;
			}
			POINT_MESSAGE pm;
			pm.type = target_id;
			pm.frame_id = frame_id;
			pm.frame_index = i;
			pm.time_stamp = 0;
			pm.x = frame->targets[i].x;
			pm.y = frame->targets[i].y;

			points.push_back(pm);
		}

		if (points.size() > 0) {
			SendNetPointListMessage(UI::GraphicsArea, 0, points);
		}

		/*TESTINTERFACE data = *((TESTINTERFACE*)&buffer[0]);
		std::cout << "data " << data.x << "  " << data.y << std::endl;
		history.push_back(data);

		POINT_MESSAGE pm;
		pm.x = data.x;
		pm.y = data.y;
		pm.size = 5;
		SendNetPointMessage(hWnd, 0, pm);*/

	}
	return 0;
}

//
//void RunUDPThread() {
//	HANDLE hThread = CreateThread(NULL, 0, RunUDP, NULL, 0, NULL);
//}

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