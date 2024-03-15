#include "dnyas_sdk.h"

IShellPluginAPI* g_pShellPluginAPI;

class IFormatDateTimeCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IFormatDateTimeCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		wchar_t wcsDate[MAX_PATH];

		std::wstring wszFormat = pContext->GetPartString(1);
		std::time_t t = std::time(nullptr);

		std::wcsftime(wcsDate, sizeof(wcsDate), wszFormat.c_str(), std::localtime(&t));

		IResultCommandInterface<dnyString>::SetResult(wcsDate);

		return true;
	}
} g_oFormatDateTimeCommandInterface;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Datetime",
	L"1.0",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Provides date and time commands"
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
	g_pShellPluginAPI->Cmd_RegisterCommand(L"fmtdatetime", &g_oFormatDateTimeCommandInterface, CT_STRING);

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	g_pShellPluginAPI->Cmd_UnregisterCommand(L"fmtdatetime");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}