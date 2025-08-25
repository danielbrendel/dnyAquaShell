#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <Windows.h>
#include <string>

namespace Message {
	int MsgBox(const std::wstring& wszText, const std::wstring& wszTitle, UINT uType, HWND hWnd)
	{
		return MessageBox(hWnd, wszText.c_str(), wszTitle.c_str(), uType);
	}

	BOOL MsgBeep(UINT uType)
	{
		return MessageBeep(uType);
	}

	BOOL SndBeep(DWORD dwFrequency, DWORD dwDuration)
	{
		return Beep(dwFrequency, dwDuration);
	}
}