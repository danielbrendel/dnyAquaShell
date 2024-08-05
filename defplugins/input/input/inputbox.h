#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <Windows.h>
#include <string>

namespace InputBox {
	class CInputBox* __pThisInstance__;

	typedef class CInputBox* InputBoxHandle;

	class CInputBox {
	private:
		bool m_bReady;
		std::wstring m_szClassName;
		ATOM m_hClass;
		HWND m_hWindow;
		HWND m_hEdit;
		HWND m_hButtonOk;
		HWND m_hButtonCncl;
		HDC m_hDeviceCtx;
		std::wstring m_szLabelText;
		std::wstring m_szResultText;
		bool m_bShallRun;
		bool* m_pbOkButtonClicked;

		static const int m_iStaticWidth = 534;
		static const int m_iStaticHeight = 115;
		static const unsigned short m_usButtonOk = 201;
		static const unsigned short m_usButtonCncl = 202;

		friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void WindowCommandEventFired(HWND hWindow, WPARAM wParam, LPARAM lParam)
		{
			//Fired when WM_COMMAND is processed 

			if (HIWORD(wParam) == BN_CLICKED) { //Check for button click event
				if (LOWORD(wParam) == m_usButtonOk) {
					//Save result text to member property and set flag if required

					this->SaveResultText();

					if (this->m_pbOkButtonClicked)
						*this->m_pbOkButtonClicked = true;
				} else if (LOWORD(wParam) == m_usButtonCncl) {
					this->m_szResultText = L""; //Clear result member property
				}

				this->m_bShallRun = false; //Indicate to discontinue loop
				PostMessage(this->m_hWindow, WM_CLOSE, 0, 0); //Post close request to main window
			}
		}

		void WindowCloseEventFired()
		{
			//Fired when close event is processed

			if (this->m_bShallRun) { //Only true when window is not closed by the abort/ok button
				this->m_szResultText = L"";
				this->m_bShallRun = false;
			}
		}

		bool SaveResultText(void)
		{
			//Save result input string to member property

			wchar_t wszCharBuf[2048];
			//Acuire edit control text
			if (!GetWindowText(this->m_hEdit, wszCharBuf, sizeof(wszCharBuf)))
				return false;

			//Save to property
			this->m_szResultText = wszCharBuf;

			return true;
		}

		void ForceUpdate(void)
		{
			//Force an update of the component

			MSG sMsg;

			if (PeekMessage(&sMsg, NULL, 0, 0, PM_REMOVE)) { //Peek a possible available message
				TranslateMessage(&sMsg); //Translate message data
				DispatchMessage(&sMsg); //Dispatch message
			}

			//Not really neccessary here...
			TextOut(this->m_hDeviceCtx, 12, 50, this->m_szLabelText.c_str(), (int)this->m_szLabelText.length());

			//Update main window
			UpdateWindow(this->m_hWindow);
		}
	public:
		CInputBox() : m_bReady(false), m_pbOkButtonClicked(nullptr) {}
		CInputBox(const std::wstring& wszTitle, const std::wstring& wszLabel, const std::wstring& wszDefaultText, int x, int y) : m_bReady(false), m_pbOkButtonClicked(nullptr) { this->Instantiate(wszTitle, wszLabel, wszDefaultText, x, y); }
		CInputBox(const std::wstring& wszTitle, const std::wstring& wszLabel, const std::wstring& wszDefaultText, int x, int y, bool* pbResult) : m_bReady(false), m_pbOkButtonClicked(nullptr) { this->Instantiate(wszTitle, wszLabel, wszDefaultText, x, y, pbResult); }
		~CInputBox() { this->Close(); }

		bool Instantiate(const std::wstring& wszTitle, const std::wstring& wszLabel, const std::wstring& wszDefaultText, int x, int y, bool* pbOkButtonClicked = nullptr)
		{
			//Instantiate the inputbox instance

			if (this->m_bReady)
				return true;

			this->m_szClassName = L"InputBox::CInputBox->" + wszTitle;

			//Setup window class data structure
			WNDCLASSEXW sWndClass = { 0 };
			sWndClass.cbSize = sizeof(WNDCLASSEXW);
			sWndClass.style = CS_VREDRAW | CS_HREDRAW;
			sWndClass.hInstance = (HINSTANCE)GetCurrentProcess();
			sWndClass.lpszClassName = this->m_szClassName.c_str();
			sWndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			sWndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			sWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			sWndClass.lpfnWndProc = &WindowProc;
			
			//Register window class
			this->m_hClass = RegisterClassEx(&sWndClass);
			if (!this->m_hClass)
				return false;
			
			//Create parent window
			this->m_hWindow = CreateWindowEx(0, this->m_szClassName.c_str(), wszTitle.c_str(), WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, x, y, this->m_iStaticWidth, this->m_iStaticHeight, 0, 0, (HINSTANCE)GetCurrentProcess(), NULL);
			if (!this->m_hWindow)
				return false;

			//Create edit control
			this->m_hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", NULL, WS_CHILD | WS_VISIBLE, 10, 10, 500, 25, this->m_hWindow, 0, (HINSTANCE)GetCurrentProcess(), NULL);
			if (!this->m_hEdit)
				return false;

			//Set edit controls default text
			if (!SetWindowText(this->m_hEdit, wszDefaultText.c_str()))
				return false;

			//Create OK-Button
			this->m_hButtonOk = CreateWindowEx(0, L"Button", NULL, WS_CHILD | WS_VISIBLE, 360, 45, 70, 30, this->m_hWindow, (HMENU)m_usButtonOk, (HINSTANCE)GetCurrentProcess(), NULL);
			if (!this->m_hButtonOk)
				return false;

			//Set button text
			if (!SetWindowText(this->m_hButtonOk, L"Ok"))
				return false;

			//Create Abort-Button
			this->m_hButtonCncl = CreateWindowEx(0, L"Button", NULL, WS_CHILD | WS_VISIBLE, 438, 45, 70, 30, this->m_hWindow, (HMENU)m_usButtonCncl, (HINSTANCE)GetCurrentProcess(), NULL);
			if (!this->m_hButtonCncl)
				return false;

			//Set button text
			if (!SetWindowText(this->m_hButtonCncl, L"Cancel"))
				return false;

			//Acquire device context descriptor for the main window
			this->m_hDeviceCtx = GetDC(this->m_hWindow);
			if (!this->m_hDeviceCtx)
				return false;

			//Update parent window
			UpdateWindow(this->m_hWindow);

			//Save/Set further data
			this->m_szLabelText = wszLabel;
			__pThisInstance__ = this;
			this->m_bShallRun = true;
			this->m_szResultText = L"";
			this->m_pbOkButtonClicked = nullptr;

			//Handle result boolean storage object if provided
			if (pbOkButtonClicked) {
				this->m_pbOkButtonClicked = pbOkButtonClicked;
				*this->m_pbOkButtonClicked = false;
			}
			
			return this->m_bReady = true;
		}

