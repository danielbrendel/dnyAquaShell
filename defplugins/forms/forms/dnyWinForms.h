#pragma once

#include <string>
#include <vector>
#include <Windows.h>
#include <CommCtrl.h>

/*
	dnyWinForms component developed by Daniel Brendel
	-> An OOP wrapper for Windows forms and controls using WinAPI

	(C) 2017 by Daniel Brendel
	
	Version: 0.1
	Contact: dbrendel1988<at>yahoo<dot>com
	GitHub: https://github.com/danielbrendel
	
	Licence: Creative Commons Attribution-NoDerivatives 4.0 International
*/

namespace dnyWinForms {
	LRESULT CALLBACK CommonWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/* About functions */
	std::wstring Name(void) { return L"dnyWinForms"; }
	std::wstring Author(void) { return L"Daniel Brendel"; }
	std::wstring Version(void) { return L"0.1"; }
	std::wstring Contact(void) { return L"dbrendel1988<at>yahoo<dot>com"; }

	/* Component classes */

	struct WFDimension { //2D dimensional handler class
	private:
		int m_iXPos, m_iYPos;
		int m_iOffDimension;
	public:
		WFDimension() : m_iXPos(0), m_iYPos(0), m_iOffDimension(-1) {}
		WFDimension(int x, int y) : m_iXPos(x), m_iYPos(y), m_iOffDimension(-1) {}
		~WFDimension() {}

		//Setters
		void SetX(int x) { this->m_iXPos = x; }
		void SetY(int y) { this->m_iYPos = y; }

		//Getters
		int GetX(void) { return this->m_iXPos; }
		int GetY(void) { return this->m_iYPos; }

		//Overloaded operators
		int operator[](int iIndex) const
		{
			switch (iIndex) {
			case 0:
				return this->m_iXPos;
				break;
			case 1:
				return this->m_iYPos;
				break;
			default:
				break;
			}

			return this->m_iOffDimension;
		}
		int& operator[](int iIndex)
		{
			switch (iIndex) {
			case 0:
				return this->m_iXPos;
				break;
			case 1:
				return this->m_iYPos;
				break;
			default:
				break;
			}

			return this->m_iOffDimension;
		}
	};

	class CFont { //Font object class
	private:
		HFONT m_hFont;
		std::wstring m_wszName;
		int m_iWidth, m_iHeight;
		bool m_bBold, m_bItalic, m_bUnderline, m_bStrikeOut;

		void Free(void)
		{
			//Release font

			if (this->m_hFont) {
				//Delete font object
				DeleteObject(this->m_hFont);
				//Clear value
				this->m_hFont = NULL;
			}
		}
	public:
		CFont() : m_hFont(NULL) {}
		CFont(const std::wstring& wszName, int iWidth, int iHeight, bool bBold, bool bItalic, bool bUnderline, bool bStrikeOut) { this->Instantiate(wszName, iWidth, iHeight, bBold, bItalic, bUnderline, bStrikeOut); }
		~CFont() { this->Free(); }

		bool Instantiate(const std::wstring& wszName, int iWidth, int iHeight, bool bBold, bool bItalic, bool bUnderline, bool bStrikeOut)
		{
			//Instantiate font object

			//Create font object
			this->m_hFont = CreateFont(iHeight, iWidth, 0, 0, (bBold) ? FW_BOLD : FW_NORMAL, (DWORD)bItalic, (DWORD)bUnderline, (DWORD)bStrikeOut, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, wszName.c_str());
			if (this->m_hFont) {
				//Store attributes
				this->m_wszName = wszName;
				this->m_iWidth = iWidth;
				this->m_iHeight = iHeight;
				this->m_bBold = bBold;
				this->m_bItalic = bItalic;
				this->m_bUnderline = bUnderline;
				this->m_bStrikeOut = bStrikeOut;

				return true;
			}

			return false;
		}

		//Getters
		inline const HFONT GetHandle(void) const { return this->m_hFont; }
		inline const std::wstring& GetName(void) const { return this->m_wszName; }
		inline const int GetWidth(void) const { return this->m_iWidth; }
		inline const int GetHeight(void) const { return this->m_iHeight; }
		inline const bool IsBold(void) const { return this->m_bBold; }
		inline const bool IsItalic(void) const { return this->m_bItalic; }
		inline const bool IsUnderline(void) const { return this->m_bUnderline; }
		inline const bool IsStrikeOut(void) const { return this->m_bStrikeOut; }
	};

	typedef std::wstring SCOMPONENTSTRING;
	typedef LPVOID PEVENTMETHOD;
	class IBaseComponent { //Base window component class
	private:
		HWND m_hParentWnd;
		SCOMPONENTSTRING m_wszTypeName;
		SCOMPONENTSTRING m_wszName;
		std::wstring m_wszText;
	protected:
		typedef void(*TpfnBaseEventMethod)(IBaseComponent* pThis, void* pEventData);
		struct eventmethod_s {
			PEVENTMETHOD pfnMethod;
			std::wstring wszEventName;
		};

		HWND m_hWindow;
		WFDimension m_dimPosition;
		WFDimension m_dimResolution;
		CFont* m_pFont;
		std::vector<eventmethod_s> m_vEventHandlers;

		virtual void SetTypeName(const SCOMPONENTSTRING& wszTypeName) { this->m_wszTypeName = wszTypeName; }

		bool AddEventHandler(const std::wstring& wszName, PEVENTMETHOD pfnEventMethod)
		{
			//Add event handler

			if ((!wszName.length()) || (!pfnEventMethod))
				return false;

			//Check if already exists
			if (this->EventHandlerExists(wszName))
				return false;

			//Setup data
			eventmethod_s sEvtMethod;
			sEvtMethod.wszEventName = wszName;
			sEvtMethod.pfnMethod = pfnEventMethod;

			//Add to list
			this->m_vEventHandlers.push_back(sEvtMethod);

			return true;
		}

		bool CallEventHandler(const std::wstring& wszName, void* pEventData)
		{
			//Call event handler

			for (size_t i = 0; i < this->m_vEventHandlers.size(); i++) { //Search in list of added event handlers
				if (this->m_vEventHandlers[i].wszEventName == wszName) { //Check if handler with given name exists
					((TpfnBaseEventMethod)(this->m_vEventHandlers[i].pfnMethod))(this, pEventData); //Call its event handler method with this-pointer and event data pointer
					return true;
				}
			}

			return false;
		}

		bool EventHandlerExists(const std::wstring& wszName, size_t* puiListId = nullptr)
		{
			//Check if event handler does already exist

			for (size_t i = 0; i < this->m_vEventHandlers.size(); i++) {
				if (this->m_vEventHandlers[i].wszEventName == wszName) {
					if (puiListId)
						*puiListId = i;

					return true;
				}
			}

			return false;
		}
	public:
		IBaseComponent() : m_wszTypeName(L"IBaseComponent"), m_hParentWnd(0), m_hWindow(0), m_pFont(NULL) {}
		~IBaseComponent() { this->Free(); }

		virtual bool Instantiate(HWND hParentWnd, const SCOMPONENTSTRING wszName, const WFDimension& dimPosition, const WFDimension& dimResolution)
		{
			//Instantiate object

			if ((!hParentWnd) || (!wszName.length()))
				return false;

			//Store component data
			this->m_hParentWnd = hParentWnd;
			this->m_wszName = wszName;
			this->m_dimPosition = dimPosition;
			this->m_dimResolution = dimResolution;

			return true;
		}

		virtual bool PostInstantiate(void)
		{
			//Perform operations after derived class has initialized its stuff

			//Move window initially
			if (!MoveWindow(this->m_hWindow, this->m_dimPosition[0], this->m_dimPosition[1], this->m_dimResolution[0], this->m_dimResolution[1], TRUE))
				return false;

			//Update window initially
			return (UpdateWindow(this->m_hWindow)) == TRUE;
		}

