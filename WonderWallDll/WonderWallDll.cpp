// WonderWallDll.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"


#include "WonderWallDll.h"
extern "C"
_declspec(dllexport)BOOL EnumProcess(PROCESSENTRY32* ProcessEntry)
{

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
		ProcessEntry[ProcessCount].th32ProcessID = ProcessEntry32.th32ProcessID;
		memcpy(ProcessEntry[ProcessCount].szExeFile, ProcessEntry32.szExeFile, sizeof(ProcessEntry32.szExeFile));
		ProcessCount++;
		bOk = Process32Next(ToolHelpHandle, &ProcessEntry32);
	}

	CloseHandle(ToolHelpHandle);
	ToolHelpHandle = INVALID_HANDLE_VALUE;
	return TRUE;

}


