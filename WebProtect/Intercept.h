#pragma once


// Intercept �Ի���

class Intercept : public CDialogEx
{
	DECLARE_DYNAMIC(Intercept)

private:
	int api;
	DWORD pid;
public:
	Intercept(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Intercept();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	void SetAPI(int api);
	void SetPID(DWORD pid);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButton1();
};
