#pragma once


//�����ļ�


#define WM_CREATEP WM_USER+102 //�����½���
#define WM_API WM_USER+103	//����API


const int apis = 16;
const CString API[apis] = { L"NetUserAdd", L"NetUserChangePassword", L"NetUserDel", L"NetGroupAddUser", L"NetGroupDel", L"NetGroupDelUser", L"RegCreateKey", 
L"RegCreateKeyEx", L"RegDeleteKey",L"RegDeleteKeyEx", L"RegDeleteKeyValue", L"SetWindowsHookW", L"SetWindowsHookExW", L"CreateRemoteThread",
L"CreateRemoteThreadEx",L"NtResumeThread" };
const CString warn[apis] = { L"��׼���½��û��˺ţ��ܿ����Ǻڿ�ͨ���������ֲ���",L"�����Ը����û�����",L"����ɾ���û�",L"��������û����Ա",L"����ɾ�����Ա",L"���ڴ���ע�����",
L"���ڴ���ע�����",L"����ɾ��ע�����",L"����ɾ��ע�����",L"����ɾ��ע���ֵ",L"����������Ϣ����",L"����������Ϣ����",L"���ڴ���Զ���߳�",L"���ڴ���Զ���߳�"};
const CString suf = L"�����ںܴ�ȫ����������������Ĳ���������������ֹ";
