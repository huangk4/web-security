#pragma once

//辅助函数

//获取系统信息
VOID SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo);

// 获取操作系统位数  
int GetSystemBits();

//通过进程ID获取进程句柄 
HANDLE GetProcessHandle(int nID);

//判断是否32位进程
BOOL Is32(int nID);

//获取进程用户函数： 
LPCTSTR GetProcessUserName(DWORD dwID); // 进程ID 