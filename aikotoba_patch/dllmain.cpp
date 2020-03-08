// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include "include/detours.h"
#pragma comment (lib,"detours.lib")

#include <stdlib.h>
#include <string>
using namespace std;

#define DLLAPI __declspec(dllexport)


#pragma data_seg("PublicData")      // 声明共享数据段，并命名该数据段
extern "C" DLLAPI BOOL exist = TRUE;
HMODULE hMod = NULL;
DWORD host_Id = 0;
extern "C" DLLAPI wchar_t ms_str[3096] = {0};
extern "C" DLLAPI int nID = 0;
#pragma data_seg()
#pragma comment(linker, "/section:PublicData,rws")

int npID = 0;

HMODULE SelfHandle = NULL;

char IpfData[16];

DWORD byteOfReading;
DWORD byteOfWriting;
struct IndexData {
    int Id;
    DWORD JpBass;
    DWORD JpLength;
    DWORD CnBass;
    DWORD CnLength;
};
#define OpenRegular(name,access) CreateFileW(name,access,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL)
#define Read(hF,buf,num) ReadFile(hF,buf,num,&byteOfReading,NULL)
#define Write(hF,buf,num) WriteFile(hF,buf,num,&byteOfWriting,NULL)

#define PutInt(a) _itoa_s(a,IpfData,10);MessageBoxA(0,IpfData,"num",0);


