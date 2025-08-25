#pragma once

#include <string>
#include <vector>
#include <Windows.h>
#include "dnyas_sdk.h"

namespace Automation {
	IShellPluginAPI* pShellPluginAPI = nullptr;
	class CSendInput* pSendInput = nullptr;

	inline HWND FindWindow(const std::wstring& wszClassName, const std::wstring& wszWindowText)
	{
		const wchar_t* pClassName = (wszClassName != L"\\0") ? wszClassName.c_str() : nullptr;
		const wchar_t* pWndName = (wszWindowText != L"\\0") ? wszWindowText.c_str() : nullptr;
		
		return ::FindWindow(pClassName, pWndName);
	}

	inline bool IsWindow(const HWND hWnd)
	{
		return ::IsWindow(hWnd) == TRUE;
	}

	inline HWND GetForegroundWindow(void)
	{
		return ::GetForegroundWindow();
	}

	inline bool SetForegroundWindow(HWND hWnd)
	{
		return ::SetForegroundWindow(hWnd) == TRUE;
	}

	inline bool SetWindowPos(HWND hWnd, int x, int y)
	{
		return ::SetWindowPos(hWnd, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE) == TRUE;
	}

	bool GetWindowPos(HWND hWnd, int& x, int& y)
	{
		RECT sRect;

		if (!::GetWindowRect(hWnd, &sRect))
			return false;

		x = sRect.left;
		y = sRect.top;

		return true;
	}

	bool GetWindowSize(HWND hWnd, int& w, int& h)
	{
		RECT sRect;

		if (!::GetWindowRect(hWnd, &sRect))
			return false;

		w = sRect.right - sRect.left;
		h = sRect.bottom - sRect.top;

		return true;
	}

	inline bool SetWindowText(HWND hWnd, const std::wstring& wszText)
	{
		return ::SetWindowText(hWnd, wszText.c_str()) == TRUE;
	}

	std::wstring GetWindowText(HWND hWnd)
	{
		
		INT len = ::GetWindowTextLength(hWnd);
		if (len <= 0)
			return L"";

		wchar_t* pwStr = new wchar_t[len + 2];
		if (!pwStr)
			return L"";

		memset(pwStr, 0x00, sizeof(wchar_t) * (len + 2));

		::GetWindowText(hWnd, pwStr, len + 1);
		std::wstring wszResult(pwStr);

		delete[] pwStr;

		return wszResult;
	}

	std::wstring GetClassName(HWND hWnd)
	{
		wchar_t* pwStr = new wchar_t[MAX_PATH * 4 + 1];
		if (!pwStr)
			return L"";

		::GetClassName(hWnd, pwStr, MAX_PATH * 4 + 1);
		std::wstring wszResult(pwStr);

		delete[] pwStr;

		return wszResult;
	}

	inline bool ShowWindow(HWND hWnd, int nShowCmd)
	{
		return ::ShowWindow(hWnd, nShowCmd) == TRUE;
	}

	bool GetCursorPos(int& x, int& y)
	{
		POINT sPoint;

		if (!::GetCursorPos(&sPoint))
			return false;

		x = sPoint.x;
		y = sPoint.y;

		return true;
	}

	inline bool SetCursorPos(int x, int y)
	{
		return ::SetCursorPos(x, y) == TRUE;
	}

	inline bool Run(const std::wstring& wszObject, const std::wstring& wszParams, const std::wstring& wszPath)
	{
		const wchar_t* pObject = (wszObject != L"\\0") ? wszObject.c_str() : nullptr;
		const wchar_t* pParams = (wszParams != L"\\0") ? wszParams.c_str() : nullptr;
		const wchar_t* pPath = (wszPath != L"\\0") ? wszPath.c_str() : nullptr;

		return (unsigned __int64)::ShellExecute(0, L"open", pObject, pParams, pPath, SW_SHOWDEFAULT) > 32;
	}

	inline bool IsKeyDown(int vKey)
	{
		return ((::GetKeyState(vKey) & 0x8000) != 0);
	}

	inline bool IsKeyUp(int vKey)
	{
		return ((::GetKeyState(vKey) & 0x8000) == 0);
	}

