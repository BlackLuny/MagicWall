#include "..\WonderWallDll.h"
#include "..\Common.h"
#include "..\stdafx.h"
#include "..\..\WonderWallDriver\Trace.h"
#include <Windows.h>
#include <WinIoCtl.h>
pfnZwQuerySystemInformation ZwQuerySystemInformation = NULL;


BOOL EnumProcessInDriver(PROCESSENTRY32 * ProcessEntry)
{
	int ProcessCount = 0;

	HANDLE ProcessHandle = NULL;

	DWORD  dwReturn = 0;

	int i = 0;
	PROCESSENTRY32 Temp = { 0 };
	for (i = 0; i<10000000; i += 4)

	{

		ProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, i);



		if (ProcessHandle == NULL)
		{
			continue;
		}
		else
		{
			//��������������
			if (SendIoControl(&i, sizeof(ULONG32), &Temp, &dwReturn) == TRUE)
			{
				ProcessEntry[ProcessCount] = Temp;
				ProcessEntry[ProcessCount].th32ProcessID = i;
				ProcessCount++;
			}

		}
		if (ProcessCount > 888)
		{
			break;
		}
	}
	return TRUE;
}
BOOL EnumProcessByCreateToolhelp32Snapshot(PROCESSENTRY32* ProcessEntry) {
	int ProcessCount = 0;
	//�������վ����CreateToolhelp32Snapshot�����������ص�ǰ���еĽ��̿��վ��
	HANDLE ToolHelpHandle = NULL;

	//PROCESSENTRY32�ṹ���¼��ǰ���̵�һЩ��Ϣ������dwSize����ָ����С����СΪ��ǰ�ṹ���С
	PROCESSENTRY32 ProcessEntry32 = { 0 };
	ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);

	ToolHelpHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (ToolHelpHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bOk = Process32First(ToolHelpHandle, &ProcessEntry32);
	while (bOk)
	{
		ProcessEntry[ProcessCount] = ProcessEntry32 ;
		//memcpy(ProcessEntry[ProcessCount].szExeFile, ProcessEntry32.szExeFile, sizeof(ProcessEntry32.szExeFile));
		ProcessCount++;
		bOk = Process32Next(ToolHelpHandle, &ProcessEntry32);
	}

	CloseHandle(ToolHelpHandle);
	ToolHelpHandle = INVALID_HANDLE_VALUE;
	return TRUE;
}
BOOL EnumProcessByZwQuerySystemInformation(PROCESSENTRY32* ProcessEntry)
{
	int ProcessCount = 0;
	PWTS_PROCESS_INFO ppi;
	DWORD dwCounts;
	BOOL bRet = WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, 0, 1, &ppi, &dwCounts);
	if (!bRet)
		return FALSE;
	//�����Ȱ�ppi�������������Ժ��ͷţ���Ȼ����������±����ʽ���ʵĻ��Ͳ�������������
	PWTS_PROCESS_INFO ppiTemp = ppi;
	for (int i = 0; i< dwCounts; i++)
	{
		ProcessEntry[ProcessCount].th32ProcessID = ppi->ProcessId;
		memcpy(ProcessEntry[ProcessCount].szExeFile, ppi->pProcessName, wcslen(ppi->pProcessName) * 2);
		ProcessCount++;
		//printf("%S \t %d \n", ppi->pProcessName, ppi->ProcessId);//ppi[i].pProcessName
		ppi++;
	}
	//�ڴ�й©���Ǵ��������ģ��ö���Ҫ��������
	WTSFreeMemory(ppiTemp);
	//getchar();



	return TRUE;

}
BOOL EnumProcessBypsapi(PROCESSENTRY32* ProcessEntry)
{

	int ProcessCount = 0;
	DWORD Processes[MAXPROCESSES], Size, ProcessesNumber;

	if (!EnumProcesses(Processes, sizeof(Processes), &Size))
	{
		return FALSE;
	}


	//�����������

	ProcessesNumber = Size / sizeof(DWORD);

	//��ӡ��������

	for (int i = 0; i < ProcessesNumber; i++)
	{
		if (Processes[i] != 0)
		{
			WCHAR ProcessName[MAX_PATH] = L"<unknown>";

			//��ý��̾��
			HANDLE ProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, Processes[i]);

			//��ý�������
			if (NULL != ProcessHandle)
			{
				HMODULE hMod;
				DWORD ReturnLength;

				if (EnumProcessModules(ProcessHandle, &hMod, sizeof(hMod),
					&ReturnLength))
				{
					GetModuleBaseName(ProcessHandle, hMod, ProcessName,
						sizeof(ProcessName) / sizeof(WCHAR));
				}
			}

			ProcessEntry[ProcessCount].th32ProcessID = Processes[i];
			memcpy(ProcessEntry[ProcessCount].szExeFile, ProcessName, sizeof(ProcessName) * 2);
			ProcessCount++;

			//�رս��̾��
			CloseHandle(ProcessHandle);

		}
	}
	return TRUE;
}
BOOL EnumProcessByWTSEnumerateProcesses(PROCESSENTRY32* ProcessEntry)
{

	int ProcessCount = 0;
	HMODULE NtdllHmodule = GetModuleHandle(L"ntdll.dll");
	ZwQuerySystemInformation = (pfnZwQuerySystemInformation)GetProcAddress(NtdllHmodule, "ZwQuerySystemInformation");

	if (ZwQuerySystemInformation == NULL)
	{
		return FALSE;
	}


	UINT32 BufferLength = 0x1000;
	void*  BufferData = NULL;

	NTSTATUS Status = STATUS_INFO_LENGTH_MISMATCH;
	HANDLE   HeapHandle = GetProcessHeap();      //��õ�ǰ����Ĭ�϶�

	UINT32 ProcessID = 0;

	BOOL   bOk = FALSE;
	while (!bOk)
	{
		BufferData = HeapAlloc(HeapHandle, HEAP_ZERO_MEMORY, BufferLength);
		if (BufferData == NULL)
		{
			return 0;
		}

		Status = ZwQuerySystemInformation(SystemProcessInformation, BufferData, BufferLength, (PULONG)&BufferLength);
		if (Status == STATUS_INFO_LENGTH_MISMATCH)
		{
			//�ڴ治��,���ڴ����������������
			HeapFree(HeapHandle, NULL, BufferData);
			BufferLength *= 2;
		}
		else if (!NT_SUCCESS(Status))
		{
			//���ÿ�
			HeapFree(HeapHandle, NULL, BufferData);
			return FALSE;
		}
		else
		{

			PSYSTEM_PROCESS_INFORMATION SystemProcess = (PSYSTEM_PROCESS_INFORMATION)BufferData;
			while (SystemProcess)
			{
				//�������ProcessName����Name
				char ProcessName[MAX_PATH];
				memset(ProcessName, 0, sizeof(ProcessName));
				ProcessEntry[ProcessCount].th32ProcessID = (UINT32)SystemProcess->ProcessId;
				memcpy(ProcessEntry[ProcessCount].szExeFile, SystemProcess->ImageName.Buffer, SystemProcess->ImageName.Length);
				ProcessCount++;
				/*	WideCharToMultiByte(0, 0, SystemProcess->ImageName.Buffer, SystemProcess->ImageName.Length, ProcessName, MAX_PATH, NULL, NULL);
				ProcessID = (UINT32)(SystemProcess->ProcessId);*/
				//printf("PID:\t%X,\tName:\t%s\r\n", ProcessID, ProcessName);

				if (!SystemProcess->NextEntryOffset)
				{
					break;
				}
				SystemProcess = (PSYSTEM_PROCESS_INFORMATION)((unsigned char*)SystemProcess + SystemProcess->NextEntryOffset);
			}

			if (BufferData)
			{
				HeapFree(HeapHandle, NULL, BufferData);
			}

			bOk = TRUE;
		}
	}

	return TRUE;
}


BOOL  SendIoControl(int* InputData, ULONG InputSize, PROCESSENTRY32* ProcessInfo, DWORD* dwReturn)
{
	HANDLE hDevice = NULL;
	BOOL   bOk = FALSE;



	hDevice = CreateFile(L"\\\\.\\WonderWallDriverLinkName", GENERIC_READ | GENERIC_WRITE,

		FILE_SHARE_READ | FILE_SHARE_WRITE,

		NULL,

		OPEN_EXISTING,

		FILE_ATTRIBUTE_NORMAL,

		NULL);



	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	bOk = DeviceIoControl(hDevice, CTL_GETPROCESSIMAGNAMEBYID,

		InputData,

		InputSize,

		ProcessInfo,

		sizeof(PROCESSENTRY32),

		dwReturn,

		NULL);





	if (bOk == FALSE)

	{

		CloseHandle(hDevice);

		hDevice = NULL;



		return FALSE;

	}

	CloseHandle(hDevice);

	hDevice = NULL;

	return TRUE;

}