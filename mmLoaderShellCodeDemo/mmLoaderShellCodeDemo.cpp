// mmLoaderShellCodeDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <strsafe.h>
#include "..\mmLoader\mmLoader.h"

int _tmain(int argc, _TCHAR* argv[])
{
	
	// 1.��ShellCode���������������ڴ�ռ�
	LPVOID lpShellCodeBase = VirtualAlloc(
		NULL, 
		sizeof(mmLoaderShellCode), 
		MEM_RESERVE | MEM_COMMIT, 
		PAGE_EXECUTE_READWRITE);

	if (NULL == lpShellCodeBase)
	{
		MessageBox(NULL, _T("ΪShellCode�����ڴ�ռ�ʧ��!"), NULL, MB_OK);
		return FALSE;
	}
	RtlCopyMemory(lpShellCodeBase, mmLoaderShellCode, sizeof(mmLoaderShellCode));

	// 2.��ȡMemModuleHlper�ĺ���ָ��
	Type_MemModuleHelper pfnMemModuleHelper = (Type_MemModuleHelper)lpShellCodeBase;


	// 3.׼������
	TCHAR tszDllPath[] = _T("F:\\RtlExUpd.dll");

	MEM_MODULE sMemModule;
	RtlZeroMemory(&sMemModule, sizeof(sMemModule));
	sMemModule.RawFile.h = INVALID_HANDLE_VALUE;

	NTFUNCPTRSTABLE sNtFuncPtrsTable;

	sNtFuncPtrsTable.pfnCreateFileW = CreateFileW;
	sNtFuncPtrsTable.pfnGetFileSize = GetFileSize;
	sNtFuncPtrsTable.pfnCreateFileMappingW = CreateFileMappingW;
	sNtFuncPtrsTable.pfnMapViewOfFile = MapViewOfFile;
	sNtFuncPtrsTable.pfnUnmapViewOfFile = UnmapViewOfFile;
	sNtFuncPtrsTable.pfnCloseHandle = CloseHandle;
	sNtFuncPtrsTable.pfnGetModuleHandleA = GetModuleHandleA;
	sNtFuncPtrsTable.pfnLoadLibraryA = LoadLibraryA;
	sNtFuncPtrsTable.pfnGetProcAddress = GetProcAddress;
	sNtFuncPtrsTable.pfnVirtualAlloc = VirtualAlloc;
	sNtFuncPtrsTable.pfnVirtualFree = VirtualFree;

	sMemModule.pNtFuncptrsTable = &sNtFuncPtrsTable;

	TCHAR tszFormat[] = _T("Address of ShowLastError: %p");
	TCHAR tszText[MAX_PATH];
	RtlZeroMemory(tszText, sizeof(tszText));

	// 4.��ʼ��ShellCode����ģ��
	if (pfnMemModuleHelper(&sMemModule, MHM_BOOL_LOAD, tszDllPath, NULL, FALSE))
	{
		LPVOID lpAddr = (LPVOID)pfnMemModuleHelper(&sMemModule, MHM_FARPROC_GETPROC, NULL, "SetCDfmt", FALSE);

		StringCchPrintf(tszText, _countof(tszText), tszFormat, lpAddr);

		MessageBox(NULL, tszText, NULL, MB_OK);

		pfnMemModuleHelper(&sMemModule, MHM_VOID_FREE, NULL, NULL, FALSE);
	}

	VirtualFree(lpShellCodeBase, 0, MEM_RELEASE);

	return 0;
}
