
// WebProtectDlg.h: 头文件
//

#pragma once


// CWebProtectDlg 对话框
class CWebProtectDlg : public CDialog
{
// 构造
public:
	CWebProtectDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WEBPROTECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	bool IS64;
	TCHAR *szDllName;
	bool had_zhu;//主动防御是否启动

	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton6();
	afx_msg LRESULT OnCreateP(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUseAPI(WPARAM wParam, LPARAM lParam);

	void inidll();
	void dllInject(DWORD pid);//dll注入	
	void UnInject(DWORD pid);//dll卸载
	bool CheckHook(DWORD pid);//判断目标进程是否要注入
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
};
