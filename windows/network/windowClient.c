#include <winsock2.h>
#include <stdio.h>
#include <Windows.h>

#pragma comment (lib, "Wtsapi32.lib")
#pragma comment (lib, "ws2_32")

BOOL WSASetup(void);
void WSAWrapup(SOCKET TCPserver, SOCKET TCPclient);
void ErrorQuit(PSTR msg, int errCode);
void TCPConnect(SOCKET TCPclient, PSTR ServerIP, int PORT);
void SendMsg(SOCKET TCPClient, char* buf, int len);

void* MemoryBin[2];

int main(void)
{
	SOCKET TCPClient = 0;
	int TCPPort = 9999;
	PSTR ServerIP = "127.0.0.1";
	
	if(!WSASetup())
	{
		ErrorQuit("WSA Setup failed.\n",1);
	}
	else printf("[+] WSA Setup success.\n");
	
	TCPClient = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	// UDPSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	// INVALID_SOCKET is an alias for -1.
	if(TCPClient == INVALID_SOCKET)
	{
		ErrorQuit("Cannot create server socket.",2);
	}
	else printf("[+] TCP Client Socket Created.\n");
	
	TCPConnect(TCPClient, ServerIP, TCPPort);
	
	for(int i = 0; i<7; i++)
	{
		SendMsg(TCPClient, "NEW STR", 7);
	}
	
	WSAWrapup(TCPClient, INVALID_SOCKET);
	
	return 0;
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
	if(TCPserver != INVALID_SOCKET && TCPserver != 0) closesocket(TCPserver);
	if(TCPclient != INVALID_SOCKET && TCPclient != 0) closesocket(TCPclient);
	
	free(MemoryBin[0]);
	// You have to free (SOCKADDR_IN) structs for better implementation.
	WSACleanup();
}

void SendMsg(SOCKET TCPClient, char* buf, int len)
{
	if (len > 1020) ErrorQuit("Cannot Sand More than 1024 bytes",5);
	char* sendBuf = (char*)malloc(len + 4);
	memcpy(sendBuf+4, buf, len);
	
	DWORD netlen = htonl(len);
	memcpy(sendBuf, &netlen, 4);
	
	send(TCPClient, sendBuf, len+4, 0);
	free(sendBuf);
}

void ErrorQuit(PSTR msg, int errCode)
{
	printf("[!] Error : %s\n", msg);
	printf("[!] Quit Program.");
	exit(errCode);
}

void TCPConnect(SOCKET TCPclient, PSTR ServerIP, int PORT)
{
	SOCKADDR_IN* serverAdd = (SOCKADDR_IN*)malloc(sizeof(SOCKADDR_IN));
	MemoryBin[0] = serverAdd;
	
	serverAdd->sin_family = AF_INET;
	serverAdd->sin_addr.s_addr = inet_addr(ServerIP);
	serverAdd->sin_port = htons(PORT);
	
	connect(TCPclient, (SOCKADDR*) serverAdd, sizeof(SOCKADDR_IN));
}