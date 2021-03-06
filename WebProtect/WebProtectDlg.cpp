
// WebProtectDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "WebProtect.h"
#include "WebProtectDlg.h"
#include "afxdialogex.h"
#include "Intercept.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//下列信息全在config.txt（相对路径）中读取
char g_sqlpath[MAX_PATH];//sql注入检测配置文件路径
char g_xsspath[MAX_PATH];//xss检测配置文件路径
char g_bpath[MAX_PATH];//upload白名单配置文件路径
char g_hpath[MAX_PATH];//upload黑名单配置文件路径
char g_vhashpath[MAX_PATH];//病毒木马检测配置文件路径
char g_logserver_ip[MAX_PATH];//云日志服务器IP
int g_logport;//云日志服务器端口
char g_virus_check_ip[MAX_PATH];//病毒云检测服务器IP
int g_virus_check_port;//病毒云检测服务器端口
bool g_checksql;
bool g_checkxss;
bool g_checkupload;
bool g_bai = true;

//多线程读取文件有未知错误，因此使用队列存储检测数据
vector<string> g_sqlpayload;
vector<string> g_xsspayload;
vector<string> g_bainame;
vector<string> g_heiname;





// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWebProtectDlg 对话框



CWebProtectDlg::CWebProtectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_WEBPROTECT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWebProtectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWebProtectDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON4, &CWebProtectDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CWebProtectDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON7, &CWebProtectDlg::OnBnClickedButton7)
	//ON_BN_CLICKED(IDC_BUTTON8, &CWebProtectDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON6, &CWebProtectDlg::OnBnClickedButton6)
	ON_MESSAGE(WM_CREATEP, &CWebProtectDlg::OnCreateP)
	ON_MESSAGE(WM_API, &CWebProtectDlg::OnUseAPI)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON2, &CWebProtectDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CWebProtectDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CWebProtectDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CWebProtectDlg 消息处理程序





DWORD WINAPI startwaf(LPVOID lparam)
{
	//启动防火墙，拦截功能初始为关闭
	WAF Waf(80, 81);
	g_checksql = false;
	g_checkxss = false;
	g_checkupload = false;
	Waf.openwaf();
	return NULL;
}




BOOL CWebProtectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//加载dll地址
	TCHAR exeFullPath[MAX_PATH]; // MAX_PATH在WINDEF.h中定义了，等于260  
	memset(exeFullPath, 0, MAX_PATH);
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	TCHAR *t = wcsrchr(exeFullPath, '\\');
	*t = 0x00;
	lstrcat(exeFullPath, L"\\zhudll.dll");
	szDllName = new TCHAR[sizeof(exeFullPath)];
	lstrcpy(szDllName, exeFullPath);

	//inidll();//启动主动防御


	//打开配置文件获取配置信息
	fstream file("config\\config.conf");
	if (!file)
	{
		MessageBoxA(NULL, "配置文件不存在，请检测config目录下是否存在config.conf", "错误", MB_OK);
	}
	char file_path[MAX_PATH];
	string temp;
	while (!file.eof())
	{
		file >> file_path;
		if (!strcmp(file_path, "sqlpath"))
		{
			file >> g_sqlpath;
			continue;
		}
		if (!strcmp(file_path, "xsspath"))
		{
			file >> g_xsspath;
			continue;
		}if (!strcmp(file_path, "bpath"))
		{
			file >> g_bpath;
			continue;
		}
		if (!strcmp(file_path, "hpath"))
		{
			file >> g_hpath;
			continue;
		}
		if (!strcmp(file_path, "vhashpath"))
		{
			file >> g_vhashpath;
			continue;
		}
		if (!strcmp(file_path, "logserver_ip"))
		{
			file >> g_logserver_ip;
			continue;
		}
		if (!strcmp(file_path, "g_logport"))
		{
			file >> temp;
			g_logport=stoi(temp);
			continue;
		}
		if (!strcmp(file_path, "virus_check_ip"))
		{
			file >> g_virus_check_ip;
			continue;
		}
		if (!strcmp(file_path, "virus_check_port"))
		{
			file >> temp;
			g_virus_check_port = stoi(temp);
			continue;
		}

	}

	((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE); //选上
	((CButton *)GetDlgItem(IDC_RADIO4))->SetCheck(TRUE); //选上
	((CButton *)GetDlgItem(IDC_RADIO7))->SetCheck(TRUE); //选上
	CreateThread(0, NULL, startwaf, NULL, NULL, 0);
	


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}



void CWebProtectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWebProtectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWebProtectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWebProtectDlg::OnBnClickedButton1()//更改防火墙设置
{
	// TODO: 在此添加控件通知处理程序代码

	int security = 0;
	if (((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck())
	{
		g_checksql = true;
		security += 2;
	}
	if (((CButton *)GetDlgItem(IDC_RADIO3))->GetCheck())
	{
		g_checkxss = true;	
		security += 2;
	}
	if (((CButton *)GetDlgItem(IDC_RADIO5))->GetCheck())
	{
		g_checkupload = true;
		g_bai = true;
		security += 2;
	}
	if (((CButton *)GetDlgItem(IDC_RADIO6))->GetCheck())
	{
		g_checkupload = true;
		g_bai = false;
		security += 1;
	}
	CString ss=L"最低";
	if (security < 2)
		ss = L"低";
	else if (security < 4)
		ss = L"中等";
	else if (security < 6)
		ss = L"高";
	else ss = L"最高";
	GetDlgItem(IDC_STATIC1)->SetWindowText(ss);
}

void CWebProtectDlg::OnBnClickedButton2()//启动主动防御
{
	// TODO: 在此添加控件通知处理程序代码
	if (had_zhu == true)//已经启动
		MessageBoxA(NULL, "主动防御已经为启动状态", "", MB_OK);
	else inidll();
	CString ss = L"运行中";
	GetDlgItem(IDC_STATIC2)->SetWindowText(ss);
}


void CWebProtectDlg::OnBnClickedButton3()//关闭主动防御
{
	// TODO: 在此添加控件通知处理程序代码
	//if (had_zhu == false)//已经关闭
	//{
	//	MessageBoxA(NULL, "主动防御已经为关闭状态", "", MB_OK);
	//	return;
	//}

	BOOL bRet;
	PROCESSENTRY32 pe32;
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	pe32.dwSize = sizeof(pe32);
	bRet = Process32First(hSnap, &pe32);
	if (IS64)
	{
		while (bRet)
		{
			if (Is32(pe32.th32ProcessID) && (GetProcessUserName(pe32.th32ProcessID)) != L"S")
				UnInject(pe32.th32ProcessID);
			bRet = Process32Next(hSnap, &pe32);
		}
	}
	else
	{
		while (bRet)
		{
			if (GetProcessUserName(pe32.th32ProcessID) == L"S")
				UnInject(pe32.th32ProcessID);
			bRet = Process32Next(hSnap, &pe32);
		}
	}
	had_zhu = false;
	CString ss = L"未启动";
	GetDlgItem(IDC_STATIC2)->SetWindowText(ss);
}






void CWebProtectDlg::OnBnClickedButton4()//单文件扫描按钮
{
	// TODO: 在此添加控件通知处理程序代码

	//打开文件
	char szFileName[MAX_PATH];
	BOOL isOpen = TRUE;        //是否打开(FALSE为保存,TRUE为打开)  
	GetModuleFileNameA(NULL, szFileName, MAX_PATH);
	CString defaultDir = L"C:\\";   //默认打开的文件路径  
	CString fileName = L"pr.exe";         //默认打开的文件名  
	CString filter = L"文件 (*.*)|*.*||";   //文件过虑的类型  
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY, filter, NULL);
	INT_PTR result = openFileDlg.DoModal();
	if (result != IDOK)  //判断是否选择文件
	{
		return;
	}
	string filePath = CT2A(openFileDlg.GetPathName().GetBuffer(0));//获取文件路径

	char *cpath = new char[strlen(filePath.c_str()) + 1];
	strcpy(cpath, filePath.c_str());
	C_Virus scan(g_vhashpath);//创建一个扫描对象
	if (scan.single_scan(cpath) == true)//对文件进行扫描
	{
		if (MessageBox(L"检测出该文件为病毒，是否将其删除", L"警告", MB_YESNO) == IDYES)
			DeleteFileA(cpath);
		return;
	}
	else
		MessageBox(L"该文件不是病毒", L"", MB_OK);
}


void CWebProtectDlg::OnBnClickedButton5()//多文件扫描
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFolderPath[MAX_PATH] = { 0 };
	CString strFolderPath = L"";
	BROWSEINFO sInfo;
	ZeroMemory(&sInfo, sizeof(BROWSEINFO));

	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = L"请选择一个文件夹：";
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名  
		if (SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		CoTaskMemFree(lpidlBrowse);
	}
	

	string filePath = CT2A(strFolderPath.GetBuffer(0));//获取文件路径
	char *cpath = new char[strlen(filePath.c_str()) + 1];
	strcpy(cpath, filePath.c_str());
	C_Virus scan(g_vhashpath);
	int filenums;
	bool *check_result=scan.multi_scan(cpath,filenums);//里面递归查杀
	vector<string> fileList;
	find(cpath, fileList);


	for (int i = 0; i < filenums; i++)
	{
		if ( check_result[i]== true)
		{
			if (MessageBox(L"检测出一个病毒，是否将其删除", L"警告", MB_YESNO) == IDYES)
			{
				if (check_result[i] == true)
				{
					string deletetmp = "";
					deletetmp=deletetmp+cpath + '\\' + fileList[i];
					DeleteFileA(deletetmp.c_str());
				}
			}
		}
	}

}

