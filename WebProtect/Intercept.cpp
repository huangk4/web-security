// Intercept.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WebProtect.h"
#include "Intercept.h"
#include "afxdialogex.h"


// Intercept �Ի���

IMPLEMENT_DYNAMIC(Intercept, CDialogEx)

Intercept::Intercept(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

Intercept::~Intercept()
{
}

void Intercept::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Intercept, CDialogEx)
	ON_BN_CLICKED(IDOK, &Intercept::OnBnClickedOk)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON1, &Intercept::OnBnClickedButton1)
END_MESSAGE_MAP()


// Intercept ��Ϣ�������
void Intercept::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck() == 0)
		CDialogEx::OnOK();
	else
		CDialogEx::OnCancel();;
}

void Intercept::SetAPI(int api)
{
	this->api = api;
}


void Intercept::SetPID(DWORD pid)
{
	this->pid = pid;
}

void Intercept::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO:  �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CDialogEx::OnPaint()
	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);


	HANDLE hndl = GetProcessHandle(pid);
	wchar_t Path[MAX_PATH];
	GetModuleFileNameEx(hndl, NULL, Path, MAX_PATH);
	CString path = Path;
	CString text = L"  ����" + path + warn[api] + suf;
	SetDlgItemText(IDC_EDIT1, text);
}


void Intercept::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck() == 0)
		CDialogEx::OnOK();
	else
		CDialogEx::OnCancel();;
}
