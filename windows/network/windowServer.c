#include <winsock2.h>
#include <stdio.h>

#pragma comment (lib, "Wtsapi32.lib")
#pragma comment (lib, "ws2_32")

BOOL WSASetup(void);
void WSAWrapup(SOCKET TCPserver, SOCKET TCPclient);
void ErrorQuit(PSTR msg, int errCode);
SOCKET WaitClient(SOCKET TCPServer);
BOOL BindSocket(SOCKET s, int port);

void* MemoryBin[2];

int main(void)
{
	SOCKET TCPServer = 0;
	SOCKET TCPClient = 0;
	int TCPPort = 9999;
	
	if(!WSASetup())
	{
		ErrorQuit("WSA Setup failed.\n",1);
	}
	else printf("[+] WSA Setup success.\n");
	
	TCPServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	// UDPSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	// INVALID_SOCKET is an alias for -1.
	if(TCPServer == INVALID_SOCKET)
	{
		ErrorQuit("Cannot create server socket.",2);
	}
	else printf("[+] TCP Server Socket Created.\n");
	
	if(!BindSocket(TCPServer,TCPPort))
	{
		ErrorQuit("Cannot Bind Socket.",3);
	}
	else printf("[+] Socket Bind success. [PORT %d]\n",TCPPort);
	
	TCPClient = WaitClient(TCPServer);
	
	if(TCPClient == INVALID_SOCKET)
	{
		ErrorQuit("Cannot create client socket.",4);
	}
	else printf("[+] TCP Client Socket Created.\n");
	
	int recvCount = 10;
	while(recvCount > 0)
	{
		
		DWORD netlen = 0;
		char* buf = 0;
		int number = recv(TCPClient, (char*)&netlen, 4, 0);
		int len = ntohl(netlen);
		if(len == 0) continue;
		
		buf = (char*)calloc(len, sizeof(char));
		recv(TCPClient, buf, len, 0);
		
		printf("[+ %d] RECV [%d bytes] : %s\n", (10-recvCount), len, buf);
		free(buf);
		recvCount = recvCount-1;
	}
	
	WSAWrapup(TCPServer, TCPClient);
	
	return 0;
}

SOCKET WaitClient(SOCKET TCPserver)
{
	SOCKET TCPclient = 0;
	SOCKADDR_IN* ADDclient = (SOCKADDR_IN*)malloc(sizeof(SOCKADDR_IN));
	MemoryBin[0] = ADDclient;
	
	listen(TCPserver, 1);
	int clientSize = sizeof(SOCKADDR_IN);
	TCPclient = accept(TCPserver, (SOCKADDR*) ADDclient, &clientSize);
	
	return TCPclient;
}

BOOL WSASetup(void)
{
	WORD version;
	WSADATA data;
	
	version = MAKEWORD(2,2);
	int result = -1;
	/*
		MAKEWORD(0x02, 0x02) == 0x0202
		MAKEWORD(0x01, 0x03) == 0x0103
		
		This Macro is defined in Windef.h
		Used to specify 2.2 version is required for connection.
	*/
	
	result = WSAStartup(version, &data);
	if (result == 0) return TRUE;
	else return FALSE;
}

void WSAWrapup(SOCKET TCPserver, SOCKET TCPclient)
{
	if(TCPserver != INVALID_SOCKET) closesocket(TCPserver);
	if(TCPclient != INVALID_SOCKET) closesocket(TCPclient);
	
	free(MemoryBin[0]);
	free(MemoryBin[1]);
	// You have to free (SOCKADDR_IN) structs for better implementation.
	WSACleanup();
}

void ErrorQuit(PSTR msg, int errCode)
{
	printf("[!] Error : %s\n", msg);
	printf("[!] Quit Program.");
	exit(errCode);
}

BOOL BindSocket(SOCKET s, int port)
{
	//SOCKADDR_IN sturct is used for AF_INET socket.
	//SOCKET_ERROR is an alias for -1.
	int result = SOCKET_ERROR;
	
	SOCKADDR_IN* NewAddr = (SOCKADDR_IN*)malloc(sizeof(SOCKADDR_IN));
	MemoryBin[1] = NewAddr;
	
	memset(NewAddr, 0, sizeof(SOCKADDR_IN));
	NewAddr->sin_family = AF_INET;
	NewAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	NewAddr->sin_port = htons(port);
	
	result = bind(s, (SOCKADDR*)NewAddr, sizeof(SOCKADDR_IN));
	if(result == 0) return TRUE;
	else return FALSE;
}
