#include "console.h"
#include "dnyScriptInterpreter.h"
#include "plugins.h"
#include "resource.h"
#include <codecvt>
#include <array>
#include <memory>

/*
	AquaShell (dnyAquaShell) developed by Daniel Brendel

	(C) 2017 - 2025 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace ShellInterface {
	class CShellInterface* __pShellInterface__;

	struct exec_argument_stack_s {
		dnyScriptInterpreter::dnyInteger argc;
		std::vector<dnyScriptInterpreter::dnyString> argv;
	};
	std::vector<exec_argument_stack_s> vExecArgumentStack;

	void PrintAboutInfo(void)
	{
		//Print about info

		std::wcout << L"* " DNY_AS_PRODUCT_NAME L" (" DNY_AS_PRODUCT_CODENAME L") v" DNY_AS_PRODUCT_VERSION << std::endl;
		std::wcout << L"* Developed by " DNY_AS_PRODUCT_AUTHOR L" (" DNY_AS_PRODUCT_CONTACT L")" << std::endl;
		std::wcout << L"* Released under " DNY_AS_PRODUCT_LICENCE << std::endl;
	}

	void SI_StandardOutput(const std::wstring& wszText)
	{
		//Standard output handler

		std::wcout << wszText << std::endl;
	}

	bool SI_FileExists(const std::wstring& wszFile)
	{
		//Check if a file exists

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
		//Convert wide string to ansi string

		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}

	std::wstring system_exec(const std::wstring& wszCmd, bool echo = false)
	{
		//Perform system command and handle result accordingly

		std::array<wchar_t, 128> buffer;
		std::wstring result;

		std::unique_ptr<FILE, decltype(&_pclose)> pipe(_wpopen(wszCmd.c_str(), L"r"), _pclose);
		if (!pipe) {
			return L"";
		}
		
		while (fgetws(buffer.data(), (int)buffer.size(), pipe.get()) != nullptr) {
			if (echo) {
				std::wcout << buffer.data();
			}

			result += buffer.data();
		}
		
		return result;
	}

	std::wstring ExtractFilePath(const std::wstring& wszFile)
	{
		//Extract file path

		wchar_t wszFullFileName[MAX_PATH * 2];
		lstrcpyW(wszFullFileName, wszFile.c_str());

		for (size_t i = wcslen(wszFullFileName); i > 0; i--) {
			if (wszFullFileName[i] == '\\')
				break;
			wszFullFileName[i] = 0;
		}

		return wszFullFileName;
	}

	std::wstring GetCurrentPath(void)
	{
		//Get current working path

		wchar_t wszPath[MAX_PATH] = { 0 };

		if (GetCurrentDirectory(sizeof(wszPath), wszPath)) {
			return wszPath;
		}

		return L"";
	}

	std::wstring GetFullShellPath(void)
	{
		//Get full shell path

		wchar_t wszFullPath[1024] = { 0 };

		GetModuleFileName(0, wszFullPath, sizeof(wszFullPath));
			
		return wszFullPath;
	}

	std::wstring GetWorkingDirectory(void)
	{
		//Get current working directory

		wchar_t wszWorkingDir[1024] = { 0 };

		if (GetCurrentDirectory(sizeof(wszWorkingDir), wszWorkingDir)) {
			return std::wstring(wszWorkingDir);
		}

		return L"";
	}

	bool AddShellToPath(HKEY hContext, const std::wstring& wszKeyPath)
	{
		//Add shell to path

		bool bResult = false;
		HKEY hKey;

		if (RegOpenKeyEx(hContext, wszKeyPath.c_str(), 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
			wchar_t wszPathContent[1024] = { 0 };
			DWORD dwDataSize = sizeof(wszPathContent);
			DWORD dwType = REG_SZ;

			if (RegQueryValueEx(hKey, L"Path", NULL, &dwType, (LPBYTE)wszPathContent, &dwDataSize) == ERROR_SUCCESS) {
				std::wstring wszEnvPath = std::wstring(wszPathContent);
				std::wstring wszFullShellPath = GetCurrentPath();

				if (wszEnvPath.find(wszFullShellPath) == std::wstring::npos) {
					std::wstring wszNewValue = wszEnvPath + L";" + wszFullShellPath;

					if (RegSetValueEx(hKey, L"Path", NULL, REG_SZ, (BYTE*)wszNewValue.data(), (DWORD)(wszNewValue.size() * 2)) == ERROR_SUCCESS) {
						SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 5000, NULL);

						bResult = true;
					}
				}
			}

			RegCloseKey(hKey);

			return bResult;
		}

		return bResult;
	}

	BOOL DetachConsole(void)
	{
		return FreeConsole();
	}

	BOOL WINAPI SI_ConsoleCtrHandler(DWORD dwCtrlType);

	class CExtendedScriptingInterface : public dnyScriptInterpreter::CScriptingInterface {
	private:
		std::vector<std::wstring> m_vCurrentScripts;
	public:
		CExtendedScriptingInterface() : dnyScriptInterpreter::CScriptingInterface() {}
		CExtendedScriptingInterface(const std::wstring& wszScriptDirectory, const dnyScriptInterpreter::CScriptingInterface::TpfnStandardOutput pfnStandardOutput) : dnyScriptInterpreter::CScriptingInterface(wszScriptDirectory, pfnStandardOutput) { }
		~CExtendedScriptingInterface() {}

		bool ExecuteScript(const std::wstring& wszScriptFile)
		{
			//Gateway to script execution function

			this->m_vCurrentScripts.push_back(wszScriptFile);

			bool bResult = dnyScriptInterpreter::CScriptingInterface::ExecuteScript(wszScriptFile);

			this->m_vCurrentScripts.erase(this->m_vCurrentScripts.begin() + this->m_vCurrentScripts.size() - 1);

			return bResult;
		}

		const std::wstring GetCurrentScript(void)
		{
			//Get current running script

			if (this->m_vCurrentScripts.size() > 0) {
				return this->m_vCurrentScripts[this->m_vCurrentScripts.size() - 1];
			}

			return L"";
		}
	};

	class CShellInterface { //Shell interface manager
	private:
		Console::CConInterface* m_pConsoleInt;
		CExtendedScriptingInterface* m_pScriptInt;
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

			virtual bool Cmd_UnregisterCommand(const std::wstring& wszCmdName)
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

		class CTextFilePrinter {
		private:
			bool m_bLastOpResult;
			std::wifstream m_hFile;
		public:
			CTextFilePrinter() : m_bLastOpResult(false) {}
			CTextFilePrinter(const std::wstring& wszFile) : m_bLastOpResult(false) { this->Print(wszFile); }
			~CTextFilePrinter() {}

			bool Print(const std::wstring& wszFile)
			{
				this->m_bLastOpResult = false;

				if (!wszFile.length())
					return this->m_bLastOpResult;

				this->m_hFile.open(wszFile, std::wifstream::in);
				if (this->m_hFile.is_open()) {
					std::wstring wszCurrentLine;
					size_t uiLineCounter = 0;

					while (!this->m_hFile.eof()) {
						std::getline(this->m_hFile, wszCurrentLine);
						uiLineCounter++;

						std::wcout << L"(#" << uiLineCounter << L") " << wszCurrentLine << std::endl;
					}

					this->m_hFile.close();

					this->m_bLastOpResult = true;
				}

				return this->m_bLastOpResult;
			}

			inline const bool GetLastResult(void) const { return this->m_bLastOpResult; }
		};

		bool m_bInteractiveMode;
		bool m_bShallRun;
		std::wstring m_wszBaseDir;

		friend void SI_StandardOutput(const std::wstring& wszText);
		friend BOOL WINAPI SI_ConsoleCtrHandler(DWORD dwCtrlType);

		class IRequireCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pObjectInstance);

				bool bResult = false;

				if (pContext->GetPartString(1) == L"__ALL__") {
					bResult = __pShellInterface__->m_pPluginInt->LoadAllPlugins(__pShellInterface__->m_wszBaseDir + L"plugins", __pShellInterface__->m_pShellInt);
				} else {
					if (__pShellInterface__->m_pPluginInt->PluginLoaded(__pShellInterface__->m_wszBaseDir + L"plugins\\" + pContext->GetPartString(1) + L".dll")) {
						//std::wcout << L"Required library \"" << pContext->GetPartString(1) << L"\" is already loaded" << std::endl;
						return true;
					}

					bResult = __pShellInterface__->m_pPluginInt->LoadPlugin(__pShellInterface__->m_wszBaseDir + L"plugins\\" + pContext->GetPartString(1) + L".dll", __pShellInterface__->m_pShellInt, DNY_AS_PRODUCT_VERSION_W);

					if (!bResult) {
						std::wcout << L"Required library \"" << pContext->GetPartString(1) << L"\" could not be loaded" << std::endl;
					}
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

				exec_argument_stack_s current_stack_item;

				dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyInteger>* pCvarCount = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyInteger>*)__pShellInterface__->m_pScriptInt->FindCVar(L"argc");
				if (!pCvarCount) {
					pCvarCount = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyInteger>*)__pShellInterface__->m_pScriptInt->RegisterCVar(L"argc", dnyScriptInterpreter::CVarManager::CT_INT, false, false);
				}

				if (pCvarCount) {
					pCvarCount->SetValue(pContext->GetPartCount() - 1);
					current_stack_item.argc = pCvarCount->GetValue();
				}

				for (size_t i = 1; i < pContext->GetPartCount(); i++) {
					std::wstring wszData = pContext->GetPartData(i);

					dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>* pCvarValue = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>*)__pShellInterface__->m_pScriptInt->FindCVar(std::to_wstring(i - 1));
					if (!pCvarValue) {
						pCvarValue = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>*)__pShellInterface__->m_pScriptInt->RegisterCVar(std::to_wstring(i - 1), dnyScriptInterpreter::CVarManager::CT_STRING, false, false);
					}

					if (pCvarValue) {
						pCvarValue->SetValue(wszData);
					}

					pCvarValue = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>*)__pShellInterface__->m_pScriptInt->FindCVar(L"argv[" + std::to_wstring(i - 1) + L"]");
					if (!pCvarValue) {
						pCvarValue = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>*)__pShellInterface__->m_pScriptInt->RegisterCVar(L"argv[" + std::to_wstring(i - 1) + L"]", dnyScriptInterpreter::CVarManager::CT_STRING, false, false);
					}

					if (pCvarValue) {
						pCvarValue->SetValue(wszData);

						current_stack_item.argv.push_back(wszData);
					}
				}

				vExecArgumentStack.push_back(current_stack_item);
				
				bool bResult = __pShellInterface__->m_pScriptInt->ExecuteScript(pContext->GetPartString(1));

				vExecArgumentStack.erase(vExecArgumentStack.begin() + vExecArgumentStack.size() - 1);
				
				if (vExecArgumentStack.size()) {
					pCvarCount->SetValue(vExecArgumentStack[vExecArgumentStack.size() - 1].argc);

					for (size_t i = 1; i < pContext->GetPartCount(); i++) {
						dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>* pCvarValue = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>*)__pShellInterface__->m_pScriptInt->FindCVar(std::to_wstring(i - 1));
						if (pCvarValue) {
							if (i <= vExecArgumentStack[vExecArgumentStack.size() - 1].argv.size()) {
								pCvarValue->SetValue(vExecArgumentStack[vExecArgumentStack.size() - 1].argv[i - 1]);
							} else {
								pCvarValue->SetValue(std::to_wstring(i - 1));
							}
						}

						pCvarValue = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>*)__pShellInterface__->m_pScriptInt->FindCVar(L"argv[" + std::to_wstring(i - 1) + L"]");
						if (pCvarValue) {
							if (i <= vExecArgumentStack[vExecArgumentStack.size() - 1].argv.size()) {
								pCvarValue->SetValue(vExecArgumentStack[vExecArgumentStack.size() - 1].argv[i - 1]);
							} else {
								pCvarValue->SetValue(L"argv[" + std::to_wstring(i - 1) + L"]");
							}
						}
					}
				} else {
					for (size_t i = 1; i < pContext->GetPartCount(); i++) {
						__pShellInterface__->m_pScriptInt->FreeCVar(std::to_wstring(i - 1));
						__pShellInterface__->m_pScriptInt->FreeCVar(L"argv[" + std::to_wstring(i - 1) + L"]");
					}

					__pShellInterface__->m_pScriptInt->FreeCVar(L"argc");
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

				pContext->ReplaceAllVariables(pObjectInstance);

				std::wstring wszVar = pContext->GetPartString(2);
				std::wstring wszResult = system_exec(pContext->GetPartString(1), wszVar.length() == 0);

				if (wszVar.length()) {
					dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>* pCVar = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>*)__pShellInterface__->m_pScriptInt->FindCVar(wszVar);
					if (!pCVar) {
						pCVar = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>*)__pShellInterface__->m_pScriptInt->RegisterCVar(wszVar, dnyScriptInterpreter::CVarManager::CT_STRING, false, false);
					}

					if (pCVar) {
						pCVar->SetValue(wszResult);
					}
				}

				return true;
			}
		} m_oSysCommand;

		class IRunCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			IRunCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pObjectInstance);

				ShellExecute(0, L"open", pContext->GetPartString(1).c_str(), pContext->GetPartString(2).c_str(), pContext->GetPartString(3).c_str(), SW_SHOWNORMAL);

				return true;
			}
		} m_oRunCommand;

		class IPauseCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			IPauseCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				system("pause");

				return true;
			}
		} m_oPauseCommand;

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

		class ICurrentScriptNameCommandInterface : public dnyScriptInterpreter::CCommandManager::IResultCommandInterface<dnyScriptInterpreter::dnyString> {
		public:
			ICurrentScriptNameCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pObjectInstance);

				std::wstring wszScriptName = __pShellInterface__->m_pScriptInt->GetCurrentScript();

				if (__pShellInterface__->m_bInteractiveMode) {
					wszScriptName = GetCurrentPath() + L"\\" + wszScriptName;
				}

				this->SetResult(wszScriptName);

				return true;
			}
		} m_oCurrentScriptNameCommand;

		class ICurrentScriptPathCommandInterface : public dnyScriptInterpreter::CCommandManager::IResultCommandInterface<dnyScriptInterpreter::dnyString> {
		public:
			ICurrentScriptPathCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pObjectInstance);

				std::wstring wszScriptName = __pShellInterface__->m_pScriptInt->GetCurrentScript();
				std::wstring wszScriptPath = (__pShellInterface__->m_bInteractiveMode) ? GetCurrentPath() : ExtractFilePath(wszScriptName);
				
				this->SetResult(wszScriptPath);

				return true;
			}
		} m_oCurrentScriptPathCommand;

		class IChangeWorkingDirCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			IChangeWorkingDirCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pObjectInstance);

				std::wstring wszNewDir = pContext->GetPartString(1);

				BOOL bResult = SetCurrentDirectory(wszNewDir.c_str());
				if (bResult) {
					__pShellInterface__->m_pConsoleInt->UpdateWorkingDir(wszNewDir);
				}

				return bResult == TRUE;
			}
		} m_oChangeWorkingDirCommand;

		class IGetWorkingDirCommandInterface : public dnyScriptInterpreter::CCommandManager::IResultCommandInterface<dnyScriptInterpreter::dnyString> {
		public:
			IGetWorkingDirCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pObjectInstance)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pObjectInstance);

				TCHAR wszCurrentDir[2048];

				DWORD dwResult = GetCurrentDirectory(sizeof(wszCurrentDir), wszCurrentDir);

				this->SetResult(wszCurrentDir);

				return dwResult != 0;
			}
		} m_oGetWorkingDirCommand;

		class ITextFilePrinterCommandInterface : dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			ITextFilePrinterCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pInterfaceObject);

				CTextFilePrinter oFilePrinter(pContext->GetPartString(1));

				return oFilePrinter.GetLastResult();
			}

		} g_oTextFilePrinterCommandInterface;

		class IRandomCommandInterface : public dnyScriptInterpreter::CCommandManager::IResultCommandInterface<dnyScriptInterpreter::dnyInteger> {
		public:
			IRandomCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pInterfaceObject);

				dnyScriptInterpreter::dnyInteger iBegin = pContext->GetPartInt(1);
				dnyScriptInterpreter::dnyInteger iEnd = pContext->GetPartInt(2);
				
				dnyScriptInterpreter::dnyInteger iRndNum = (rand() % (iEnd - iBegin)) + iBegin;

				IResultCommandInterface<dnyScriptInterpreter::dnyInteger>::SetResult(iRndNum);

				return true;
			}
		} g_oRandomCommandInterface;

		class ISleepCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			ISleepCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pInterfaceObject);

				Sleep((DWORD)pContext->GetPartInt(1));

				return true;
			}

		} g_oSleepCommandInterface;

		class IBitOpCommandInterface : public dnyScriptInterpreter::CCommandManager::IResultCommandInterface<dnyScriptInterpreter::dnyInteger> {
		public:
			IBitOpCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pInterfaceObject);

				std::wstring wszOperation = pContext->GetPartString(1);
				std::vector<std::wstring> vOperands = pContext->GetPartArray(2);

				if (vOperands.size() < 2) {
					return false;
				}

				dnyScriptInterpreter::dnyInteger iResultValue = (vOperands.size() > 0) ? (dnyScriptInterpreter::dnyInteger)_wtoi64(vOperands[0].c_str()) : 0;

				for (size_t i = 1; i < vOperands.size(); i++) {
					if (wszOperation == L"or") {
						iResultValue = iResultValue | (dnyScriptInterpreter::dnyInteger)_wtoi64(vOperands[i].c_str());
					} else if (wszOperation == L"and") {
						iResultValue = iResultValue & (dnyScriptInterpreter::dnyInteger)_wtoi64(vOperands[i].c_str());
					} else if (wszOperation == L"xor") {
						iResultValue = iResultValue ^ (dnyScriptInterpreter::dnyInteger)_wtoi64(vOperands[i].c_str());
					}
				}

				IResultCommandInterface<dnyScriptInterpreter::dnyInteger>::SetResult(iResultValue);

				return true;
			}

		} g_oBitOpCommandInterface;

		class IGetTickCountCommandInterface : public dnyScriptInterpreter::CCommandManager::IResultCommandInterface<dnyScriptInterpreter::dnyInteger> {
		public:
			IGetTickCountCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				IResultCommandInterface<dnyScriptInterpreter::dnyInteger>::SetResult(GetTickCount64());

				return true;
			}

		} g_oGetTickCountCommandInterface;

		class ITimeStampCommandInterface : public dnyScriptInterpreter::CCommandManager::IResultCommandInterface<dnyScriptInterpreter::dnyInteger> {
		public:
			ITimeStampCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pInterfaceObject);

				std::time_t tm = std::time(nullptr);

				IResultCommandInterface<dnyScriptInterpreter::dnyInteger>::SetResult(tm);

				return true;
			}
		} g_oTimeStampCommandInterface;

		class IFormatDateTimeCommandInterface : public dnyScriptInterpreter::CCommandManager::IResultCommandInterface<dnyScriptInterpreter::dnyString> {
		public:
			IFormatDateTimeCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pInterfaceObject);

				wchar_t wcsDate[MAX_PATH];

				std::wstring wszFormat = pContext->GetPartString(1);
				std::time_t t = std::time(nullptr);

				if (pContext->GetPartCount() >= 4) {
					t = pContext->GetPartInt(2);
				}

				std::wcsftime(wcsDate, sizeof(wcsDate), wszFormat.c_str(), std::localtime(&t));

				IResultCommandInterface<dnyScriptInterpreter::dnyString>::SetResult(wcsDate);

				return true;
			}
		} g_oFormatDateTimeCommandInterface;

		class IGetSystemErrorCommandInterface : public dnyScriptInterpreter::CCommandManager::IResultCommandInterface<dnyScriptInterpreter::dnyInteger> {
		public:
			IGetSystemErrorCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				IResultCommandInterface<dnyScriptInterpreter::dnyInteger>::SetResult(GetLastError());

				return true;
			}

		} g_oGetSystemErrorCommandInterface;

		class ISetSystemErrorCommandInterface : public dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			ISetSystemErrorCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pInterfaceObject);

				SetLastError((DWORD)pContext->GetPartInt(1));

				return true;
			}

		} g_oSetSystemErrorCommandInterface;

		class IHideConsoleCommandInterface : dnyScriptInterpreter::CCommandManager::IVoidCommandInterface {
		public:
			IHideConsoleCommandInterface() {}

			virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
			{
				dnyScriptInterpreter::ICodeContext* pContext = (dnyScriptInterpreter::ICodeContext*)pCodeContext;

				pContext->ReplaceAllVariables(pInterfaceObject);

				DetachConsole();

				return true;
			}

		} g_oHideConsoleCommandInterface;

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

			//Random seed generation
			srand((unsigned int)time(NULL));

			//Set indicator
			this->m_bInteractiveMode = (argc <= 1);

			//Print info text if in interactive mode
			if (this->m_bInteractiveMode) {
				std::wcout << L"==============================================================" << std::endl;

				PrintAboutInfo();

				std::wcout << L"==============================================================" << std::endl << std::endl;

			}

			//Initialize console
			this->m_pConsoleInt = new Console::CConInterface(GetWorkingDirectory(), argc, argv);
			if (!this->m_pConsoleInt)
				return false;

			//Initialize scripting interface
			this->m_pScriptInt = new CExtendedScriptingInterface(/*this->m_wszBaseDir*/L"", &SI_StandardOutput);
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

			//Register constants for shell version
			this->m_pScriptInt->ExecuteCode(L"const DNYAS_SHELL_VERSION string <= \"" DNY_AS_PRODUCT_VERSION L"\";");
			this->m_pScriptInt->ExecuteCode(L"const DNYAS_SHELL_VERNUM int <= " + std::to_wstring(DNY_AS_PRODUCT_VERSION_W) + L";");

			//Register constants for platform and build info
			this->m_pScriptInt->ExecuteCode(L"const DNYAS_PLATFORM_NAME string <= \"" PLATFORM_NAME L"\";");
			this->m_pScriptInt->ExecuteCode(L"const DNYAS_BUILD_TYPE string <= \"" BUILD_TYPE L"\";");

			//Register interactive-mode indicator constant
			std::wstring wszConstStrVal = ((this->m_bInteractiveMode) ? L"true" : L"false");
			this->m_pScriptInt->ExecuteCode(L"const DNYAS_IS_INTERACTIVE_MODE bool <= " + wszConstStrVal + L";");

			//Register special character constants
			this->m_pScriptInt->ExecuteCode(L"const CR string <= \"\r\";");
			this->m_pScriptInt->ExecuteCode(L"const LF string <= \"\n\";");
			this->m_pScriptInt->ExecuteCode(L"const TAB string <= \"\t\";");
			dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>* pQuotSingleConst = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>*)this->m_pScriptInt->RegisterCVar(L"QUOT_SINGLE", dnyScriptInterpreter::CVarManager::CT_STRING, true, false);
			if (pQuotSingleConst) pQuotSingleConst->SetValue(L"\'");
			dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>* pQuotDoubleConst = (dnyScriptInterpreter::CVarManager::ICVar<dnyScriptInterpreter::dnyString>*)this->m_pScriptInt->RegisterCVar(L"QUOT_DOUBLE", dnyScriptInterpreter::CVarManager::CT_STRING, true, false);
			if (pQuotDoubleConst) pQuotDoubleConst->SetValue(L"\"");

			//Register constants for various folders
			this->m_pScriptInt->ExecuteCode(L"const DNYAS_BASE_PATH string <= \"" + this->m_wszBaseDir + L"\";");
			this->m_pScriptInt->ExecuteCode(L"const DNYAS_PLUGIN_PATH string <= \"" + this->m_wszBaseDir + L"plugins" + L"\";");
			this->m_pScriptInt->ExecuteCode(L"const DNYAS_DEFSCRIPTS_PATH string <= \"" + this->m_wszBaseDir + L"scripts" + L"\";");
			
			//Register void variable in order to allow dropping result values
			this->m_pScriptInt->ExecuteCode(L"global void string;");

			//Add further commands
			#define SI_ADD_COMMAND(name, obj, type) if (!this->m_pScriptInt->RegisterCommand(name, obj, type)) { this->Free(); return false; }
			SI_ADD_COMMAND(L"getscriptpath", &m_oCurrentScriptPathCommand, dnyScriptInterpreter::CVarManager::CT_STRING);
			SI_ADD_COMMAND(L"getscriptname", &m_oCurrentScriptNameCommand, dnyScriptInterpreter::CVarManager::CT_STRING);
			SI_ADD_COMMAND(L"textview", &g_oTextFilePrinterCommandInterface, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"random", &g_oRandomCommandInterface, dnyScriptInterpreter::CVarManager::CT_INT);
			SI_ADD_COMMAND(L"sleep", &g_oSleepCommandInterface, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"bitop", &g_oBitOpCommandInterface, dnyScriptInterpreter::CVarManager::CT_INT);
			SI_ADD_COMMAND(L"gettickcount", &g_oGetTickCountCommandInterface, dnyScriptInterpreter::CVarManager::CT_INT);
			SI_ADD_COMMAND(L"timestamp", &g_oTimeStampCommandInterface, dnyScriptInterpreter::CVarManager::CT_INT);
			SI_ADD_COMMAND(L"fmtdatetime", &g_oFormatDateTimeCommandInterface, dnyScriptInterpreter::CVarManager::CT_STRING);
			SI_ADD_COMMAND(L"getsystemerror", &g_oGetSystemErrorCommandInterface, dnyScriptInterpreter::CVarManager::CT_INT);
			SI_ADD_COMMAND(L"setsystemerror", &g_oSetSystemErrorCommandInterface, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"hideconsole", &g_oHideConsoleCommandInterface, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"cwd", &m_oChangeWorkingDirCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"gwd", &m_oGetWorkingDirCommand, dnyScriptInterpreter::CVarManager::CT_STRING);
			SI_ADD_COMMAND(L"require", &m_oRequireCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"exec", &m_oExecCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"sys", &m_oSysCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"run", &m_oRunCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"pause", &m_oPauseCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"./", &m_oRunCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"listlibs", &m_oLibListCommand, dnyScriptInterpreter::CVarManager::CT_VOID);
			SI_ADD_COMMAND(L"quit", &m_oExitCommand, dnyScriptInterpreter::CVarManager::CT_VOID);

			//Store class instance pointer
			__pShellInterface__ = this;

			//Execute init-script if exists
			if (SI_FileExists(this->m_wszBaseDir + L"scripts\\init.dnys")) {
				this->m_pScriptInt->ExecuteScript(this->m_wszBaseDir + L"scripts\\init.dnys");
			}

			//Handle arguments as scripts if provided
			if (!this->m_bInteractiveMode) {
				if (argc > 1) {
					std::wstring wszArgCmd = argv[1];

					if (wszArgCmd[0] != '-') {
						std::wstring wszExecScript = (argc >= 2) ? argv[1] : L"";
						if (wszExecScript.length() > 0) {
							if (wszExecScript.find(L":") == std::wstring::npos) {
								wszExecScript = GetWorkingDirectory() + L"\\" + wszExecScript;
							}

							std::wstring wszExecArgs = L"";
							for (int i = 2; i < argc; i++) {
								wszExecArgs += L" \"" + std::wstring(argv[i]) + L"\"";
							}

							this->m_pScriptInt->ExecuteCode(L"exec \"" + wszExecScript + L"\"" + wszExecArgs + L";");

							if (dnyScriptInterpreter::GetErrorInformation().GetErrorCode() != dnyScriptInterpreter::SET_NO_ERROR) {
								std::wcout << L"** Error **\n" << wszExecScript << L" (" << dnyScriptInterpreter::GetErrorInformation().GetErrorCode() << ")\n" << dnyScriptInterpreter::GetErrorInformation().GetErrorText() << std::endl;
							}
						} else {
							std::wcout << L"** Error ** No script input provided" << std::endl;
						}
					} else {
						if (wszArgCmd == L"-v") {
							PrintAboutInfo();
						} else if (wszArgCmd == L"-c") {
							std::wstring wszExecCode = (argc >= 3) ? argv[2] : L"";
							if (wszExecCode.length() > 0) {
								this->m_pScriptInt->ExecuteCode(wszExecCode);

								if (dnyScriptInterpreter::GetErrorInformation().GetErrorCode() != dnyScriptInterpreter::SET_NO_ERROR) {
									std::wcout << L"** Error **\n" << wszExecCode << L" (" << dnyScriptInterpreter::GetErrorInformation().GetErrorCode() << ")\n" << dnyScriptInterpreter::GetErrorInformation().GetErrorText() << std::endl;
								}
							} else {
								std::wcout << L"** Error ** No code input provided" << std::endl;
							}
						} else if (wszArgCmd == L"-libs") {
							this->m_pPluginInt->LoadAllPlugins(this->m_wszBaseDir + L"plugins", this->m_pShellInt);
							this->m_pPluginInt->ListPlugins();
						} else if (wszArgCmd == L"-path") {
							std::wstring wszContext = (argc == 3) ? argv[2] : L"user";

							if (wszContext == L"user") {
								((AddShellToPath(HKEY_CURRENT_USER, L"Environment")) ? std::wcout << L"Success" << std::endl : std::wcout << L"Error: " << GetLastError() << std::endl);
							} else if (wszContext == L"machine") {
								((AddShellToPath(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment")) ? std::wcout << L"Success" << std::endl : std::wcout << L"Error: " << GetLastError() << std::endl);
							}
						}
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

			//Unset void-variable
			this->m_pScriptInt->ExecuteCode(L"unset void;");

			//Remove handler routine if set
			if (this->m_bInteractiveMode) {
				SetConsoleCtrlHandler(&SI_ConsoleCtrHandler, FALSE);
			}

			//Unregister commands
			this->m_pScriptInt->UnregisterCommand(L"getscriptpath");
			this->m_pScriptInt->UnregisterCommand(L"getscriptname");
			this->m_pScriptInt->UnregisterCommand(L"textview");
			this->m_pScriptInt->UnregisterCommand(L"random");
			this->m_pScriptInt->UnregisterCommand(L"sleep");
			this->m_pScriptInt->UnregisterCommand(L"bitop");
			this->m_pScriptInt->UnregisterCommand(L"gettickcount");
			this->m_pScriptInt->UnregisterCommand(L"timestamp");
			this->m_pScriptInt->UnregisterCommand(L"fmtdatetime");
			this->m_pScriptInt->UnregisterCommand(L"getsystemerror");
			this->m_pScriptInt->UnregisterCommand(L"setsystemerror");
			this->m_pScriptInt->UnregisterCommand(L"hideconsole");
			this->m_pScriptInt->UnregisterCommand(L"cwd");
			this->m_pScriptInt->UnregisterCommand(L"gwd");
			this->m_pScriptInt->UnregisterCommand(L"require");
			this->m_pScriptInt->UnregisterCommand(L"exec");
			this->m_pScriptInt->UnregisterCommand(L"sys");
			this->m_pScriptInt->UnregisterCommand(L"run");
			this->m_pScriptInt->UnregisterCommand(L"pause");
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

				//Clear input buffer
				std::wcin.clear();
				fflush(stdin);

				if ((wszConsoleLine == L"<") && (!bInMulitLine)) { //Check for multiline mode indicator
					bInMulitLine = true;
					wszMultiLine = L"";
					this->m_pConsoleInt->SetColor("3E");
					continue;
				}

				if (bInMulitLine) {
					if (wszConsoleLine == L">") { //Check for multiline close-mode indicator
						this->m_pScriptInt->ExecuteCode(wszMultiLine); //Execute multiline code
						bInMulitLine = false; //Clear indicator
						this->m_pConsoleInt->SetColor("07");
					} else {
						wszMultiLine += wszConsoleLine; //Append to multiline string
					}

					continue;
				}

				if ((wszConsoleLine.length() > 0) && (wszConsoleLine[wszConsoleLine.length() - 1] != L';')) {
					wszConsoleLine += L";";
				}

				if (!this->m_pScriptInt->ExecuteCode(wszConsoleLine)) { //Execute code
					if (dnyScriptInterpreter::GetErrorInformation().GetErrorCode() == dnyScriptInterpreter::SET_UNKNOWN_COMMAND) {
						_wsystem(wszConsoleLine.substr(0, wszConsoleLine.length() - 1).c_str());
					}
				}

				if ((dnyScriptInterpreter::GetErrorInformation().GetErrorCode() != dnyScriptInterpreter::SET_NO_ERROR) && (dnyScriptInterpreter::GetErrorInformation().GetErrorCode() != dnyScriptInterpreter::SET_UNKNOWN_COMMAND)) {
					std::wcout << L"** Error **\n" << wszConsoleLine << L" (" << dnyScriptInterpreter::GetErrorInformation().GetErrorCode() << ")\n" << dnyScriptInterpreter::GetErrorInformation().GetErrorText() << std::endl;
				}
			}
		}

		void ShutdownShell(void) { this->m_bShallRun = false; }

		const bool IsInInteractiveMode(void) const { return this->m_bInteractiveMode; }
	};

	BOOL WINAPI SI_ConsoleCtrHandler(DWORD dwCtrlType)
	{
		//Handle console control events
		
		if ((dwCtrlType == CTRL_C_EVENT) || (dwCtrlType == CTRL_BREAK_EVENT)) {
			//Only shutdown when not in interactive mode
			if (!__pShellInterface__->IsInInteractiveMode()) {
				__pShellInterface__->ShutdownShell();
			} else {
				std::wcout << std::endl;
			}
			
			return TRUE;
		}

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
