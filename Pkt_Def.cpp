#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "Pkt_Def.h"
#include <string> 
PktDef::PktDef()
{
	header.PktCount = 0;
	header.Drive = 0;
	header.Status = 0;
	header.Sleep = 0;
	header.Arm = 0;
	header.Claw = 0;
	header.Ack = 0;
	header.Length = 0;
	header.Padding = 0; 
	Data = nullptr;
	CRC = 0;
	RawBuffer = nullptr; 
}

PktDef::PktDef(char *src)
{
	memcpy(&header, src, HEADERSIZE);
	int SizeOfData = header.Length - HEADERSIZE - 1;
	Data = new char[SizeOfData];
	memcpy(Data, src + HEADERSIZE, SizeOfData);
	memcpy(&CRC, src + HEADERSIZE + SizeOfData, sizeof(char));
}

PktDef::~PktDef()
{
	if (Data != 0)
	{
		delete [] Data;
	}
}

void PktDef::SetCmd(CmdType type)
{
	if (type == DRIVE)
		header.Drive = 1;
	else if (type == SLEEP)
		header.Sleep = 1;
	else if (type == ARM)
		header.Arm = 1;
	else if (type == CLAW)
		header.Claw = 1;
	else if (type == ACK)
		header.Ack = 1;
}

void PktDef::SetBodyData(char *src, int size) 
{
	if (size == 0)
		header.Length = HEADERSIZE + 1;
	else
	{
		Data = new char[size];
		for (int i = 0; i < size; i++)
		{
			Data[i] = src[i];
		}
		header.Length = size + HEADERSIZE + 1;
	}


}
void PktDef::SetPktCount(int head)
{
	header.PktCount = head;
}

CmdType PktDef::GetCmd()
{
	if (header.Drive)
		return DRIVE;
	else if (header.Claw)
		return CLAW;
	else if (header.Arm)
		return ARM;
	else if (header.Ack)
		return ACK;
	else
		return SLEEP;
}

bool PktDef::GetAck()
{
	return header.Ack;
}


int PktDef::GetLength()
{
	return header.Length;
}


char* PktDef::GetBodyData()
{
	return Data;
}

unsigned int  PktDef::GetPktCount()    
{
	return header.PktCount;
}

bool  PktDef::GetPktStatus()    
{
	return (header.Status) ? true : false;
}

int PktDef::GetLengthData() 
{
	int SizeOfData = header.Length - HEADERSIZE - 1;
	return SizeOfData;
}

bool PktDef::CheckCRC(char *src, int size)
{
	bool bRes = false;
	int crc = 0;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			crc += (src[i] >> j) & 1;
		}
	}

	if (crc == CRC)
		bRes = true;

	return bRes;
}

int bit_count(unsigned int uValue)
{
	int crc = 0;
	crc = uValue & 1;
	while (uValue >>= 1)
	{
		crc += uValue & 1;
	}
	return crc;
}

void PktDef::CalcCRC()
{
	int crc = 0;

	crc += bit_count(header.PktCount);

	crc += (header.Ack);
	crc += (header.Arm);
	crc += (header.Claw);
	crc += (header.Drive);
	crc += (header.Sleep);
	crc += (header.Status);

	for (int i = 0; i < 8; i++)
	{
		crc += (header.Length >> i) & 1;
	}

	int SizeOfData = header.Length - HEADERSIZE - 1;

	for (int i = 0; i < SizeOfData; i++)
	{
		for (int j = 0; j < 8; j++) {
			crc += (Data[i] >> j) & 1;
		}
	}

	CRC = crc;
}

char* PktDef::GenPacket()  
{
	RawBuffer = new char[header.Length];
	memcpy(RawBuffer, &header, HEADERSIZE); // Header
	int SizeOfData = header.Length - HEADERSIZE - 1;
	if (SizeOfData > 0) 
		memcpy(RawBuffer + HEADERSIZE, Data, SizeOfData);   // Data
	memcpy(RawBuffer + HEADERSIZE + SizeOfData, &CRC, sizeof(char)); // CRC
	return RawBuffer;
}

bool PktDef::IsNegativePrt()
{
	bool bRes = false;
	if (!header.Ack && !header.Arm && !header.Claw && !header.Drive && !header.Sleep && !header.Status)
		bRes = true;
	return bRes;
}