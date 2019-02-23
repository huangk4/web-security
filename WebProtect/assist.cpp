//获取系统信息
#include "stdafx.h"
#include"assist.h"

VOID SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo)
{
	if (NULL == lpSystemInfo)    return;
	typedef VOID(WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(L"kernel32"), "GetNativeSystemInfo");;
	if (NULL != fnGetNativeSystemInfo)
	{
		fnGetNativeSystemInfo(lpSystemInfo);
	}
	else
	{
		GetSystemInfo(lpSystemInfo);
	}
}

// 获取操作系统位数  
int GetSystemBits()
{
	SYSTEM_INFO si;
	SafeGetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		return 64;
	}
	return 32;
}

//通过进程ID获取进程句柄 
HANDLE GetProcessHandle(int nID)
{
	return OpenProcess(PROCESS_ALL_ACCESS, FALSE, nID);
}

//判断是否32位进程
BOOL Is32(int nID)
{
	BOOL re;
	if (IsWow64Process(GetProcessHandle(nID), &re))
	{
		return re;
	}
	//MessageBox(NULL, L"在判断进程位数时出现错误", L"错误", MB_OK);
	return false;
}


//获取进程用户函数： 
LPCTSTR GetProcessUserName(DWORD dwID) // 进程ID 
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwID);
	if (hProcess == NULL)
		return NULL;

	HANDLE hToken = NULL;
	BOOL bResult = FALSE;
	DWORD dwSize = 0;

	static TCHAR szUserName[256] = { 0 };
	TCHAR szDomain[256] = { 0 };
	DWORD dwDomainSize = 256;
	DWORD dwNameSize = 256;

	SID_NAME_USE    SNU;
	PTOKEN_USER pTokenUser = NULL;
	__try
	{
		if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
		{
			bResult = FALSE;
			__leave;
		}

		if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize))
		{
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				bResult = FALSE;
				__leave;
			}
		}

		pTokenUser = NULL;
		pTokenUser = (PTOKEN_USER)malloc(dwSize);
		if (pTokenUser == NULL)
		{
			bResult = FALSE;
			__leave;
		}

		if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize))
		{
			bResult = FALSE;
			__leave;
		}

		if (LookupAccountSid(NULL, pTokenUser->User.Sid, szUserName, &dwNameSize, szDomain, &dwDomainSize, &SNU) != 0)
		{
			return szUserName;
		}
	}
	__finally
	{
		if (pTokenUser != NULL)
			free(pTokenUser);
	}

	return NULL;
}