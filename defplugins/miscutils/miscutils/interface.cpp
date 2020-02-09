#include "utils.h"
#include "clipboard.h"

IShellPluginAPI* g_pShellPluginAPI;

class IAddTimerCommandInterface : public IVoidCommandInterface {
public:
	IAddTimerCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return Utils::AddTimer(pContext->GetPartString(1), pContext->GetPartInt(2));
	}

} g_oAddTimerCommandInterface;

class ITimerExistsCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ITimerExistsCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyInteger>::SetResult((dnyInteger)Utils::TimerExists(pContext->GetPartString(1)));

		return true;
	}
} g_oTimerExistsCommandInterface;

class IProcessTimersCommandInterface : public IVoidCommandInterface {
public:
	IProcessTimersCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		return Utils::Process();
	}

} g_oProcessTimersCommandInterface;

class ITextFilePrinterCommandInterface : public IVoidCommandInterface {
public:
	ITextFilePrinterCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		Utils::CTextFilePrinter oFilePrinter(pContext->GetPartString(1));

		return oFilePrinter.GetLastResult();
	}

} g_oTextFilePrinterCommandInterface;

class IRandomCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	IRandomCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyInteger>::SetResult(Utils::Random(pContext->GetPartInt(1), pContext->GetPartInt(2)));

		return true;
	}
} g_oRandomCommandInterface;

class ISleepCommandInterface : public IVoidCommandInterface {
public:
	ISleepCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		Utils::Sleep(pContext->GetPartInt(1));

		return true;
	}

} g_oSleepCommandInterface;

class ISetClipboardStringCommandInterface : public IVoidCommandInterface {
public:
	ISetClipboardStringCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return Clipboard::SetClipboardString(pContext->GetPartString(1));
	}

} g_oSetClipboardStringCommandInterface;

class IGetClipboardStringCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IGetClipboardStringCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyString>::SetResult(Clipboard::GetClipboardString());

		return true;
	}
} g_oGetClipboardStringCommandInterface;

class IClearClipboardCommandInterface : public IVoidCommandInterface {
public:
	IClearClipboardCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		return Clipboard::ClearClipboardData();
	}

} g_oClearClipboardCommandInterface;

class IGetTickCountCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	IGetTickCountCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		IResultCommandInterface<dnyInteger>::SetResult(GetTickCount64());

		return true;
	}

} g_oGetTickCountCommandInterface;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Miscutils",
	L"0.1",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Miscellaneous Utilities"
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

	if (!Utils::Initialize(pInterfaceData))
		return false;

	//Register example commands
	g_pShellPluginAPI->Cmd_RegisterCommand(L"addtimer", &g_oAddTimerCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"timerexists", &g_oTimerExistsCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"calctimers", &g_oProcessTimersCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"textview", &g_oTextFilePrinterCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"random", &g_oRandomCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"sleep", &g_oSleepCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"clpb_setstring", &g_oSetClipboardStringCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"clpb_getstring", &g_oGetClipboardStringCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"clpb_clear", &g_oClearClipboardCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"gettickcount", &g_oGetTickCountCommandInterface, CT_INT);

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	g_pShellPluginAPI->Cmd_UnregisterCommand(L"addtimer");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"timerexists");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"calctimers");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"textview");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"random");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"sleep");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"clpb_setstring");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"clpb_getstring");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"clpb_clear");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"gettickcount");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}