
// aikotoba_chsDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "aikotoba_chs.h"
#include "aikotoba_chsDlg.h"
#include "afxdialogex.h"
#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int* nID = NULL;

INT UCS4_To_UTF8(DWORD dwUCS4, BYTE* pbUTF8)
{
    const BYTE  abPrefix[] = { 0, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
    const DWORD adwCodeUp[] = {
        0x80,           // U+00000000 ～ U+0000007F  
        0x800,          // U+00000080 ～ U+000007FF  
        0x10000,        // U+00000800 ～ U+0000FFFF  
        0x200000,       // U+00010000 ～ U+001FFFFF  
        0x4000000,      // U+00200000 ～ U+03FFFFFF  
        0x80000000      // U+04000000 ～ U+7FFFFFFF  
    };

    INT i, iLen;

    // 根据UCS4编码范围确定对应的UTF-8编码字节数  
    iLen = sizeof(adwCodeUp) / sizeof(DWORD);
    for (i = 0; i < iLen; i++)
    {
        if (dwUCS4 < adwCodeUp[i])
        {
            break;
        }
    }

    if (i == iLen)return 0;    // 无效的UCS4编码  

    iLen = i + 1;   // UTF-8编码字节数  
    if (pbUTF8 != NULL)
    {   // 转换为UTF-8编码  
        for (; i > 0; i--)
        {
            pbUTF8[i] = static_cast<BYTE>((dwUCS4 & 0x3F) | 0x80);
            dwUCS4 >>= 6;
        }

        pbUTF8[0] = static_cast<BYTE>(dwUCS4 | abPrefix[iLen - 1]);
    }

    return iLen;
}
// 转换UTF8编码到UCS4编码  
INT UTF8_To_UCS4(const BYTE* pbUTF8, DWORD& dwUCS4)
{
    INT     i, iLen;
    BYTE    b;

    if (pbUTF8 == NULL)
    {   // 参数错误  
        return 0;
    }

    b = *pbUTF8++;
    if (b < 0x80)
    {
        dwUCS4 = b;
        return 1;
    }

    if (b < 0xC0 || b > 0xFD)
    {   // 非法UTF8  
        return 0;
    }

    if (b < 0xE0)
    {
        dwUCS4 = b & 0x1F;
        iLen = 2;
    }
    else if (b < 0xF0)
    {
        dwUCS4 = b & 0x0F;
        iLen = 3;
    }
    else if (b < 0xF8)
    {
        dwUCS4 = b & 7;
        iLen = 4;
    }
    else if (b < 0xFC)
    {
        dwUCS4 = b & 3;
        iLen = 5;
    }
    else
    {
        dwUCS4 = b & 1;
        iLen = 6;
    }

    for (i = 1; i < iLen; i++)
    {
        b = *pbUTF8++;
        if (b < 0x80 || b > 0xBF)
        {   // 非法UTF8  
            break;
        }

        dwUCS4 = (dwUCS4 << 6) + (b & 0x3F);
    }

    if (i < iLen)
    {   // 非法UTF8  
        return 0;
    }
    else
    {
        return iLen;
    }
}

// 转换UCS4编码到UCS2编码  
INT UCS4_To_UTF16(DWORD dwUCS4, WORD* pwUTF16)
{
    if (dwUCS4 <= 0xFFFF)
    {
        if (pwUTF16 != NULL)
        {
            *pwUTF16 = static_cast<WORD>(dwUCS4);
        }

        return 1;
    }
    else if (dwUCS4 <= 0xEFFFF)
    {
        if (pwUTF16 != NULL)
        {
            pwUTF16[0] = static_cast<WORD>(0xD800 + (dwUCS4 >> 10) - 0x40);   // 高10位  
            pwUTF16[1] = static_cast<WORD>(0xDC00 + (dwUCS4 & 0x03FF));     // 低10位  
        }

        return 2;
    }
    else
    {
        return 0;
    }
}

// 转换UCS2编码到UCS4编码  
INT UTF16_To_UCS4(const WORD* pwUTF16, DWORD& dwUCS4)
{
    WORD    w1, w2;

    if (pwUTF16 == NULL)
    {   // 参数错误  
        return 0;
    }

    w1 = pwUTF16[0];
    if (w1 >= 0xD800 && w1 <= 0xDFFF)
    {   // 编码在替代区域（Surrogate Area）  
        if (w1 < 0xDC00)
        {
            w2 = pwUTF16[1];
            if (w2 >= 0xDC00 && w2 <= 0xDFFF)
            {
                dwUCS4 = (w2 & 0x03FF) + (((w1 & 0x03FF) + 0x40) << 10);
                return 2;
            }
        }

        return 0;   // 非法UTF16编码      
    }
    else
    {
        dwUCS4 = w1;
        return 1;
    }
}
INT UTF8Str_To_UTF16Str(const BYTE* pbszUTF8Str, WORD* pwszUTF16Str)
{
    INT     iNum, iLen;
    DWORD   dwUCS4;

    if (pbszUTF8Str == NULL)
    {   // 参数错误  
        return 0;
    }

    iNum = 0;   // 统计有效字符个数  
    while (*pbszUTF8Str)
    {   // UTF8编码转换为UCS4编码  
        iLen = UTF8_To_UCS4(pbszUTF8Str, dwUCS4);
        if (iLen == 0)
        {   // 非法的UTF8编码  
            return 0;
        }

        pbszUTF8Str += iLen;

        // UCS4编码转换为UTF16编码  
        iLen = UCS4_To_UTF16(dwUCS4, pwszUTF16Str);
        if (iLen == 0)
        {
            return 0;
        }

        if (pwszUTF16Str != NULL)
        {
            pwszUTF16Str += iLen;
        }

        iNum += iLen;
    }

    if (pwszUTF16Str != NULL)
    {
        *pwszUTF16Str = 0;  // 写入字符串结束标记  
    }

    return iNum;
}
INT UTF16Str_To_UTF8Str(const WORD* pwszUTF16Str, BYTE* pbszUTF8Str)
{
    INT     iNum, iLen;
    DWORD   dwUCS4;

    if (pwszUTF16Str == NULL)
    {   // 参数错误  
        return 0;
    }

    iNum = 0;
    while (*pwszUTF16Str)
    {   // UTF16编码转换为UCS4编码  
        iLen = UTF16_To_UCS4(pwszUTF16Str, dwUCS4);
        if (iLen == 0)
        {   // 非法的UTF16编码  
            return 0;
        }

        pwszUTF16Str += iLen;

        // UCS4编码转换为UTF8编码  
        iLen = UCS4_To_UTF8(dwUCS4, pbszUTF8Str);
        if (iLen == 0)
        {
            return 0;
        }

        if (pbszUTF8Str != NULL)
        {
            pbszUTF8Str += iLen;
        }

        iNum += iLen;
    }

    if (pbszUTF8Str != NULL)
    {
        *pbszUTF8Str = 0;   // 写入字符串结束标记  
    }

    return iNum;
}



// CaikotobachsDlg 对话框
DWORD GetProcessIDByName(const WCHAR* pName)

{

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (INVALID_HANDLE_VALUE == hSnapshot) {

		return NULL;

	}

	PROCESSENTRY32 pe = { sizeof(pe) };

	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {

		if (lstrcmpW(pe.szExeFile, pName) == 0) {

			CloseHandle(hSnapshot);

			return pe.th32ProcessID;

		}

		//printf("%-6d %s\n", pe.th32ProcessID, pe.szExeFile);

	}

	CloseHandle(hSnapshot);

	return 0;

}

BOOL* isexist = NULL;
HANDLE hProcess;
HMODULE patch = NULL;

wchar_t* ms_str = NULL;

CaikotobachsDlg::CaikotobachsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AIKOTOBA_CHS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CaikotobachsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CaikotobachsDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CaikotobachsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CaikotobachsDlg::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CaikotobachsDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CaikotobachsDlg::OnBnClickedButton2)
	ON_WM_HOTKEY()
	ON_WM_NCPAINT()
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_BUTTON3, &CaikotobachsDlg::OnBnClickedButton3)
    ON_EN_CHANGE(IDC_EDIT1, &CaikotobachsDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CaikotobachsDlg 消息处理程序
void InjectDLL(HANDLE hProcess, PCWSTR dllFilePathName) {
	int cch = 1 + lstrlenW(dllFilePathName);
	int cb = cch * sizeof(wchar_t);
	PWSTR PszLibFileRemote = (PWSTR)VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, PszLibFileRemote, (PVOID)dllFilePathName, cb, NULL);
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (PTHREAD_START_ROUTINE)LoadLibraryW, PszLibFileRemote, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
}
HANDLE InjectSelfTo(const WCHAR inptr[])
{

	LPPROCESS_INFORMATION info = new PROCESS_INFORMATION;
	STARTUPINFO si = { sizeof(si) };
	do {
		wchar_t m_CommandLine[] = L"";
		BOOL hF = CreateProcess(inptr, NULL,
			NULL, NULL, FALSE,
			NULL, NULL, NULL, &si, info);
		if (!hF) {
			MessageBox(0, L"创建进程失败", L"错误", MB_ICONERROR);
			return 0;
		}
		return info->hProcess;
	} while (0);


	return   0;
}//同时调用InjectDLL，并试图自动远程执行HOOKSTART
DWORD trID = 0;

