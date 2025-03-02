#pragma once

#include "dnyas_sdk.h"
#include <windowsx.h>

namespace DxWindow {
	const std::wstring DXSTR_FLAG_FALSE = L"0";
	const std::wstring DXSTR_FLAG_TRUE = L"1";

	class CDxWindow* pDxWindowInstance = nullptr;
	IShellPluginAPI* pShellPluginAPI = nullptr;

	struct wnd_script_events_s {
		dnyString wszWndOnCreate;
		dnyString wszWndOnRelease;
		dnyString wszWndOnDraw;
		dnyString wszWndOnKeyEvent;
		dnyString wszWndOnMouseEvent;
	} sWndScriptEvents;

	class CDxWindow {
	private:
		bool m_bReady;
		std::wstring m_szClassName;
		ATOM m_hClass;
		HWND m_hWindow;
		wnd_script_events_s m_sEvents;

		friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void CreateEvent(HWND hWnd)
		{
			pShellPluginAPI->Scr_ExecuteCode(L"call " + this->m_sEvents.wszWndOnCreate + L"(" + std::to_wstring((dnyInteger)hWnd) + L") => void;");
		}

		LRESULT KeyEvent(int vKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld)
		{
			pShellPluginAPI->Scr_ExecuteCode(L"call " + this->m_sEvents.wszWndOnKeyEvent + L"(" + std::to_wstring(vKey) + L", " + (bDown ? DXSTR_FLAG_TRUE : DXSTR_FLAG_FALSE) + +L", " + (bCtrlHeld ? DXSTR_FLAG_TRUE : DXSTR_FLAG_FALSE) + L", " + (bShiftHeld ? DXSTR_FLAG_TRUE : DXSTR_FLAG_FALSE) + L", " + (bAltHeld ? DXSTR_FLAG_TRUE : DXSTR_FLAG_FALSE) + L") => void;");
			return 0;
		}

		LRESULT MouseEvent(int x, int y, int iMouseKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld)
		{
			pShellPluginAPI->Scr_ExecuteCode(L"call " + this->m_sEvents.wszWndOnMouseEvent + L"(" + std::to_wstring(x) + L", " + std::to_wstring(y) + L", " + std::to_wstring(iMouseKey) + L", " + (bDown ? DXSTR_FLAG_TRUE : DXSTR_FLAG_FALSE) + +L", " + (bCtrlHeld ? DXSTR_FLAG_TRUE : DXSTR_FLAG_FALSE) + L", " + (bShiftHeld ? DXSTR_FLAG_TRUE : DXSTR_FLAG_FALSE) + L", " + (bAltHeld ? DXSTR_FLAG_TRUE : DXSTR_FLAG_FALSE) + L") => void;");
			return 0;
		}

		void DrawEvent(void)
		{
			pShellPluginAPI->Scr_ExecuteCode(L"call " + this->m_sEvents.wszWndOnDraw + L"() => void;");
		}

		void ReleaseEvent(void)
		{
			pShellPluginAPI->Scr_ExecuteCode(L"call " + this->m_sEvents.wszWndOnRelease + L"() => void;");
		}

		LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			//Window procedure handling

