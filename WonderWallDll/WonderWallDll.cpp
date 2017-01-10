// WonderWallDll.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "WonderWallDll.h"


WONDERWALL_BOOL_EXPORT	EnumProcess(PROCESSENTRY32* ProcessEntry,ULONG32 Index)
{
	BOOL bRet = FALSE;
	if (ProcessEntry == NULL)
	{
		return bRet;
	}
	switch (Index)
	{
	case 0:
		bRet = EnumProcessByCreateToolhelp32Snapshot(ProcessEntry);
		break;
	case 1:
		bRet = EnumProcessBypsapi(ProcessEntry);  
		break;
	case 2:
		bRet = EnumProcessByZwQuerySystemInformation(ProcessEntry);
		break;
	case 3:
		bRet = EnumProcessByWTSEnumerateProcesses(ProcessEntry);
		break;
	case 4:
		RhInstallDriver(L"WonderWallDriver.sys", L"WonderWallDriver.sys");
		bRet = EnumProcessInDriver(ProcessEntry);
		break;
	default:
		break;
	}
	return bRet;
}







BOOL  WcharToChar(CHAR** szDestString, WCHAR* wzSourString)
{
	SIZE_T StringLength = 0;
	if (wzSourString == NULL)
	{
		return FALSE;
	}

	StringLength = (wcslen(wzSourString) + 1) * sizeof(CHAR);

	*szDestString = (CHAR*)malloc(StringLength);

	if (*szDestString == NULL)
	{
		return FALSE;
	}

	memset(*szDestString, 0, StringLength);
	wcstombs(*szDestString, wzSourString, StringLength);


	return TRUE;
}




