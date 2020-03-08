// CUniwerApp.cpp: 实现文件
//

#include "pch.h"
#include "aikotoba_chs.h"
#include "CUniwerApp.h"
#include "afxdialogex.h"


// CUniwerApp 对话框

IMPLEMENT_DYNAMIC(CUniwerApp, CDialogEx)

CUniwerApp::CUniwerApp(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CUniwerApp::~CUniwerApp()
{
}

void CUniwerApp::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUniwerApp, CDialogEx)
	ON_BN_CLICKED(IDOK, &CUniwerApp::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CUniwerApp::OnBnClickedCancel)
END_MESSAGE_MAP()


// CUniwerApp 消息处理程序


void CUniwerApp::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
}


void CUniwerApp::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnCancel();

	ShowWindow(SW_HIDE);
}