int GEtLargestID() {
    BOOL enFailed;
    DWORD dwSize;
    DWORD Largest = 0;

    HANDLE hIndex = OpenRegular(L"Index.ax", GENERIC_READ);
    if (hIndex == INVALID_HANDLE_VALUE) { MessageBoxA(0, "error", "5", 0); exit(-1); }
    dwSize = GetFileSize(hIndex, NULL);
    if (dwSize <= 0) {
        CloseHandle(hIndex);
        return 0;
    }
    BYTE* lpMem = new BYTE[dwSize];
    enFailed = Read(hIndex, lpMem, dwSize);
    if (!enFailed) {
        int aok = MessageBox(0, L"程序出现了一个异常（0），但是他仍然有可能正常运行，你要继续吗？", L"提示", MB_ICONERROR | MB_OKCANCEL);
        if (aok == IDCANCEL)exit(-1);
    }
    IndexData* tData = (IndexData*)lpMem;


    for (DWORD i = 0; i <= dwSize / sizeof(IndexData); i++) {
        if (i == (dwSize / sizeof(IndexData))) {
            CloseHandle(hIndex);
            delete[] lpMem;
            return Largest;
        }

        if ((tData + i)->Id > Largest) {
            Largest = (tData + i)->Id;
       }
    }

    return 0;
}
BOOL GetDataByID(int ID, LPWSTR jpBuff, LPWSTR cnBuffer) {
    BOOL enFailed;
    DWORD dwSize;

    HANDLE hIndex = OpenRegular(L"Index.ax",GENERIC_READ);
    if (hIndex == INVALID_HANDLE_VALUE) { MessageBoxA(0, "error", "5", 0); exit(-1); }
    dwSize = GetFileSize(hIndex, NULL);
    if (dwSize <= 0) {
        CloseHandle(hIndex);
        return 0;
    }
    BYTE* lpMem = new BYTE[dwSize];
    enFailed = Read(hIndex, lpMem, dwSize);
    if (!enFailed) {
        int aok = MessageBox(0, L"程序出现了一个异常（1），但是他仍然有可能正常运行，你要继续吗？", L"提示", MB_ICONERROR | MB_OKCANCEL);
        if (aok == IDCANCEL)exit(-1);
    }
    IndexData* tData = (IndexData*)lpMem;

    if ((tData + ID)->Id != ID)

        for (DWORD i = 0; i <= dwSize / sizeof(IndexData); i++) {
            if (i == (dwSize / sizeof(IndexData))) {
                CloseHandle(hIndex);
                delete[] lpMem;
                return 0;
            }

            if ((tData + i)->Id == ID) {
                tData += i;
                break;
            }
        }
    else
        tData += ID;


    HANDLE hData = OpenRegular(L"Data.ax", GENERIC_READ);
    if (hData == INVALID_HANDLE_VALUE) { MessageBoxA(0, "error", "5", 0); exit(-1); }
    //  PutInt(tData->JpBass);
    SetFilePointer(hData, tData->JpBass, NULL, FILE_BEGIN);
    enFailed = Read(hData, jpBuff, tData->JpLength);
    if (!enFailed) {
        int aok = MessageBox(0, L"程序出现了一个异常（2），但是他仍然有可能正常运行，你要继续吗？", L"提示", MB_ICONERROR | MB_OKCANCEL);
        if (aok == IDCANCEL)exit(-1);
    }
    //  PutInt(tData->CnBass);
    SetFilePointer(hData, tData->CnBass, NULL, FILE_BEGIN);
    enFailed = Read(hData, cnBuffer, tData->CnLength);
    if (!enFailed) {
        int aok = MessageBox(0, L"程序出现了一个异常（3），但是他仍然有可能正常运行，你要继续吗？", L"提示", MB_ICONERROR | MB_OKCANCEL);
        if (aok == IDCANCEL)exit(-1);
    }
    CloseHandle(hData);
    CloseHandle(hIndex);
    delete[] lpMem;
    return 1;
}
BOOL GetDataByJP(int* ID, LPCWSTR jpBuff, LPWSTR cnBuffer) {
    wstring LocalString = jpBuff;

    DWORD dwSize;
    BOOL  enFailed;
    HANDLE hIndex = OpenRegular(L"Index.ax", GENERIC_READ);
    HANDLE hData = OpenRegular(L"Data.ax", GENERIC_READ);
    if (hIndex == INVALID_HANDLE_VALUE) { MessageBoxA(0, "error", "5", 0); exit(-1); }
    if (hData == INVALID_HANDLE_VALUE) { MessageBoxA(0, "error", "5", 0); exit(-1); }
    dwSize = GetFileSize(hIndex, NULL);
    if (dwSize == 0) {
        *ID = 0;
        CloseHandle(hIndex);
        CloseHandle(hData);
        return 0;
    }

    BYTE* lpMem = new BYTE[dwSize];
    enFailed = Read(hIndex, lpMem, dwSize);
    if (!enFailed) {
        int aok = MessageBox(0, L"程序出现了一个异常（4），但是他仍然有可能正常运行，你要继续吗？", L"提示", MB_ICONERROR | MB_OKCANCEL);
        if (aok == IDCANCEL)exit(-1);
    }
    IndexData* tData = (IndexData*)lpMem;

    for (DWORD i = 0; i <= dwSize / sizeof(IndexData); i++) {
        if (i == dwSize / sizeof(IndexData)) {
            CloseHandle(hIndex);
            CloseHandle(hData);
            delete[] lpMem;
            *ID = i;
            return 0;
        }

        WCHAR njp[3096];
        WCHAR ncn[3096];

        SetFilePointer(hData, (tData+i)->JpBass, NULL, FILE_BEGIN);
        enFailed = Read(hData, njp, (tData + i)->JpLength);
        if (!enFailed) {
            int aok = MessageBox(0, L"程序出现了一个异常（5），但是他仍然有可能正常运行，你要继续吗？", L"提示", MB_ICONERROR | MB_OKCANCEL);
            if (aok == IDCANCEL)exit(-1);
        }
        //  PutInt(tData->CnBass);
        SetFilePointer(hData, (tData + i)->CnBass, NULL, FILE_BEGIN);
        enFailed = Read(hData, ncn, (tData + i)->CnLength);
        if (!enFailed) {
            int a = GetLastError();
            PutInt(a);
            int aok = MessageBox(0, L"程序出现了一个异常（6），但是他仍然有可能正常运行，你要继续吗？", L"提示", MB_ICONERROR | MB_OKCANCEL);
            if (aok == IDCANCEL)exit(-1);
        }

        if (LocalString == njp) {
            lstrcpyW(cnBuffer, ncn);
            *ID = (tData + i)->Id;
            CloseHandle(hIndex);
            CloseHandle(hData);

            delete[] lpMem;
            return 1;
        }
    }

    return 0;
}
BOOL CreateDataByID(int ID, LPCWSTR jpBuff, int ljp, LPCWSTR cnBuffer, int lcn) {
 //   MessageBoxW(0, jpBuff, cnBuffer, 0);
    IndexData createData;
    createData.Id = ID;
    createData.JpLength = ljp;
    createData.CnLength = lcn;

    HANDLE hIndex = OpenRegular(L"Index.ax", GENERIC_READ|GENERIC_WRITE);
    HANDLE hData = OpenRegular(L"Data.ax", GENERIC_READ | GENERIC_WRITE);
    SetFilePointer(hIndex, NULL, NULL, FILE_END);
    SetFilePointer(hData, NULL, NULL, FILE_END);

    createData.JpBass = SetFilePointer(hData, NULL, NULL, FILE_CURRENT);
  //  PutInt(createData.JpBass);
    Write(hData, jpBuff, ljp);
  //  MessageBoxW(0, jpBuff,L"jp",0);
    createData.CnBass = SetFilePointer(hData, NULL, NULL, FILE_CURRENT);
  //  PutInt(createData.CnBass);
    Write(hData, cnBuffer, lcn);
  //  MessageBoxW(0, cnBuffer, L"cn", 0);

    Write(hIndex, &createData, sizeof(IndexData));

    CloseHandle(hData);
    CloseHandle(hIndex);
    return 1;
}
BOOL CreateDataByIDEx(LPCWSTR name,LPCWSTR name2,int ID, LPCWSTR jpBuff, int ljp, LPCWSTR cnBuffer, int lcn) {
    string abc = "created ID :";
    abc += ID;

  //  MessageBoxA(0, abc.c_str(),"",0);

    //   MessageBoxW(0, jpBuff, cnBuffer, 0);
    IndexData createData;
    createData.Id = ID;
    createData.JpLength = ljp;
    createData.CnLength = lcn;

    HANDLE hIndex = OpenRegular(name, GENERIC_READ | GENERIC_WRITE);
    HANDLE hData = OpenRegular(name2, GENERIC_READ | GENERIC_WRITE);
    SetFilePointer(hIndex, NULL, NULL, FILE_END);
    SetFilePointer(hData, NULL, NULL, FILE_END);

    createData.JpBass = SetFilePointer(hData, NULL, NULL, FILE_CURRENT);
    //  PutInt(createData.JpBass);
    Write(hData, jpBuff, ljp);
    //  MessageBoxW(0, jpBuff,L"jp",0);
    createData.CnBass = SetFilePointer(hData, NULL, NULL, FILE_CURRENT);
    //  PutInt(createData.CnBass);
    Write(hData, cnBuffer, lcn);
    //  MessageBoxW(0, cnBuffer, L"cn", 0);

    Write(hIndex, &createData, sizeof(IndexData));

    CloseHandle(hData);
    CloseHandle(hIndex);
    return 1;
}

