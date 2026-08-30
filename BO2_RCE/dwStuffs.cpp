#include "main.h"

opd_s dwInstantSendMessage_t = { 0x00196000, TOC }; //bo2
void(*dwInstantSendMessage)(int LocalClient, unsigned long long* xuid, int unk, int id, void* data, int size) = (void(*)(int LocalClient, unsigned long long* xuid, int unk, int id, void* data, int size))&dwInstantSendMessage_t;

opd_s getUserID_t = { 0x0081E378, TOC }; //bo2
unsigned long long(*getUserID)(char* name) = (unsigned long long(*)(char* name))&getUserID_t;

opd_s AcceptInvite_t = { 0x52FAA0, TOC };
void(*AcceptInvite)(SceNpId* id, int unk) = (void(*)(SceNpId* id, int unk))&AcceptInvite_t;

int doLookupNpId(const char *onlineIdString, SceNpId *npId)
{
	int32_t ret, tmp;
	int32_t lookupTitleCtxId;

	SceNpOnlineId onlineId;

	memset(&onlineId, 0, sizeof(onlineId));

	strncpy(onlineId.data, onlineIdString, SCE_NET_NP_ONLINEID_MAX_LENGTH);

	int32_t transId = sceNpLookupCreateTransactionCtx(*(int*)(0x2670640));

	//if (transId > 0)
		sceNpLookupNpId(transId, &onlineId, npId, NULL);

	sceNpLookupDestroyTransactionCtx(*(int*)(0x2670640));

	return ret;
}

void ForceJoinUserByName(const char* Name)
{
	SceNpId Id = { 0 };

	doLookupNpId(Name, &Id);

	AcceptInvite(&Id, 1);
}

//0x014B65CB71031AFA //our servers xuid
void JoinDediViaXuid(unsigned long long xuid)
{
	unsigned char joinPatch[16] = {
		0x38, 0x81, 0x0E, 0x8A, 0x38, 0x60, 0x00, 0x00, 0x48, 0x53, 0x1F, 0xD7,
		0x48, 0x88, 0x4E, 0xCA
	};

	memcpy((void*)0x884E90, joinPatch, 16);

	JoinSessionMessage coolBuffer;

	coolBuffer.inviteID = 0;
	coolBuffer.isJoinable = true;
	coolBuffer.inviteInfo.fromMPInvite = LittleLong(1);
	coolBuffer.inviteInfo.isDedicated = LittleLong(1);
	coolBuffer.mType = LittleLong(0);

	unsigned long long localxuid = xuid;

	dwInstantSendMessage(0, &localxuid, 1, 102, &coolBuffer, 92);
}