#include <iostream>
#include "MySocket.h"
#include "Pkt_Def.h"
#include <thread>   

void Telemetry();

bool ExeComplete = false; 
bool bFlagTelemetry = false;

bool DisplayTelemetry(char *Packet, int iSize); 
void Telemetry();
void Command();

void Command() 
{
	PktDef * pPkt = nullptr;
	CmdType type;
	MotorBody motorBody;
	int command, direction, duration;
	char *rec, ret[50];
	unsigned int uNumberPkt = 0;
	MySocket ClientSocket(SocketType::CLIENT, "127.0.0.1", 27000, ConnectionType::TCP, 100);
	ClientSocket.ConnectTCP();
	do  
	{
		if (!pPkt)
		{
			delete[] pPkt;
		}
		if (!bFlagTelemetry)
		{
			duration = 0;
			std::cout << "\nInput a command (DRIVE 1, SLEEP 2, ARM 3, CLAW 4):";
			std::cin >> command;
			if (command == 1)
				 std::cout << "\nInput a direction (FORWARD 1, BACKWARD 2, RIGHT 3, LEFT 4):";
			else if (command == 3)
				std::cout << "\nInput a direction (UP 5, DOWN 6):";
			else if (command == 4)
				std::cout << "\nInput a direction (OPEN 7, CLOSE 8):";

			if (command == 1 || command == 3 || command == 4)
				std::cin >> direction;

			if (command == 1)
			{
				std::cout << "\nInput a duration:";
				std::cin >> duration;
			}

			if (command == 1 || command == 3 || command == 4)
			{
				motorBody.Direction = direction;
				motorBody.Duration = duration;
			}

			switch (command)
			{
			case 1:
				type = DRIVE;
				break;
			case 2:
				type = SLEEP;
				break;
			case 3:
				type = ARM;
				break;
			case 4:
				type = CLAW;
				break;
			case 5:
				type = ACK;
				break;
			}
			pPkt = new PktDef();
			pPkt->SetCmd(type);
			switch (command)
			{
			case 1:
			case 3:
			case 4:
				pPkt->SetBodyData((char *)&motorBody, sizeof(MotorBody));
				break;
			default:
				pPkt->SetBodyData(NULL, 0);
			}

			pPkt->SetPktCount(++uNumberPkt);
			pPkt->CalcCRC();
			rec = pPkt->GenPacket();

			ClientSocket.SendData(rec, pPkt->GetLength());

			ClientSocket.GetData(ret);
			PktDef newPkt(ret);

			if (newPkt.IsNegativePrt()) 
				std::cout << "\nNegative acknoledgement";

			bFlagTelemetry = true;
		}

	} while (pPkt->GetCmd() != SLEEP);

	ClientSocket.DisconnectTCP();
}

void Telemetry() 
{
	char ret[128];
	PktDef newPkt;
	MySocket ClientSocket(SocketType::CLIENT, "127.0.0.1", 27501, ConnectionType::TCP, 100);
	ClientSocket.ConnectTCP();
	while (1)
	{
		if (bFlagTelemetry)
		{
			int iSize = ClientSocket.GetData(ret);
			DisplayTelemetry(ret, iSize);
			bFlagTelemetry = false;
		}
	}
}

int main()
{
	std::thread(Command).detach(); 
	std::thread(Telemetry).detach();
	while (ExeComplete == false);
}

bool DisplayTelemetry(char *Packet, int iSizeOfPacket) 
{
	bool bRes = true;

	struct pktBody
	{
		char ssd[2];
		char apd[2];
		unsigned char drive : 1;
		unsigned char ArmUp : 1;
		unsigned char ArmDown : 1;
		unsigned char ClawOpen : 1;
		unsigned char ClawClose : 1;
		unsigned char Padding : 3;
	};

	pktBody body;

	PktDef newPkt = PktDef(Packet);

	if (newPkt.CheckCRC(Packet, newPkt.GetLength() - 1))
	{
		std::cout << "\nCRC check passed!" << std::endl;
	}
	else
	{
		std::cout << "\nCRC failed!" << std::endl;
		bRes = false;
	}

	if (bRes)
	{
		if (newPkt.GetPktStatus()) 
		{
			std::cout << "\nStatus checked = passed!" << std::endl;
		}
		else
		{
			std::cout << "\nStatus check = failed!" << std::endl;
			bRes = false;
		}

		if (bRes)
		{
			int iSizeOfData = newPkt.GetLengthData();
			memcpy(&body, newPkt.GetBodyData(), iSizeOfData);
			body.Padding = 0x0;

		//	std::cout << "\nSonar Sensor Data: " << std::endl;
		//	for (int i = 0; i < 2; i++)
		//	{
		//		std::cout << std::hex << std::setw(4) << (unsigned int)*&body.ssd[i] << ", ";
		//	}
		//	std::cout << std::endl;

		//	std::cout << "\nArm Position Data: " << std::endl;
		//	for (int i = 0; i < 2; i++)
		//		std::cout << std::hex << std::setw(4) << (unsigned int)*&body.apd[i] << ", ";
		//	std::cout << std::endl;

		//	if (body.drive == 0)
		//		std::cout << "\nDrive flag bit : " << 0 << std::endl;
		//	else
		//		std::cout << "\nDrive flag bit : " << 1 << std::endl;

			if (body.ArmUp == 1)
				std::cout << "\nArm is Up, ";
			else
				std::cout << "\nArm is Down, ";

			if (body.ClawClose == 1)
				std::cout << "\nClaw is Closed";
			else
				std::cout << "\nClaw is Open";
		}
	}
	return bRes;
}
