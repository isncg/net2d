#pragma once
#include<WinSock2.h>
#include<WS2tcpip.h>
#include <Windows.h>
#include<string>
#include<vector>

class Winsock {
	WSADATA wsaData;
	std::vector<SOCKET> sockets;
public:
	Winsock() {
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	}

	SOCKET UdpServerSocket(int port, std::string addr_str = "0.0.0.0") {
		SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		inet_pton(AF_INET, addr_str.c_str(), &addr.sin_addr.S_un.S_addr);
		addr.sin_port = htons(port);

		bind(s, (SOCKADDR*)&addr, sizeof(SOCKADDR));
		sockets.push_back(s);
		return s;
	}

	~Winsock() {
		for (int i = 0; i < sockets.size(); i++) {
			closesocket(sockets[i]);
		}
		WSACleanup();
	}
};


DWORD WINAPI RunUDP(LPVOID params);

class NetThread {
	HANDLE hThread = 0;
public:
	NetThread(LPTHREAD_START_ROUTINE entry);
	~NetThread();
};