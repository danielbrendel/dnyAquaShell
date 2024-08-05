#include "dnyas_sdk.h"
#include "inputbox.h"

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

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Input",
	L"1.0",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Input provider plugin"
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

	//Register command
	g_pShellPluginAPI->Cmd_RegisterCommand(L"input", &g_oInputCliResultCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"inputbox", &g_oInputBoxCommandInterface, CT_STRING);

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	g_pShellPluginAPI->Cmd_UnregisterCommand(L"input");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"inputbox");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}