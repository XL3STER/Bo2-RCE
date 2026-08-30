#pragma once


enum e_JoinRejectionReason
{
	REASON_NONE = 0x0,
	REASON_PARTY_PRIVACY_CLOSED = 0x1,
	REASON_PARTY_PRIVACY_INVITE_ONLY = 0x2,
	REASON_PARTY_PRIVACY_FRIEND_ONLY = 0x3,
	REASON_SYSTEMLINK_LOBBY = 0x4,
};

struct bdSecurityID
{
	char ab[8];
};

struct XNADDR
{
	char addrBuff[37];
};

struct bdSecurityKey
{
	char ab[16];
};

struct XSESSION_INFO
{
	bdSecurityID sessionID;
	XNADDR hostAddress;
	bdSecurityKey keyExchangeKey;
};

struct __declspec(align(4)) InviteMessage
{
	XSESSION_INFO sessionInfo;
	int fromMPInvite;
	bool isDedicated;
};

enum IMType
{
	JOIN_REQUEST = 0x0,
	JOIN_REPLY = 0x1,
	INVITE = 0x2,
	UPDATE_INVITE_INFO = 0x3,
	NUM_INSTANT_MESSAGE_TYPES = 0x4,
};


struct JoinSessionMessage
{
	int mType;
	int inviteID;
	bool isJoinable;
	bool invited;
	int maxLocalPlayersAllowed;
	bool allowGuests;
	e_JoinRejectionReason rejectionReason;
	InviteMessage inviteInfo;
};

void ForceJoinUserByName(const char* Name);
void JoinDediViaXuid(unsigned long long xuid);
int doLookupNpId(const char *onlineIdString, SceNpId *npId);