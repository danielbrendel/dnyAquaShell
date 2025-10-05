#include "dnyas_sdk.h"
#include <sstream>

IShellPluginAPI* g_pShellPluginAPI;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Env",
	L"1.0",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Windows environment variables provider"
};

std::vector<std::wstring> split(const std::wstring& wszString, const wchar_t wcSplitChar)
{
	std::vector<std::wstring> vResult;
	std::wstringstream wStringStream(wszString);
	std::wstring wszTemp;

	while (std::getline(wStringStream, wszTemp, wcSplitChar)) {
		vResult.push_back(wszTemp);
	}

	return vResult;
}

class IGetEnvVarCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IGetEnvVarCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		std::wstring wszName = pContext->GetPartString(1);

		wchar_t wszValue[32767] = { 0 };
		DWORD dwSize = GetEnvironmentVariable(wszName.c_str(), wszValue, sizeof(wszValue));

		if ((dwSize > 0) && (dwSize < sizeof(wszValue))) {
			this->SetResult(std::wstring(wszValue));
		}

		return ((dwSize) && (GetLastError() != ERROR_ENVVAR_NOT_FOUND));
	}

} g_oGetEnvVarCommandInterface;

class ISetEnvVarCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	ISetEnvVarCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		std::wstring wszName = pContext->GetPartString(1);
		std::wstring wszValue = pContext->GetPartString(2);

		BOOL bResult = SetEnvironmentVariable(wszName.c_str(), wszValue.c_str());

		this->SetResult(bResult == TRUE);

		return true;
	}

} g_oSetEnvVarCommandInterface;

bool RegisterEnvironmentVariables(void)
{
	//Register environment variables

	LPWCH lpwEnvStrings = GetEnvironmentStrings();
	if (!lpwEnvStrings)
		return false;

	LPWSTR pEnvString = (LPWSTR)lpwEnvStrings;
	while (*pEnvString) {
		std::vector<std::wstring> vList = split(pEnvString, '=');
		if (vList.size() == 2) {
			ICVar<dnyString>* pCvar = (ICVar<dnyString>*)g_pShellPluginAPI->Cv_RegisterCVar(vList[0], CT_STRING, true);
			if (!pCvar)
				return false;

			pCvar->SetValue(vList[1]);
		}

		pEnvString += wcslen(pEnvString) + 1;
	}

	FreeEnvironmentStrings(lpwEnvStrings);
}

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
	g_pShellPluginAPI->Cmd_RegisterCommand(L"env_getvariable", &g_oGetEnvVarCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"env_setvariable", &g_oSetEnvVarCommandInterface, CT_BOOL);

	//Register environment variables
	return RegisterEnvironmentVariables();
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	//Unregister commands
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"env_getvariable");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"env_setvariable");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}