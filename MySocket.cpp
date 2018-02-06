#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "MySocket.h"

MySocket::MySocket(SocketType sockType, std::string ip, unsigned int port, ConnectionType conType, unsigned int mSize)
{
	this->mySocket = sockType;
	this->IPAddr = ip;
	this->Port = port;
	this->connectionType = conType;

	if (mSize != 0)
	{
		MaxSize = mSize; 
	}
	else
	{
		MaxSize = DEFAULT_SIZE;
	}

	Buffer = new char[MaxSize];

	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_port = htons(this->Port);
	SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str());
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		std::cout << "\nCould not start DLLs" << std::endl;
		WSACleanup();
		exit(0);
	}



	if (mySocket == SERVER) {
		if ((this->WelcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		{
			WSACleanup();
			std::cout << "\nCould not initialize socket" << std::endl;
			std::cin.get();
			exit(0);
		}


		if ((bind(this->WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
		{
			closesocket(this->WelcomeSocket);
			WSACleanup();
			std::cout << "\nCould not bind to the socket" << std::endl;
			std::cin.get();
			exit(0);
		}


		if (listen(this->WelcomeSocket, 1) == SOCKET_ERROR)
		{
			closesocket(this->WelcomeSocket);
			WSACleanup();
			std::cout << "\nCould not listen to the provided socket." << std::endl;
			std::cin.get();
			exit(0);
		}
		else {
			std::cout << "\nWaiting for client connection" << std::endl;
		}

		if ((this->ConnectionSocket = accept(this->WelcomeSocket, NULL, NULL)) == SOCKET_ERROR)
		{
			closesocket(this->WelcomeSocket);
			WSACleanup();
			std::cout << "\nCould not accept incoming connection." << std::endl;
			std::cin.get();
			exit(0);
		}
		else
		{
			std::cout << "\nConnection Accepted" << std::endl;
			this->bTCPConnect = 1;
		}
	}



	else 
	{
		//initializes socket
		if ((this->ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		{
			WSACleanup();
			std::cout << "\nCould not initialize socket" << std::endl;
			std::cin.get();
			exit(0);
		}
	}
}

MySocket::~MySocket()
{
	delete[] Buffer;
}

void MySocket::ConnectTCP()
{
	//SvrAddr.sin_family = AF_INET;
	//SvrAddr.sin_port = htons(this->Port);
	//SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str());
	//WSADATA wsaData;
	if ((connect(this->ConnectionSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
		DisconnectTCP();
		std::cout << "\nCould not connect to the server" << std::endl;
		std::cin.get();
		exit(0);
	}

	else
	{
		std::cout << "\nClient connected" << std::endl;
		this->bTCPConnect = 1;
	}
}

void MySocket::DisconnectTCP()
{
	closesocket(this->ConnectionSocket);
	WSACleanup();
}

void MySocket::SendData(const char* tx_buffer, int length) 
{
	if (this->connectionType == TCP) {
		send(this->ConnectionSocket, tx_buffer, length, 0);
	}
	else {
		sendto(this->ConnectionSocket, tx_buffer, length, 0, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr));
	}
}

int MySocket::GetData(char* rx_buffer)  
{
	int iLen = 0;  
	if (this->connectionType == TCP) {
		iLen = recv(this->ConnectionSocket, rx_buffer, MaxSize, 0); 
	}
	else {
		server_length = sizeof(struct sockaddr_in);
		iLen = recvfrom(this->ConnectionSocket, rx_buffer, MaxSize, 0, (sockaddr *)&SvrAddr, &server_length); 
	}
	return iLen; 
}

std::string MySocket::GetIPAddr()
{
	return IPAddr;
}

void MySocket::SetIPAddr(std::string ip)
{
	if (bTCPConnect) {
		this->IPAddr = ip;
	}
	else {
		std::cout << "\nError: Connection has already been established!!!" << std::endl;
	}
}

void MySocket::SetPort(int port)
{
	if (bTCPConnect) {
		this->Port = port;
	}
	else {
		std::cout << "\nError: Connection has already been established!!!" << std::endl;
	}
}

int MySocket::GetPort()
{
	return Port;
}

SocketType MySocket::GetType()
{
	return mySocket;
}

void MySocket::SetType(SocketType socktype)
{
	this->mySocket = socktype;
}

