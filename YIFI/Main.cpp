#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
#include "..\WonderWallDll\WonderWallDll.h"
using namespace std;
typedef VOID(*pfnRhInjectLibrary)(PROCESSENTRY32* ProcessEntry);

int main()
{
	HMODULE DllModuleHandle = LoadLibrary(L"WonderWallDll.dll");

	if (DllModuleHandle != NULL)
	{
		//��ȡDll�е����ĺ����ĵ�ַ
		pfnRhInjectLibrary   RhInjectLibrary = NULL;
		RhInjectLibrary = (pfnRhInjectLibrary)GetProcAddress(DllModuleHandle, "EnumProcess");
		int a = GetLastError();
		PROCESSENTRY32 ProcessEntry[1000];
		memset(ProcessEntry, 0, sizeof(ProcessEntry));
		RhInjectLibrary(ProcessEntry);
		{
			int i = 0;
			while (ProcessEntry[i].th32ProcessID != 0 || i == 0)
			{
				printf("PID:\t0x%X,", ProcessEntry[i].th32ProcessID);
				printf("\tName:\t%S\r\n", ProcessEntry[i].szExeFile);
				i++;
			}
		}
	}
}

