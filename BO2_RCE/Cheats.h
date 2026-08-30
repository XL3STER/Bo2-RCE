#pragma once

#define MEM_COMMIT                      0x00001000  
#define MEM_RESERVE                     0x00002000  
#define PAGE_EXECUTE_READWRITE			0x40
#define CalculateRelativeJMPAddress(X, Y) (((int)Y - (int)X) - 5)


//
struct SessionStaticData
{
	char *sessionName;
	bool registerUsersWithVoice;
	bool isDedicated;
};

struct SessionData
{
	SessionStaticData staticData;
	//SessionDynamicData dyn;
};

extern int(*CG_GetClientNum)(int localClientNum);
extern void(*Cbuf_AddText)(int LocalClient, char* cmd);

void PathSlCrash();
int InstallListener();
bool ReadFileToMemory(char* Name, int* Buffer, int* Size);
bool RemoteExecuteCode(unsigned char* function, int Size);
unsigned char* GetServerMemory(int Address, int Size);

void RCE_WriteFloat(unsigned int Address, float Value);
void RCE_CreateThreadAtAddress(int Address);
void RCE_CallFunctionAtPointerWithParams(int Pointer, int p1, int p2);
void RCE_WriteToPointer(int Pointer, int Offset, int Value);
void RCE_CallFunctionAtPointer(int Pointer);
void RCE_SentToClient(int ClientNumber, int Address, int Lenght);
void RCE_VirtualAlloc(int Size);
void RCE_SV_GameSendServerCommand(int ClientNumber, char* Command);
void RCE_Cbuf_AddText(char* Command);
void RCE_WriteString(int Address, const char* string);
void RCE_Memcpy(int dst, unsigned char* src, int size);

void RCE_WriteUInt8(unsigned int Address, unsigned char Value);
void RCE_WriteUInt32(unsigned int Address, unsigned int Value);
void RCE_WriteUInt16(unsigned int Address, unsigned short Value);

int GetWeaponIndex(char* name);
void G_GivePlayerWeapon(int playerstate, int weapon);
bool FixMatch();

bool FixSV_SanitizeName();
bool FixClientUserinfoChanged();
bool FixNames();
void ExecutePayload(unsigned char* payload, int size);

void RCE_WriteIntToClient(int clientNum, unsigned int address, unsigned int value);

void ExecuteCrashlessChain(std::vector<unsigned int> chain);

extern SessionData* g_serverSession;