void BuildCV(TESTDATA* pcv, TESTDATA flt,DWORD posinCross,DWORD posInLine,DWORD m_count,WCHAR nchar) {
    pcv->ForCast = flt.ForCast;
    pcv->nLine = posinCross;
    pcv->nChar = nchar;
    pcv->nCross = posInLine;
    pcv->Flag1 = flt.Flag1;
    pcv->Flag2 = flt.Flag2;
    pcv->Flag3 = flt.Flag3;
    pcv->Flag4 = flt.Flag4;
    pcv->Flagn4 = flt.Flagn4;
    pcv->Flag5 = flt.Flag5;
    pcv->Flag6 = flt.Flag6;
    pcv->count = m_count;
    pcv->Flag7 = posInLine;
    pcv->Flag8 = flt.Flag8;
    pcv->Flagn8 = flt.Flagn8;

    pcv->FlagnW = flt.FlagnW;
    pcv->FlagnQ = flt.FlagnQ;
    pcv->nWQ = flt.nWQ;
    pcv->ntime = flt.ntime;
    pcv->ntime2 = flt.ntime2;

    pcv->Over1 = flt.Over1;
    pcv->Over2 = flt.Over2;

    return;
}

DWORD WINAPI MsgBox(LPVOID lp) {
    MessageBoxW(0, (LPWSTR)lp,L"",0);

    return 0;
}

