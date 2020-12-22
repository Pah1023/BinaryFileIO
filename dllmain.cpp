// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <stdio.h>
#include "RedoBlHooks.hpp"
#include "torque.hpp"
#define debug for(int n = 0; n < 8; n++) BlPrintf("Reached line %u", __LINE__);
void TS_FileObject__writeByte(ADDR obj, int argc, const char* argv[]) {
    HANDLE h = (HANDLE)(*(int*)(obj + 76));
    if (!h) return;
    char* b = new char;
    *b = (char)atoi(argv[2]);
    LPDWORD d = LPDWORD();
    LPOVERLAPPED o = LPOVERLAPPED();
    WriteFile(h, (void*)b, 1, d, o);
    
}
void TS_FileObject__writeBytes(ADDR obj, int argc, const char* argv[]) {
    HANDLE h = (HANDLE)(*(int*)(obj + 76));
    if (!h) return;
    LPDWORD d = LPDWORD();
    LPOVERLAPPED o = LPOVERLAPPED();
    char* newBuff = new char[strlen(argv[2])]{ 0 };
    int n = 0;
    char* chars = new char[3]{ 0 };
    for (char* k = (char*)argv[2]; *k; k++) {
        if ((*k >= 48 && *k <= 57) || (*k >= 65 && *k <= 70) || (*k >= 97 && *k <= 102)) {
            newBuff[n] = *k;
            n++;
        }
    }
    newBuff[n] = 0;
    char* b = new char;
    for (int i = 0; i < n / 2; i++) {
        
        chars[0] = newBuff[i * 2];
        chars[1] = newBuff[i * 2+1];
        *b = (char)strtoul(chars, NULL, 16);
        WriteFile(h, (void*)b, 1, d, o);
    }
    delete newBuff;
    delete chars;
}
int TS_FileObject__readByte(ADDR obj, int argc, const char* argv[]) {
    unsigned char* mFileBuffer = *(unsigned char**)(obj + 52);
    int* mBufferSize = (int*)(obj + 56);
    int* mCurPos = (int*)(obj + 60);
    if (*mCurPos == *mBufferSize) return -1;
    int ret = mFileBuffer[*mCurPos]&0xFF;
    *mCurPos = *mCurPos + 1;
    return ret;

}
int TS_FileObject__bytesLeft(ADDR obj, int argc, const char* argv[]) {
    int* mBufferSize = (int*)(obj + 56);
    int* mCurPos = (int*)(obj + 60);
    return *mBufferSize - *mCurPos;
}
int TS_FileObject__getPos(ADDR obj, int argc, const char* argv[]) {
    return *(int*)(obj + 60);
}
void TS_FileObject__seek(ADDR obj, int argc, const char* argv[]) {
    int seek = atoi(argv[2]);
    if (seek > * (int*)(obj + 56)) *(int*)(obj + 60) = *(int*)(obj + 56);
    else if (seek < 0) *(int*)(obj + 60) = 0;
    else *(int*)(obj + 60) = seek;

}
int TS_FileObject__getFileSize(ADDR obj, int argc, const char* argv[]) {
    return *(int*)(obj + 56);
}
static const char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
const char* TS_FileObject__readBytes(ADDR obj, int argc, const char* argv[]) {
    int count = atoi(argv[2]);
    unsigned char* mFileBuffer = *(unsigned char**)(obj + 52);
    int* mBufferSize = (int*)(obj + 56);
    int* mCurPos = (int*)(obj + 60);
    if ((count + *mCurPos) > * mBufferSize) {
        count = *mBufferSize - *mCurPos;
    }
    if (count <= 0) return "";
    char* buff = new char[count * 2 + 1] {0};
    for (int i = 0; i < count; i++) {
        int c = mFileBuffer[*mCurPos + i];
        buff[i * 2 + 1] = hex[c & 0xF];
        buff[i * 2] = hex[(c >> 4)&0xF];
    }
    *mCurPos += count;
    //char* retBuff = tsf_Con__getReturnBuffer(strlen(buff) + 1);
    //retBuff[strlen(buff)] = 0;
    //strcpy_s(retBuff, strlen(buff), buff);
    return buff;
}

bool init() {
    BlInit;
    BlPrintf("Binary File IO Attaching.");
    if (!tsf_InitInternal())
        return false;
    tsf_AddConsoleFunc(NULL, "FileObject", "writeByte", TS_FileObject__writeByte, "(int byte) - writes a byte to the file stream.", 3, 3);
    tsf_AddConsoleFunc(NULL, "FileObject", "writeBytes", TS_FileObject__writeBytes, "(string bytes) - writes bytes to the file stream. (hex formatted)", 3, 3);
    tsf_AddConsoleFunc(NULL, "FileObject", "readByte", TS_FileObject__readByte, "() - reads byte", 2, 2);
    tsf_AddConsoleFunc(NULL, "FileObject", "bytesLeft", TS_FileObject__bytesLeft, "() - returns bytes left till EOF", 2, 2);
    tsf_AddConsoleFunc(NULL, "FileObject", "readBytes", TS_FileObject__readBytes, "(int count) - reads bytes", 3, 3);
    tsf_AddConsoleFunc(NULL, "FileObject", "getPos", TS_FileObject__getPos, "() - gets the current position in the file", 2, 2);
    tsf_AddConsoleFunc(NULL, "FileObject", "seek", TS_FileObject__seek, "(int idx) - seeks to the position of the file", 3, 3);
    tsf_AddConsoleFunc(NULL, "FileObject", "getFileSize", TS_FileObject__getFileSize, "() - gets the file size", 3, 3);


    return true;
}
bool deinit() {
    return true;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        return init();
    case DLL_PROCESS_DETACH:
        return deinit();
    }
    return TRUE;
}