BOOL CaikotobachsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	patch = LoadLibrary(L"aikotoba_patch.dll");
	void (*pStart)(wchar_t[]) = (void(*)(wchar_t[]))::GetProcAddress(patch, "InjectSelfTo");
	pStart(L"cs2.exe");

	ms_str = (LPWSTR)::GetProcAddress(patch, "ms_str");
	SetTimer(1, 100, NULL);

	trID = GetProcessIDByName(L"cs2.exe");

	::RegisterHotKey(GetSafeHwnd(), WM_HOTKEY, MOD_ALT, 'N');
	isexist = (BOOL*)::GetProcAddress(patch,"exist");
    nID=(int*)::GetProcAddress(patch, "nID");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
static int i = 1;
void CaikotobachsDlg::OnPaint()
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
		if (i > 0) {
			i--;
			ShowWindow(SW_HIDE);
		}
	
		CDialogEx::OnPaint();
	}
	
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CaikotobachsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CaikotobachsDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
}


void CaikotobachsDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnCancel();
	ShowWindow(SW_HIDE);
}

HANDLE TestProcess = NULL;
char numStr[16];
void CaikotobachsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnTimer(nIDEvent);
}


void CaikotobachsDlg::OnBnClickedButton1()
{
	//AfxMessageBox(ms_str);
	// TODO: 在此添加控件通知处理程序代码
	void(*SetPch)(WCHAR[]) =(void(*)(WCHAR[]))::GetProcAddress(patch,"CreateDataExport");
	CString buf;
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(buf);
	SetPch(buf.GetBuffer());
}


void CaikotobachsDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
}

static int ShowInfo = 1;

void CaikotobachsDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{

    if (ShowInfo > 0) {
        int result = ::MessageBoxW(::GetForegroundWindow(), L"文本修改的功能是作者在 ※调试阶段※ 遗留的功能，\n这也就是说他是有效的，但是可能会降低程序的稳定性，\n你确定要继续吗？", L"提示", MB_ICONWARNING | MB_OKCANCEL);
        if (result == IDOK) {
            ShowInfo--;
        }
        else {
            CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
            return;
        }
    }

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nKey1 == MOD_ALT&& nKey2=='N') {
		//SetWindowPos();
		ShowWindow(SW_RESTORE);
	}
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


void CaikotobachsDlg::OnNcPaint()
{

}


void CaikotobachsDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//lpwndpos->flags &= ~SWP_SHOWWINDOW; // 去掉SWP_SHOWWINDOW标记


	CDialogEx::OnWindowPosChanging(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
}


void CaikotobachsDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
  
	
    GetDlgItem(IDC_EDIT1)->SetWindowTextW(ms_str);
}


void CaikotobachsDlg::OnEnChangeEdit1()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialogEx::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
}