static int i = 1;
static HWND m_hWnd = NULL;
static int lastcontleid = 0xFFFFFFFF;
void ApplyStringToCV(LPCWSTR ws, DWORD pDf, int id) 
{  

  //  SetWindowTextW(m_hWnd, L"函数开始");
    

    wstring localString = ws;
    if (!pDf)return;
    if (!ws)return;
    DWORD Df = *(DWORD*)pDf;//Df 数据的指针
    if (!Df)return;//为零则终止函数
    TESTDATA* nCView = (TESTDATA*)Df;//使一个TESTDATA*指向Df
    DWORD pOld;
    VirtualProtect(nCView, 10 + (localString.length() * 2), PAGE_READWRITE, &pOld);
    TESTDATA loadFirst = *nCView;//保存第一个数据
    int posInLine = 0;//横排位置
    int posinCross = 0xD;//竖排位置

  //  SetWindowTextW(m_hWnd, L"进入循环");
    for (int i = 0; i < localString.length();i++)
    {
        if (localString[i] == L'\n') 
        {
            BuildCV(nCView, loadFirst, posinCross, posInLine, i * 2, 0);
            nCView++;
            BuildCV(nCView, loadFirst, posinCross, posInLine, i * 2, 0XFFFF);
            nCView++;
            posinCross += 0x28;
            posInLine = 0;
        }
        else 
        {
            BuildCV(nCView, loadFirst, posinCross, posInLine, i*2, localString[i]);
            nCView++;
            posInLine += 0X1A;
        }
    }
  //  SetWindowTextW(m_hWnd, L"跳出循环");
    *(DWORD*)(pDf + 4) = (DWORD)nCView;

    VirtualProtect(nCView, 10 + (localString.length() * 2), pOld, NULL);
}



