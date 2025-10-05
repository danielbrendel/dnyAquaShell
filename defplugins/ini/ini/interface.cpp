#include "dnyas_sdk.h"

IShellPluginAPI* g_pShellPluginAPI;

class IIniReadCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IIniReadCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		std::wstring wszFile = pContext->GetPartString(1);
		std::wstring wszSection = pContext->GetPartString(2);
		std::wstring wszKey = pContext->GetPartString(3);
		
		wchar_t wszOutput[2048] = { 0 };
		DWORD dwResult = 0;

		SetLastError(ERROR_SUCCESS);

		dwResult = GetPrivateProfileString(wszSection.c_str(), wszKey.c_str(), L"", wszOutput, sizeof(wszOutput), wszFile.c_str());

		this->SetResult(std::wstring(wszOutput));
		
		return GetLastError() == ERROR_SUCCESS;
	}

} g_oIniReadCommandInterface;

class IIniWriteCommandInterface : public IVoidCommandInterface {
public:
	IIniWriteCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		std::wstring wszFile = pContext->GetPartString(1);
		std::wstring wszSection = pContext->GetPartString(2);
		std::wstring wszKey = pContext->GetPartString(3);
		std::wstring wszValue = pContext->GetPartString(4);
		
		DWORD dwResult = WritePrivateProfileString(wszSection.c_str(), wszKey.c_str(), wszValue.c_str(), wszFile.c_str());
		
		return dwResult != 0;
	}

} g_oIniWriteCommandInterface;

class IIniDeleteCommandInterface : public IVoidCommandInterface {
public:
	IIniDeleteCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		std::wstring wszFile = pContext->GetPartString(1);
		std::wstring wszSection = pContext->GetPartString(2);
		std::wstring wszKey = pContext->GetPartString(3);

		DWORD dwResult = WritePrivateProfileString(wszSection.c_str(), wszKey.c_str(), NULL, wszFile.c_str());

		return dwResult != 0;
	}

} g_oIniDeleteCommandInterface;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Ini",
	L"1.0",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Access INI configuration files"
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

	//Register commands
	g_pShellPluginAPI->Cmd_RegisterCommand(L"ini_read", &g_oIniReadCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"ini_write", &g_oIniWriteCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"ini_delete", &g_oIniDeleteCommandInterface, CT_VOID);

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	//Unregister commands
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"ini_read");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"ini_write");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"ini_delete");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}