		void Close(void)
		{
			//Cleanup resources

			if (!this->m_bReady)
				return;

			//Destroy parent window and controls
			DestroyWindow(this->m_hButtonCncl);
			DestroyWindow(this->m_hButtonOk);
			DestroyWindow(this->m_hEdit);
			DestroyWindow(this->m_hWindow);

			//Release acquired device context descriptor
			ReleaseDC(this->m_hWindow, this->m_hDeviceCtx);

			//Release registered class
			UnregisterClass(this->m_szClassName.c_str(), (HINSTANCE)GetCurrentProcess());

			//Clear data

			this->m_hButtonCncl = 0;
			this->m_hButtonOk = 0;
			this->m_hEdit = 0;
			this->m_hDeviceCtx = 0;
			this->m_hWindow = 0;
			this->m_hClass = 0;

			this->m_pbOkButtonClicked = nullptr;
			__pThisInstance__ = nullptr;
			this->m_bShallRun = false;

			this->m_bReady = false;
		}

		void Show(void)
		{
			//Show and handle inputbox

			MSG sMsg;

			while (this->m_bShallRun) { //Continue as long as no close request is fired and no button is pressed
				if (PeekMessage(&sMsg, NULL, 0, 0, PM_REMOVE)) { //Peek possible available message
					TranslateMessage(&sMsg); //Translate message data
					DispatchMessage(&sMsg); //Dispatch message to WndProc
				}

				//Draw label text
				TextOut(this->m_hDeviceCtx, 12, 50, this->m_szLabelText.c_str(), (int)this->m_szLabelText.length());

				//Update parent window
				UpdateWindow(this->m_hWindow);
			}
		}

		//Setters
		void SetLabel(const std::wstring& wszText) { this->m_szLabelText = wszText; }
		bool SetDefaultInput(const std::wstring& wszInput) { return SetWindowText(this->m_hEdit, wszInput.c_str()) == TRUE; }

		//Getters
		const bool Ready(void) const { return this->m_bReady; }
		std::wstring GetInputText(void) { return this->m_szResultText; }
		static const int GetStaticWidth(void) { return CInputBox::m_iStaticWidth; }
		static const int GetStaticHeight(void) { return CInputBox::m_iStaticHeight; }
	};

	/* Wrapper functions */

	InputBoxHandle Spawn(const std::wstring& wszTitle, const std::wstring& wszLabel, const std::wstring& wszDefaultText, int x, int y, bool* pbResult = nullptr)
	{
		CInputBox* pDialog = new CInputBox(wszTitle, wszLabel, wszDefaultText, x, y, pbResult);
		if (!pDialog)
			return  NULL;

		if (!pDialog->Ready()) {
			delete pDialog;
			return NULL;
		}

		return pDialog;
	}

	bool IsValid(InputBoxHandle pDialog)
	{
		return ((pDialog) && (pDialog->Ready()));
	}

	void Show(InputBoxHandle pDialog)
	{
		if (pDialog)
			pDialog->Show();
	}

	std::wstring Text(InputBoxHandle pDialog)
	{
		if (!pDialog)
			return L"";

		return pDialog->GetInputText();
	}

	const int GetStaticWidth(void)
	{
		return CInputBox::GetStaticWidth();
	}

	const int GetStaticHeight(void)
	{
		return CInputBox::GetStaticHeight();
	}

	void Free(InputBoxHandle pDialog)
	{
		if (!pDialog)
			return;

		delete pDialog;
	}

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//Window procedure

		switch (uMsg) {
		case WM_COMMAND: { //Also fired for button click events
				if (__pThisInstance__)
					__pThisInstance__->WindowCommandEventFired(hWnd, wParam, lParam);
			}
			break;
		case WM_CLOSE: { //Window shall be closed
				if (__pThisInstance__)
					__pThisInstance__->WindowCloseEventFired();
			}
			break;
		case WM_DESTROY: { //Window shall be destroyed
			PostQuitMessage(EXIT_SUCCESS);

			return 0;
		}
		default:
			break;
		}

		//Let Windows system handle the given message
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}