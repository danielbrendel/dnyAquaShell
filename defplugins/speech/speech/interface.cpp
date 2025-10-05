#include "dnyas_sdk.h"
#include "speechint.h"

IShellPluginAPI* g_pShellPluginAPI;

class ISetVoiceCommandInterface : public IVoidCommandInterface {
public:
	ISetVoiceCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return SpeechInt::SetVoice(pContext->GetPartString(1));
	}

} g_oSetVoiceCommandInterface;

class ISetPitchCommandInterface : public IVoidCommandInterface {
public:
	ISetPitchCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		SpeechInt::SetPitch((USHORT)pContext->GetPartInt(1));

		return true;
	}

} g_oSetPitchCommandInterface;

class ISetVolumeCommandInterface : public IVoidCommandInterface {
public:
	ISetVolumeCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		SpeechInt::SetVolume((USHORT)pContext->GetPartInt(1));

		return true;
	}

} g_oSetVolumeCommandInterface;

class ISetSpeedCommandInterface : public IVoidCommandInterface {
public:
	ISetSpeedCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		SpeechInt::SetSpeed((USHORT)pContext->GetPartInt(1));

		return true;
	}

} g_oSetSpeedCommandInterface;

class IGetVoiceCommandInterface : public IResultCommandInterface<dnyString> {
public:
	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		IResultCommandInterface<dnyString>::SetResult(SpeechInt::GetVoice());

		return true;
	}
} g_oGetVoiceCommandInterface;

class IGetPitchCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		IResultCommandInterface<dnyInteger>::SetResult(SpeechInt::GetPitch());

		return true;
	}
} g_oGetPitchCommandInterface;

class IGetVolumeCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		IResultCommandInterface<dnyInteger>::SetResult(SpeechInt::GetVolume());

		return true;
	}
} g_oGetVolumeCommandInterface;

class IGetSpeedCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		IResultCommandInterface<dnyInteger>::SetResult(SpeechInt::GetSpeed());

		return true;
	}
} g_oGetSpeedCommandInterface;

class ISpeakCommandInterface : public IVoidCommandInterface {
public:
	ISpeakCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		SpeechInt::Speak(pContext->GetPartString(1));

		return true;
	}

} g_oSpeakCommandInterface;

class ISpeakAsyncCommandInterface : public IVoidCommandInterface {
public:
	ISpeakAsyncCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		SpeechInt::SpeakAsync(pContext->GetPartString(1));

		return true;
	}

} g_oSpeakAsyncCommandInterface;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Speech",
	L"1.0",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Microsoft SAPI Interface"
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

	//Initialize speech
	if (!SpeechInt::Initialize())
		return false;

	//Register commands
	g_pShellPluginAPI->Cmd_RegisterCommand(L"spk_setvoice", &g_oSetVoiceCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"spk_setpitch", &g_oSetPitchCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"spk_setvolume", &g_oSetVolumeCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"spk_setspeed", &g_oSetSpeedCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"spk_getvoice", &g_oGetVoiceCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"spk_getpitch", &g_oGetPitchCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"spk_getvolume", &g_oGetVolumeCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"spk_getspeed", &g_oGetSpeedCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"spk_speak", &g_oSpeakCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"spk_speakasync", &g_oSpeakAsyncCommandInterface, CT_VOID);

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	//Unregister commands
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"spk_setvoice");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"spk_setpitch");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"spk_setvolume");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"spk_setspeed");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"spk_getvoice");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"spk_getpitch");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"spk_getvolume");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"spk_getspeed");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"spk_speak");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"spk_speakasync");

	//Free resources
	SpeechInt::Free();
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}