		virtual bool Free(void)
		{
			//Free resources
			
			//Release font if used
			if (this->m_pFont) {
				delete this->m_pFont;
				this->m_pFont = nullptr;
			}
			
			//Destroy the window if it is valid
			if (IsWindow(this->m_hWindow)) {
				if (!DestroyWindow(this->m_hWindow))
					return false;

				this->m_hWindow = 0;
			}

			//Clear data
			this->m_wszName.clear();
			this->m_vEventHandlers.clear();
			
			return true;
		}

		//Handle drawing
		virtual bool Draw(void) = 0;
		//Perform processing
		virtual bool Process(void) = 0;
		//Handle WM_COMMAND message. Return true if handled, otherwise false
		virtual bool HandleCommandMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
		//Handle WM_NOTIFY message. Return -1 if not handled, otherwise a custom value
		virtual LRESULT HandleNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

		//Setters
		virtual bool SetPosition(int x, int y)
		{
			//Update window position

			//Attempt to update the position
			if (MoveWindow(this->m_hWindow, x, y, this->m_dimResolution[0], this->m_dimResolution[1], TRUE)) {
				//Update new position data
				this->m_dimPosition[0] = x;
				this->m_dimPosition[1] = y;

				return true;
			}

			return false;
		}
		virtual bool SetResolution(int w, int h)
		{
			//Update window resolution

			//Attempt to update the resolution
			if (MoveWindow(this->m_hWindow, this->m_dimPosition[0], this->m_dimPosition[1], w, h, TRUE)) {
				//Update new position data
				this->m_dimResolution[0] = w;
				this->m_dimResolution[1] = h;

				return true;
			}

			return false;
		}
		virtual bool SetFont(const std::wstring& wszFont, int iWidth, int iHeight, bool bBold, bool bItalic, bool bUnderline, bool bStrikeOut)
		{
			//Update component font

			//Release previous font if exists
			if (this->m_pFont)
				delete this->m_pFont;

			//Instantiate new font object
			this->m_pFont = new CFont(wszFont, iWidth, iHeight, bBold, bItalic, bUnderline, bStrikeOut);
			if (!this->m_pFont)
				return false;

			//Inform Windows to update the font
			/*if (!SendMessage(this->GetHandle(), WM_SETFONT, (WPARAM)this->m_pFont->GetHandle(), (LPARAM)TRUE)) {
				delete this->m_pFont;
				return false;
			}*/
			SendMessage(this->GetHandle(), WM_SETFONT, (WPARAM)this->m_pFont->GetHandle(), (LPARAM)TRUE);

			return true;
		}
		virtual bool SetFont(const CFont& rFont)
		{
			//Update component font

			//Release previous font if exists
			if (this->m_pFont)
				delete this->m_pFont;

			//Copy font object
			this->m_pFont = new CFont(rFont);
			if (!this->m_pFont)
				return false;

			//Inform Windows to update the font
			/*if (!SendMessage(this->GetHandle(), WM_SETFONT, (WPARAM)this->m_pFont->GetHandle(), (LPARAM)TRUE)) {
				delete this->m_pFont;
				return false;
			}*/
			SendMessage(this->GetHandle(), WM_SETFONT, (WPARAM)this->m_pFont->GetHandle(), (LPARAM)TRUE);

			return true;
		}
		virtual bool SetText(const std::wstring& wszText)
		{
			//Update label text

			if (SendMessage(this->GetHandle(), WM_SETTEXT, 0, (LPARAM)wszText.c_str())) {
				this->m_wszText = wszText;
				return true;
			}

			return false;
		}

		//Getters
		virtual bool IsValid(void) const { return IsWindow(this->m_hWindow) == TRUE; }
		virtual HWND GetHandle(void) const { return this->m_hWindow; }
		virtual HWND GetParent(void) const { return this->m_hParentWnd; }
		virtual const SCOMPONENTSTRING& GetType(void) const { return this->m_wszTypeName; }
		virtual const SCOMPONENTSTRING& GetName(void) const { return this->m_wszName; }
		virtual const WFDimension& GetPosition(void) const { return this->m_dimPosition; }
		virtual const WFDimension& GetResolution(void) const { return this->m_dimResolution; }
		virtual const std::wstring& GetText(void) const { return this->m_wszText; }
		virtual const CFont* GetFont(void) const { return this->m_pFont; }
	};

	typedef size_t HCOMPONENT;
	#define INVALID_COMPONENT_HANDLE std::wstring::npos
	class CForm {
	private:
		std::wstring m_wszClassName;
		ATOM m_hClass;
		HWND m_hWindow;
		WFDimension m_sPosition;
		WFDimension m_sResolution;
		std::vector<IBaseComponent*> m_vAttachedComponents;
		bool m_bSignalRelease;
	public:
		CForm() : m_hWindow(0), m_bSignalRelease(false) {}
		CForm(const std::wstring& wszName, const std::wstring& wszWindowText, int x, int y, int iWidth, int iHeight, DWORD dwStyle) : m_bSignalRelease(false) { this->Instantiate(wszName, wszWindowText, x, y, iWidth, iHeight, dwStyle); }
		~CForm() { this->Free(); }

		bool Instantiate(const std::wstring& wszName, const std::wstring& wszWindowText, int x, int y, int iWidth, int iHeight, DWORD dwStyle)
		{
			//Instantiate the form

			this->m_wszClassName = wszName;

			//Setup window class data
			WNDCLASSEX sWndClass = { 0 };
			sWndClass.cbSize = sizeof(WNDCLASSEXW);
			sWndClass.style = CS_VREDRAW | CS_HREDRAW;
			sWndClass.hInstance = (HINSTANCE)GetCurrentProcess();
			sWndClass.lpszClassName = this->m_wszClassName.c_str();
			sWndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			sWndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			sWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			sWndClass.lpfnWndProc = &CommonWindowProc;

			//Register window class
			this->m_hClass = RegisterClassEx(&sWndClass);
			if (!this->m_hClass)
				return false;

			//Register window
			this->m_hWindow = CreateWindowEx(0, this->m_wszClassName.c_str(), wszWindowText.c_str(), WS_OVERLAPPED | WS_SYSMENU  | WS_VISIBLE | dwStyle, x, y, iWidth, iHeight, 0, 0, (HINSTANCE)GetCurrentProcess(), NULL);
			if (!this->m_hWindow) {
				UnregisterClass(this->m_wszClassName.c_str(), (HINSTANCE)GetCurrentProcess());
				return false;
			}

			//Update the window
			if (!UpdateWindow(this->m_hWindow)) {
				DestroyWindow(this->m_hWindow);
				UnregisterClass(this->m_wszClassName.c_str(), (HINSTANCE)GetCurrentProcess());
				return false;
			}

			//Update member values
			this->m_sPosition[0] = x;
			this->m_sPosition[1] = y;
			this->m_sResolution[0] = iWidth;
			this->m_sResolution[1] = iHeight;

			return true;
		}

		//Setters
		bool SetPosition(int x, int y)
		{
			//Update window position

			//Attempt to update the position
			if (MoveWindow(this->m_hWindow, x, y, this->m_sResolution[0], this->m_sResolution[1], TRUE)) {
				//Update new position data
				this->m_sPosition[0] = x;
				this->m_sPosition[1] = y;

				return true;
			}

			return false;
		}
		bool SetResolution(int w, int h)
		{
			//Update window resolution

			//Attempt to update the resolution
			if (MoveWindow(this->m_hWindow, this->m_sPosition[0], this->m_sPosition[1], w, h, TRUE)) {
				//Update new position data
				this->m_sResolution[0] = w;
				this->m_sResolution[1] = h;

				return true;
			}

			return false;
		}
		void SignalRelease(void) { this->m_bSignalRelease = true; }

		//Getters
		bool IsReady(void) { return (this->m_hClass) && (this->m_hWindow); }
		HWND GetHandle(void) { return this->m_hWindow; }
		const std::wstring& GetName(void) const { return this->m_wszClassName; }
		const WFDimension& GetPosition(void) const { return this->m_sPosition; }
		const WFDimension& GetResolution(void) const { return this->m_sResolution; }
		inline bool ShallRelease(void) { return this->m_bSignalRelease; }