	typedef bool(*TpfnEnumWndProc)(HWND hWnd, const std::wstring& wszTitleText);
	BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
	{
		TpfnEnumWndProc pfnRoutine = (TpfnEnumWndProc)lParam;
		if (!pfnRoutine)
			return FALSE;

		std::wstring wszText = GetWindowText(hwnd);

		return pfnRoutine(hwnd, wszText);
	}
	inline bool EnumWindows(const TpfnEnumWndProc pfnCallbackRoutine)
	{
		return ::EnumWindows(&EnumWindowsProc, (LPARAM)pfnCallbackRoutine) == TRUE;
	}
	inline bool EnumChildWindows(HWND hWnd, const TpfnEnumWndProc pfnCallbackRoutine)
	{
		return ::EnumChildWindows(hWnd, &EnumWindowsProc, (LPARAM)pfnCallbackRoutine) == TRUE;
	}

	#define VK_A 'A'
	#define VK_B 'B'
	#define VK_C 'C'
	#define VK_D 'D'
	#define VK_E 'E'
	#define VK_F 'F'
	#define VK_G 'G'
	#define VK_H 'H'
	#define VK_I 'I'
	#define VK_J 'J'
	#define VK_K 'K'
	#define VK_L 'L'
	#define VK_M 'M'
	#define VK_N 'N'
	#define VK_O 'O'
	#define VK_P 'P'
	#define VK_Q 'Q'
	#define VK_R 'R'
	#define VK_S 'S'
	#define VK_T 'T'
	#define VK_U 'U'
	#define VK_V 'V'
	#define VK_W 'W'
	#define VK_X 'X'
	#define VK_Y 'Y'
	#define VK_Z 'Z'
	class CSendInput {
	private:
		struct inpident_s {
			INT vKey;
			std::wstring wszName;
		};

		inpident_s m_vKeyIdentTable[256];
		inpident_s m_vMouseIdentTable[8];

