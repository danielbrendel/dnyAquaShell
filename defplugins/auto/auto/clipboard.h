#pragma once

#include <Windows.h>
#include <string>

namespace Clipboard {
	class CClipboardHandler {
	private:
		bool m_bReady;

		bool Initialize(HWND hWnd = NULL)
		{
			this->m_bReady = OpenClipboard(hWnd) == TRUE;
			return this->m_bReady;
		}

		void Free(void)
		{
			if (!this->m_bReady)
				return;

			CloseClipboard();
		}
	public:
		CClipboardHandler() : m_bReady(false) {}
		CClipboardHandler(HWND hWnd) { this->Initialize(hWnd); }
		~CClipboardHandler() { this->Free(); }

		inline bool IsReady(void) const { return this->m_bReady; }

		bool SetString(const std::wstring& wszString)
		{
			HGLOBAL hGlobal = GlobalAlloc(GHND, wszString.length() * 2 + 2);
			if (!hGlobal)
				return false;

			LPVOID lpvData = GlobalLock(hGlobal);
			if (!lpvData) {
				GlobalFree(hGlobal);
				return false;
			}

			wcscpy_s((wchar_t*)lpvData, wszString.length() * 2 + 2, wszString.c_str());

			if ((!GlobalUnlock(hGlobal)) && (GetLastError() != NO_ERROR)) {
				GlobalFree(hGlobal);
				return false;
			}

			return SetClipboardData(CF_UNICODETEXT, hGlobal) != NULL;
		}

		std::wstring GetString(void)
		{
			HANDLE hData = GetClipboardData(CF_UNICODETEXT);
			if (!hData)
				return L"";

			LPVOID lpvData = GlobalLock(hData);
			if (!lpvData)
				return L"";

			std::wstring wszResult = std::wstring((wchar_t*)lpvData);

			GlobalUnlock(hData);

			return wszResult;
		}

		bool ClearData(void)
		{
			//return EmptyClipboard() == TRUE;
			return this->SetString(L"");
		}
	};

	std::wstring GetClipboardString(void)
	{
		CClipboardHandler oClipboardHandler(NULL);
		return oClipboardHandler.GetString();
	}

	bool SetClipboardString(const std::wstring& wszString)
	{
		CClipboardHandler oClipboardHandler(NULL);
		return oClipboardHandler.SetString(wszString);
	}

	bool ClearClipboardData(void)
	{
		CClipboardHandler oClipboardHandler(NULL);
		return oClipboardHandler.ClearData();
	}
}