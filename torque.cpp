#include <stdarg.h>
#include <stdio.h>

#include "torque.hpp"

ADDR tsf_mCacheSequence;
ADDR tsf_mCacheAllocator;
ADDR tsf_gIdDictionary;
ADDR tsf_gEvalState_globalVars;

BlFunctionDefIntern(tsf_BlStringTable__insert);
BlFunctionDefIntern(tsf_BlNamespace__find);
BlFunctionDefIntern(tsf_BlNamespace__createLocalEntry);
BlFunctionDefIntern(tsf_BlDataChunker__freeBlocks);
BlFunctionDefIntern(tsf_BlCon__evaluate);
BlFunctionDefIntern(tsf_BlCon__executef);
BlFunctionDefIntern(tsf_BlCon__executefSimObj);
BlFunctionDefIntern(tsf_BlCon__getVariable);
BlFunctionDefIntern(tsf_BlDictionary__addVariable);
BlFunctionDefIntern(tsf_BlSim__findObject_name);
BlFunctionDefIntern(tsf_BlStringStack__getArgBuffer);
BlFunctionDefIntern(tsf_Con__getReturnBuffer);
char* tsf_GetIntArg(signed int value)
{
	char* ret = tsf_BlStringStack__getArgBuffer(16);
	snprintf(ret, 16, "%d", value);

	return ret;
}

char* tsf_GetFloatArg(float value)
{
	char* ret = tsf_BlStringStack__getArgBuffer(32);
	snprintf(ret, 32, "%g", value);

	return ret;
}

char* tsf_ScriptThis(ADDR obj)
{
	return tsf_GetIntArg(*(signed int*)(obj + 32));
}

const char* tsf_Eval(const char* code)
{
	const char* argv[] = { nullptr, code };
	return tsf_BlCon__evaluate(0, 2, argv);
}

const char* tsf_Evalf(const char* fmt, ...)
{
	va_list args;
	char code[4096];
	va_start(args, fmt);
	vsnprintf(code, 4096, fmt, args);
	va_end(args);

	return tsf_Eval((const char*)code);
}

ADDR tsf_FindObject(unsigned int id)
{
	ADDR obj = *(ADDR*)(*(ADDR*)(tsf_gIdDictionary)+4 * (id & 0xFFF));

	if (!obj)
		return 0;

	while (obj && *(unsigned int*)(obj + 32) != id)
	{
		obj = *(ADDR*)(obj + 16);
		if (!obj)
			return 0;
	}

	return obj;
}

ADDR tsf_FindObject(const char* name)
{
	return (ADDR)tsf_BlSim__findObject_name(name);
}

ADDR tsf_LookupNamespace(const char* ns, const char* package)
{
	const char* ste_package;
	if (package)
		ste_package = tsf_BlStringTable__insert(package, 0);
	else
		ste_package = nullptr;

	if (ns)
	{
		const char* ste_namespace = tsf_BlStringTable__insert(ns, 0);
		return tsf_BlNamespace__find(ste_namespace, ste_package);
	}
	else
		return tsf_BlNamespace__find(nullptr, ste_package);
}

ADDR tsf_AddConsoleFuncInternal(const char* pname, const char* cname, const char* fname, signed int cbtype, const char* usage, signed int mina, signed int maxa)
{
	const char* ste_fname = tsf_BlStringTable__insert(fname, 0);
	ADDR ns = tsf_LookupNamespace(cname, pname);
	ADDR ent = tsf_BlNamespace__createLocalEntry(ns, ste_fname);

	*(signed int*)tsf_mCacheSequence += 1;
	tsf_BlDataChunker__freeBlocks(*(ADDR*)tsf_mCacheAllocator);

	*(const char**)(ent + 24) = usage;
	*(signed int*)(ent + 16) = mina;
	*(signed int*)(ent + 20) = maxa;
	*(signed int*)(ent + 12) = cbtype;

	return ent;
}

const char* tsf_GetVar(const char* name)
{
	return tsf_BlCon__getVariable(name);
}

void tsf_AddVarInternal(const char* name, signed int varType, void* data)
{
	tsf_BlDictionary__addVariable((ADDR*)tsf_gEvalState_globalVars, name, varType, data);
}

void tsf_AddVar(const char* name, const char** data)
{
	tsf_AddVarInternal(name, 10, data);
}

void tsf_AddVar(const char* name, signed int* data)
{
	tsf_AddVarInternal(name, 4, data);
}

void tsf_AddVar(const char* name, float* data)
{
	tsf_AddVarInternal(name, 8, data);
}

void tsf_AddVar(const char* name, bool* data)
{
	tsf_AddVarInternal(name, 6, data);
}

