#include "main.h"

#pragma control %push notocrestore=2


Detour sendToDetour;
sendTo_t sendTOriginal;

Detour CL_ConnectionlessPacketDetour;
CL_ConnectionlessPacket_t CL_ConnectionlessPacketOriginal;

int CL_ConnectionlessPacketHook(unsigned long long r3, unsigned long long r4, unsigned long long r5, unsigned long long r6, msg_t* r7, unsigned long long r8)
{
	int isGamePacket = CL_ConnectionlessPacketOriginal(r3, r4, r5, r6, r7, r8);

	//printf("%s\n", r7->data + 4);

	if (!isGamePacket && r7 && r7->data && *(int*)(0x10070000) == 0)
	{
		memcpy((void*)0x10070004, r7->data, r7->cursize);

		*(int*)0x10070000 = 0x1;
	}
	return isGamePacket;
}

int sendToHook(unsigned long long r3, unsigned char * r4, unsigned short r5, unsigned long long r6, unsigned long long r7, unsigned long long r8)
{
	unsigned char ip[4] = { 0 };

	memcpy(ip, (void*)(int)&r4, 0x4);

	//printf("%i.%i.%i.%i:%i %X %X\n", ip[0], ip[1], ip[2], ip[3], r5, r6, r7);


	//return sendTOriginal(r3, (unsigned char*)0x05c427a3, 42340, r6, r7, r8);

	return sendTOriginal(r3, r4, r5, r6, r7, r8);
}

#pragma control %pop notocrestore