		HCOMPONENT AttachComponent(IBaseComponent* pComponent)
		{
			//Attach component to window
			
			if ((!pComponent) || (!pComponent->IsValid()))
				return INVALID_COMPONENT_HANDLE;
			
			this->m_vAttachedComponents.push_back(pComponent);

			return this->m_vAttachedComponents.size() - 1;
		}

		IBaseComponent* FindComponent(const SCOMPONENTSTRING& wszCompName)
		{
			//Find component by name

			for (size_t i = 0; i < this->m_vAttachedComponents.size(); i++) {
				if (this->m_vAttachedComponents[i]->GetName() == wszCompName)
					return this->m_vAttachedComponents[i];
			}

			return nullptr;
		}

		IBaseComponent* FindComponent(const SCOMPONENTSTRING& wszCompType, const SCOMPONENTSTRING& wszCompName)
		{
			//Find component by type and name

			for (size_t i = 0; i < this->m_vAttachedComponents.size(); i++) {
				if ((this->m_vAttachedComponents[i]->GetType() == wszCompType) && (this->m_vAttachedComponents[i]->GetName() == wszCompName))
					return this->m_vAttachedComponents[i];
			}

			return nullptr;
		}

		bool ComponentWithNameOfTypeExists(const SCOMPONENTSTRING& wszType, const SCOMPONENTSTRING& wszName)
		{
			//Check if a component with the given name of the given type does exist

			for (size_t i = 0; i < this->m_vAttachedComponents.size(); i++) {
				if ((this->m_vAttachedComponents[i]->GetType() == wszType) && (this->m_vAttachedComponents[i]->GetName() == wszName))
					return true;
			}

			return false;
		}

		bool FreeComponent(IBaseComponent* pComponent)
		{
			//Free component

			for (size_t i = 0; i < this->m_vAttachedComponents.size(); i++) { //Loop through component list
				if (this->m_vAttachedComponents[i] == pComponent) { //Check if component exists inside list by comparing the instance memory addresses
					this->m_vAttachedComponents[i]->Free(); //Let component free its resources

					delete this->m_vAttachedComponents[i]; //Free component memory

					this->m_vAttachedComponents.erase(this->m_vAttachedComponents.begin() + i); //Erase from list

					return true;
				}
			}

			return false;
		}

		bool FreeComponent(HCOMPONENT hComponent)
		{
			//Free component

			//Validate argument
			if ((hComponent == INVALID_COMPONENT_HANDLE) || (hComponent >= this->m_vAttachedComponents.size()))
				return false;

			this->m_vAttachedComponents[hComponent]->Free(); //Let component free its resources

			delete this->m_vAttachedComponents[hComponent]; //Free component memory
			this->m_vAttachedComponents.erase(this->m_vAttachedComponents.begin() + hComponent); //Erase from list

			return true;
		}

		bool Draw(void)
		{
			//Perform drawing

			//Let components draw themselves
			for (size_t i = 0; i < this->m_vAttachedComponents.size(); i++) {
				if (!this->m_vAttachedComponents[i]->Draw())
					return false;
			}

			return true;
		}

		bool Process(void)
		{
			//Process form and all attached components

			MSG sMsg = { 0 };

			//Attempt to peek message from queue
			if (PeekMessage(&sMsg, NULL, 0, 0, PM_REMOVE)) {
				//Translate message data
				TranslateMessage(&sMsg);
				//Dispatch message to window procedure
				DispatchMessage(&sMsg);
			}

			//Update Form window
			if (!UpdateWindow(this->m_hWindow))
				return false;
			
			//Let components process and draw their stuff
			for (size_t i = 0; i < this->m_vAttachedComponents.size(); i++) {
				if (!this->m_vAttachedComponents[i]->Process())
					return false;
				
				if (!UpdateWindow(this->m_vAttachedComponents[i]->GetHandle()))
					return false;
			}

			return true;
		}

		bool InformCommandMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			//Inform components of WM_COMMAND message

			for (size_t i = 0; i < this->m_vAttachedComponents.size(); i++) {
				if (this->m_vAttachedComponents[i]->HandleCommandMessage(hWnd, uMsg, wParam, lParam))
					return true;
			}

			return false;
		}

		LRESULT InformNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			//Inform components of WM_COMMAND message

			for (size_t i = 0; i < this->m_vAttachedComponents.size(); i++) {
				LRESULT lResult = this->m_vAttachedComponents[i]->HandleNotifyMessage(hWnd, uMsg, wParam, lParam);
				if (lResult != -1L)
					return lResult;
			}

