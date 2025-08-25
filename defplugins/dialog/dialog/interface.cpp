#include "dnyas_sdk.h"
#include "inputbox.h"
#include "filedialog.h"
#include "message.h"

IShellPluginAPI* g_pShellPluginAPI;

class IInputCliVoidCommandInterface : public IVoidCommandInterface {
public:
	IInputCliVoidCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		if (pCodeContext->GetPartCount() <= 1)
			return false;

		pCodeContext->ReplaceAllVariables(pArg2);

		ICVar<dnyString>* pCvar = (ICVar<dnyString>*)g_pShellPluginAPI->Cv_FindCVar(pCodeContext->GetPartString(1));
		if (!pCvar)
			return false;

		std::wstring wszQueryText = pCodeContext->GetPartString(2);
		if (wszQueryText.length()) {
			std::wcout << wszQueryText;
		}

		std::wstring wszInput;
		std::getline(std::wcin, wszInput);

		pCvar->SetValue(wszInput);

		return true;
	}
} g_oInputCliResultCommand;

class IInputBoxCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IInputBoxCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		std::wstring wszResult(L"");

		InputBox::InputBoxHandle hInputBox = InputBox::Spawn(pCodeContext->GetPartString(1), pCodeContext->GetPartString(2), pCodeContext->GetPartString(3), (int)pCodeContext->GetPartInt(4), (int)pCodeContext->GetPartInt(5));
		if (InputBox::IsValid(hInputBox)) {
			InputBox::Show(hInputBox);
			wszResult = InputBox::Text(hInputBox);
			InputBox::Free(hInputBox);
		}

		IResultCommandInterface<dnyString>::SetResult(wszResult);

		return true;
	}
} g_oInputBoxCommandInterface;

class IOpenDialogCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IOpenDialogCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		DWORD dwOptions = (DWORD)pCodeContext->GetPartInt(1);
		std::wstring wszResult = FileDialog::OpenDialog(dwOptions);

		IResultCommandInterface<dnyString>::SetResult(wszResult);

		return true;
	}
} g_oOpenDialogCommandInterface;

class ISaveDialogCommandInterface : public IResultCommandInterface<dnyString> {
public:
	ISaveDialogCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		DWORD dwOptions = (DWORD)pCodeContext->GetPartInt(1);
		std::wstring wszDefName = pCodeContext->GetPartString(2);
		std::vector<std::wstring> vFileSpec = pCodeContext->GetPartArray(3);

		std::wstring wszResult = FileDialog::SaveDialog(dwOptions, wszDefName, vFileSpec);

		IResultCommandInterface<dnyString>::SetResult(wszResult);

		return true;
	}
} g_oSaveDialogCommandInterface;

class IMsgBoxCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	IMsgBoxCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		dnyString wszText = pCodeContext->GetPartString(1);
		dnyString wszTitle = pCodeContext->GetPartString(2);
		UINT uType = (UINT)pCodeContext->GetPartInt(3);
		HWND hWnd = (HWND)pCodeContext->GetPartInt(4);

		int result = Message::MsgBox(wszText, wszTitle, uType, hWnd);

		IResultCommandInterface<dnyInteger>::SetResult(result);

		return true;
	}
} g_oMsgBoxCommandInterface;

class IMsgBeepVoidCommandInterface : public IVoidCommandInterface {
public:
	IMsgBeepVoidCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		UINT uType = pCodeContext->GetPartInt(1);

		Message::MsgBeep(uType);

		return true;
	}
} g_oMsgBeepResultCommand;

class ISndBeepVoidCommandInterface : public IVoidCommandInterface {
public:
	ISndBeepVoidCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		DWORD dwFrequency = pCodeContext->GetPartInt(1);
		DWORD dwDuration = pCodeContext->GetPartInt(2);

		Message::SndBeep(dwFrequency, dwDuration);

		return true;
	}
} g_oSndBeepResultCommand;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Dialog",
	L"1.0",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Dialog provider plugin"
};

