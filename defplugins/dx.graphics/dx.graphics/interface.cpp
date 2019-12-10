#include "renderer.h"

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"DX.Graphics",
	L"0.1",
	L"Daniel Brendel",
	L"dbrendel1988<at>yahoo<dot>com",
	L"DX Graphics Provider"
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

	//Initialize DX renderer
	return DxRenderer::Initialize(pInterfaceData);
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	DxRenderer::Release();
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}