		void Setup(void)
		{
			for (size_t i = 0; i < _countof(this->m_vKeyIdentTable); i++) {
				this->m_vKeyIdentTable[i].vKey = 0;
				this->m_vKeyIdentTable[i].wszName = L"";
			}
			for (size_t i = 0; i < _countof(this->m_vMouseIdentTable); i++) {
				this->m_vMouseIdentTable[i].vKey = 0;
				this->m_vMouseIdentTable[i].wszName = L"";
			}
			
			#define SET_KEY(key) this->m_vKeyIdentTable[key].vKey = key; this->m_vKeyIdentTable[key].wszName = TEXT(#key); pShellPluginAPI->Scr_ExecuteCode(L"const " + std::wstring(TEXT(#key)) + L" int <= " + std::to_wstring(key) + L";");
			#define SET_MOUSE(id, key) this->m_vMouseIdentTable[id].vKey = key; this->m_vMouseIdentTable[id].wszName = TEXT(#key); pShellPluginAPI->Scr_ExecuteCode(L"const " + std::wstring(TEXT(#key)) + L" int <= " + std::to_wstring(key) + L";");
			#define VM_LEFTDOWN MOUSEEVENTF_LEFTDOWN
			#define VM_LEFTUP MOUSEEVENTF_LEFTUP
			#define VM_RIGHTDOWN MOUSEEVENTF_RIGHTDOWN
			#define VM_RIGHTUP MOUSEEVENTF_RIGHTUP
			#define VM_MIDDLEDOWN MOUSEEVENTF_MIDDLEDOWN
			#define VM_MIDDLEUP MOUSEEVENTF_MIDDLEUP
			#define VM_XDOWN MOUSEEVENTF_XDOWN
			#define VM_XUP MOUSEEVENTF_XUP
			SET_MOUSE(0, VM_LEFTDOWN);
			SET_MOUSE(1, VM_LEFTUP);
			SET_MOUSE(2, VM_RIGHTDOWN);
			SET_MOUSE(3, VM_RIGHTUP);
			SET_MOUSE(4, VM_MIDDLEDOWN);
			SET_MOUSE(5, VM_MIDDLEUP);
			SET_MOUSE(6, VM_XDOWN);
			SET_MOUSE(7, VM_XUP);
			SET_KEY(VK_BACK); //0x08
			SET_KEY(VK_TAB); //0x09
			SET_KEY(VK_CLEAR); //0x0C
			SET_KEY(VK_RETURN); //0x0D
			SET_KEY(VK_SHIFT); //0x10
			SET_KEY(VK_CONTROL); //0x11
			SET_KEY(VK_MENU); //0x12
			SET_KEY(VK_PAUSE); //0x13
			SET_KEY(VK_CAPITAL); //0x14
			SET_KEY(VK_KANA); //0x15
			SET_KEY(VK_HANGEUL); //0x15
			SET_KEY(VK_HANGUL); //0x15
			SET_KEY(VK_JUNJA); //0x17
			SET_KEY(VK_FINAL); //0x18
			SET_KEY(VK_HANJA); //0x19
			SET_KEY(VK_KANJI); //0x19
			SET_KEY(VK_ESCAPE); //0x1B
			SET_KEY(VK_CONVERT); //0x1C
			SET_KEY(VK_NONCONVERT); //0x1D
			SET_KEY(VK_ACCEPT); //0x1E
			SET_KEY(VK_MODECHANGE); //0x1F
			SET_KEY(VK_SPACE); //0x20
			SET_KEY(VK_PRIOR); //0x21
			SET_KEY(VK_NEXT); //0x22
			SET_KEY(VK_END); //0x23
			SET_KEY(VK_HOME); //0x24
			SET_KEY(VK_LEFT); //0x25
			SET_KEY(VK_UP); //0x26
			SET_KEY(VK_RIGHT); //0x27
			SET_KEY(VK_DOWN); //0x28
			SET_KEY(VK_SELECT); //0x29
			SET_KEY(VK_PRINT); //0x2A
			SET_KEY(VK_EXECUTE); //0x2B
			SET_KEY(VK_SNAPSHOT); //0x2C
			SET_KEY(VK_INSERT); //0x2D
			SET_KEY(VK_DELETE); //0x2E
			SET_KEY(VK_HELP); //0x2F
			SET_KEY(VK_LWIN); //0x5B
			SET_KEY(VK_RWIN); //0x5C
			SET_KEY(VK_APPS); //0x5D
			SET_KEY(VK_SLEEP); //0x5F
			SET_KEY(VK_NUMPAD0); //0x60
			SET_KEY(VK_NUMPAD1); //0x61
			SET_KEY(VK_NUMPAD2); //0x62
			SET_KEY(VK_NUMPAD3); //0x63
			SET_KEY(VK_NUMPAD4); //0x64
			SET_KEY(VK_NUMPAD5); //0x65
			SET_KEY(VK_NUMPAD6); //0x66
			SET_KEY(VK_NUMPAD7); //0x67
			SET_KEY(VK_NUMPAD8); //0x68
			SET_KEY(VK_NUMPAD9); //0x69
			SET_KEY(VK_MULTIPLY); //0x6A
			SET_KEY(VK_ADD); //0x6B
			SET_KEY(VK_SEPARATOR); //0x6C
			SET_KEY(VK_SUBTRACT); //0x6D
			SET_KEY(VK_DECIMAL); //0x6E
			SET_KEY(VK_DIVIDE); //0x6F
			SET_KEY(VK_F1); //0x70
			SET_KEY(VK_F2); //0x71
			SET_KEY(VK_F3); //0x72
			SET_KEY(VK_F4); //0x73
			SET_KEY(VK_F5); //0x74
			SET_KEY(VK_F6); //0x75
			SET_KEY(VK_F7); //0x76
			SET_KEY(VK_F8); //0x77
			SET_KEY(VK_F9); //0x78
			SET_KEY(VK_F10); //0x79
			SET_KEY(VK_F11); //0x7A
			SET_KEY(VK_F12); //0x7B
			SET_KEY(VK_F13); //0x7C
			SET_KEY(VK_F14); //0x7D
			SET_KEY(VK_F15); //0x7E
			SET_KEY(VK_F16); //0x7F
			SET_KEY(VK_F17); //0x80
			SET_KEY(VK_F18); //0x81
			SET_KEY(VK_F19); //0x82
			SET_KEY(VK_F20); //0x83
			SET_KEY(VK_F21); //0x84
			SET_KEY(VK_F22); //0x85
			SET_KEY(VK_F23); //0x86
			SET_KEY(VK_F24); //0x87
			SET_KEY(VK_NUMLOCK); //0x90
			SET_KEY(VK_SCROLL); //0x91
			SET_KEY(VK_OEM_NEC_EQUAL); //0x92
			SET_KEY(VK_OEM_FJ_JISHO); //0x92
			SET_KEY(VK_OEM_FJ_MASSHOU); //0x93
			SET_KEY(VK_OEM_FJ_TOUROKU); //0x94
			SET_KEY(VK_OEM_FJ_LOYA); //0x95
			SET_KEY(VK_OEM_FJ_ROYA); //0x96
			SET_KEY(VK_LSHIFT); //0xA0
			SET_KEY(VK_RSHIFT); //0xA1
			SET_KEY(VK_LCONTROL); //0xA2
			SET_KEY(VK_RCONTROL); //0xA3
			SET_KEY(VK_LMENU); //0xA4
			SET_KEY(VK_RMENU); //0xA5
			SET_KEY(VK_A); //A
			SET_KEY(VK_B); //B
			SET_KEY(VK_C); //C
			SET_KEY(VK_D); //D
			SET_KEY(VK_E); //E
			SET_KEY(VK_F); //F
			SET_KEY(VK_G); //G
			SET_KEY(VK_H); //H
			SET_KEY(VK_I); //I
			SET_KEY(VK_J); //J
			SET_KEY(VK_K); //K
			SET_KEY(VK_L); //L
			SET_KEY(VK_M); //M
			SET_KEY(VK_N); //N
			SET_KEY(VK_O); //O
			SET_KEY(VK_P); //P
			SET_KEY(VK_Q); //Q
			SET_KEY(VK_R); //R
			SET_KEY(VK_S); //S
			SET_KEY(VK_T); //T
			SET_KEY(VK_U); //U
			SET_KEY(VK_V); //V
			SET_KEY(VK_W); //W
			SET_KEY(VK_X); //X
			SET_KEY(VK_Y); //Y
			SET_KEY(VK_Z); //Z
		}