			switch (uMsg) {
			case WM_CREATE:
				this->CreateEvent(hWnd);
				break;
			//Input handling:
			case WM_KEYUP: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->KeyEvent((int)wParam, false, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_KEYDOWN: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->KeyEvent((int)wParam, true, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_LBUTTONUP: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->MouseEvent(-1, -1, VK_LBUTTON, false, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_LBUTTONDOWN: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->MouseEvent(-1, -1, VK_LBUTTON, true, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_RBUTTONUP: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->MouseEvent(-1, -1, VK_RBUTTON, false, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_RBUTTONDOWN: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->MouseEvent(-1, -1, VK_RBUTTON, true, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_MBUTTONUP: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->MouseEvent(-1, -1, VK_MBUTTON, false, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_MBUTTONDOWN: {
				bool bCtrlHeld = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
				bool bShiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
				bool bAltHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
				return this->MouseEvent(-1, -1, VK_MBUTTON, true, bCtrlHeld, bShiftHeld, bAltHeld);
				break;
			}
			case WM_MOUSEMOVE:
				return this->MouseEvent(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, false, false, false, false);
				break;
			case WM_PAINT: //Window shall paint
				this->DrawEvent();
				return 0;
				break;
			case WM_QUIT: //Window shall close
				this->ReleaseEvent();
				this->Release();
				return 0;
				break;
			case WM_DESTROY: { //Window gets destroyed
				this->ReleaseEvent();
				PostQuitMessage(EXIT_SUCCESS);
				this->Release();
				return 0;
			}
			default:
				break;
			}

			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

		bool CalcWindowPosition(int w, int h, int& x, int& y)
		{
			//Calculate center screen coordinates for window according to its resolution

			//Get handle to desktop window
			HWND hDesktopWindow = GetDesktopWindow();
			if (!hDesktopWindow)
				return false;

			RECT sRect;

			//Get screen size data of window
			if (!GetWindowRect(hDesktopWindow, &sRect))
				return false;

			//Calculate position
			x = sRect.right / 2 - w / 2;
			y = sRect.bottom / 2 - h / 2;

			return true;
		}
	public:
		CDxWindow() : m_bReady(false) { pDxWindowInstance = this; }
		~CDxWindow() { this->Release(); }

		bool Initialize(const std::wstring& wszTitle, int w, int h, const wnd_script_events_s& rEvents)
		{
			//Initialize window
			
			if (this->m_bReady)
				return true;

			//Save/Set data
			this->m_sEvents = rEvents;
			this->m_szClassName = L"DxWindow->" + wszTitle;

			//Setup window class data struct
			WNDCLASSEXW sWndClass = { 0 };
			sWndClass.cbSize = sizeof(WNDCLASSEXW);
			sWndClass.style = CS_VREDRAW | CS_HREDRAW;
			sWndClass.hInstance = (HINSTANCE)GetCurrentProcess();
			sWndClass.lpszClassName = this->m_szClassName.c_str();
			sWndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			sWndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			sWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			sWndClass.lpfnWndProc = &WindowProc;

			//Calculate window position
			int x, y;
			if (!this->CalcWindowPosition(w, h, x, y))
				return false;
			
			//Register window class
			this->m_hClass = RegisterClassEx(&sWndClass);
			if (!this->m_hClass)
				return false;
			
			//Create the window
			this->m_hWindow = CreateWindowEx(0, this->m_szClassName.c_str(), wszTitle.c_str(), WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, x, y, w, h, 0, 0, (HINSTANCE)GetCurrentProcess(), NULL);
			if (!this->m_hWindow)
				return false;
			
			//Update the window initiallly
			UpdateWindow(this->m_hWindow);

			return this->m_bReady = true;
		}

		void Release(void)
		{
			//Release resources

			if (!this->m_bReady)
				return;

			//Release window and unregister class
			DestroyWindow(this->m_hWindow);
			UnregisterClass(this->m_szClassName.c_str(), (HINSTANCE)GetCurrentProcess());

			//Clear data
			this->m_hWindow = 0;
			this->m_hClass = 0;

			this->m_bReady = false;
		}

		bool Run(void)
		{
			//Perform main loop

			MSG sMsg;

			//Handle message queue
			if (PeekMessage(&sMsg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&sMsg);
				DispatchMessage(&sMsg);
			}

			//Update window
			UpdateWindow(this->m_hWindow);

			//Return indicator whether window does still exist or not
			return IsWindow(this->m_hWindow) == TRUE;
		}

		//Getters
		inline const bool IsReady(void) const { return this->m_bReady; }
		inline const HWND GetHandle(void) const { return this->m_hWindow; }
	} oDxWindow;

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return pDxWindowInstance->WndProc(hWnd, uMsg, wParam, lParam);
	}

	class ISetEventCommandInterface : public IVoidCommandInterface {
	public:
		ISetEventCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			dnyString wszEventName = pContext->GetPartString(1);
			if (wszEventName == L"OnCreate") {
				sWndScriptEvents.wszWndOnCreate = pContext->GetPartString(2);
			} else if (wszEventName == L"OnDestroy") {
				sWndScriptEvents.wszWndOnRelease = pContext->GetPartString(2);
			} else if (wszEventName == L"OnKeyInput") {
				sWndScriptEvents.wszWndOnKeyEvent = pContext->GetPartString(2);
			} else if (wszEventName == L"OnMouseInput") {
				sWndScriptEvents.wszWndOnMouseEvent = pContext->GetPartString(2);
			} else if (wszEventName == L"OnDraw") {
				sWndScriptEvents.wszWndOnDraw = pContext->GetPartString(2);
			} else { return false; }

			return true;
		}

	} oSetEventCommandInterface;

	class ICreateCommandInterface : public IVoidCommandInterface {
	public:
		ICreateCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			return oDxWindow.Initialize(pContext->GetPartString(1), (int)pContext->GetPartInt(2), (int)pContext->GetPartInt(3), sWndScriptEvents);
		}

	} oCreateCommandInterface;

	class IReleaseCommandInterface : public IVoidCommandInterface {
	public:
		IReleaseCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			oDxWindow.Release();

			return true;
		}

	} oReleaseCommandInterface;

	class IGetHandleCommandInterface : public IResultCommandInterface<dnyInteger> {
	public:
		IGetHandleCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			IResultCommandInterface<dnyInteger>::SetResult((dnyInteger)oDxWindow.GetHandle());

			return true;
		}

	} oGetHandleCommandInterface;

	class IIsValidCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		IIsValidCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			IResultCommandInterface<dnyBoolean>::SetResult(oDxWindow.IsReady());

			return true;
		}

	} oIsValidCommandInterface;

	class IProcessCommandInterface : public IVoidCommandInterface {
	public:
		IProcessCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			oDxWindow.Run();

			return true;
		}

	} oProcessCommandInterface;

	bool Initialize(IShellPluginAPI* pInterface)
	{
		//Initialize component

		if (!pInterface)
			return false;

		pShellPluginAPI = pInterface;

		#define REG_CMD(n, o, t) if (!pShellPluginAPI->Cmd_RegisterCommand(n, &o, t)) return false;
		REG_CMD(L"dx.wnd.set_event", oSetEventCommandInterface, CT_VOID);
		REG_CMD(L"dx.wnd.initialize", oCreateCommandInterface, CT_VOID);
		REG_CMD(L"dx.wnd.release", oReleaseCommandInterface, CT_VOID);
		REG_CMD(L"dx.wnd.is_valid", oIsValidCommandInterface, CT_BOOL);
		REG_CMD(L"dx.wnd.get_handle", oGetHandleCommandInterface, CT_INT);
		REG_CMD(L"dx.wnd.process", oProcessCommandInterface, CT_VOID);

		return true;
	}

	void Release(void)
	{
		//Release component

		#define UNREG_CMD(n) pShellPluginAPI->Cmd_UnregisterCommand(n)
		UNREG_CMD(L"dx.wnd.set_event");
		UNREG_CMD(L"dx.wnd.initialize");
		UNREG_CMD(L"dx.wnd.release");
		UNREG_CMD(L"dx.wnd.is_valid");
		UNREG_CMD(L"dx.wnd.get_handle");
		UNREG_CMD(L"dx.wnd.process");

		pShellPluginAPI = nullptr;
	}
}