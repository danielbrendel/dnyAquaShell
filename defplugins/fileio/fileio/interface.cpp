#include "fileio.h"

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"FileIO",
	L"1.0",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"File I/O provider"
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

	//Initialize FileIO component
	if (!FileIO::Initialize(pInterfaceData))
		return false;

	//Store plugin infos
	memcpy(pPluginInfos, &g_sPluginInfos, sizeof(plugininfo_s));

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	//Cleanup component resources
	FileIO::Free();
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}