BOOL InjectDLL(HANDLE hProcess, LPCWSTR dllFilePathName)
{
    if (!hProcess)
        return FALSE;
    int cch = 1 + lstrlenW(dllFilePathName);
    int cb = cch * sizeof(wchar_t);

    LPWSTR PszLibFileRemote = (LPWSTR)VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);

    if (!PszLibFileRemote)
        return FALSE;
    if (!WriteProcessMemory(hProcess, PszLibFileRemote, (LPVOID)dllFilePathName, cb, NULL))
        return FALSE;
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)
        LoadLibraryW, PszLibFileRemote, 0, NULL);
    if (!hThread) return FALSE;
    WaitForSingleObject(hThread, INFINITE);

    return TRUE;
}//实现dll注入
extern "C" DLLAPI HANDLE InjectSelfTo(wchar_t inptr[])
{
  //  host_Id = GetCurrentProcessId();

    HANDLE currentThread = NULL;

    LPPROCESS_INFORMATION info = new PROCESS_INFORMATION;
    STARTUPINFO si = { sizeof(si) };
    do {
        wchar_t m_CommandLine[] = L"";
        BOOL hF = CreateProcess(inptr, NULL,
            NULL, NULL, FALSE,
            CREATE_SUSPENDED, NULL, NULL, &si, info);
        if (!hF) {
            MessageBox(0, L"创建进程失败", L"错误", MB_ICONERROR);
            return 0;
        }
        //   MessageBox(0, L"1", L"", 0);
        wchar_t m_Path[MAX_PATH];
        GetModuleFileName(hMod, m_Path, MAX_PATH);
        if (!InjectDLL(info->hProcess, m_Path)) {
            MessageBoxA(0, "", "", 0);
            return 0;
        }

        currentThread = info->hThread;
    } while (0);

    HANDLE hHookStart = CreateRemoteThread(info->hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)
        ::GetProcAddress(hMod, "start"), 0, 0, NULL);

    if (!hHookStart)
    {
        MessageBox(0, L"无法创建远程线程(IAT HOOK)", L"错误", MB_ICONERROR);
        return  0;
    }
    WaitForSingleObject(hHookStart, 0);
    ResumeThread(info->hThread);
    delete info;

    lstrcpyW(ms_str, L" ");
    return  currentThread;
}//同时调用InjectDLL，并试图自动远程执行HOOKSTART
extern "C" DLLAPI void CreateDataExport(WCHAR data[]){


    WCHAR sjp[3096];
    WCHAR scn[3096];

    int lasger = GEtLargestID();

    if (nID - lasger > 2) {
        MessageBox(0, L"ID数值似乎不太对劲，\n程序因此取消了这次的行动。", L"错误", MB_ICONERROR);
        return;
    }
    if (!GetDataByID(nID-1, sjp, scn)) {

        CreateDataByID(nID - 1, ms_str, 2 * (lstrlenW(ms_str) + 1), data, 2 * (lstrlenW(data) + 1));
        WCHAR abv[16];
        _itow_s(nID - 1, abv, 10);
        wstring str;
        str += L"应用：ID:";
        str += abv;
        str += L"\n";
        str += ms_str;
        str += L"->";
        str += data;
        MessageBoxW(NULL, str.c_str(), L"成功添加翻译", MB_ICONINFORMATION);
        return;
    }
    else {
        wstring nString = L"该翻译条目已经存在，是否决定要替换他？\n(这会重构数据文件，根据您的电脑性能，可能会占用一些时间)\n";
        WCHAR abv[16];
        _itow_s(nID - 1, abv, 10);
        nString += L"ID:";
        nString += abv;
        nString += L"\n";
        nString += sjp;
        int result = MessageBoxW(NULL, nString.c_str(), L"信息", MB_ICONINFORMATION | MB_OKCANCEL);


        if (result != IDOK)
            return;


        int p = 0;
        while (GetDataByID(p, sjp, scn)) {
            if (p == (nID - 1)) {
                CreateDataByIDEx(L"~Index.ax", L"~Data.ax", p, sjp, 2 * (lstrlenW(sjp) + 1), data, 2 * (lstrlenW(data) + 1));
            }
            else
            {
                CreateDataByIDEx(L"~Index.ax", L"~Data.ax", p, sjp, 2 * (lstrlenW(sjp) + 1), scn, 2 * (lstrlenW(scn) + 1));
            }
            p++;
        }
        DeleteFile(L"Data.ax");
        DeleteFile(L"Index.ax");

        rename("~Data.ax","Data.ax");
        rename("~Index.ax", "Index.ax");
        return;
    }
}




extern "C" DLLAPI int TranSplete(DWORD lp)
{

    
    if (!lp)return -1;
    DWORD ppDf = *(DWORD*)(lp + 0x2B0);
    if (!ppDf)return -1;
    DWORD pDf = *(DWORD*)(ppDf + 8);
    if (!pDf)return -1;
    DWORD Df = *(DWORD*)pDf;
    if (!Df)return -1;
    DWORD pEnd = *(DWORD*)(pDf + 4);
    if (!pEnd)return -1;

    WCHAR sjp[3096];
    WCHAR scn[3096];

    DWORD Num = (pEnd - Df) / 96;

    wstring nStr;
    TESTDATA* nCView = (TESTDATA*)Df;

    for (DWORD i = 0; i < Num; i++) {
        if (nCView->nChar == 0xFFFF) {
         
        }
        else if (nCView->nChar == 0) {
            nStr += L'\n';
        }
        else {
            nStr += nCView->nChar;
        }

        nCView++;
    }
    npID++;
   // MessageBoxW(0,L"0",L"",0);
    WCHAR csjp[3096];
    WCHAR cscn[3096];
    int ppId = 0;
    if (!GetDataByID(nID - 1, csjp, cscn)) {
        if (nID != 0)
            if (!GetDataByJP(&ppId, nStr.c_str(), cscn))
                return -1;
   }

   // MessageBoxW(0, L"1", L"", 0);
    if (lstrcmpW(nStr.c_str(), cscn) == 0) {

   //     MessageBoxW(0, nStr.c_str(), L"", 0);
        return -1;
    }

  //  MessageBoxW(0, L"2", L"", 0);
    if (wcsstr(nStr.c_str(), L"………")) {
  
        return -1;
   }
    if (wcsstr(nStr.c_str(), L"…ああ"))
        return -1;

    if (wcsstr(nStr.c_str(), L"ふふふっ"))
        return -1;

 //   MessageBoxW(0, L"3", L"", 0);

    lstrcpyW(ms_str, nStr.c_str());

 //   MessageBoxW(0, L"4", L"", 0);

    if (!GetDataByID(nID, sjp, scn)) {
        int pID = 0;
        if (GetDataByJP(&pID, nStr.c_str(), scn)) {
            nID = pID+1;
            ApplyStringToCV(scn, pDf, pID);
            return 0;
        }
    }
    else
    {
        if (lstrcmpW(sjp, nStr.c_str()) == 0) {
            ApplyStringToCV(scn, pDf, nID);
        }
        else
        {
            int pID = 0;
            if (GetDataByJP(&pID, nStr.c_str(), scn)) {
                nID = pID + 1;
                ApplyStringToCV(scn, pDf, pID);
                return 0;
            }
            else
                nID = pID;
        }
    }
    nID++;
    return 0;
}

