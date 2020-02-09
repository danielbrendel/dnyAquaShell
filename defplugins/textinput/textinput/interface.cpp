#include "dnyas_sdk.h"

IShellPluginAPI* g_pShellPluginAPI;

class IInputVoidCommandInterface : public IVoidCommandInterface {
public:
	IInputVoidCommandInterface() {}

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
} g_oInputResultCommand;

class ISetInputVoidCommandInterface : public IVoidCommandInterface {
public:
	ISetInputVoidCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		ICVar<dnyString>* pCvar = (ICVar<dnyString>*)g_pShellPluginAPI->Cv_FindCVar(pCodeContext->GetPartString(1));
		if (!pCvar) {
			pCvar = (ICVar<dnyString>*)g_pShellPluginAPI->Cv_RegisterCVar(pCodeContext->GetPartString(1), CT_STRING);
			if (!pCvar)
				return false;
		}

		std::wstring wszQueryText = pCodeContext->GetPartString(2);
		if (wszQueryText.length()) {
			std::wcout << wszQueryText;
		}

		std::wstring wszInput;
		std::getline(std::wcin, wszInput);
		
		pCvar->SetValue(wszInput);

		return true;
	}
} g_oSetInputResultCommand;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"TextInput",
	L"0.1",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Text input query plugin"
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

	//Register example commands
	g_pShellPluginAPI->Cmd_RegisterCommand(L"input", &g_oInputResultCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"setinput", &g_oSetInputResultCommand, CT_VOID);

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	g_pShellPluginAPI->Cmd_UnregisterCommand(L"input");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"setinput");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}