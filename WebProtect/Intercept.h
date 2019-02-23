#pragma once


// Intercept 对话框

class Intercept : public CDialogEx
{
	DECLARE_DYNAMIC(Intercept)

private:
	int api;
	DWORD pid;
public:
	Intercept(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Intercept();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	void SetAPI(int api);
	void SetPID(DWORD pid);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButton1();
};