int (*sub_63F6B0)(DWORD,DWORD,DWORD) = (int(*)(DWORD, DWORD, DWORD))0x63F6B0;

signed int (*sub_5FC1C0)(
    ) = (signed int(*)(void))0x5FC1C0;
signed int Fakesub_5FC1C0(
) {
    DWORD leax,lebx,lecx,ledx,lesi,ledi;
    __asm {
        mov dword ptr [leax],eax
        mov dword ptr [lebx], ebx
        mov dword ptr [lecx], ecx
        mov dword ptr [ledx], edx
        mov dword ptr [lesi], esi
        mov dword ptr [ledi], edi
    }
   // SetWindowTextW(m_hWnd, L"进入hook");
    ((int(*)(DWORD))::GetProcAddress(SelfHandle, "TranSplete"))(lecx);
  //  SetWindowTextW(m_hWnd, L"hook结束");
    __asm {
        mov  eax, dword ptr[leax]
        mov  ebx, dword ptr[lebx]
        mov ecx, dword ptr[lecx]
        mov edx, dword ptr[ledx]
        mov  esi, dword ptr[lesi]
        mov edi, dword ptr[ledi]
    }

    return sub_5FC1C0();
}

bool start_falg = false;

DWORD WINAPI TitleThread(LPVOID lp) {
    while (!(m_hWnd = ::FindWindowW(NULL, L"アイコトバ")))
        Sleep(10);
    if (i > 0) {
       
        SetWindowTextW(m_hWnd, L"恋语");
        i--;
    }

    return 0;
}

extern "C" DLLAPI void start() {
    //?printSub@RetouchPrintManager@@AAEXPBDKAAH1@Z
    CreateThread(0, 0, TitleThread, 0, 0, 0);

    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)sub_5FC1C0, Fakesub_5FC1C0);
    DetourTransactionCommit();

    start_falg=TRUE;
    host_Id = GetCurrentProcessId();
    //  pProc(0,"HHH",1);

}

void end() {
    //?printSub@RetouchPrintManager@@AAEXPBDKAAH1@Z

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)sub_5FC1C0, Fakesub_5FC1C0);
    DetourTransactionCommit();
    //  pProc(0,"HHH",1);

}
DWORD WINAPI ShowcountThread(LPVOID lPvoid)
{
    WCHAR pSfn[36];

    while (1)
    {
        _itow_s(npID, pSfn, 10);
        SetWindowTextW(m_hWnd, pSfn);
        Sleep(100);
    }

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, ShowcountThread, 0, 0, 0);
        hMod = hModule;
        SelfHandle = hModule;
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:

  
        if (start_falg)
            end();
        break;
    }
    return TRUE;
}

