#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <Winsock2.h> 
#pragma comment(lib, "Ws2_32.lib")

const int DEFAULT_SIZE = 100;
enum SocketType { CLIENT, SERVER };
enum ConnectionType { TCP, UDP };


class MySocket
{
private:
	char *Buffer;
	SOCKET WelcomeSocket, ConnectionSocket;
	struct sockaddr_in SvrAddr;
	SocketType mySocket;
	std::string IPAddr;
	int Port;
	ConnectionType connectionType;
	bool bTCPConnect;
	int MaxSize;
	int server_length;
public:
	MySocket(SocketType, std::string, unsigned int, ConnectionType, unsigned int);
	~MySocket();
	void ConnectTCP();
	void DisconnectTCP();
	void SendData(const char*, int);
	int GetData(char*);
	std::string GetIPAddr();
	void SetIPAddr(std::string);
	void SetPort(int);
	int GetPort();
	SocketType GetType();
	void SetType(SocketType);

};
