#pragma once

typedef int(*CL_ConnectionlessPacket_t)(unsigned long long r3, unsigned long long r4, unsigned long long r5, unsigned long long r6, msg_t* r7, unsigned long long r8);
typedef int(*sendTo_t)(unsigned long long r3, unsigned char * r4, unsigned short r5, unsigned long long r6, unsigned long long r7, unsigned long long r8);

extern Detour CL_ConnectionlessPacketDetour;
extern CL_ConnectionlessPacket_t CL_ConnectionlessPacketOriginal;

extern Detour sendToDetour;
extern sendTo_t sendTOriginal;

int CL_ConnectionlessPacketHook(unsigned long long r3, unsigned long long r4, unsigned long long r5, unsigned long long r6, msg_t* r7, unsigned long long r8);

int sendToHook(unsigned long long r3, unsigned char * r4, unsigned short r5, unsigned long long r6, unsigned long long r7, unsigned long long r8);
