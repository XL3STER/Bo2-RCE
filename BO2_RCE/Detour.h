#pragma once

#define EXECUTABLE_SECTION __attribute__((section(".text"))); // Cant Execute Data Segment

class Detour
{
public:
	Detour();
	~Detour();
	void* HookFunction(unsigned int Address, void* Hook);
	void RestoreFunction();
	void PatchInJump(unsigned int Address, unsigned int Destination, bool Linked);
private:
	int FunctionOpd[2];
	int IndexDetour;
	static int DetourCount;
	static unsigned char DetourSection[0x1000] EXECUTABLE_SECTION;
	int BranchResolve(int Instruction, int BranchAddress);
	int* StubAddress;
	void* Destination;
	void* HookAddress;
	int OriginalInstructions[4];
	bool isHooked;
};
