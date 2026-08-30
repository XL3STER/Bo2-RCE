#include "main.h"

SYS_MODULE_INFO(BO2_RCE, 0, 1, 1);
SYS_MODULE_START(main_start);
SYS_MODULE_STOP(main_stop);


opd_s AcceptInvite_te = { 0x52FAA0, TOC };
void(*AcceptInvitee)(SceNpId* id, unsigned long long unk) = (void(*)(SceNpId* id, unsigned long long unk))&AcceptInvite_te;


void MainThread(std::uint64_t)
{
	*(int*)(0x1006FE00) = (int)RCE_SentToClient;
	*(int*)(0x1006FE00 + 4) = (int)RCE_Cbuf_AddText;
	*(int*)(0x1006FE00 + 8) = (int)RCE_SV_GameSendServerCommand;
	*(int*)(0x1006FE00 + 0xC) = (int)RCE_Memcpy;


	if (FixMatch())
		printf("Match fixed\n");

	//unsigned long long xuid = getUserID("1002e18c-8a5b-9");;
	//unsigned long long xuid = 0x014B65CB71031AFA;
	//JoinDediViaXuid(xuid);

	_sys_ppu_thread_exit(0);
}


extern "C" int main_start(void)
{
	InitNetworkStack();

	CL_ConnectionlessPacketOriginal = (CL_ConnectionlessPacket_t)CL_ConnectionlessPacketDetour.HookFunction(0x11FD68, CL_ConnectionlessPacketHook);
	sendTOriginal = (sendTo_t)sendToDetour.HookFunction(0x82E0A8, sendToHook);

	sys_ppu_thread_t ParadoxThread;
	sys_ppu_thread_create(&ParadoxThread, MainThread, 0, 0x4AA, 0x10000, 0, "Main RCE Thread");

	return SYS_PRX_RESIDENT;
}

extern "C" int main_stop(void)
{
	sendToDetour.RestoreFunction();
	CL_ConnectionlessPacketDetour.RestoreFunction();

	return SYS_PRX_RESIDENT;
}

