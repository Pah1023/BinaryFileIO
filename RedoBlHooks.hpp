
//////////////////////////////////////////////////
// RedoBlHooks Version 2.2

#pragma once

typedef unsigned char BYTE;
typedef unsigned int  ADDR;

#ifndef RBH_DEBUGPRINT
#define RBH_DEBUGPRINT true
#endif

#define BlFunctionDef(returnType, convention, name, ...) \
	typedef returnType (convention *rbh_##name##FnT)(__VA_ARGS__); \
	rbh_##name##FnT name;

#define BlFunctionDefExtern(returnType, convention, name, ...) \
	typedef returnType (convention *rbh_##name##FnT)(__VA_ARGS__); \
	extern rbh_##name##FnT name;

#define BlFunctionDefIntern(name) \
	rbh_##name##FnT name;

#define BlScanFunctionCode(target, pattern, mask) \
	target = (rbh_##target##FnT)rbh_ScanFunctionCode((char*)pattern, (char*)mask); \
	if(!target){ \
		rbh_BlPrintf("RedoBlHooks | Cannot find function "#target"!"); \
		return false; \
	}else{ \
		if(RBH_DEBUGPRINT) rbh_BlPrintf("RedoBlHooks | Found function "#target" at %08x", (int)target); \
	}

#define BlScanFunctionHex(target, text) \
	target = (rbh_##target##FnT)rbh_ScanFunctionHex((char*)text); \
	if(!target){ \
		rbh_BlPrintf("RedoBlHooks | Cannot find function "#target"!"); \
		return false; \
	}else{ \
		if(RBH_DEBUGPRINT) rbh_BlPrintf("RedoBlHooks | Found function "#target" at %08x", (int)target); \
	}

#define BlScanCode(target, pattern, mask) \
	target = rbh_ScanFunctionCode((char*)pattern, (char*)mask); \
	if(!target){ \
		rbh_BlPrintf("RedoBlHooks | Cannot find pattern "#target"!"); \
		return false; \
	}else{ \
		if(RBH_DEBUGPRINT) rbh_BlPrintf("RedoBlHooks | Found "#target" at %08x", (int)target); \
	}

#define BlScanHex(target, text) \
	target = rbh_ScanFunctionHex((char*)text); \
	if(!target){ \
		rbh_BlPrintf("RedoBlHooks | Cannot find "#target"!"); \
		return false; \
	}else{ \
		if(RBH_DEBUGPRINT) rbh_BlPrintf("RedoBlHooks | Found "#target" at %08x", (int)target); \
	}

#define BlFunctionHookDef(func) \
	BYTE rbh_BlFunctionHook##func##Data[6]; \
	void func##HookOn(){ rbh_HookFunction((ADDR)func, (ADDR)func##Hook, rbh_BlFunctionHook##func##Data); } \
	void func##HookOff(){ rbh_UnhookFunction((ADDR)func, rbh_BlFunctionHook##func##Data); }

#define BlPatchAllMatches(len, patt, mask, repl) \
	rbh_PatchAllMatches(len, (char*)patt, (char*)mask, (char*)repl, RBH_DEBUGPRINT);

#define BlPatchByte(addr, byte) \
	rbh_PatchByte((ADDR)addr, (BYTE)byte);

#define BlPatchBytes(len, addr, repl) \
	rbh_PatchBytes(len, (ADDR)addr, (BYTE*)repl);

#define BlInit if(!rbh_InitInternal()) return false;

#define BlPrintf rbh_BlPrintf

bool rbh_InitInternal();
ADDR rbh_ScanFunctionCode(char*, char*);
ADDR rbh_ScanFunctionHex(char*);
void rbh_HookFunction(ADDR, ADDR, BYTE*);
void rbh_UnhookFunction(ADDR, BYTE*);
int  rbh_PatchAllMatches(unsigned int, char*, char*, char*, bool);
void rbh_PatchByte(ADDR, BYTE);
void rbh_PatchBytes(unsigned int, ADDR, BYTE*);
void rbh_PatchInt(ADDR, int);

BlFunctionDefExtern(void, , rbh_BlPrintf, const char*, ...);
//BlFunctionDefExtern(char* , , rbh_BlCon__execute, int, const char**);
typedef char* (__fastcall *rbh_rbh_BlCon__executeFnT)(int, const char**);
extern rbh_rbh_BlCon__executeFnT rbh_BlCon__execute;