void tsf_AddConsoleFunc(const char* pname, const char* cname, const char* fname, tsf_StringCallback sc, const char* usage, signed int mina, signed int maxa)
{
	ADDR ent = tsf_AddConsoleFuncInternal(pname, cname, fname, 1, usage, mina, maxa);
	*(tsf_StringCallback*)(ent + 40) = sc;
}

void tsf_AddConsoleFunc(const char* pname, const char* cname, const char* fname, tsf_IntCallback ic, const char* usage, signed int mina, signed int maxa)
{
	ADDR ent = tsf_AddConsoleFuncInternal(pname, cname, fname, 2, usage, mina, maxa);
	*(tsf_IntCallback*)(ent + 40) = ic;
}

void tsf_AddConsoleFunc(const char* pname, const char* cname, const char* fname, tsf_FloatCallback fc, const char* usage, signed int mina, signed int maxa)
{
	ADDR ent = tsf_AddConsoleFuncInternal(pname, cname, fname, 3, usage, mina, maxa);
	*(tsf_FloatCallback*)(ent + 40) = fc;
}

void tsf_AddConsoleFunc(const char* pname, const char* cname, const char* fname, tsf_VoidCallback vc, const char* usage, signed int mina, signed int maxa)
{
	ADDR ent = tsf_AddConsoleFuncInternal(pname, cname, fname, 4, usage, mina, maxa);
	*(tsf_VoidCallback*)(ent + 40) = vc;
}

void tsf_AddConsoleFunc(const char* pname, const char* cname, const char* fname, tsf_BoolCallback bc, const char* usage, signed int mina, signed int maxa)
{
	ADDR ent = tsf_AddConsoleFuncInternal(pname, cname, fname, 5, usage, mina, maxa);
	*(tsf_BoolCallback*)(ent + 40) = bc;
}

bool tsf_InitInternal()
{
	BlScanFunctionHex(tsf_BlStringTable__insert, "83 EC 0C 80 3D ? ? ? ? ?");
	BlScanFunctionHex(tsf_BlNamespace__find, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 0C 53 56 57 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 8B DA 8B D1");
	BlScanFunctionHex(tsf_BlNamespace__createLocalEntry, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 08 53 56 57 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 89 4D F0");
	BlScanFunctionHex(tsf_BlDataChunker__freeBlocks, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 51 53 56 57 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 8B D9 8B 33");
	BlScanFunctionHex(tsf_BlCon__evaluate, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 56 57 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 8B 75 10");
	BlScanFunctionHex(tsf_BlCon__executef, "81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 ? ? ? ? 53 55 56 8B B4 24 ? ? ? ? 33 C9");
	BlScanFunctionHex(tsf_BlCon__executefSimObj, "81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 ? ? ? ? 53 56 8B B4 24 ? ? ? ? 33 C9");
	BlScanFunctionHex(tsf_BlCon__getVariable, "53 56 8B F1 57 85 F6 0F 84 ? ? ? ?");
	BlScanFunctionHex(tsf_BlDictionary__addVariable, "8B 44 24 04 56 57 8B F9");
	BlScanFunctionHex(tsf_BlSim__findObject_name, "57 8B F9 8A 17");
	BlScanFunctionHex(tsf_BlStringStack__getArgBuffer, "55 8B EC 83 E4 F8 8B 0D ? ? ? ? A1 ? ? ? ? 56 57 8B 7D 08 8D 14 01 03 D7 3B 15 ? ? ? ? 72 2C 8B 0D");
	BlScanFunctionHex(tsf_Con__getReturnBuffer, "81 F9 ? ? ? ? 76 2B");
	ADDR BlScanHex(tsf_mCacheSequenceLoc, "FF 05 ? ? ? ? B9 ? ? ? ? 8B F8 E8 ? ? ? ? 8B 44 24 1C 89 47 18 8B 44 24 14");
	ADDR BlScanHex(tsf_mCacheAllocatorLoc, "89 35 ? ? ? ? C7 06 ? ? ? ? A1 ? ? ? ? 68 ? ? ? ? C7 40 ? ? ? ? ? E8 ? ? ? ? 83 C4 04 8B 4D F4 64 89 0D ? ? ? ? 59 5E 8B E5 5D C3");
	ADDR BlScanHex(tsf_gIdDictionaryLoc, "89 15 ? ? ? ? E8 ? ? ? ? 8B F0 89 75 F0");
	ADDR BlScanHex(tsf_gEvalState_globalVarsLoc, "B9 ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? 6A 0A 68 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ?");

	tsf_mCacheSequence = *(ADDR*)(tsf_mCacheSequenceLoc + 2);
	tsf_mCacheAllocator = *(ADDR*)(tsf_mCacheAllocatorLoc + 2);
	tsf_gIdDictionary = *(ADDR*)(tsf_gIdDictionaryLoc + 2);
	tsf_gEvalState_globalVars = *(ADDR*)(tsf_gEvalState_globalVarsLoc + 1);

	return true;
}

