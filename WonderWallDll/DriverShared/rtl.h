

#ifndef _EASYHOOK_RTL_
#define _EASYHOOK_RTL_

#ifdef __cplusplus
extern "C"{
#endif

#include "..\stdafx.h"

#if _DEBUG
    #define DEBUGMSG(message) { WCHAR debugMsg[1024] = { 0 }; _snwprintf_s(debugMsg, 1024, _TRUNCATE, L"%s\n", message); OutputDebugStringW(debugMsg); }
#else
    #define DEBUGMSG(message) { }
#endif

#ifndef DRIVER
	#define ASSERT(expr, Msg)            RtlAssert((BOOL)(expr),(LPCWSTR) Msg);
	#define THROW(code, Msg)        { NtStatus = (code); RtlSetLastError(GetLastError(), NtStatus, Msg); goto THROW_OUTRO; }
#else
#pragma warning(disable: 4005)
    #define ASSERT( exp, Msg )           ((!(exp)) ? (RtlAssert(#exp, __FILE__, __LINE__, NULL), FALSE) : TRUE)
#pragma warning(default: 4005)
#define THROW(code, Msg)        { NtStatus = (code); RtlSetLastError(NtStatus, NtStatus, Msg); goto THROW_OUTRO; }
#endif

#define RETURN                      { RtlSetLastError(STATUS_SUCCESS, STATUS_SUCCESS, L""); NtStatus = STATUS_SUCCESS; goto FINALLY_OUTRO; }
#define FORCE(expr)                 { if(!RTL_SUCCESS(NtStatus = (expr))) goto THROW_OUTRO; }
#define IsValidPointer				RtlIsValidPointer

#ifdef DRIVER

    typedef struct _RTL_SPIN_LOCK_
    {
        KSPIN_LOCK				Lock;  
        KIRQL                   OldIrql;
    }RTL_SPIN_LOCK;

#else

    typedef struct _RTL_SPIN_LOCK_
    {
        CRITICAL_SECTION        Lock;
        BOOL                 IsOwned;
    }RTL_SPIN_LOCK;

#endif

void RtlInitializeLock(RTL_SPIN_LOCK* InLock);

void RtlAcquireLock(RTL_SPIN_LOCK* InLock);

void RtlReleaseLock(RTL_SPIN_LOCK* InLock);

void RtlDeleteLock(RTL_SPIN_LOCK* InLock);

void RtlSleep(ULONG InTimeout);

void* RtlAllocateMemory(
            BOOL InZeroMemory, 
            ULONG InSize);

void RtlFreeMemory(void* InPointer);

#undef RtlCopyMemory
void RtlCopyMemory(
            PVOID InDest,
            PVOID InSource,
            ULONG InByteCount);

#undef RtlMoveMemory
BOOL RtlMoveMemory(
            PVOID InDest,
            PVOID InSource,
            ULONG InByteCount);

#undef RtlZeroMemory
void RtlZeroMemory(
            PVOID InTarget,
            ULONG InByteCount);

LONG RtlProtectMemory(
            void* InPointer, 
            ULONG InSize, 
            ULONG InNewProtection);

#ifndef DRIVER
	void RtlAssert(BOOL InAssert,LPCWSTR lpMessageText);
#endif

void RtlSetLastError(
            LONG InCode, 
            LONG InNtStatus,
            WCHAR* InMessage);

LONGLONG RtlAnsiHexToLongLong(
	const CHAR *s, 
	int len);

BOOL RtlAnsiDbgHexToLongLong(
            CHAR* InHexString,
            ULONG InMinStrLen,
            LONGLONG* OutValue);

LONG RtlAnsiSubString(
            CHAR* InString,
            ULONG InOffset,
            ULONG InCount,
            CHAR* InTarget,
            ULONG InTargetMaxLen);

LONG RtlAnsiIndexOf(
            CHAR* InString,
            CHAR InChar);

ULONG RtlAnsiLength(CHAR* InString);

ULONG RtlUnicodeLength(WCHAR* InString);

void RtlLongLongToUnicodeHex(LONGLONG InValue, WCHAR* InBuffer);

BOOL RtlIsValidPointer(PVOID InPtr, ULONG InSize);

#if X64_DRIVER
// Write Protection Off
KIRQL RtlWPOff();
//Write Protection On
void RtlWPOn(KIRQL irql);

#endif

#ifdef __cplusplus
}
#endif

#endif