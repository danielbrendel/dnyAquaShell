#include "sound.h"

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"DX.Sound",
	L"0.1",
	L"Daniel Brendel",
	L"Daniel Brendel<at>gmail<dot>com",
	L"DX Sound Provider"
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

	//Store plugin infos
	memcpy(pPluginInfos, &g_sPluginInfos, sizeof(plugininfo_s));

	//Initialize sound
	return DxSound::Initialize(pInterfaceData);
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	DxSound::Release();
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}