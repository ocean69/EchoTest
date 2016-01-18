// EchoTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdint.h>
#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
#include <Shlwapi.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")

void Usage()
{
	char *usage = R"(
Usage:
	EchoTest Ip Port NumberOfConnections Interval(in ms) PacketSize(in bytes)
Example:
	EchoTest 127.0.0.1 8080 500 300 300
)";
	printf(usage);
}

sockaddr_in addr;
BOOL Testing = TRUE;
char *testdata;
DWORD g_len;
DWORD interval;

uint32_t WINAPI EchoThread(void* p)
{

RECONNECT:
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		return 0;

	
	int r = connect(sock, (sockaddr*)&addr, sizeof(sockaddr));
	if (r == SOCKET_ERROR) {
		closesocket(sock);
		goto RECONNECT;
	}

	while (Testing) {
		send(sock, testdata, g_len, 0);
		r = recv(sock, testdata, g_len, 0);
		if (r == SOCKET_ERROR) {
			closesocket(sock);
			goto RECONNECT;
		}
		Sleep(interval);
	}

	return 0;
}


void StopTest()
{
	Testing = FALSE;
}

void GenerateRandomData(unsigned long len)
{
	srand(GetTickCount());
	testdata = new char[len];
	for (uint32_t i = 0; i < len; i++) {
		testdata[i] = rand();
	}
}


void StartTest(char *ip, short port, unsigned long numberOfThreads, unsigned long Interval, unsigned long len)
{
	WSADATA WSAData;

	WSAStartup(0x202, &WSAData);
	addr = { 0 };
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	g_len = len;
	GenerateRandomData(len);

	for (uint32_t i = 0; i < numberOfThreads; i++) {
		CloseHandle((HANDLE)_beginthreadex(0, 0, EchoThread, 0, 0, 0));
	}

	return;
}


int main(uint32_t argc, char * argv[])
{
	if (argc < 5) {
		Usage();
		return 0;
	}

	StartTest(argv[1], StrToIntA(argv[2]), StrToIntA(argv[3]), StrToIntA(argv[4]), StrToIntA(argv[5]));
	getchar();
	StopTest();
    return 0;
}

