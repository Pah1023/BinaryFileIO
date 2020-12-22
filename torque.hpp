#pragma once

#include "RedoBlHooks.hpp"

typedef const char* (*tsf_StringCallback)(ADDR, signed int, const char* []);
typedef signed int      (*tsf_IntCallback)(ADDR, signed int, const char* []);
typedef float         (*tsf_FloatCallback)(ADDR, signed int, const char* []);
typedef void           (*tsf_VoidCallback)(ADDR, signed int, const char* []);
typedef bool           (*tsf_BoolCallback)(ADDR, signed int, const char* []);

/* These functions are used for tsf_BlCon__executefSimObj.
   They refer to a special buffer for the argument stack.
   For tsf_BlCon__executef, you need to use your own buffers. */
char* tsf_GetIntArg(int);
char* tsf_GetFloatArg(float);
char* tsf_ScriptThis(ADDR);

const char* tsf_Eval(const char*);
const char* tsf_Evalf(const char*, ...);

ADDR tsf_FindObject(unsigned int);
ADDR tsf_FindObject(const char*);

ADDR tsf_LookupNamespace(const char*, const char*);

const char* tsf_GetVar(const char*);

void tsf_AddVarInternal(const char*, signed int, void*);
void tsf_AddVar(const char*, const char**);
void tsf_AddVar(const char*, signed int*);
void tsf_AddVar(const char*, float*);
void tsf_AddVar(const char*, bool*);

ADDR tsf_AddConsoleFuncInternal(const char*, const char*, const char*, signed int, const char*, signed int, signed int);
void tsf_AddConsoleFunc(const char*, const char*, const char*, tsf_StringCallback, const char*, signed int, signed int);
void tsf_AddConsoleFunc(const char*, const char*, const char*, tsf_IntCallback, const char*, signed int, signed int);
void tsf_AddConsoleFunc(const char*, const char*, const char*, tsf_FloatCallback, const char*, signed int, signed int);
void tsf_AddConsoleFunc(const char*, const char*, const char*, tsf_VoidCallback, const char*, signed int, signed int);
void tsf_AddConsoleFunc(const char*, const char*, const char*, tsf_BoolCallback, const char*, signed int, signed int);

bool tsf_InitInternal();

extern ADDR tsf_mCacheSequence;
extern ADDR tsf_mCacheAllocator;
extern ADDR tsf_gIdDictionary;
extern ADDR tsf_gEvalState_globalVars;

BlFunctionDefExtern(const char*, __stdcall, tsf_BlStringTable__insert, const char*, bool);
BlFunctionDefExtern(ADDR, __fastcall, tsf_BlNamespace__find, const char*, const char*);
BlFunctionDefExtern(ADDR, __thiscall, tsf_BlNamespace__createLocalEntry, ADDR, const char*);
BlFunctionDefExtern(void, __thiscall, tsf_BlDataChunker__freeBlocks, ADDR);
BlFunctionDefExtern(const char*, , tsf_BlCon__evaluate, ADDR, signed int, const char**);
BlFunctionDefExtern(const char*, , tsf_BlCon__executef, signed int, ...);
BlFunctionDefExtern(const char*, , tsf_BlCon__executefSimObj, ADDR*, signed int, ...);
BlFunctionDefExtern(const char*, __thiscall, tsf_BlCon__getVariable, const char*);
BlFunctionDefExtern(void, __thiscall, tsf_BlDictionary__addVariable, ADDR*, const char*, signed int, void*);
BlFunctionDefExtern(ADDR*, __thiscall, tsf_BlSim__findObject_name, const char*);
BlFunctionDefExtern(char*, __stdcall, tsf_BlStringStack__getArgBuffer, unsigned int);

BlFunctionDefExtern(char*, __fastcall, tsf_Con__getReturnBuffer, unsigned int);