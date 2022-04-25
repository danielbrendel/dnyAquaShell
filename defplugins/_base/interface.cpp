#include "dnyas_sdk.h"

IShellPluginAPI* g_pShellPluginAPI;

//Example void-command interface
class IExampleVoidCommandInterface : public IVoidCommandInterface {
public:
	IExampleVoidCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		std::wcout << pContext->GetPartString(1) << std::endl;

		return true;
	}

} g_oExampleVoidCommand;

//Example result-type-command interface
class IExampleResultCommandInterface : public IResultCommandInterface<dnyFloat> {
public:
	IExampleResultCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		IResultCommandInterface<dnyFloat>::SetResult(pContext->GetPartFloat(1) * 2);

		return true;
	}
} g_oExampleResultCommand;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Demo",
	L"1.0",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Demo Plugin"
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
	g_pShellPluginAPI->Cmd_RegisterCommand(L"mycommand", &g_oExampleVoidCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"mycommand2", &g_oExampleResultCommand, CT_FLOAT);

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	g_pShellPluginAPI->Cmd_UnregisterCommand(L"mycommand");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"mycommand2");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}