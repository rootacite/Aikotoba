

#pragma once

#include <Windows.h>

class ClipboardExample
{
public:
    ClipboardExample();
    ~ClipboardExample();

    BOOL SetClipData(char* pstr);
    char* GetClipData();
    char* GetError();
private:
    DWORD errCode;
    HGLOBAL hClip;
};