bool dnyAS_PluginLoad(dnyVersionInfo version, IShellPluginAPI* pInterfaceData, plugininfo_s* pPluginInfos)
{
	//Called when plugin gets loaded

	if ((!pInterfaceData) || (!pPluginInfos))
		return false;

	//Check version
	if (version != DNY_AS_PRODUCT_VERSION_W) {
		return false;
	}

	//Store interface pointer
	g_pShellPluginAPI = pInterfaceData;

	//Store plugin infos
	memcpy(pPluginInfos, &g_sPluginInfos, sizeof(plugininfo_s));

	//Register constants
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_OK int <= 0;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_OKCANCEL int <= 1;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ABORTRETRYIGNORE int <= 2;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_YESNOCANCEL int <= 3;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_YESNO int <= 4;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_RETRYCANCEL int <= 5;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_CANCELTRYCONTINUE int <= 6;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONHAND int <= 16;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONQUESTION int <= 32;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONEXCLAMATION int <= 48;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONASTERISK int <= 64;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_USERICON int <= 128;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONWARNING int <= 48;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONERROR int <= 16;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONINFORMATION int <= 64;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONSTOP int <= 16;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_DEFBUTTON1 int <= 0;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_DEFBUTTON2 int <= 256;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_DEFBUTTON3 int <= 512;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_DEFBUTTON4 int <= 768;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_APPLMODAL int <= 0;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_SYSTEMMODAL int <= 4096;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_TASKMODAL int <= 8192;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_HELP int <= 16384;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_NOFOCUS int <= 32768;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_SETFOREGROUND int <= 65536;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_DEFAULT_DESKTOP_ONLY int <= 131072;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_TOPMOST int <= 262144;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_RIGHT int <= 524288;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_RTLREADING int <= 1048576;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDOK int <= 1;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDCANCEL int <= 2;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDABORT int <= 3;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDRETRY int <= 4;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDIGNORE int <= 5;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDYES int <= 6;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDNO int <= 7;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDCLOSE int <= 8;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDHELP int <= 9;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDTRYAGAIN int <= 10;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDCONTINUE int <= 11;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_OVERWRITEPROMPT int <= 2;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_STRICTFILETYPES int <= 4;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_NOCHANGEDIR int <= 8;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_PICKFOLDERS int <= 32;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_FORCEFILESYSTEM int <= 64;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_ALLNONSTORAGEITEMS int <= 128;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_NOVALIDATE int <= 256;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_ALLOWMULTISELECT int <= 512;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_PATHMUSTEXIST int <= 2048;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_FILEMUSTEXIST int <= 4096;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_CREATEPROMPT int <= 8192;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_SHAREAWARE int <= 16384;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_NOREADONLYRETURN int <= 32768;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_NOTESTFILECREATE int <= 65536;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_HIDEMRUPLACES int <= 131072;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_HIDEPINNEDPLACES int <= 262144;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_NODEREFERENCELINKS int <= 1048576;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_OKBUTTONNEEDSINTERACTION int <= 2097152;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_DONTADDTORECENT int <= 33554432;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_FORCESHOWHIDDEN int <= 268435456;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_DEFAULTNOMINIMODE int <= 536870912;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_FORCEPREVIEWPANEON int <= 1073741824;");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_SUPPORTSTREAMABLEITEMS int <= 2147483648;");

	//Register command
	g_pShellPluginAPI->Cmd_RegisterCommand(L"input", &g_oInputCliResultCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"inputbox", &g_oInputBoxCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"opendialog", &g_oOpenDialogCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"savedialog", &g_oSaveDialogCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"msgbox", &g_oMsgBoxCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"msgbeep", &g_oMsgBeepResultCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"sndbeep", &g_oSndBeepResultCommand, CT_VOID);

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	g_pShellPluginAPI->Cmd_UnregisterCommand(L"input");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"inputbox");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"opendialog");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"savedialog");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"msgbox");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"msgbeep");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"sndbeep");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}