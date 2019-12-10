#include "array.h"

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Array",
	L"0.1",
	L"Daniel Brendel",
	L"dbrendel1988<at>yahoo<dot>com",
	L"Dynamic and static array provider"
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

	//Initialize Array component
	if (!Array::Initialize(pInterfaceData))
		return false;

	//Store plugin infos
	memcpy(pPluginInfos, &g_sPluginInfos, sizeof(plugininfo_s));

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	Array::Free();
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}