void CWebProtectDlg::OnBnClickedButton6()//云查杀
{
	// TODO: 在此添加控件通知处理程序代码
	char szFileName[MAX_PATH];
	BOOL isOpen = TRUE;        //是否打开(FALSE为保存,TRUE为打开)  
	GetModuleFileNameA(NULL, szFileName, MAX_PATH);
	CString defaultDir = L"C:\\";   //默认打开的文件路径  
	CString fileName = L"pr.exe";         //默认打开的文件名  
	CString filter = L"文件 (*.*)|*.*||";   //文件过虑的类型  
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY, filter, NULL);
	INT_PTR result = openFileDlg.DoModal();
	if (result != IDOK)  //判断是否选择文件
	{
		return;
	}
	string filePath = CT2A(openFileDlg.GetPathName().GetBuffer(0));//获取文件路径

	char *cpath = new char[strlen(filePath.c_str()) + 1];
	strcpy(cpath, filePath.c_str());

	char hash[256];
	char cloudresult[4]="not" ;
	memset(cloudresult, 0, 4);
	FileSHA256(cpath, hash);
	/*CTCP Cloud(2,2);
	SOCKET newsock=Cloud.InitSocket(SOCK_NOBIND, g_virus_check_ip, g_virus_check_port);
	Cloud.mysend(newsock, hash, 256, 0);
	Cloud.myrecv(newsock, cloudresult, 4, 0);*/
	C_Virus scan(g_vhashpath);
	if (cloudresult=="yes" || scan.single_scan(cpath) == true)
	{
		if (MessageBox(L"检测出该文件为病毒，是否将其删除", L"警告", MB_YESNO) == IDYES)
			DeleteFileA(cpath);
	}
	else
		MessageBox(L"该文件不是病毒", L"", MB_OK);

}



void CWebProtectDlg::OnBnClickedButton7()//查看日志
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, _T("open"), _T("log"), NULL, NULL, SW_SHOWNORMAL);// 下划线的地方可以是网址或者是文件夹的位置，亦或者是文件的路径。
}


void CWebProtectDlg::OnBnClickedButton8()//下载云端日志
{
	// TODO: 在此添加控件通知处理程序代码
	//URLDownloadToFile(NULL,)
	MessageBoxA(NULL, "下载完成，请点击查看日志查看", "成功", MB_OK);
}



//初始化工作
void CWebProtectDlg::inidll()
{
	//判断系统位数
	if (GetSystemBits() == 64)
		IS64 = true;
	else IS64 = false;


	BOOL bRet;
	PROCESSENTRY32 pe32;
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	pe32.dwSize = sizeof(pe32);
	bRet = Process32First(hSnap, &pe32);
	if (IS64)
	{
		while (bRet)
		{
			if (Is32(pe32.th32ProcessID) && (GetProcessUserName(pe32.th32ProcessID)) != L"S"&&pe32.th32ProcessID != GetCurrentProcessId())
				dllInject(pe32.th32ProcessID);
			bRet = Process32Next(hSnap, &pe32);
		}
	}
	else
	{
		while (bRet)
		{
			if (GetProcessUserName(pe32.th32ProcessID) != L"S"&&pe32.th32ProcessID != GetCurrentProcessId())
				dllInject(pe32.th32ProcessID);
			bRet = Process32Next(hSnap, &pe32);
		}
	}
	had_zhu = true;
}

