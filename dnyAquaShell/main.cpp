#include "console.h"
#include "dnyScriptInterpreter.h"
#include "plugins.h"
#include "resource.h"
#include <codecvt>

/*
	AquaShell (dnyAquaShell) developed by Daniel Brendel

	(C) 2017 - 2022 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace ShellInterface {
	class CShellInterface* __pShellInterface__;

	void SI_StandardOutput(const std::wstring& wszText)
	{
		std::wcout << wszText << std::endl;
	}

	bool SI_FileExists(const std::wstring& wszFile)
	{
		std::wifstream hFile;
		hFile.open(wszFile, std::wifstream::in);
		if (hFile.is_open()) {
			hFile.close();
			return true;
		}

		return false;
	}

	std::string SI_WStringToString(const std::wstring& wstr)
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}

	BOOL WINAPI SI_ConsoleCtrHandler(DWORD dwCtrlType);

	class CShellInterface { //Shell interface manager
	private:
		Console::CConInterface* m_pConsoleInt;
		dnyScriptInterpreter::CScriptingInterface* m_pScriptInt;
		Plugins::CPluginInt* m_pPluginInt;

		struct IShellPluginAPI { //Plugin API interface
			IShellPluginAPI() {}
			IShellPluginAPI(dnyScriptInterpreter::CScriptingInterface* pScriptInt, Console::CConInterface* pConInt) : m_pScriptInt(pScriptInt), m_pConInt(pConInt) {}
			~IShellPluginAPI() { this->m_pScriptInt = nullptr; this->m_pConInt = nullptr; }

			virtual bool Scr_ExecuteCode(const std::wstring& wszCode)
			{
				return this->m_pScriptInt->ExecuteCode(wszCode);
			}

			virtual bool Scr_ExecuteScript(const std::wstring& wszScriptFile)
			{
				return this->m_pScriptInt->ExecuteScript(wszScriptFile);
			}

			virtual bool Fnc_BeginFunctionCall(const std::wstring& wszFuncName, const dnyScriptInterpreter::CVarManager::cvartype_e eResultType)
			{
				return this->m_pScriptInt->BeginFunctionCall(wszFuncName, eResultType);
			}

			virtual bool Fnc_PushFunctionParam(dnyScriptInterpreter::dnyBoolean value)
			{
				return this->m_pScriptInt->PushFunctionParam(value);
			}

			virtual bool Fnc_PushFunctionParam(dnyScriptInterpreter::dnyInteger value)
			{
				return this->m_pScriptInt->PushFunctionParam(value);
			}

			virtual bool Fnc_PushFunctionParam(dnyScriptInterpreter::dnyFloat value)
			{
				return this->m_pScriptInt->PushFunctionParam(value);
			}

			virtual bool Fnc_PushFunctionParam(const dnyScriptInterpreter::dnyString& value)
			{
				return this->m_pScriptInt->PushFunctionParam(value);
			}

			virtual bool Fnc_ExecuteFunction(void)
			{
				return this->m_pScriptInt->ExecuteFunction();
			}

			virtual dnyScriptInterpreter::dnyBoolean Fnc_QueryFunctionResultAsBoolean(void)
			{
				return this->m_pScriptInt->QueryFunctionResultAsBoolean();
			}

			virtual dnyScriptInterpreter::dnyInteger Fnc_QueryFunctionResultAsInteger(void)
			{
				return this->m_pScriptInt->QueryFunctionResultAsInteger();
			}

			virtual dnyScriptInterpreter::dnyFloat Fnc_QueryFunctionResultAsFloat(void)
			{
				return this->m_pScriptInt->QueryFunctionResultAsFloat();
			}

			virtual dnyScriptInterpreter::dnyString Fnc_QueryFunctionResultAsString(void)
			{
				return this->m_pScriptInt->QueryFunctionResultAsString();
			}

			virtual bool Fnc_EndFunctionCall(void)
			{
				return this->m_pScriptInt->EndFunctionCall();
			}

			virtual bool Cv_RegisterCVarType(const std::wstring& wszTypeName, dnyScriptInterpreter::CVarManager::custom_cvar_type_s::cvar_type_event_table_s* pEventTable)
			{
				return this->m_pScriptInt->RegisterDataType(wszTypeName, pEventTable);
			}

			virtual dnyScriptInterpreter::CVarManager::cvarptr_t Cv_RegisterCVar(const std::wstring& wszName, const dnyScriptInterpreter::CVarManager::cvartype_e eType, bool bConst)
			{
				return this->m_pScriptInt->RegisterCVar(wszName, eType, bConst, false);
			}

			virtual dnyScriptInterpreter::CVarManager::cvarptr_t Cv_FindCVar(const std::wstring& wszName)
			{
				return this->m_pScriptInt->FindCVar(wszName);
			}

			virtual bool Cv_FreeCVar(const std::wstring& wszName)
			{
				return this->m_pScriptInt->FreeCVar(wszName);
			}

			virtual bool Cmd_RegisterCommand(const std::wstring& wszCmdName, dnyScriptInterpreter::CCommandManager::cmdinterface_t pCmdInterface, dnyScriptInterpreter::CVarManager::cvartype_e eType)
			{
				return this->m_pScriptInt->RegisterCommand(wszCmdName, pCmdInterface, eType);
			}

			bool Cmd_UnregisterCommand(const std::wstring& wszCmdName)
			{
				return this->m_pScriptInt->UnregisterCommand(wszCmdName);
			}

			virtual bool Con_WriteLine(const std::wstring& wszTextLine)
			{
				return this->m_pConInt->Int_WriteLine(wszTextLine);
			}

			virtual std::wstring Con_QueryInput(void)
			{
				return this->m_pConInt->Int_QueryInput();
			}
		private:
			dnyScriptInterpreter::CScriptingInterface* m_pScriptInt;
			Console::CConInterface* m_pConInt;
		} *m_pShellInt;

		bool m_bInteractiveMode;
		bool m_bShallRun;
		std::wstring m_wszBaseDir;

		friend void SI_StandardOutput(const std::wstring& wszText);
		friend BOOL WINAPI SI_ConsoleCtrHandler(DWORD dwCtrlType);

		class IBaseReadmeCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		#define TXT_DOCUMENTATION_TEMP_FILENAME L"documentation.txt"
		protected:
			virtual bool ShowResource(const WORD wResourceId)
			{
				//Show resource in editor

				if (!wResourceId)
					return false;
				
				//Find resource
				HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(wResourceId), MAKEINTRESOURCE(TEXTFILE));
				if (!hResource)
					return false;
				
				//Obtain size of resource
				DWORD dwResSize = SizeofResource(NULL, hResource);
				if (!dwResSize)
					return false;
				
				//Get handle to resource
				HGLOBAL hGlobal = LoadResource(NULL, hResource);
				if (!hGlobal)
					return false;
				
				//Obtain pointer to memory start address of resource
				LPVOID lpvMemoryBlock = LockResource(hGlobal);
				if (!lpvMemoryBlock)
					return false;
				
				//Delete old doc file if exists
				if (SI_FileExists(TXT_DOCUMENTATION_TEMP_FILENAME))
					DeleteFile(TXT_DOCUMENTATION_TEMP_FILENAME);
				
				//Save doc content to file

				HANDLE hFile = CreateFile(TXT_DOCUMENTATION_TEMP_FILENAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile == INVALID_HANDLE_VALUE)
					return false;
				
				DWORD dwBytesWritten;
				if (!WriteFile(hFile, lpvMemoryBlock, dwResSize, &dwBytesWritten, NULL)) {
					CloseHandle(hFile);
					DeleteFile(TXT_DOCUMENTATION_TEMP_FILENAME);
					return false;
				}

				CloseHandle(hFile);

				//Run with ShellExecute to view in default text viewer and wait until viewer is closed

				SHELLEXECUTEINFO sShellExecInfo = { 0 };
				sShellExecInfo.cbSize = sizeof(sShellExecInfo);
				sShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
				sShellExecInfo.lpFile = TXT_DOCUMENTATION_TEMP_FILENAME;
				sShellExecInfo.lpDirectory = L"";
				sShellExecInfo.lpParameters = L"";
				sShellExecInfo.nShow = SW_SHOWNORMAL;

				if (!ShellExecuteEx(&sShellExecInfo)) {
					DeleteFile(TXT_DOCUMENTATION_TEMP_FILENAME);
					return false;
				}
				
				WaitForSingleObject(sShellExecInfo.hProcess, INFINITE);

				CloseHandle(sShellExecInfo.hProcess);

				//Delete temp doc file
				return DeleteFile(TXT_DOCUMENTATION_TEMP_FILENAME) == TRUE;
			}
		public:
			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance) = 0;
		};

		class IRequireCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				if (__pShellInterface__->m_pPluginInt->PluginLoaded(__pShellInterface__->m_wszBaseDir + L"plugins\\" + pContext->GetPartString(1) + L".dll")) {
					std::wcout << L"Required library \"" << pContext->GetPartString(1) << L"\" is already loaded" << std::endl;
					return true;
				}

				bool bResult = __pShellInterface__->m_pPluginInt->LoadPlugin(__pShellInterface__->m_wszBaseDir + L"plugins\\" + pContext->GetPartString(1) + L".dll", __pShellInterface__->m_pShellInt, DNY_AS_PRODUCT_VERSION_W);

				if (!bResult) {
					std::wcout << L"Required library \"" << pContext->GetPartString(1) << L"\" could not be loaded" << std::endl;
				}

				return bResult;
			}
		} m_oRequireCommand;

		class IExecCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pObjectInstance);

				for (size_t i = 1; i < pContext->GetPartCount(); i++) {
					std::wstring wszData = pContext->GetPartData(i);

					dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>* pCvar = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>*)__pShellInterface__->m_pScriptInt->RegisterCVar(std::to_wstring(i), dnyScriptInterpreter::CVarManager::CT_STRING, false, false);
					if (pCvar) {
						pCvar->SetValue(wszData);
					}
				}
				
				bool bResult = __pShellInterface__->m_pScriptInt->ExecuteScript(pContext->GetPartString(1));

				for (size_t i = 1; i < pContext->GetPartCount(); i++) {
					__pShellInterface__->m_pScriptInt->FreeCVar(std::to_wstring(i));
				}

				return bResult;
			}
		} m_oExecCommand;

		class ISysCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			ISysCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				system(SI_WStringToString(pContext->GetPartString(1)).c_str());

				return true;
			}
		} m_oSysCommand;

		class IRunCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			IRunCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				ShellExecute(0, L"open", pContext->GetPartString(1).c_str(), pContext->GetPartString(2).c_str(), pContext->GetPartString(3).c_str(), SW_SHOWNORMAL);

				return true;
			}
		} m_oRunCommand;

		class IListLibsCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			IListLibsCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				__pShellInterface__->m_pPluginInt->ListPlugins();

				return true;
			}
		} m_oLibListCommand;

		class IExitCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			IExitCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				__pShellInterface__->m_bShallRun = false;

				return true;
			}
		} m_oExitCommand;

		bool Initialize(int argc, wchar_t* argv[])
		{
			//Initialize shell interface
			
			if (this->m_pShellInt)
				return true;

			//Setup base directory

			wchar_t wszFileName[MAX_PATH];
			if (!GetModuleFileName(0, wszFileName, sizeof(wszFileName)))
				return false;

			for (size_t i = wcslen(wszFileName); i >= 0; i--) {
				if (wszFileName[i] == '\\')
					break;

				wszFileName[i] = 0x0000;
			}

			this->m_wszBaseDir = wszFileName;

			//Set indicator
			this->m_bInteractiveMode = (argc <= 1);

			//Initialize console
			this->m_pConsoleInt = new Console::CConInterface(argc, argv);
			if (!this->m_pConsoleInt)
				return false;

			//Initialize scripting interface
			this->m_pScriptInt = new dnyScriptInterpreter::CScriptingInterface(/*this->m_wszBaseDir*/L"", &SI_StandardOutput);
			if (!this->m_pScriptInt) {
				this->Free();
				return false;
			}

			//Initialize shell interface
			this->m_pShellInt = new IShellPluginAPI(this->m_pScriptInt, this->m_pConsoleInt);
			if (!this->m_pShellInt) {
				this->Free();
				return false;
			}

			//Initialize plugin manager
			if (this->m_bInteractiveMode)
				this->m_pPluginInt = new Plugins::CPluginInt(this->m_wszBaseDir + L"plugins", this->m_pShellInt);
			else
				this->m_pPluginInt = new Plugins::CPluginInt();
			if (!this->m_pPluginInt) {
				this->Free();
				return false;
			}

			//Register interactive-mode indicator constant
			std::wstring wszConstStrVal = ((this->m_bInteractiveMode) ? L"true" : L"false");
			this->m_pScriptInt->ExecuteCode(L"const DNYAS_IS_INTERACTIVE_MODE bool <= " + wszConstStrVal + L";");

			//Add further commands
			#define SI_ADD_COMMAND(name, obj, type) if (!this->m_pScriptInt->RegisterCommand(name, obj, type)) { this->Free(); return false; }
			SI_ADD_COMMAND(L"require", &m_oRequireCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"exec", &m_oExecCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"sys", &m_oSysCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"run", &m_oRunCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"./", &m_oRunCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"listlibs", &m_oLibListCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"quit", &m_oExitCommand, dnyScriptInterpreter::CVarManager::CT_VOID);

			//Execute init-script if exists
			if (SI_FileExists(this->m_wszBaseDir + L"scripts\\init.dnys")) {
				this->m_pScriptInt->ExecuteScript(this->m_wszBaseDir + L"scripts\\init.dnys");
			}

			//Store class instance pointer
			__pShellInterface__ = this;

			//Handle arguments as scripts if provided
			if (!this->m_bInteractiveMode) {
				for (int i = 1; i < argc; i++) {
					std::wstring wszScriptFileName = argv[i];
					if (wszScriptFileName.find(L":") == std::wstring::npos) {
						wszScriptFileName = this->m_wszBaseDir + wszScriptFileName;
					}

					this->m_pScriptInt->ExecuteScript(wszScriptFileName);

					if (dnyScriptInterpreter::GetErrorInformation().GetErrorCode() != dnyScriptInterpreter::SET_NO_ERROR) {
						std::wcout << L"** Error **\n" << wszScriptFileName << L" (" << dnyScriptInterpreter::GetErrorInformation().GetErrorCode() << ")\n" << dnyScriptInterpreter::GetErrorInformation().GetErrorText() << std::endl;
					}
				}
			} else { 
				this->m_bShallRun = true; //Set run-indicator

				//Create console ctrl handler
				if (!SetConsoleCtrlHandler(&SI_ConsoleCtrHandler, TRUE)) {
					this->Free();
					return false;
				}
			}

			return true;
		}

		void Free(void)
		{
			//Free resources

			//Execute unload-script if exists
			if (SI_FileExists(this->m_wszBaseDir + L"scripts\\unload.dnys")) {
				this->m_pScriptInt->ExecuteScript(this->m_wszBaseDir + L"scripts\\unload.dnys");
			}

			//Remove handler routine if set
			if (this->m_bInteractiveMode) {
				SetConsoleCtrlHandler(&SI_ConsoleCtrHandler, FALSE);
			}

			//Unregister commands
			this->m_pScriptInt->UnregisterCommand(L"shelldoc");
			this->m_pScriptInt->UnregisterCommand(L"scriptdoc");
			this->m_pScriptInt->UnregisterCommand(L"require");
			this->m_pScriptInt->UnregisterCommand(L"exec");
			this->m_pScriptInt->UnregisterCommand(L"sys");
			this->m_pScriptInt->UnregisterCommand(L"run");
			this->m_pScriptInt->UnregisterCommand(L"./");
			this->m_pScriptInt->UnregisterCommand(L"listlibs");
			this->m_pScriptInt->UnregisterCommand(L"exit");

			//Free components

			if (this->m_pPluginInt) {
				delete this->m_pPluginInt;
				this->m_pPluginInt = nullptr;
			}

			if (this->m_pShellInt) {
				delete this->m_pShellInt;
				this->m_pShellInt = nullptr;
			}

			if (this->m_pScriptInt) {
				delete this->m_pScriptInt;
				this->m_pScriptInt = nullptr;
			}

			if (this->m_pConsoleInt) {
				delete this->m_pConsoleInt;
				this->m_pConsoleInt = nullptr;
			}
		}
	public:
		CShellInterface() : m_pShellInt(nullptr), m_bInteractiveMode(false) { }
		CShellInterface(int argc, wchar_t* argv[]) : m_pShellInt(nullptr), m_bInteractiveMode(false) { this->Initialize(argc, argv); }
		~CShellInterface() { this->Free(); }

		void Process(void)
		{
			//Process shell stuff

			if (!this->m_bInteractiveMode)
				return;

			std::wstring wszConsoleLine;
			std::wstring wszMultiLine;
			bool bInMulitLine = false;

			while (this->m_bShallRun) {
				*this->m_pConsoleInt >> wszConsoleLine; //Query input

				if ((wszConsoleLine == L"<") && (!bInMulitLine)) { //Check for multiline mode indicator
					bInMulitLine = true;
					wszMultiLine = L"";
					this->m_pConsoleInt->SetColor("B4");
					continue;
				}

				if (bInMulitLine) {
					if (wszConsoleLine == L">") { //Check for multiline close-mode indicator
						this->m_pScriptInt->ExecuteCode(wszMultiLine); //Execute multiline code
						bInMulitLine = false; //Clear indicator
						this->m_pConsoleInt->SetDefaultColor();
					} else {
						wszMultiLine += wszConsoleLine; //Append to multiline string
					}

					continue;
				}

				if (wszConsoleLine[wszConsoleLine.length() - 1] != L';')
					wszConsoleLine += L";";

				this->m_pScriptInt->ExecuteCode(wszConsoleLine); //Execute code

				if (dnyScriptInterpreter::GetErrorInformation().GetErrorCode() != dnyScriptInterpreter::SET_NO_ERROR) {
					std::wcout << L"** Error **\n" << wszConsoleLine << L" (" << dnyScriptInterpreter::GetErrorInformation().GetErrorCode() << ")\n" << dnyScriptInterpreter::GetErrorInformation().GetErrorText() << std::endl;
				}
			}
		}

		void ShutdownShell(void) { this->m_bShallRun = false; }
	};

	BOOL WINAPI SI_ConsoleCtrHandler(DWORD dwCtrlType)
	{
		//Handle console control events

		if ((dwCtrlType == CTRL_CLOSE_EVENT) || (dwCtrlType == CTRL_SHUTDOWN_EVENT)) { //Check for close events
			__pShellInterface__->ShutdownShell(); //Clear indicator in order to let the program close
			return TRUE;
		}

		return FALSE;
	}
}

int wmain(int argc, wchar_t* argv[], wchar_t *envp[])
{
	//Windows entry point

	ShellInterface::CShellInterface* pShellInterface = new ShellInterface::CShellInterface(argc, argv);
	if (!pShellInterface) {
		MessageBox(0, L"Failed to instantiate shell", DNY_AS_PRODUCT_NAME, MB_ICONERROR);
		return EXIT_FAILURE;
	}

	pShellInterface->Process();

	delete pShellInterface;
	
	return EXIT_SUCCESS;
}
