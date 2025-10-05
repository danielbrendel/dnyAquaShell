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
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_OK int <= " + std::to_wstring(MB_OK) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_OKCANCEL int <= " + std::to_wstring(MB_OKCANCEL) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ABORTRETRYIGNORE int <= " + std::to_wstring(MB_ABORTRETRYIGNORE) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_YESNOCANCEL int <= " + std::to_wstring(MB_YESNOCANCEL) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_YESNO int <= " + std::to_wstring(MB_YESNO) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_RETRYCANCEL int <= " + std::to_wstring(MB_RETRYCANCEL) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_CANCELTRYCONTINUE int <= " + std::to_wstring(MB_CANCELTRYCONTINUE) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONHAND int <= " + std::to_wstring(MB_ICONHAND) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONQUESTION int <= " + std::to_wstring(MB_ICONQUESTION) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONEXCLAMATION int <= " + std::to_wstring(MB_ICONEXCLAMATION) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONASTERISK int <= " + std::to_wstring(MB_ICONASTERISK) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_USERICON int <= " + std::to_wstring(MB_USERICON) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONWARNING int <= " + std::to_wstring(MB_ICONWARNING) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONERROR int <= " + std::to_wstring(MB_ICONERROR) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONINFORMATION int <= " + std::to_wstring(MB_ICONINFORMATION) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_ICONSTOP int <= " + std::to_wstring(MB_ICONSTOP) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_DEFBUTTON1 int <= " + std::to_wstring(MB_DEFBUTTON1) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_DEFBUTTON2 int <= " + std::to_wstring(MB_DEFBUTTON2) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_DEFBUTTON3 int <= " + std::to_wstring(MB_DEFBUTTON3) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_DEFBUTTON4 int <= " + std::to_wstring(MB_DEFBUTTON4) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_APPLMODAL int <= " + std::to_wstring(MB_APPLMODAL) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_SYSTEMMODAL int <= " + std::to_wstring(MB_SYSTEMMODAL) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_TASKMODAL int <= " + std::to_wstring(MB_TASKMODAL) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_HELP int <= " + std::to_wstring(MB_HELP) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_NOFOCUS int <= " + std::to_wstring(MB_NOFOCUS) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_SETFOREGROUND int <= " + std::to_wstring(MB_SETFOREGROUND) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_DEFAULT_DESKTOP_ONLY int <= " + std::to_wstring(MB_DEFAULT_DESKTOP_ONLY) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_TOPMOST int <= " + std::to_wstring(MB_TOPMOST) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_RIGHT int <= " + std::to_wstring(MB_RIGHT) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const MB_RTLREADING int <= " + std::to_wstring(MB_RTLREADING) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDOK int <= " + std::to_wstring(IDOK) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDCANCEL int <= " + std::to_wstring(IDCANCEL) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDABORT int <= " + std::to_wstring(IDABORT) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDRETRY int <= " + std::to_wstring(IDRETRY) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDIGNORE int <= " + std::to_wstring(IDIGNORE) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDYES int <= " + std::to_wstring(IDYES) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDNO int <= " + std::to_wstring(IDNO) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDCLOSE int <= " + std::to_wstring(IDCLOSE) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDHELP int <= " + std::to_wstring(IDHELP) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDTRYAGAIN int <= " + std::to_wstring(IDTRYAGAIN) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const IDCONTINUE int <= " + std::to_wstring(IDCONTINUE) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_OVERWRITEPROMPT int <= " + std::to_wstring(FOS_OVERWRITEPROMPT) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_STRICTFILETYPES int <= " + std::to_wstring(FOS_STRICTFILETYPES) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_NOCHANGEDIR int <= " + std::to_wstring(FOS_NOCHANGEDIR) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_PICKFOLDERS int <= " + std::to_wstring(FOS_PICKFOLDERS) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_FORCEFILESYSTEM int <= " + std::to_wstring(FOS_FORCEFILESYSTEM) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_ALLNONSTORAGEITEMS int <= " + std::to_wstring(FOS_ALLNONSTORAGEITEMS) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_NOVALIDATE int <= " + std::to_wstring(FOS_NOVALIDATE) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_ALLOWMULTISELECT int <= " + std::to_wstring(FOS_ALLOWMULTISELECT) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_PATHMUSTEXIST int <= " + std::to_wstring(FOS_PATHMUSTEXIST) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_FILEMUSTEXIST int <= " + std::to_wstring(FOS_FILEMUSTEXIST) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_CREATEPROMPT int <= " + std::to_wstring(FOS_CREATEPROMPT) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_SHAREAWARE int <= " + std::to_wstring(FOS_SHAREAWARE) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_NOREADONLYRETURN int <= " + std::to_wstring(FOS_NOREADONLYRETURN) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_NOTESTFILECREATE int <= " + std::to_wstring(FOS_NOTESTFILECREATE) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_HIDEMRUPLACES int <= " + std::to_wstring(FOS_HIDEMRUPLACES) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_HIDEPINNEDPLACES int <= " + std::to_wstring(FOS_HIDEPINNEDPLACES) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_NODEREFERENCELINKS int <= " + std::to_wstring(FOS_NODEREFERENCELINKS) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_OKBUTTONNEEDSINTERACTION int <= " + std::to_wstring(FOS_OKBUTTONNEEDSINTERACTION) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_DONTADDTORECENT int <= " + std::to_wstring(FOS_DONTADDTORECENT) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_FORCESHOWHIDDEN int <= " + std::to_wstring(FOS_FORCESHOWHIDDEN) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_DEFAULTNOMINIMODE int <= " + std::to_wstring(FOS_DEFAULTNOMINIMODE) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_FORCEPREVIEWPANEON int <= " + std::to_wstring(FOS_FORCEPREVIEWPANEON) + L";");
	g_pShellPluginAPI->Scr_ExecuteCode(L"const FOS_SUPPORTSTREAMABLEITEMS int <= " + std::to_wstring(FOS_SUPPORTSTREAMABLEITEMS) + L";");

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