//dll注入指定pid进程
void CWebProtectDlg::dllInject(DWORD pid)
{
	if (pid == 0)
	{
		return;
	}

	//	char *pFunName = "LoadLibraryW";



	//获取令牌提升权限
	HANDLE hToken = NULL;
	BOOL bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	if (bRet == TRUE)
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		CloseHandle(hToken);
	}



	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL)
	{
		return;
	}


	DWORD nDllLen = (DWORD)(_tcslen(szDllName) + 1) * sizeof(TCHAR);

	LPVOID pDllAddr = VirtualAllocEx(hProcess, NULL, nDllLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (pDllAddr == NULL)
	{
		CloseHandle(hProcess);
		return;
	}
	//SIZE_T dwWriteNum = 0;



	WriteProcessMemory(hProcess, pDllAddr, (LPVOID)szDllName, nDllLen, NULL);
	//	er1 = GetLastError();
	LPTHREAD_START_ROUTINE pFunAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
	//	er1 = GetLastError();

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pFunAddr, pDllAddr, 0, NULL);	//错误
	DWORD er3 = GetLastError();
	WaitForSingleObject(hThread, 2000);
	CloseHandle(hThread);
	CloseHandle(hProcess);


}

void CWebProtectDlg::UnInject(DWORD pid)
{
	if (pid == 0)
	{
		return;
	}
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(me32);
	//查找匹配的进程名称
	BOOL bRet = Module32First(hSnap, &me32);

	while (bRet)
	{
		/*WideCharToMultiByte(CP_ACP, 0, (me32.szExePath), -1, test, sizeof(me32.szExePath), NULL, NULL);
		(_strupr_s(test, strlen(test) + 1));*/
		if (lstrcmp(me32.szExePath, szDllName) == 0)
		{
			break;
		}
		bRet = Module32Next(hSnap, &me32);
	}
	CloseHandle(hSnap);
	char *pFunName = "FreeLibrary";
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL)
	{
		return;
	}
	FARPROC pFunAddr = GetProcAddress(GetModuleHandleA("kernel32.dll"), pFunName);
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunAddr, me32.hModule, 0, NULL);
	WaitForSingleObject(hThread, 2000);
	CloseHandle(hThread);
	CloseHandle(hProcess);
}




bool CWebProtectDlg::CheckHook(DWORD pid)
{
	if (pid == 0)
	{
		return false;
	}
	if (GetProcessUserName(pid) == L"S")
		return false;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(me32);
	//查找匹配的进程名称
	BOOL bRet = Module32First(hSnap, &me32);

	while (bRet)
	{
		/*WideCharToMultiByte(CP_ACP, 0, (me32.szExePath), -1, test, sizeof(me32.szExePath), NULL, NULL);
		(_strupr_s(test, strlen(test) + 1));*/
		if (lstrcmp(me32.szExePath, szDllName) == 0)
		{
			return false;
		}
		bRet = Module32Next(hSnap, &me32);
	}
	return true;
}


void CWebProtectDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	BOOL bRet;
	PROCESSENTRY32 pe32;
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	pe32.dwSize = sizeof(pe32);
	bRet = Process32First(hSnap, &pe32);
	if (IS64)
	{
		while (bRet)
		{
			if (Is32(pe32.th32ProcessID) && (GetProcessUserName(pe32.th32ProcessID)) != L"S")
				UnInject(pe32.th32ProcessID);
			bRet = Process32Next(hSnap, &pe32);
		}
	}
	else
	{
		while (bRet)
		{
			if (GetProcessUserName(pe32.th32ProcessID) != L"S")
				UnInject(pe32.th32ProcessID);
			bRet = Process32Next(hSnap, &pe32);
		}
	}

	CDialog::OnClose();
}


//受到创建新进程消息后的回调函数
LRESULT CWebProtectDlg::OnCreateP(WPARAM wParam, LPARAM lParam)
{
	DWORD pid = wParam;
	CString temp;
	temp.Format(_T("%d"), pid);
	DWORD pid2 = lParam;
	CString temp2;
	temp2.Format(_T("%d"), pid2);
	if (CheckHook(pid) == true)
	{
		//MessageBox(L"a", L"B", MB_OK);
		dllInject(pid);
		return true;
	}
	return false;
}

//收到调用API的消息相应函数
LRESULT CWebProtectDlg::OnUseAPI(WPARAM wParam, LPARAM lParam)
{
	if (had_zhu == false)
		return true;
	
	Intercept message;
	message.SetAPI(wParam);
	message.SetPID(lParam);


	//存入日志
	int api = wParam;
	CString content = warn[api];
	int len = WideCharToMultiByte(CP_ACP, 0, content, content.GetLength() / 2, NULL, 0, NULL, NULL);
	char *t = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, content, content.GetLength() / 2, t, len, NULL, NULL);
	t[len] = '\0';
	savelog(t);

	if (message.DoModal() == IDOK)
		return true;
	else return false;



}

