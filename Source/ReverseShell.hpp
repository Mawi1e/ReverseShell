#pragma once
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

#include <iostream>
#include <exception>
#include <string>
#include <thread>

/* �ݵ�� WinSock2.h ���� �������־�� �մϴ�. */
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h> // inet_pton()

#define BUFSIZE (0x1000)

namespace mawile {
	class ReverseShell {
	public:
		void Listen(int);
		void Connect(const char*, int);

		ReverseShell();
		ReverseShell(int);
		ReverseShell(const char*, int);
		~ReverseShell();

	private:
		SOCKET svSocket, clSocket;

	};
}