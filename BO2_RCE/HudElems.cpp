#include "main.h"

void RCE_HudElem_Alloc(int clientNum, int teamNum)
{
	if (!g_serverSession->staticData.isDedicated) return;

	ROP Chain;
	Chain.SetUpChain();

	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(0x005958A0 - 0xC00);			//HudElem_Alloc
	Chain.Push(0x00510448 - 0xC00);			//call eax
	Chain.Push(clientNum);					//clientNum
	Chain.Push(teamNum);					//teamNum
	Chain.Push(0x009DD740 - 0xC00);			//pop esi
	Chain.Push(0x00F9F390);					//empty address 0x00F9F390
	Chain.Push(0x006B8EC5);					//weaponOptions
	Chain.Push(0x00642F5D - 0xC00);
	Chain.Push(0x00642F5D - 0xC00);
	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();
}

void RCE_G_LocalizedStringIndex(const char* Material)
{
	if (!g_serverSession->staticData.isDedicated) return;

	RCE_WriteString(0x00F9F350, Material);

	ROP Chain;
	Chain.SetUpChain();

	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(0x005FFA60 - 0xC00);			//G_LocalizedStringIndex
	Chain.Push(0x00510448 - 0xC00);			//call eax
	Chain.Push(0x00F9F350);					//string
	Chain.Push(0x00F9F350);					//string
	Chain.Push(0x009DD740 - 0xC00);			//pop esi
	Chain.Push(0x00F9F390);					//empty address 0x00F9F390
	Chain.Push(0x006B8EC5);					//weaponOptions
	Chain.Push(0x00642F5D - 0xC00);
	Chain.Push(0x00642F5D - 0xC00);
	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();
}

void RCE_G_MaterialIndex(const char* Material)
{
	if (!g_serverSession->staticData.isDedicated) return;

	RCE_WriteString(0x00F9F350, Material);


	ROP Chain;
	Chain.SetUpChain();

	Chain.Push(0x009621C3 - 0xC00);			//pop eax
	Chain.Push(0x00F9F350);					//0x00F9F350
	Chain.Push(0x00960247 - 0xC00);			//pop edx
	Chain.Push(0x005FFB60 - 0xC00);			//G_MaterialIndex
	Chain.Push(0x009B0429 - 0xC00);			//call edx
	Chain.Push(0x009DD740 - 0xC00);			//pop esi
	Chain.Push(0x00F9F390);					//empty address 0x00F9F390
	Chain.Push(0x006B8EC5);					//weaponOptions
	Chain.Push(0x00642F5D - 0xC00);
	Chain.Push(0x00642F5D - 0xC00);
	Chain.Push(0x00666498 - 0xC00);

	Chain.ExecuteChain();
	Chain.ClearChain();
}

int GetAlloc_HudElemen(int clientNum, int teamNum)
{
	if (!g_serverSession->staticData.isDedicated) return NULL;  

	RCE_WriteUInt32(0x00F9F390, 0);
	Sleep(10);
	RCE_HudElem_Alloc(clientNum, teamNum);

	int Allocated = 0;

	while ((Allocated = LittleLong(*(int*)GetServerMemory(0x00F9F390, 0x4))) == 0) { Sleep(1); }

	return Allocated;
}

int GetMaterialIndex(const char* Material)
{
	if (!g_serverSession->staticData.isDedicated) return NULL;

	RCE_WriteUInt32(0x00F9F390, 0);
	Sleep(10);
	RCE_G_MaterialIndex(Material);

	int Allocated = 0;

	while ((Allocated = LittleLong(*(int*)GetServerMemory(0x00F9F390, 0x4))) == 0) { Sleep(1); }

	return Allocated;
}

int GetLocalizedStringIndex(const char* String)
{
	if (!g_serverSession->staticData.isDedicated) return NULL;

	RCE_WriteUInt32(0x00F9F390, 0);
	Sleep(10);
	RCE_G_LocalizedStringIndex(String);

	int Allocated = 0;

	while ((Allocated = LittleLong(*(int*)GetServerMemory(0x00F9F390, 0x4))) == 0) { Sleep(1); }

	return Allocated;
}

game_hudelem_s* setShader(int clientNum, const char* material, int width, int height, float x, float y, float sort, color_t color) 
{
	game_hudelem_s* Element = (game_hudelem_s*)GetAlloc_HudElemen(clientNum, 0);

	RCE_WriteUInt8((unsigned int)&Element->elem.type, HE_TYPE_MATERIAL);
	RCE_WriteUInt8((unsigned int)&Element->elem.materialIndex, GetMaterialIndex(material));
	RCE_WriteUInt16((unsigned int)&Element->elem.width, width);
	RCE_WriteUInt16((unsigned int)&Element->elem.height, height);
	RCE_WriteFloat((unsigned int)&Element->elem.x, x);
	RCE_WriteFloat((unsigned int)&Element->elem.y, y);
	RCE_WriteFloat((unsigned int)&Element->elem.sort, sort);
	RCE_WriteUInt32((unsigned int)&Element->elem.color, LittleLong(*(unsigned int*)(&color)));
	RCE_WriteUInt8((unsigned int)&Element->elem.ui3dWindow, 0xFF);

	return Element;
}

game_hudelem_s* setText(int clientNum, const char* text, int font, float fontScale, float x, float y, int allign, float sort, color_t color, color_t glowColor) 
{
	game_hudelem_s* Element = (game_hudelem_s*)GetAlloc_HudElemen(clientNum, 0);

	RCE_WriteUInt8((unsigned int)&Element->elem.type, HE_TYPE_TEXT);
	RCE_WriteUInt16((unsigned int)&Element->elem.text, GetLocalizedStringIndex(text));
	RCE_WriteFloat((unsigned int)&Element->elem.x, x);
	RCE_WriteFloat((unsigned int)&Element->elem.y, y);
	RCE_WriteFloat((unsigned int)&Element->elem.sort, sort);
	RCE_WriteUInt8((unsigned int)&Element->elem.alignOrg, allign);
	RCE_WriteUInt8((unsigned int)&Element->elem.alignScreen, 0x01);
	RCE_WriteFloat((unsigned int)&Element->elem.fontSize, fontScale);
	RCE_WriteUInt8((unsigned int)&Element->elem.Font, font);
	RCE_WriteUInt32((unsigned int)&Element->elem.color, LittleLong(*(unsigned int*)(&color)));
	RCE_WriteUInt32((unsigned int)&Element->elem.glowColor, LittleLong(*(unsigned int*)(&glowColor)));
	RCE_WriteUInt8((unsigned int)&Element->elem.ui3dWindow, 0xFF);
	return Element;
}