		int GetKeyFromTable(const std::wstring& wszKey)
		{
			for (size_t i = 0; i < _countof(this->m_vKeyIdentTable); i++) {
				if (this->m_vKeyIdentTable[i].wszName == wszKey)
					return this->m_vKeyIdentTable[i].vKey;
			}

			return -1;
		}

		int GetMouseFromTable(const std::wstring& wszKey)
		{
			for (size_t i = 0; i < _countof(this->m_vMouseIdentTable); i++) {
				if (this->m_vMouseIdentTable[i].wszName == wszKey)
					return this->m_vMouseIdentTable[i].vKey;
			}

			return -1;
		}

		std::vector<::INPUT> SplitKeyboardInputs(const std::wstring& wszInputStr)
		{
			std::vector<::INPUT> vResult;
			::INPUT sInput;
			bool bInSpecial = false;
			std::wstring wszSpecial;

			memset(&sInput, 0x00, sizeof(sInput));
			sInput.type = INPUT_KEYBOARD;
			
			for (size_t i = 0; i < wszInputStr.length(); i++) {
				if (wszInputStr[i] == '\\') {
					bInSpecial = true;
					continue;
				} else if ((wszInputStr[i] == ';') && (bInSpecial)) {
					sInput.ki.wVk = this->GetKeyFromTable(wszSpecial);
					sInput.ki.wScan = 0;
					sInput.ki.dwFlags = 0;
					vResult.push_back(sInput);
					sInput.ki.dwFlags = KEYEVENTF_KEYUP;
					vResult.push_back(sInput);

					wszSpecial.clear();
					bInSpecial = false;
					continue;
				}

				if (bInSpecial) wszSpecial += wszInputStr[i];
				else {
					sInput.ki.wVk = 0;
					sInput.ki.wScan = wszInputStr[i];
					sInput.ki.dwFlags = KEYEVENTF_UNICODE;
					vResult.push_back(sInput);
					sInput.ki.dwFlags |= KEYEVENTF_KEYUP;
					vResult.push_back(sInput);
				}
			}
			
			return vResult;
		}

