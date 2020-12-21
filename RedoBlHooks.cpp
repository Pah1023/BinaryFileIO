
//////////////////////////////////////////////////
// RedoBlHooks Version 2.2

//////////////////////////////////////////////////
// Includes

#include <windows.h>
#include <psapi.h>

#include "RedoBlHooks.hpp"

//////////////////////////////////////////////////
// Sig Scanning

ADDR ImageBase;
ADDR ImageSize;

void InitScanner() {
	HMODULE module = GetModuleHandle(NULL);
	if (module) {
		MODULEINFO info;
		GetModuleInformation(GetCurrentProcess(), module, &info, sizeof(MODULEINFO));
		ImageBase = (ADDR)info.lpBaseOfDll;
		ImageSize = info.SizeOfImage;
	}
}

bool CompareData(BYTE* data, BYTE* pattern, char* mask) {
	for (; *mask; ++data, ++pattern, ++mask) {
		if (*mask == 'x' && *data != *pattern)
			return false;
	}
	return (*mask) == 0;
}

ADDR FindPattern(ADDR imageBase, ADDR imageSize, BYTE* pattern, char* mask) {
	for (ADDR i = imageBase; i < imageBase + imageSize; i++) {
		if (CompareData((PBYTE)i, pattern, mask)) {
			return i;
		}
	}
	return 0;
}

// Public

ADDR rbh_ScanFunctionCode(char* pattern, char* mask) {
	return FindPattern(ImageBase, ImageSize - strlen(mask), (BYTE*)pattern, mask);
}

ADDR rbh_ScanFunctionHex(char* text) {
	unsigned int len = strlen(text);
	char* patt = new char[len];
	char* mask = new char[len];

	int outidx = 0;
	int val = 0;
	bool uk = false;
	for (unsigned int i = 0; i < len; i++) {
		char c = text[i];
		if (c == '?') {
			uk = true;
		}
		else if (c >= '0' && c <= '9') {
			val = (val << 4) + (c - '0');
		}
		else if (c >= 'A' && c <= 'F') {
			val = (val << 4) + (c - 'A' + 10);
		}
		else if (c >= 'a' && c <= 'f') {
			val = (val << 4) + (c - 'a' + 10);
		}
		else if (c == ' ') {
			patt[outidx] = uk ? 0 : val;
			mask[outidx] = uk ? '?' : 'x';
			val = 0;
			uk = false;
			outidx++;
		}
	}

	patt[outidx] = uk ? 0 : val;
	mask[outidx] = uk ? '?' : 'x';
	outidx++;
	patt[outidx] = 0;
	mask[outidx] = 0;

	ADDR res = rbh_ScanFunctionCode(patt, mask);

	delete(patt);
	delete(mask);

	return res;
}

//////////////////////////////////////////////////
// Call Patching and Hooking

void rbh_PatchByte(ADDR location, BYTE value) {
	DWORD oldProtection;
	VirtualProtect((void*)location, 1, PAGE_EXECUTE_READWRITE, &oldProtection);
	*((BYTE*)location) = value;
	VirtualProtect((void*)location, 1, oldProtection, &oldProtection);
}

void rbh_PatchBytes(unsigned int len, ADDR location, BYTE* repl) {
	for (unsigned int i = 0; i < len; i++) {
		rbh_PatchByte(location + i, repl[i]);
	}
}

void rbh_PatchInt(ADDR addr, int rval) {
	for (unsigned int i = 0; i < 4; i++) {
		BYTE repl = (rval >> (i * 8)) & 0xFF;
		rbh_PatchByte(addr + i, repl);
	}
}

int CallOffset(ADDR instr, ADDR func) {
	return func - (instr + 4);
}

void PatchCall(ADDR instr, ADDR target) {
	rbh_PatchInt(instr, CallOffset(instr, target));
}

void PatchCopy(ADDR dest, ADDR src, unsigned int len) {
	for (unsigned int i = 0; i < len; i++) {
		rbh_PatchByte(dest + i, *((BYTE*)(src + i)));
	}
}

void rbh_HookFunction(ADDR victim, ADDR detour, BYTE* origbytes) {
	memcpy(origbytes, (BYTE*)victim, 6); //save old data

	rbh_PatchByte(victim, 0xE9); //call
	PatchCall(victim + 1, detour); //detour func with call offset
	rbh_PatchByte(victim + 5, 0xC3); //retn
}

void rbh_UnhookFunction(ADDR victim, BYTE* origbytes) {
	PatchCopy(victim, (ADDR)origbytes, 6); //restore old data
}

int rbh_PatchAllMatches(unsigned int len, char* pattern, char* mask, char* replace, bool debugprint) {
	int numpatched = 0;
	for (ADDR i = ImageBase; i < ImageBase + ImageSize - len; i++) {
		if (CompareData((BYTE*)i, (BYTE*)pattern, mask)) {
			if (debugprint) BlPrintf("RedoBlHooks: Patching call at %08x", i);

			numpatched++;
			for (ADDR c = 0; c < len; c++) {
				rbh_PatchByte(i + c, replace[c]);
			}
		}
	}
	return numpatched;
}

//////////////////////////////////////////////////
// Init

BlFunctionDefIntern(rbh_BlPrintf);
BlFunctionDefIntern(rbh_BlCon__execute);
bool rbh_InitInternal() {
	InitScanner();

	rbh_BlPrintf = (rbh_rbh_BlPrintfFnT)rbh_ScanFunctionHex((char*)"8D 44 24 08 33 D2 50 FF 74 24 08 33 C9 E8 ? ? ? ? 83 C4 08 C3");
	rbh_BlCon__execute = (rbh_rbh_BlCon__executeFnT)rbh_ScanFunctionHex((char*)"51 56 57 8B F2 8B F9 6A 00");
	if (!rbh_BlPrintf) return false;

	return true;
}

