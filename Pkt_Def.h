#include <stdio.h>
#include <iostream>
#include <iomanip>
const int FORWARD = 1;
const int BACKWARD = 2;
const int RIGHT = 3;
const int LEFT = 4;
const int UP = 5;
const int DOWN = 6;
const int OPEN = 7;
const int CLOSE = 8;
const int HEADERSIZE = 6;  

struct Header
{
	unsigned int PktCount;
	unsigned char Drive : 1;
	unsigned char Status : 1;
	unsigned char Sleep : 1;
	unsigned char Arm : 1;
	unsigned char Claw : 1;
	unsigned char Ack : 1;
	unsigned char Padding : 2;
	unsigned char Length;

};

#pragma pack(push,1)  
struct MotorBody 
{
	char Direction;    
	unsigned int Duration;
};
#pragma pack(pop)  

enum CmdType { DRIVE, SLEEP, ARM, CLAW, ACK };

class PktDef
{
private:
	Header header;
	char * Data;
	unsigned char CRC;
	char *RawBuffer;
public:
	PktDef();
	~PktDef();
	PktDef(char *);
	void SetCmd(CmdType);
	void SetBodyData(char *, int);
	void SetPktCount(int);
	CmdType GetCmd();
	bool GetAck();
	int GetLength();
	char *GetBodyData();
	unsigned int  GetPktCount();  
	bool CheckCRC(char *, int);
	void CalcCRC();
	char *GenPacket();
	bool GetPktStatus();  
	int GetLengthData();
	bool IsNegativePrt();
};