		std::vector<::INPUT> SplitMouseInputs(const std::wstring& wszInputStr)
		{
			std::vector<::INPUT> vResult;
			::INPUT sInput;
			bool bInSpecial = false;
			std::wstring wszSpecial;

			memset(&sInput, 0x00, sizeof(sInput));
			sInput.type = INPUT_MOUSE;

			for (size_t i = 0; i < wszInputStr.length(); i++) {
				if (wszInputStr[i] == '\\') {
					bInSpecial = true;
					continue;
				} else if ((wszInputStr[i] == ';') && (bInSpecial)) {
					sInput.mi.dwFlags = this->GetMouseFromTable(wszSpecial);
					sInput.mi.dwExtraInfo = 0;
					sInput.mi.dx = sInput.mi.dy = 0;
					sInput.mi.mouseData = 0;
					sInput.mi.time = 0;
					vResult.push_back(sInput);

					wszSpecial.clear();
					bInSpecial = false;
					continue;
				}

				if (bInSpecial) wszSpecial += wszInputStr[i];
			}

			return vResult;
		}

		bool SetCtrlState(bool bState)
		{
			INPUT sInput = { 0 };
			sInput.type = INPUT_KEYBOARD;
			sInput.ki.wVk = VK_CONTROL;
			sInput.ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
			sInput.ki.dwFlags = (bState) ? 0 : KEYEVENTF_KEYUP;

			return SendInput(1, &sInput, sizeof(INPUT)) == 1;
		}

		bool SetShiftState(bool bState)
		{
			INPUT sInput = { 0 };
			sInput.type = INPUT_KEYBOARD;
			sInput.ki.wVk = VK_SHIFT;
			sInput.ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
			sInput.ki.dwFlags = (bState) ? 0 : KEYEVENTF_KEYUP;

			return SendInput(1, &sInput, sizeof(INPUT)) == 1;
		}

		bool SetAltState(bool bState)
		{
			INPUT sInput = { 0 };
			sInput.type = INPUT_KEYBOARD;
			sInput.ki.wVk = VK_MENU;
			sInput.ki.wScan = MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC);
			sInput.ki.dwFlags = (bState) ? 0 : KEYEVENTF_KEYUP;

			return SendInput(1, &sInput, sizeof(INPUT)) == 1;
		}
	public:
		CSendInput() { this->Setup(); }
		~CSendInput() {}

		bool KbdSend(const std::wstring& wszInputStr, bool bWithCtrl, bool bWithShift, bool bWithAlt)
		{
			std::vector<::INPUT> vInputs = this->SplitKeyboardInputs(wszInputStr);
			
			INPUT* pInputs = new INPUT[vInputs.size()];
			if (!pInputs)
				return false;
			
			for (size_t i = 0; i < vInputs.size(); i++) {
				pInputs[i] = vInputs[i];
			}
			
			if (bWithCtrl) this->SetCtrlState(true);
			if (bWithShift) this->SetShiftState(true);
			if (bWithAlt) this->SetAltState(true);
			
			bool bResult = ::SendInput((UINT)vInputs.size(), pInputs, sizeof(INPUT)) != 0;
			
			if (bWithCtrl) this->SetCtrlState(false);
			if (bWithShift) this->SetShiftState(false);
			if (bWithAlt) this->SetAltState(false);
			
			delete[] pInputs;
			
			return bResult;
		}

