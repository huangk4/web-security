//��ȡϵͳ��Ϣ
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

// ��ȡ����ϵͳλ��  
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

//ͨ������ID��ȡ���̾�� 
HANDLE GetProcessHandle(int nID)
{
	return OpenProcess(PROCESS_ALL_ACCESS, FALSE, nID);
}

//�ж��Ƿ�32λ����
BOOL Is32(int nID)
{
	BOOL re;
	if (IsWow64Process(GetProcessHandle(nID), &re))
	{
		return re;
	}
	//MessageBox(NULL, L"���жϽ���λ��ʱ���ִ���", L"����", MB_OK);
	return false;
}


//��ȡ�����û������� 
LPCTSTR GetProcessUserName(DWORD dwID) // ����ID 
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