			return 0;
		}

		LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			//Window procedure of form
			
			switch (uMsg) {
			case WM_COMMAND: //Window command message fired
				if (this->InformCommandMessage(hWnd, uMsg, wParam, lParam)) return 0;
				break;
			case WM_NOTIFY: //Window notify message fired
				return this->InformNotifyMessage(hWnd, uMsg, wParam, lParam);
				break;
			case WM_PAINT: //Window shall paint
				this->Draw();
				return 0;
				break;
			case WM_QUIT: //Window shall close
				this->SignalRelease(); //Signal host to release this Form
				return 0;
				break;
			case WM_DESTROY: { //Window destroying requested
				PostQuitMessage(EXIT_SUCCESS); //Post quit message into queue
				this->SignalRelease(); //Signal host to release this Form
				return 0;
			}
			default:
				break;
			}

			return DefWindowProc(hWnd, uMsg, wParam, lParam); //Pass message for defaulted handling to subsystem
		}

		void Free(void)
		{
			//Free resources
			
			//Release all attached components
			for (size_t i = 0; i < this->m_vAttachedComponents.size(); i++) {
				//Let component free its resources
				this->m_vAttachedComponents[i]->Free();

				//Release memory
				delete this->m_vAttachedComponents[i];
			}
			
			//Clear list
			this->m_vAttachedComponents.clear();
			
			//Destroy the window
			if (this->m_hWindow) {
				DestroyWindow(this->m_hWindow);
				this->m_hWindow = 0;
			}
			
			//Unregister window class
			if (this->m_hClass) {
				UnregisterClass(this->m_wszClassName.c_str(), (HINSTANCE)GetCurrentProcess());
				this->m_hClass = 0;
			}
			
			//Clear name
			this->m_wszClassName.clear();
		}
	};

	class CLabel : public IBaseComponent {
	public:
		CLabel() {}
		CLabel(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution) { this->Instantiate(hParentWnd, wszName, dimPosition, dimResolution); }
		~CLabel() {}

		virtual bool Instantiate(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution)
		{
			//Instantiate label component

			//Set type name
			this->SetTypeName(L"CLabel");
			
			//Instantiate base component
			if (!IBaseComponent::Instantiate(hParentWnd, wszName, dimPosition, dimResolution))
				return false;

			//Attach label as static control to parent window
			this->m_hWindow = CreateWindowEx(0, L"static", wszName.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, dimPosition[0], dimPosition[1], dimResolution[0], dimResolution[1], hParentWnd, 0, (HINSTANCE)GetCurrentProcess(), NULL);

			return this->m_hWindow != NULL;
		}

		virtual bool Process(void) { return true; }
		virtual bool Draw(void) { return true; }
		virtual bool HandleCommandMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
		virtual LRESULT HandleNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return -1; }
	};

	class CButton : public IBaseComponent {
	private:
		static USHORT ButtonClickCounter;
		USHORT m_usButtonClickId;
	public:
		CButton() {}
		CButton(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution) { this->Instantiate(hParentWnd, wszName, dimPosition, dimResolution); }
		~CButton() {}

		virtual bool Instantiate(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution)
		{
			//Instantiate button component

			//Set type name
			this->SetTypeName(L"CButton");

			//Instantiate base component
			if (!IBaseComponent::Instantiate(hParentWnd, wszName, dimPosition, dimResolution))
				return false;
			
			//Store and increment button click ID counter
			this->m_usButtonClickId = CButton::ButtonClickCounter++;

			//Attach button to parent window
			this->m_hWindow = CreateWindowEx(0, L"Button", NULL, WS_CHILD | WS_VISIBLE, dimPosition[0], dimPosition[1], dimResolution[0], dimResolution[1], hParentWnd, (HMENU)this->m_usButtonClickId, (HINSTANCE)GetCurrentProcess(), NULL);

			return this->m_hWindow != NULL;
		}

		virtual bool Process(void) { return true; }
		virtual bool Draw(void) { return true; }
		virtual bool HandleCommandMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			//Handle WM_COMMAND message

			//Check if button has been clicked
			if (HIWORD(wParam) == BN_CLICKED) {
				if (LOWORD(wParam) == this->m_usButtonClickId) {
					this->OnClick();
					return true;
				}
			}

			return false;
		}
		virtual LRESULT HandleNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return -1; }

		//Events
		bool AddOnClickEventHandler(PEVENTMETHOD pfnMethod)
		{
			//Add OnClick event handler

			return this->AddEventHandler(L"OnClick", pfnMethod);
		}
		void OnClick(void)
		{
			//Call event handler

			this->CallEventHandler(L"OnClick", nullptr);
		}

		//Getters
		static USHORT GetCount(void) { return CButton::ButtonClickCounter; }
		USHORT GetCommandId(void) { return this->m_usButtonClickId; }
	};
	USHORT CButton::ButtonClickCounter = 1;

	class CTextbox : public IBaseComponent {
	public:
		CTextbox() {}
		CTextbox(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution) { this->Instantiate(hParentWnd, wszName, dimPosition, dimResolution); }
		~CTextbox() {}

		virtual bool Instantiate(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution)
		{
			//Instantiate textbox component

			//Set type name
			this->SetTypeName(L"CTextbox");

			//Perform base instantiation
			if (!IBaseComponent::Instantiate(hParentWnd, wszName, dimPosition, dimResolution))
				return false;

			//Create textbox control
			this->m_hWindow = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE, dimPosition[0], dimPosition[1], dimResolution[0], dimResolution[1], this->GetParent(), 0, (HINSTANCE)GetCurrentProcess(), NULL);
			
			return this->m_hWindow != NULL;
		}

		virtual bool SetText(const std::wstring& wszText)
		{
			//Set control text

			if (!SetWindowText(this->m_hWindow, wszText.c_str()))
				return false;

			return IBaseComponent::SetText(wszText);
		}

		virtual std::wstring GetText(void)
		{
			//Get control text

			//Get text length
			int iCharCount = GetWindowTextLength(this->m_hWindow);
			if (!iCharCount)
				return L"";

			//Allocate buffer
			wchar_t* pWndText = new wchar_t[iCharCount + 2];
			if (!pWndText)
				return L"";

			std::wstring wszResult(L"");

			//Get window text
			if (GetWindowText(this->m_hWindow, pWndText, iCharCount + 1))
				wszResult = pWndText;

			//Free memory
			delete[] pWndText;

			return wszResult;
		}

		//Handlers
		virtual bool Process(void) { return true; }
		virtual bool Draw(void) { return true; }
		virtual bool HandleCommandMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{ 
			//Handle command message

			if ((HWND)lParam == this->GetHandle()) { //If this textbox control is affected
				if (HIWORD(wParam) == EN_CHANGE) { //If change event occured
					//Raise event
					this->CallEventHandler(L"OnChange", nullptr);
					return true;
				}
			}

			return false; 
		}
		virtual LRESULT HandleNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return -1; }
		bool AddOnChangeEventHandler(PEVENTMETHOD pfnMethod)
		{
			//Add OnClick event handler

			return this->AddEventHandler(L"OnChange", pfnMethod);
		}
	};

	class CCheckbox : public IBaseComponent {
	private:
		static USHORT CheckboxClickCounter;
		USHORT m_usCheckboxClickId;
	public:
		CCheckbox() {}
		CCheckbox(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution) { this->Instantiate(hParentWnd, wszName, dimPosition, dimResolution); }
		~CCheckbox() {}

		virtual bool Instantiate(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution)
		{
			//Instantiate checkbox component

			//Set type name
			this->SetTypeName(L"CCheckbox");

			//Perform base instantiation
			if (!IBaseComponent::Instantiate(hParentWnd, wszName, dimPosition, dimResolution))
				return false;

			//Update ID values
			this->m_usCheckboxClickId = CCheckbox::CheckboxClickCounter++;

			//Create checkbox control
			this->m_hWindow = CreateWindowEx(0, L"BUTTON", wszName.c_str(), WS_CHILD | BS_CHECKBOX | WS_VISIBLE, dimPosition[0], dimPosition[1], dimResolution[0], dimResolution[1], this->GetParent(), (HMENU)this->m_usCheckboxClickId, (HINSTANCE)GetCurrentProcess(), NULL);

			return this->m_hWindow != NULL;
		}

		//Handlers
		virtual bool Process(void) { return true; }
		virtual bool Draw(void) { return true; }
		virtual bool HandleCommandMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
		{ 
			//Handle command message

			//Check if this checkbox control is affected
			if(LOWORD(wParam) == this->m_usCheckboxClickId) {
				//Toggle button value

				//Get checked-status value
				UINT uiStatus = IsDlgButtonChecked(hWnd, this->m_usCheckboxClickId);
				if (uiStatus == BST_CHECKED) { //If control is checked
					//Uncheck control
					CheckDlgButton(hWnd, this->m_usCheckboxClickId, BST_UNCHECKED);
				} else if (uiStatus == BST_UNCHECKED) { //If control is unchecked
					//Check control
					CheckDlgButton(hWnd, this->m_usCheckboxClickId, BST_CHECKED);
				}

				return true;
			}

			return false; 
		}
		virtual LRESULT HandleNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return -1; }

		//Setters
		virtual bool SetCheckValue(bool bChecked)
		{
			//Set check value

			switch (bChecked) {
			case false:
				return CheckDlgButton(this->GetParent(), this->m_usCheckboxClickId, BST_UNCHECKED) == TRUE;
				break;
			case true:
				return CheckDlgButton(this->GetParent(), this->m_usCheckboxClickId, BST_CHECKED) == TRUE;
				break;
			}

			return false;
		}

		//Getters
		virtual bool IsChecked(void) const { return IsDlgButtonChecked(this->GetParent(), this->m_usCheckboxClickId) == BST_CHECKED; }
	};
	USHORT CCheckbox::CheckboxClickCounter = 201;

	typedef LRESULT COMBOBOXITEM;
	class CCombobox : public IBaseComponent {
	public:
		CCombobox() {}
		CCombobox(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution) { this->Instantiate(hParentWnd, wszName, dimPosition, dimResolution); }
		~CCombobox() {}

		virtual bool Instantiate(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution)
		{
			//Instantiate combobox component

			//Set type name
			this->SetTypeName(L"CCombobox");

			//Perform base instantiation
			if (!IBaseComponent::Instantiate(hParentWnd, wszName, dimPosition, dimResolution))
				return false;

			//Create listbox control
			this->m_hWindow = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, wszName.c_str(), WS_CHILD | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWN, dimPosition[0], dimPosition[1], dimResolution[0], dimResolution[1], this->GetParent(), 0, (HINSTANCE)GetCurrentProcess(), NULL);

			return this->m_hWindow != NULL;
		}

		virtual bool AddOnChangeEventHandler(const PEVENTMETHOD pEventMethod)
		{
			//Add OnSelect event handler

			return this->AddEventHandler(L"OnSelect", pEventMethod);
		}

		virtual bool Add(const std::wstring& wszItem)
		{
			//Add item to combobox

			LRESULT lResult = SendMessage(this->GetHandle(), CB_ADDSTRING, 0,(LPARAM)wszItem.c_str());

			return (lResult != CB_ERR) && (lResult != CB_ERRSPACE);
		}

		virtual bool Remove(const COMBOBOXITEM iItem)
		{
			//Remove item

			return SendMessage(this->GetHandle(), CB_DELETESTRING, iItem, 0) != CB_ERR;
		}

		virtual COMBOBOXITEM Count(void)
		{
			//Get count

			return SendMessage(this->GetHandle(), CB_GETCOUNT, 0, 0);
		}

		virtual bool SelectItem(const COMBOBOXITEM iItem)
		{
			//Select item

			return SendMessage(this->GetHandle(), CB_SETCURSEL, iItem, 0) != CB_ERR;
		}

		virtual std::wstring Selection(void)
		{
			//Get string of current selection

			//Get ID of selected item
			LRESULT lCurSelection = SendMessage(this->GetHandle(), CB_GETCURSEL, 0, 0);
			if (lCurSelection == CB_ERR)
				return L"";

			//Get text
			return this->GetText(lCurSelection);
		}

		virtual COMBOBOXITEM SelectionId(void)
		{
			//Get selection ID

			return SendMessage(this->GetHandle(), CB_GETCURSEL, 0, 0);
		}

		virtual std::wstring GetText(const COMBOBOXITEM iItem)
		{
			//Get string of item

			//Get string length first
			LRESULT lStrLen = SendMessage(this->GetHandle(), CB_GETLBTEXTLEN, iItem, 0);
			if (lStrLen == CB_ERR)
				return L"";
			
			//Allocate memory for string buffer
			wchar_t* pwString = new wchar_t[lStrLen + 2];
			if (!pwString)
				return L"";

			//Clear buffer
			memset(pwString, 0x00, lStrLen * 2 + 2);
			
			//Obtain item string
			SendMessage(this->GetHandle(), CB_GETLBTEXT, iItem, (LPARAM)pwString);
			
			//Setup result string
			std::wstring wszResult(pwString);

			//Free memory
			delete[] pwString;

			return wszResult;
		}

		virtual bool HandleCommandMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			//Handle command message
			
			if (HIWORD(wParam) == CBN_SELCHANGE) { //Check if a selection change has occured
				if ((HWND)lParam == this->GetHandle()) { //Check if this combobox is refered
					//Raise event
					this->CallEventHandler(L"OnSelect", nullptr);

					return true;
				}
			}

			return false;
		}

		virtual LRESULT HandleNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return -1; }

		virtual bool Process(void) { return true; }
		virtual bool Draw(void) { return true; }
	};

	typedef LRESULT LLISTBOXITEM;
	#define LB_ERRORINDEX LB_ERR
	class CListbox : public IBaseComponent {
	public:
		CListbox() {}
		CListbox(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution) { this->Instantiate(hParentWnd, wszName, dimPosition, dimResolution); }
		~CListbox() {}

		virtual bool Instantiate(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution)
		{
			//Instantiate listbox component

			//Set type name
			this->SetTypeName(L"CListbox");

			//Perform base instantiation
			if (!IBaseComponent::Instantiate(hParentWnd, wszName, dimPosition, dimResolution))
				return false;

			//Create listbox control
			this->m_hWindow = CreateWindowEx(WS_EX_CLIENTEDGE, L"listbox", wszName.c_str(), WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY, dimPosition[0], dimPosition[1], dimResolution[0], dimResolution[1], this->GetParent(), 0, (HINSTANCE)GetCurrentProcess(), NULL);

			return this->m_hWindow != NULL;
		}

		//Setters
		virtual bool Add(const std::wstring& wszText)
		{
			//Add string to listbox

			LRESULT lResult = SendMessage(this->m_hWindow, LB_ADDSTRING, 0, (LPARAM)wszText.c_str());

			return (lResult != LB_ERR) && (lResult != LB_ERRSPACE);
		}
		virtual LLISTBOXITEM Insert(const LLISTBOXITEM lbiAfter, const std::wstring& wszText)
		{
			//Insert item after given item

			if (lbiAfter >= this->Count())
				return LB_ERRORINDEX;

			return SendMessage(this->m_hWindow, LB_INSERTSTRING, lbiAfter + 1, (LPARAM)wszText.c_str());
		}
		virtual bool Update(const LLISTBOXITEM lbiItem, const std::wstring& wszNewText)
		{
			//Update item text

			if ((lbiItem == LB_ERRORINDEX) || (lbiItem >= this->Count()))
				return false;

			//Get current selected item
			LLISTBOXITEM lbiSelItem = this->Selection();

			bool bResult = false;

			//Insert new string after item and then remove target item in order to apply a text replacement
			LLISTBOXITEM lbiInsertedItem = this->Insert(lbiItem, wszNewText);
			if (lbiInsertedItem != LB_ERRORINDEX) {
				bResult = this->Remove(lbiInsertedItem-1);
			}
			
			//Select item again
			bResult = (bResult) && (this->Select(lbiSelItem));

			return bResult;
		}
		virtual bool Remove(const LLISTBOXITEM lbiItem)
		{
			//Remove item

			return SendMessage(this->m_hWindow, LB_DELETESTRING, lbiItem, 0) != LB_ERR;
		}
		virtual bool Select(const LLISTBOXITEM lbiItem)
		{
			//Select item
			
			return SendMessage(this->m_hWindow, LB_SETCURSEL, lbiItem, 0) != LB_ERR;
		}

		//Getters
		virtual LLISTBOXITEM Count(void)
		{
			//Get amount of items

			return SendMessage(this->m_hWindow, LB_GETCOUNT, 0, 0);
		}
		virtual LLISTBOXITEM Selection(void)
		{
			//Get selection index

			return SendMessage(this->m_hWindow, LB_GETCURSEL, 0, 0);
		}
		virtual std::wstring GetText(const LLISTBOXITEM lbiItem)
		{
			//Get text of listbox item

			if ((lbiItem == LB_ERRORINDEX) || (lbiItem >= this->Count()))
				return L"";

			//Get text length of item
			LRESULT lTextLen = SendMessage(this->m_hWindow, LB_GETTEXTLEN, lbiItem, 0);
			if (lTextLen == LB_ERR)
				return L"";

			std::wstring wszResult(L"");

			//Allocate buffer to text
			wchar_t* pItemText = new wchar_t[lTextLen + 2];
			if (!pItemText)
				return L"";

			//Get item text
			if (SendMessage(this->m_hWindow, LB_GETTEXT, lbiItem, (LPARAM)pItemText) == LB_ERR) {
				delete [] pItemText;
				return L"";
			}

			//Save to string object
			wszResult = pItemText;

			//Free memory
			delete [] pItemText;

			return wszResult;
		}

		//Handlers
		virtual bool Process(void) { return true; }
		virtual bool Draw(void) { return true; }
		virtual bool HandleCommandMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
		{ 
			//Handle command message

			if ((HWND)lParam == this->GetHandle()) { //Check if event affects this listbox
				//Extract notification code
				WORD wNotificationCode = HIWORD(wParam);
				switch (wNotificationCode) {
				case LBN_SELCHANGE: //Handle selection change
					//Raise event
					this->CallEventHandler(L"OnSelectionChange", nullptr);
					break;
				case LBN_DBLCLK: //Handle double click
					//Raise event
					this->CallEventHandler(L"OnDblClick", nullptr);
					break;
				default:
					break;
				}

				return true;
			}

			return false; 
		}
		virtual LRESULT HandleNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return -1; }
		bool AddOnSelectionChangeEventHandler(PEVENTMETHOD pfnMethod)
		{
			//Add OnClick event handler

			return this->AddEventHandler(L"OnSelectionChange", pfnMethod);
		}
		bool AddOnDblClickEventHandler(PEVENTMETHOD pfnMethod)
		{
			//Add OnClick event handler

			return this->AddEventHandler(L"OnDblClick", pfnMethod);
		}
	};

	typedef LRESULT LISTVIEWITEM;
	#define LV_MAXITEMBUFFERSIZE 2048
	class CListview : public IBaseComponent {
	private:
		LRESULT m_lColums;
	public:
		CListview() : m_lColums(0) {}
		CListview(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution) : m_lColums(0) { this->Instantiate(hParentWnd, wszName, dimPosition, dimResolution); }
		~CListview() {}

		virtual bool Instantiate(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution)
		{
			//Instantiate listbox component

			//Set type name
			this->SetTypeName(L"CListview");

			//Perform base instantiation
			if (!IBaseComponent::Instantiate(hParentWnd, wszName, dimPosition, dimResolution))
				return false;

			//Create listbox control
			this->m_hWindow = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, wszName.c_str(), WS_CHILD | WS_VISIBLE | LVS_REPORT, dimPosition[0], dimPosition[1], dimResolution[0], dimResolution[1], this->GetParent(), 0, (HINSTANCE)GetCurrentProcess(), NULL);

			return this->m_hWindow != NULL;
		}

		bool AddOnSelectEventHandler(const PEVENTMETHOD pfnEventHandler)
		{
			//Add OnSelect event handler

			return this->AddEventHandler(L"OnSelect", pfnEventHandler);
		}

		bool AddOnDblClickEventHandler(const PEVENTMETHOD pfnEventHandler)
		{
			//Add OnDblClick event handler

			return this->AddEventHandler(L"OnDblClick", pfnEventHandler);
		}

		bool AddCategory(const std::wstring& wszCategory, int iColumnWidth, bool bLeftAligned)
		{
			//Add category to listview

			if (!wszCategory.length())
				return false;

			//Setup data struct
			LVCOLUMN sColumnData;
			sColumnData.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			sColumnData.iSubItem = (int)this->m_lColums;
			sColumnData.cx = iColumnWidth;
			sColumnData.fmt = (bLeftAligned) ? LVCFMT_LEFT : LVCFMT_RIGHT;
			sColumnData.pszText = (LPWSTR)wszCategory.c_str();

			//Add column to listview
			if (SendMessage(this->GetHandle(), LVM_INSERTCOLUMN, this->m_lColums, (LPARAM)&sColumnData) != -1) {
				this->m_lColums++;
				return true;
			}

			return false;
		}

		int GetSubItemCount(void)
		{
			//Get sub item count
			
			return (int)this->m_lColums;
		}

		int GetItemCount(void)
		{
			//Get item count

			return (int)SendMessage(this->GetHandle(), LVM_GETITEMCOUNT, 0, 0);
		}

		int SetItemText(const std::wstring& wszText, int iItemNum)
		{
			//Set text of an item

			//Setup data struct
			LVITEM sItem;
			sItem.mask = LVIF_TEXT;
			sItem.iItem = iItemNum;
			sItem.iSubItem = 0;
			sItem.pszText = (LPWSTR)wszText.c_str();

			//Insert item
			LRESULT lItemNum = SendMessage(this->GetHandle(), LVM_INSERTITEM, 0, (LPARAM)&sItem);
			if (lItemNum != CB_ERR) {
				return (int)lItemNum + 1; //Return index of next item
			}

			return CB_ERR;
		}

		bool SetSubItemText(int iItemNum, const std::wstring& wszText, int iSubItemNum)
		{
			//Set sub item text of an item

			//Setup data struct
			LVITEM sItem;
			sItem.mask = LVIF_TEXT;
			sItem.iItem = iItemNum;
			sItem.iSubItem = iSubItemNum;
			sItem.pszText = (LPWSTR)wszText.c_str();

			//Set sub item
			return SendMessage(this->GetHandle(), LVM_SETITEM, 0, (LPARAM)&sItem) == TRUE;
		}

		std::wstring GetItemText(int iItemNum, int iSubItemNum, int iBufferCount = LV_MAXITEMBUFFERSIZE)
		{
			//Setup buffer
			wchar_t* pwcBuffer = new wchar_t[iBufferCount + 2];
			if (!pwcBuffer)
				return L"";

			//Setup data struct
			LVITEM sItem;
			sItem.mask = LVIF_TEXT;
			sItem.iItem = iItemNum;
			sItem.iSubItem = iSubItemNum;
			sItem.cchTextMax = iBufferCount;
			sItem.pszText = pwcBuffer;

			std::wstring wszResult(L"");

			//Attempt to obtain item text
			if (SendMessage(this->GetHandle(), LVM_GETITEM, 0, (LPARAM)&sItem) == TRUE) {
				wszResult = std::wstring(sItem.pszText);
			}

			//Free buffer memory
			delete[] pwcBuffer;

			return wszResult;
		}

		int Selection(void)
		{
			//Get selected item index

			return (int)SendMessage(this->GetHandle(), LVM_GETNEXTITEM, -1, LVNI_SELECTED);
		}

		bool Delete(int iItem)
		{
			//Delete item

			return SendMessage(this->GetHandle(), LVM_DELETEITEM, iItem, 0) == TRUE;
		}

		//Handlers
		virtual bool Process(void) { return true; }
		virtual bool Draw(void) { return true; }
		virtual bool HandleCommandMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
		virtual LRESULT HandleNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
		{ 
			//Handle WM_NOTIFY message

			NMHDR* pNmhdr = (NMHDR*)lParam;
			if (pNmhdr->hwndFrom == this->GetHandle()) { //Check if this listview is meant
				if (pNmhdr->code == NM_CLICK) { //Check if item has been clicked
					this->CallEventHandler(L"OnSelect", nullptr); //Raise event
					return 0;
				} else if (pNmhdr->code == NM_DBLCLK) { //Check if item has been double-clicked
					this->CallEventHandler(L"OnDblClick", nullptr); //Raise event
					return 0;
				}
			}

			return -1; 
		}
	};

	class CProgressbar : public IBaseComponent {
	private:
		int m_iMaxValue;
		int m_iCurPos;
	public:
		CProgressbar() {}
		CProgressbar(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution) { this->Instantiate(hParentWnd, wszName, dimPosition, dimResolution); }
		~CProgressbar() {}

		virtual bool Instantiate(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution)
		{
			//Instantiate progressbar component

			//Set type name
			this->SetTypeName(L"CProgressbar");

			//Perform base instantiation
			if (!IBaseComponent::Instantiate(hParentWnd, wszName, dimPosition, dimResolution))
				return false;

			//Create progressbar control
			this->m_hWindow = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE, dimPosition[0], dimPosition[1], dimResolution[0], dimResolution[1], this->GetParent(), 0, (HINSTANCE)GetCurrentProcess(), NULL);
			
			return this->m_hWindow != NULL;
		}

		virtual bool SetRange(int iMaxValue)
		{
			//Set maximum range value

			if (!SendMessage(this->m_hWindow, PBM_SETRANGE, 0, MAKELPARAM(0, iMaxValue)))
				return false;

			this->m_iMaxValue = iMaxValue;

			return true;
		}

		virtual bool SetPos(int iPos)
		{
			//Set position value

			if (!SendMessage(this->m_hWindow, PBM_SETPOS, iPos, 0))
				return false;

			this->m_iCurPos = iPos;

			return true;
		}

		//Getters
		inline const int GetRange(void) const { return this->m_iMaxValue; }
		inline const int GetPos(void) const { return this->m_iCurPos; }

		//Handlers
		virtual bool Process(void) { return true; }
		virtual bool Draw(void) { return true; }
		virtual bool HandleCommandMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
		virtual LRESULT HandleNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return -1; }
	};

	class CImagebox : public IBaseComponent {
	private:
		HBITMAP m_hBitmap;
		std::wstring m_wszImageFile;
	public:
		CImagebox() {}
		CImagebox(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution) { this->Instantiate(hParentWnd, wszName, dimPosition, dimResolution); }
		~CImagebox() { this->Free(); }

		virtual bool Instantiate(HWND hParentWnd, const std::wstring& wszName, const WFDimension& dimPosition, const WFDimension& dimResolution)
		{
			//Instantiate imagebox component

			//Set type name
			this->SetTypeName(L"CImagebox");

			//Perform base instantiation
			if (!IBaseComponent::Instantiate(hParentWnd, wszName, dimPosition, dimResolution))
				return false;

			//Create imagebox control
			this->m_hWindow = CreateWindowEx(WS_EX_CLIENTEDGE, L"static", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, dimPosition[0], dimPosition[1], dimResolution[0], dimResolution[1], this->GetParent(), 0, (HINSTANCE)GetCurrentProcess(), NULL);

			return this->m_hWindow != NULL;
		}

		virtual bool Free(void)
		{
			//Free resources

			//Free bitmap
			if (this->m_hBitmap) {
				DeleteObject(this->m_hBitmap);
				this->m_hBitmap = 0;
			}

			//Perform base cleanup
			return IBaseComponent::Free();
		}

		//Setters
		virtual bool SetImage(const std::wstring& wszImageFile)
		{
			//Set imagage

			//Free old image if set first
			if (this->m_hBitmap) {
				DeleteObject(this->m_hBitmap);
			}

			//Attempt to load image
			this->m_hBitmap = (HBITMAP)LoadImage(NULL, wszImageFile.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (this->m_hBitmap) {
				//Update image to box
				SendMessage(this->m_hWindow, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)this->m_hBitmap);

				//Update string
				this->m_wszImageFile = wszImageFile;

				return true;
			}

			return false;
		}

		//Getters
		virtual std::wstring GetImage(void) { return this->m_wszImageFile; }

		//Handlers
		virtual bool Process(void) { return true; }
		virtual bool Draw(void) { return true; }
		virtual bool HandleCommandMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
		virtual LRESULT HandleNotifyMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return -1; }
	};

	typedef size_t HFORM;
	typedef std::wstring SFORMNAME;
	#define INVALID_FORM_HANDLE std::wstring::npos
	class CFormMgr* pFormMgrInstance = nullptr;
	class CFormMgr { //Forms manager component
	private:
		std::vector<CForm*> m_vForms;
	public:
		CFormMgr() { pFormMgrInstance = this; }
		~CFormMgr() {}

		HFORM CreateForm(const SFORMNAME& wszName, const std::wstring& wszWindowText, int x, int y, int iWidth, int iHeight, DWORD dwStyle)
		{
			//Create new form

			//Check if name is already in use
			if (this->FindForm(wszName))
				return INVALID_FORM_HANDLE;

			//Instantiate form
			CForm* pForm = new CForm(wszName, wszWindowText, x, y, iWidth, iHeight, dwStyle);
			if ((!pForm) || (!pForm->IsReady()))
				return INVALID_FORM_HANDLE;

			//Add to list
			this->m_vForms.push_back(pForm);

			//Return ID
			return this->m_vForms.size() - 1;
		}

		bool FindForm(const SFORMNAME& wszFormName, HFORM* phForm)
		{
			//Check whether a Form with given name does already exists. Also return ID if requested

			for (size_t i = 0; i < this->m_vForms.size(); i++) {
				if (this->m_vForms[i]->GetName() == wszFormName) {
					if (phForm)
						*phForm = i;

					return true;
				}
			}

			return false;
		}

		bool IsFormValid(const SFORMNAME& wszName)
		{
			//Check if form does still exist

			return this->FindForm(wszName, nullptr);
		}

		CForm* FindForm(const SFORMNAME& wszFormName)
		{
			//Attempt to find form and return its class instance pointer

			for (size_t i = 0; i < this->m_vForms.size(); i++) {
				if (this->m_vForms[i]->GetName() == wszFormName) {
					return this->m_vForms[i];
				}
			}

			return nullptr;
		}

		HCOMPONENT AttachComponentToForm(HFORM hForm, IBaseComponent* pComponent)
		{
			//Attach component to form

			if (hForm == INVALID_FORM_HANDLE)
				return INVALID_COMPONENT_HANDLE;

			return this->m_vForms[hForm]->AttachComponent(pComponent);
		}

		HCOMPONENT AttachComponentToForm(const SFORMNAME& wszForm, IBaseComponent* pComponent)
		{
			//Attach component to form

			HFORM hForm = INVALID_FORM_HANDLE;

			if (!this->FindForm(wszForm, &hForm))
				return false;

			return this->m_vForms[hForm]->AttachComponent(pComponent);
		}

		bool NameOfComponentAlreadyInUse(const SFORMNAME& wszForm, const SCOMPONENTSTRING& wszComponentName, const SCOMPONENTSTRING& wszType)
		{
			//Check if the given name of a component of its type is already in use

			//Attempt to find form
			CForm* pForm = FindForm(wszForm);
			if (!pForm)
				return false;

			//Indicate whether a component with given name of given type does exist
			return pForm->ComponentWithNameOfTypeExists(wszType, wszComponentName);
		}

		bool ReleaseComponentFromForm(HFORM hForm, HCOMPONENT hComponent)
		{
			//Release component from form

			if (hForm == INVALID_FORM_HANDLE)
				return false;

			return this->m_vForms[hForm]->FreeComponent(hComponent);
		}

		bool ReleaseComponentFromForm(const SFORMNAME& wszForm, HCOMPONENT hComponent)
		{
			//Release component from form

			HFORM hForm;

			if (!this->FindForm(wszForm, &hForm))
				return false;

			return this->ReleaseComponentFromForm(hForm, hComponent);
		}

		void ProcessForms(void)
		{
			//Process all Forms
			
			for (size_t i = 0; i < this->m_vForms.size(); i++) { //Loop through all Forms
				this->m_vForms[i]->Process(); //Let the Form process its stuff

				//Handle whether Form request release
				if (this->m_vForms[i]->ShallRelease()) {
					this->FreeForm(i);
					continue;
				}
			}
		}

		LRESULT CallWindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			//Pass event to belonging form

			for (size_t i = 0; i < this->m_vForms.size(); i++) {
				if (this->m_vForms[i]->GetHandle() == hWnd) {
					return this->m_vForms[i]->WindowProc(hWnd, uMsg, wParam, lParam);
				}
			}

			//Let Windows system handle the message
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

		bool FreeForm(HFORM hForm)
		{
			//Free form

			if ((hForm == INVALID_FORM_HANDLE) || (hForm >= this->m_vForms.size()))
				return false;
			
			//Free form data
			this->m_vForms[hForm]->Free();
			
			//Free instance memory
			delete this->m_vForms[hForm];
			
			//Remove from list
			this->m_vForms.erase(this->m_vForms.begin() + hForm);
			
			return true;
		}

		bool FreeForm(const SFORMNAME& wszForm)
		{
			//Free form

			HFORM hForm;

			if (!this->FindForm(wszForm, &hForm))
				return false;

			return this->FreeForm(hForm);
		}
	} oFormMgr;

	/* Interface functions */

	HFORM SpawnForm(const SFORMNAME& wszName, const std::wstring& wszWindowText, int x, int y, int iWidth, int iHeight, DWORD dwStyle)
	{
		return oFormMgr.CreateForm(wszName, wszWindowText, x, y, iWidth, iHeight, dwStyle);
	}

	CLabel* SpawnLabel(const SFORMNAME& wszParentForm, const SCOMPONENTSTRING& wszLabelName, int x, int y, int w, int h, const std::wstring& wszText)
	{
		CForm* pForm = oFormMgr.FindForm(wszParentForm);
		if (!pForm)
			return nullptr;

		if (pForm->ComponentWithNameOfTypeExists(L"CLabel", wszLabelName))
			return nullptr;

		CLabel* pLabel = new CLabel(pForm->GetHandle(), wszLabelName, WFDimension(x, y), WFDimension(w, h));
		if (!pLabel)
			return nullptr;

		pLabel->PostInstantiate();
		pLabel->SetText(wszText);
		
		if (pForm->AttachComponent(pLabel) == INVALID_COMPONENT_HANDLE) {
			delete pLabel;
			return nullptr;
		}

		return pLabel;
	}

	CButton* SpawnButton(const SFORMNAME& wszParentForm, const SCOMPONENTSTRING& wszButtonName, int x, int y, int w, int h, const std::wstring& wszText, PEVENTMETHOD pfnOnClickEventHandler)
	{
		CForm* pForm = oFormMgr.FindForm(wszParentForm);
		if (!pForm)
			return nullptr;

		if (pForm->ComponentWithNameOfTypeExists(L"CButton", wszButtonName))
			return nullptr;

		CButton* pButton = new CButton(pForm->GetHandle(), wszButtonName, WFDimension(x, y), WFDimension(w, h));
		if (!pButton)
			return nullptr;

		pButton->PostInstantiate();
		pButton->AddOnClickEventHandler(pfnOnClickEventHandler);
		//pButton->SetFont(L"Verdana", 15, 15, false, false, false, false);
		pButton->SetText(wszText);

		if (pForm->AttachComponent(pButton) == INVALID_COMPONENT_HANDLE) {
			delete pButton;
			return nullptr;
		}

		return pButton;
	}

	CTextbox* SpawnTextbox(const SFORMNAME& wszParentForm, const SCOMPONENTSTRING& wszTextboxName, int x, int y, int w, int h, const std::wstring& wszInitialText)
	{
		CForm* pForm = oFormMgr.FindForm(wszParentForm);
		if (!pForm)
			return nullptr;

		if (pForm->ComponentWithNameOfTypeExists(L"CTextbox", wszTextboxName))
			return nullptr;

		CTextbox* pTextbox = new CTextbox(pForm->GetHandle(), wszTextboxName, WFDimension(x, y), WFDimension(w, h));
		if (!pTextbox)
			return nullptr;

		pTextbox->PostInstantiate();
		pTextbox->SetText(wszInitialText);

		if (pForm->AttachComponent(pTextbox) == INVALID_COMPONENT_HANDLE) {
			delete pTextbox;
			return nullptr;
		}

		return pTextbox;
	}

	CCheckbox* SpawnCheckbox(const SFORMNAME& wszParentForm, const SCOMPONENTSTRING& wszCheckboxName, int x, int y, int w, int h, const std::wstring& wszInitialText, bool bInitialValue)
	{
		CForm* pForm = oFormMgr.FindForm(wszParentForm);
		if (!pForm)
			return nullptr;

		if (pForm->ComponentWithNameOfTypeExists(L"CCheckbox", wszCheckboxName))
			return nullptr;

		CCheckbox* pCheckbox = new CCheckbox(pForm->GetHandle(), wszCheckboxName, WFDimension(x, y), WFDimension(w, h));
		if (!pCheckbox)
			return nullptr;

		pCheckbox->PostInstantiate();
		pCheckbox->SetText(wszInitialText);
		pCheckbox->SetCheckValue(bInitialValue);

		if (pForm->AttachComponent(pCheckbox) == INVALID_COMPONENT_HANDLE) {
			delete pCheckbox;
			return nullptr;
		}

		return pCheckbox;
	}

	CCombobox* SpawnCombobox(const SFORMNAME& wszParentForm, const SCOMPONENTSTRING& wszComboboxName, int x, int y, int w, int h, const PEVENTMETHOD pfnOnSelect)
	{
		CForm* pForm = oFormMgr.FindForm(wszParentForm);
		if (!pForm)
			return nullptr;

		if (pForm->ComponentWithNameOfTypeExists(L"CCombobox", wszComboboxName))
			return nullptr;

		CCombobox* pCombobox = new CCombobox(pForm->GetHandle(), wszComboboxName, WFDimension(x, y), WFDimension(w, h));
		if (!pCombobox)
			return nullptr;

		pCombobox->PostInstantiate();
		pCombobox->AddOnChangeEventHandler(pfnOnSelect);

		if (pForm->AttachComponent(pCombobox) == INVALID_COMPONENT_HANDLE) {
			delete pCombobox;
			return nullptr;
		}

		return pCombobox;
	}

	CListbox* SpawnListbox(const SFORMNAME& wszParentForm, const SCOMPONENTSTRING& wszListboxName, int x, int y, int w, int h)
	{
		CForm* pForm = oFormMgr.FindForm(wszParentForm);
		if (!pForm)
			return nullptr;

		if (pForm->ComponentWithNameOfTypeExists(L"CListbox", wszListboxName))
			return nullptr;

		CListbox* pListbox = new CListbox(pForm->GetHandle(), wszListboxName, WFDimension(x, y), WFDimension(w, h));
		if (!pListbox)
			return nullptr;

		pListbox->PostInstantiate();

		if (pForm->AttachComponent(pListbox) == INVALID_COMPONENT_HANDLE) {
			delete pListbox;
			return nullptr;
		}

		return pListbox;
	}

	CListview* SpawnListview(const SFORMNAME& wszParentForm, const SCOMPONENTSTRING& wszListviewName, int x, int y, int w, int h, const PEVENTMETHOD pfnOnSelect, const PEVENTMETHOD pfnOnDblCLick)
	{
		CForm* pForm = oFormMgr.FindForm(wszParentForm);
		if (!pForm)
			return nullptr;

		if (pForm->ComponentWithNameOfTypeExists(L"CListview", wszListviewName))
			return nullptr;

		CListview* pListview = new CListview(pForm->GetHandle(), wszListviewName, WFDimension(x, y), WFDimension(w, h));
		if (!pListview)
			return nullptr;

		pListview->PostInstantiate();
		pListview->AddOnSelectEventHandler(pfnOnSelect);
		pListview->AddOnDblClickEventHandler(pfnOnDblCLick);

		if (pForm->AttachComponent(pListview) == INVALID_COMPONENT_HANDLE) {
			delete pListview;
			return nullptr;
		}

		return pListview;
	}

	CProgressbar* SpawnProgressbar(const SFORMNAME& wszParentForm, const SCOMPONENTSTRING& wszProgressbarName, int x, int y, int w, int h, int iMaxValue, int iStartValue)
	{
		CForm* pForm = oFormMgr.FindForm(wszParentForm);
		if (!pForm)
			return nullptr;

		if (pForm->ComponentWithNameOfTypeExists(L"CProgressbar", wszProgressbarName))
			return nullptr;

		CProgressbar* pProgressbar = new CProgressbar(pForm->GetHandle(), wszProgressbarName, WFDimension(x, y), WFDimension(w, h));
		if (!pProgressbar)
			return nullptr;

		pProgressbar->PostInstantiate();
		pProgressbar->SetRange(iMaxValue);
		pProgressbar->SetPos(iStartValue);

		if (pForm->AttachComponent(pProgressbar) == INVALID_COMPONENT_HANDLE) {
			delete pProgressbar;
			return nullptr;
		}

		return pProgressbar;
	}

	CImagebox* SpawnImagebox(const SFORMNAME& wszParentForm, const SCOMPONENTSTRING& wszImageboxName, int x, int y, int w, int h, const std::wstring& wszImageFile)
	{
		CForm* pForm = oFormMgr.FindForm(wszParentForm);
		if (!pForm)
			return nullptr;

		if (pForm->ComponentWithNameOfTypeExists(L"CProgressbar", wszImageboxName))
			return nullptr;

		CImagebox* pImagebox = new CImagebox(pForm->GetHandle(), wszImageboxName, WFDimension(x, y), WFDimension(w, h));
		if (!pImagebox)
			return nullptr;

		pImagebox->PostInstantiate();
		
		if (!pImagebox->SetImage(wszImageFile)) {
			delete pImagebox;
			return nullptr;
		}

		if (pForm->AttachComponent(pImagebox) == INVALID_COMPONENT_HANDLE) {
			delete pImagebox;
			return nullptr;
		}

		return pImagebox;
	}

	bool IsFormValid(const SFORMNAME& wszName)
	{
		return oFormMgr.IsFormValid(wszName);
	}

	CForm* FindForm(const SFORMNAME& wszFormName)
	{
		return oFormMgr.FindForm(wszFormName);
	}

	HCOMPONENT AttachComponentToForm(HFORM hForm, IBaseComponent* pComponent)
	{
		return oFormMgr.AttachComponentToForm(hForm, pComponent);
	}

	HCOMPONENT AttachComponentToForm(const SFORMNAME& wszForm, IBaseComponent* pComponent)
	{
		return oFormMgr.AttachComponentToForm(wszForm, pComponent);
	}

	bool ReleaseComponentFromForm(HFORM hForm, HCOMPONENT hComponent)
	{
		return oFormMgr.ReleaseComponentFromForm(hForm, hComponent);
	}

	void Process(void)
	{
		oFormMgr.ProcessForms();
	}

	bool ReleaseForm(HFORM hForm)
	{
		return oFormMgr.FreeForm(hForm);
	}

	/*Window subsystem callback function */

	LRESULT CALLBACK CommonWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//Window procedure

		//Pass to forms manager
		return pFormMgrInstance->CallWindowProcedure(hWnd, uMsg, wParam, lParam);
	}
}