		bool MouSend(const std::wstring& wszInputStr, bool bWithCtrl, bool bWithShift, bool bWithAlt)
		{
			std::vector<::INPUT> vInputs = this->SplitMouseInputs(wszInputStr);

			INPUT* pInputs = new INPUT[vInputs.size()];
			if (!pInputs)
				return false;

			for (size_t i = 0; i < vInputs.size(); i++) {
				pInputs[i] = vInputs[i];
			}

			if (bWithCtrl) this->SetCtrlState(true);
			if (bWithShift) this->SetShiftState(true);
			if (bWithAlt) this->SetAltState(true);

			bool bResult = ::SendInput((UINT)vInputs.size(), pInputs, sizeof(INPUT)) != 0;

			if (bWithCtrl) this->SetCtrlState(false);
			if (bWithShift) this->SetShiftState(false);
			if (bWithAlt) this->SetAltState(false);

			delete[] pInputs;

			return bResult;
		}
	};
	
	inline LRESULT SendMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return ::SendMessage(hWnd, msg, wParam, lParam);
	}
	inline bool PostMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return ::PostMessage(hWnd, msg, wParam, lParam) == TRUE;
	}
	inline LONG GetWindowLong(HWND hWnd, int nIndex)
	{
		return ::GetWindowLong(hWnd, nIndex);
	}
	inline LONG SetWindowLong(HWND hWnd, int nIndex, LONG dwNewLong)
	{
		return ::SetWindowLong(hWnd, nIndex, dwNewLong);
	}
	inline BOOL UpdateWindow(HWND hWnd)
	{
		return ::UpdateWindow(hWnd);
	}
	inline bool SendKeyboardInput(const std::wstring& wszInputStr, bool bWithCtrl, bool bWithShift, bool bWithAlt)
	{
		return pSendInput->KbdSend(wszInputStr, bWithCtrl, bWithShift, bWithAlt);
	}
	inline bool SendMouseInput(const std::wstring& wszInputStr, bool bWithCtrl, bool bWithShift, bool bWithAlt)
	{
		return pSendInput->MouSend(wszInputStr, bWithCtrl, bWithShift, bWithAlt);
	}
	inline void KeyboardEvent(int vk, int scan, int flags)
	{
		keybd_event((BYTE)vk, (BYTE)scan, (DWORD)flags, 0);
	}
	inline void MouseEvent(int flags, int x, int y, int data)
	{
		mouse_event((DWORD)flags, (DWORD)x, (DWORD)y, (DWORD)data, 0);
	}

	class CInput* pInput = nullptr;
	LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	typedef void(*TpfnOnKeyInputEvent)(const std::wstring& wszString, bool bIsDown, bool bWithShift, bool bWithCtrl, bool bWithAlt);
	typedef void(*TpfnOnMouseInputEvent)(const std::wstring& wszString, bool bWithShift, bool bWithCtrl, bool bWithAlt);
	class CInput {
	private:
		struct inputevent_s {
			int vKey;
			std::wstring wszString;
			union {
				TpfnOnKeyInputEvent pKeyEvent;
				TpfnOnMouseInputEvent pMouseEvent;
			};
		};

		HHOOK m_hKeyboardHook, m_hMouseHook;
		std::vector<inputevent_s> m_vKeyEvents;
		std::vector<inputevent_s> m_vMouseEvents;

		friend LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
		friend LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

		LRESULT KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
		{
			if (nCode == HC_ACTION) {
				KBDLLHOOKSTRUCT* pKeyboardHookData = (KBDLLHOOKSTRUCT*)lParam;
				bool bShiftHeld = (GetKeyState(VK_LSHIFT) & 0x8000) > 0;
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) > 0;
				bool bAltlHeld = (GetKeyState(VK_MENU) & 0x8000) > 0;

				inputevent_s* pKeyEvent = this->GetEventDataByKey(pKeyboardHookData->vkCode);
				if (pKeyEvent) {
					pKeyEvent->pKeyEvent(pKeyEvent->wszString, wParam == WM_KEYDOWN, bShiftHeld, bCtrlHeld, bAltlHeld);
				}
			}

			return CallNextHookEx(this->m_hKeyboardHook, nCode, wParam, lParam);
		}

		LRESULT MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
		{
			if (nCode == HC_ACTION) {
				MSLLHOOKSTRUCT* pMouseHookData = (MSLLHOOKSTRUCT*)lParam;
				bool bShiftHeld = (GetKeyState(VK_LSHIFT) & 0x8000) > 0;
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) > 0;
				bool bAltlHeld = (GetKeyState(VK_MENU) & 0x8000) > 0;

				inputevent_s* pMouseEvent = this->GetEventDataByMouse((int)wParam);
				if (pMouseEvent) {
					pMouseEvent->pMouseEvent(pMouseEvent->wszString, bShiftHeld, bCtrlHeld, bAltlHeld);
				}
			}

			return CallNextHookEx(this->m_hMouseHook, nCode, wParam, lParam);
		}

		inputevent_s* GetEventDataByKey(int nKeyCode)
		{
			for (size_t i = 0; i < this->m_vKeyEvents.size(); i++) {
				if (this->m_vKeyEvents[i].vKey == nKeyCode) {
					return &this->m_vKeyEvents[i];
				}
			}

			return nullptr;
		}

		inputevent_s* GetEventDataByMouse(int nMouseCode)
		{
			for (size_t i = 0; i < this->m_vMouseEvents.size(); i++) {
				if (this->m_vMouseEvents[i].vKey == nMouseCode) {
					return &this->m_vMouseEvents[i];
				}
			}

			return nullptr;
		}

		void Free(void)
		{
			if (this->m_hKeyboardHook) {
				UnhookWindowsHookEx(this->m_hKeyboardHook);
				this->m_hKeyboardHook = 0;
			}

			if (this->m_hMouseHook) {
				UnhookWindowsHookEx(this->m_hMouseHook);
				this->m_hMouseHook = 0;
			}

			this->m_vKeyEvents.clear();
			this->m_vMouseEvents.clear();
		}
	public:
		CInput() : m_hKeyboardHook(0), m_hMouseHook(0) { pInput = this; }
		~CInput() { this->Free(); }

		bool AddKeyboardHook(const std::wstring& wszString, int vKey, const TpfnOnKeyInputEvent pEvent)
		{
			if (!this->m_hKeyboardHook) {
				this->m_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, NULL, 0);
				if (!this->m_hKeyboardHook)
					return false;
			}

			inputevent_s sEvent;
			sEvent.vKey = vKey;
			sEvent.wszString = wszString;
			sEvent.pKeyEvent = pEvent;
			this->m_vKeyEvents.push_back(sEvent);

			return true;
		}

		bool AddMouseHook(const std::wstring& wszString, int vKey, const TpfnOnMouseInputEvent pEvent)
		{
			if (!this->m_hMouseHook) {
				this->m_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, &LowLevelMouseProc, NULL, 0);
				if (!this->m_hMouseHook)
					return false;
			}

			inputevent_s sEvent;
			sEvent.vKey = vKey;
			sEvent.wszString = wszString;
			sEvent.pMouseEvent = pEvent;
			this->m_vMouseEvents.push_back(sEvent);

			return true;
		}

		void Process(void)
		{
			MSG sMsg;
			if (PeekMessage(&sMsg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&sMsg);
				DispatchMessage(&sMsg);
			}
		}
	} oInputMgr;
	LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		return pInput->KeyboardProc(nCode, wParam, lParam);
	}
	LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		return pInput->MouseProc(nCode, wParam, lParam);
	}
	bool AddKeyboardHook(const std::wstring& wszString, int vKey, const TpfnOnKeyInputEvent pEvent)
	{
		return oInputMgr.AddKeyboardHook(wszString, vKey, pEvent);
	}
	bool AddMouseHook(const std::wstring& wszString, int vKey, const TpfnOnMouseInputEvent pEvent)
	{
		return oInputMgr.AddMouseHook(wszString, vKey, pEvent);
	}
	void MK_Process(void)
	{
		oInputMgr.Process();
	}

	bool Init(IShellPluginAPI* pInterface)
	{
		if (!pInterface)
			return false;

		pShellPluginAPI = pInterface;
		pSendInput = new CSendInput();

		#define REG_SCRIPT_CONST(c) if (!pShellPluginAPI->Scr_ExecuteCode(L"const " #c L" int <= " + std::to_wstring(c) + L";")) return false;
		REG_SCRIPT_CONST(SW_FORCEMINIMIZE);
		REG_SCRIPT_CONST(SW_HIDE);
		REG_SCRIPT_CONST(SW_MAXIMIZE);
		REG_SCRIPT_CONST(SW_MINIMIZE);
		REG_SCRIPT_CONST(SW_RESTORE);
		REG_SCRIPT_CONST(SW_SHOW);
		REG_SCRIPT_CONST(SW_SHOWDEFAULT);
		REG_SCRIPT_CONST(SW_SHOWMAXIMIZED);
		REG_SCRIPT_CONST(SW_SHOWMINIMIZED);
		REG_SCRIPT_CONST(SW_SHOWMINNOACTIVE);
		REG_SCRIPT_CONST(SW_SHOWNA);
		REG_SCRIPT_CONST(SW_SHOWNOACTIVATE);
		REG_SCRIPT_CONST(SW_SHOWNORMAL);
		REG_SCRIPT_CONST(WM_LBUTTONDOWN);
		REG_SCRIPT_CONST(WM_LBUTTONUP);
		REG_SCRIPT_CONST(WM_RBUTTONDOWN);
		REG_SCRIPT_CONST(WM_RBUTTONUP);
		REG_SCRIPT_CONST(WM_MOUSEMOVE);

		return true;
	}
	void Release(void)
	{
		pShellPluginAPI = nullptr;
		delete pSendInput;
	}
}