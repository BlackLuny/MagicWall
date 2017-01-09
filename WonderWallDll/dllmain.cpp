// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"

HMODULE             hNtDll = NULL;
HMODULE             hKernel32 = NULL;
HMODULE             hCurrentModule = NULL;
DWORD               RhTlsIndex;
HANDLE              HookHeap = NULL;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		hCurrentModule = hModule;

		if (((hNtDll = LoadLibraryA("ntdll.dll")) == NULL) ||
			((hKernel32 = LoadLibraryA("kernel32.dll")) == NULL))
			return FALSE;

		HookHeap = HeapCreate(0, 0, 0);
	}break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

