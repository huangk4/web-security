#pragma once


//配置文件


#define WM_CREATEP WM_USER+102 //创建新进程
#define WM_API WM_USER+103	//调用API


const int apis = 16;
const CString API[apis] = { L"NetUserAdd", L"NetUserChangePassword", L"NetUserDel", L"NetGroupAddUser", L"NetGroupDel", L"NetGroupDelUser", L"RegCreateKey", 
L"RegCreateKeyEx", L"RegDeleteKey",L"RegDeleteKeyEx", L"RegDeleteKeyValue", L"SetWindowsHookW", L"SetWindowsHookExW", L"CreateRemoteThread",
L"CreateRemoteThreadEx",L"NtResumeThread" };
const CString warn[apis] = { L"正准备新建用户账号，很可能是黑客通过网络入侵操作",L"正尝试更改用户密码",L"正在删除用户",L"正在添加用户组成员",L"正在删除组成员",L"正在创建注册表项",
L"正在创建注册表项",L"正在删除注册表项",L"正在删除注册表项",L"正在删除注册表值",L"正在设置消息钩子",L"正在设置消息钩子",L"正在创建远程线程",L"正在创建远程线程"};
const CString suf = L"，存在很大安全隐患，如果不是您的操作，建议立刻阻止";
