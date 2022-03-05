#include "dnyas_sdk.h"
#include <sstream>

IShellPluginAPI* g_pShellPluginAPI;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"EnvVars",
	L"1.0",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Windows environment variable provider"
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

	//Register environment variables
	return RegisterEnvironmentVariables();
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}