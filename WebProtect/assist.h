#pragma once

//��������

//��ȡϵͳ��Ϣ
VOID SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo);

// ��ȡ����ϵͳλ��  
int GetSystemBits();

//ͨ������ID��ȡ���̾�� 
HANDLE GetProcessHandle(int nID);

//�ж��Ƿ�32λ����
BOOL Is32(int nID);

//��ȡ�����û������� 
LPCTSTR GetProcessUserName(DWORD dwID); // ����ID 