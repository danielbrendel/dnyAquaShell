#include "automation.h"
#include "clipboard.h"
#include "timer.h"

IShellPluginAPI* g_pShellPluginAPI;

class IFindWindowCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	IFindWindowCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		HWND hWnd = Automation::FindWindow(pContext->GetPartString(1), pContext->GetPartString(2));

		IResultCommandInterface<dnyInteger>::SetResult((dnyInteger)hWnd);
		
		return true;
	}
} g_oFindWindowCommandInterface;

class IIsWindowCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IIsWindowCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::IsWindow((HWND)pContext->GetPartInt(1)));

		return true;
	}
} g_oIsWindowCommandInterface;

class IGetForegroundWindowCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	IGetForegroundWindowCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyInteger>::SetResult((dnyInteger)Automation::GetForegroundWindow());

		return true;
	}
} g_oGetForegroundWindowCommandInterface;

class ISetForegroundWindowCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	ISetForegroundWindowCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::SetForegroundWindow((HWND)pContext->GetPartInt(1)));

		return true;
	}
} g_oSetForegroundWindowCommandInterface;

class ISetWindowPosCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	ISetWindowPosCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::SetWindowPos((HWND)pContext->GetPartInt(1), (int)pContext->GetPartInt(2), (int)pContext->GetPartInt(3)));

		return true;
	}
} g_oSetWindowPosCommandInterface;

class IGetWindowPosCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IGetWindowPosCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		ICVar<dnyInteger>* pVarX = (ICVar<dnyInteger>*)g_pShellPluginAPI->Cv_FindCVar(pContext->GetPartString(2));
		ICVar<dnyInteger>* pVarY = (ICVar<dnyInteger>*)g_pShellPluginAPI->Cv_FindCVar(pContext->GetPartString(3));

		int x, y;

		bool bResult = Automation::GetWindowPos((HWND)pContext->GetPartInt(1), x, y);

		pVarX->SetValue(x);
		pVarY->SetValue(y);

		IResultCommandInterface<dnyBoolean>::SetResult(bResult);

		return true;
	}
} g_oGetWindowPosCommandInterface;

class IGetWindowSizeCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IGetWindowSizeCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		ICVar<dnyInteger>* pVarW = (ICVar<dnyInteger>*)g_pShellPluginAPI->Cv_FindCVar(pContext->GetPartString(2));
		ICVar<dnyInteger>* pVarH = (ICVar<dnyInteger>*)g_pShellPluginAPI->Cv_FindCVar(pContext->GetPartString(3));

		int w, h;

		bool bResult = Automation::GetWindowSize((HWND)pContext->GetPartInt(1), w, h);

		pVarW->SetValue(w);
		pVarH->SetValue(h);

		IResultCommandInterface<dnyBoolean>::SetResult(bResult);

		return true;
	}
} g_oGetWindowSizeCommandInterface;

class ISetWindowTextCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	ISetWindowTextCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::SetWindowText((HWND)pContext->GetPartInt(1), pContext->GetPartString(2)));

		return true;
	}
} g_oSetWindowTextCommandInterface;

class IGetWindowTextCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IGetWindowTextCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyString>::SetResult(Automation::GetWindowText((HWND)pContext->GetPartInt(1)));

		return true;
	}
} g_oGetWindowTextCommandInterface;

class IGetClassNameCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IGetClassNameCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyString>::SetResult(Automation::GetClassName((HWND)pContext->GetPartInt(1)));

		return true;
	}
} g_oGetClassNameCommandInterface;

class IShowWindowCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IShowWindowCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::ShowWindow((HWND)pContext->GetPartInt(1), (int)pContext->GetPartInt(2)));

		return true;
	}
} g_oShowWindowCommandInterface;

class IGetCursorPosCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IGetCursorPosCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		ICVar<dnyInteger>* pVarX = (ICVar<dnyInteger>*)g_pShellPluginAPI->Cv_FindCVar(pContext->GetPartString(2));
		ICVar<dnyInteger>* pVarY = (ICVar<dnyInteger>*)g_pShellPluginAPI->Cv_FindCVar(pContext->GetPartString(3));

		int x, y;

		bool bResult = Automation::GetCursorPos(x, y);

		pVarX->SetValue(x);
		pVarY->SetValue(y);

		IResultCommandInterface<dnyBoolean>::SetResult(bResult);

		return true;
	}
} g_oGetCursorPosCommandInterface;

class ISetCursorPosCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	ISetCursorPosCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::SetCursorPos((int)pContext->GetPartInt(1), (int)pContext->GetPartInt(2)));

		return true;
	}
} g_oSetCursorPosCommandInterface;

class IRunCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IRunCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::Run(pContext->GetPartString(1), pContext->GetPartString(2), pContext->GetPartString(3)));

		return true;
	}
} g_oRunCommandInterface;

class IIsKeyDownCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IIsKeyDownCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::IsKeyDown((int)pContext->GetPartInt(1)));

		return true;
	}
} g_oIsKeyDownCommandInterface;

class IIsKeyUpCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IIsKeyUpCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::IsKeyUp((int)pContext->GetPartInt(1)));

		return true;
	}
} g_oIsKeyUpCommandInterface;

std::wstring wszScriptRoutine;
bool Event_EnumWindows(HWND hWnd, const std::wstring& wszTitleText)
{
	/*g_pShellPluginAPI->Fnc_BeginFunctionCall(wszScriptRoutine, CT_BOOL);
	g_pShellPluginAPI->Fnc_PushFunctionParam((dnyInteger)hWnd);
	g_pShellPluginAPI->Fnc_PushFunctionParam(wszTitleText);
	g_pShellPluginAPI->Fnc_ExecuteFunction();
	bool bResult = g_pShellPluginAPI->Fnc_QueryFunctionResultAsBoolean();
	g_pShellPluginAPI->Fnc_EndFunctionCall();*/

	ICVar<dnyBoolean>* pCvar = (ICVar<dnyBoolean>*)g_pShellPluginAPI->Cv_RegisterCVar(L"__automation__Event_EnumWindows", CT_BOOL);
	
	g_pShellPluginAPI->Scr_ExecuteCode(L"call " + wszScriptRoutine + L"(" + std::to_wstring((dnyInteger)hWnd) + L", \"" + wszTitleText + L"\") => __automation__Event_EnumWindows;");

	bool bResult = pCvar->GetValue();
	
	g_pShellPluginAPI->Cv_FreeCVar(L"__automation__Event_EnumWindows");

	return bResult;
}
std::wstring wszScriptChildRoutine;
bool Event_EnumChildWindows(HWND hWnd, const std::wstring& wszTitleText)
{
	/*g_pShellPluginAPI->Fnc_BeginFunctionCall(wszScriptRoutine, CT_BOOL);
	g_pShellPluginAPI->Fnc_PushFunctionParam((dnyInteger)hWnd);
	g_pShellPluginAPI->Fnc_PushFunctionParam(wszTitleText);
	g_pShellPluginAPI->Fnc_ExecuteFunction();
	bool bResult = g_pShellPluginAPI->Fnc_QueryFunctionResultAsBoolean();
	g_pShellPluginAPI->Fnc_EndFunctionCall();*/

	ICVar<dnyBoolean>* pCvar = (ICVar<dnyBoolean>*)g_pShellPluginAPI->Cv_RegisterCVar(L"__automation__Event_EnumChildWindows", CT_BOOL);

	g_pShellPluginAPI->Scr_ExecuteCode(L"call " + wszScriptChildRoutine + L"(" + std::to_wstring((dnyInteger)hWnd) + L", \"" + wszTitleText + L"\") => __automation__Event_EnumChildWindows;");

	bool bResult = pCvar->GetValue();

	g_pShellPluginAPI->Cv_FreeCVar(L"__automation__Event_EnumChildWindows");

	return bResult;
}
class IEnumWindowsCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IEnumWindowsCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		wszScriptRoutine = pContext->GetPartString(1);
		IResultCommandInterface<dnyBoolean>::SetResult(Automation::EnumWindows(&Event_EnumWindows));

		return true;
	}
} g_oEnumWindowsCommandInterface;
class IEnumChildWindowsCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IEnumChildWindowsCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		wszScriptChildRoutine = pContext->GetPartString(2);
		IResultCommandInterface<dnyBoolean>::SetResult(Automation::EnumChildWindows((HWND)pContext->GetPartInt(1), &Event_EnumChildWindows));

		return true;
	}
} g_oEnumChildWindowsCommandInterface;

class ISendMessageCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ISendMessageCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyInteger>::SetResult(Automation::SendMessage((HWND)pContext->GetPartInt(1), (UINT)pContext->GetPartInt(2), (WPARAM)pContext->GetPartInt(3), (LPARAM)pContext->GetPartInt(4)));
		
		return true;
	}
} g_oSendMessageCommandInterface;

class IPostMessageCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IPostMessageCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::PostMessage((HWND)pContext->GetPartInt(1), (UINT)pContext->GetPartInt(2), (WPARAM)pContext->GetPartInt(3), (LPARAM)pContext->GetPartInt(4)));

		return true;
	}
} g_oPostMessageCommandInterface;

class ISendKeyboardInputCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	ISendKeyboardInputCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::SendKeyboardInput(pContext->GetPartString(1), pContext->GetPartBool(2), pContext->GetPartBool(3), pContext->GetPartBool(4)));
		
		return true;
	}
} g_oSendKeyboardInputCommandInterface;

class ISendMouseInputCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	ISendMouseInputCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::SendMouseInput(pContext->GetPartString(1), pContext->GetPartBool(2), pContext->GetPartBool(3), pContext->GetPartBool(4)));

		return true;
	}
} g_oSendMouseInputCommandInterface;

void Event_KeyboardHook(const std::wstring& wszString, bool bIsDown, bool bWithShift, bool bWithCtrl, bool bWithAlt)
{
	/*g_pShellPluginAPI->Fnc_BeginFunctionCall(wszString, CT_VOID);
	g_pShellPluginAPI->Fnc_PushFunctionParam(bIsDown);
	g_pShellPluginAPI->Fnc_PushFunctionParam(bWithShift);
	g_pShellPluginAPI->Fnc_PushFunctionParam(bWithCtrl);
	g_pShellPluginAPI->Fnc_PushFunctionParam(bWithAlt);
	g_pShellPluginAPI->Fnc_ExecuteFunction();
	g_pShellPluginAPI->Fnc_EndFunctionCall();*/
	g_pShellPluginAPI->Scr_ExecuteCode(L"call " + wszString + L"(" + ((bIsDown) ? L"1" : L"0") + L", " + ((bWithShift) ? L"1" : L"0") + L", " + ((bWithCtrl) ? L"1" : L"0") + L", " + ((bWithAlt) ? L"1" : L"0") + L") => void;");
}
void Event_MouseHook(const std::wstring& wszString, bool bWithShift, bool bWithCtrl, bool bWithAlt)
{
	/*g_pShellPluginAPI->Fnc_BeginFunctionCall(wszString, CT_VOID);
	g_pShellPluginAPI->Fnc_PushFunctionParam(bWithShift);
	g_pShellPluginAPI->Fnc_PushFunctionParam(bWithCtrl);
	g_pShellPluginAPI->Fnc_PushFunctionParam(bWithAlt);
	g_pShellPluginAPI->Fnc_ExecuteFunction();
	g_pShellPluginAPI->Fnc_EndFunctionCall();*/
	g_pShellPluginAPI->Scr_ExecuteCode(L"call " + wszString + L"(" + ((bWithShift) ? L"1" : L"0") + L", " + ((bWithCtrl) ? L"1" : L"0") + L", " + ((bWithAlt) ? L"1" : L"0") + L") => void;");
}
class IAddKeyboardHookCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IAddKeyboardHookCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::AddKeyboardHook(pContext->GetPartString(1), (int)pContext->GetPartInt(2), &Event_KeyboardHook));

		return true;
	}
} g_oAddKeyboardHookCommandInterface;
class IAddMouseHookCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IAddMouseHookCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(Automation::AddMouseHook(pContext->GetPartString(1), (int)pContext->GetPartInt(2), &Event_MouseHook));

		return true;
	}
} g_oAddMouseHookCommandInterface;
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
class IAddTimerCommandInterface : public IVoidCommandInterface {
public:
	IAddTimerCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return Timer::AddTimer(pContext->GetPartString(1), pContext->GetPartInt(2));
	}
} g_oAddTimerCommandInterface;
class ITimerExistsCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ITimerExistsCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyInteger>::SetResult((dnyInteger)Timer::TimerExists(pContext->GetPartString(1)));

		return true;
	}
} g_oTimerExistsCommandInterface;
class IProcessTimersCommandInterface : public IVoidCommandInterface {
public:
	IProcessTimersCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		return Timer::Process();
	}

} g_oProcessTimersCommandInterface;
class IProcessHooksCommandInterface : public IVoidCommandInterface {
public:
	IProcessHooksCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		Automation::MK_Process();

		return true;
	}
} g_oProcessHooksCommandInterface;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Auto",
	L"1.0",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Automation utility provider"
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

	//Set pointer
	Automation::Init(pInterfaceData);
	Timer::Init(pInterfaceData);

	//Register commands
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_findwindow", &g_oFindWindowCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_iswindow", &g_oIsWindowCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_getfgwindow", &g_oGetForegroundWindowCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_setfgwindow", &g_oSetForegroundWindowCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_setwndpos", &g_oSetWindowPosCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_getwndpos", &g_oGetWindowPosCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_getwndsize", &g_oGetWindowSizeCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_setwndtext", &g_oSetWindowTextCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_getwndtext", &g_oGetWindowTextCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_getclassname", &g_oGetClassNameCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_showwnd", &g_oShowWindowCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_getcursorpos", &g_oGetCursorPosCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_setcursorpos", &g_oSetCursorPosCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_run", &g_oRunCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_iskeydown", &g_oIsKeyDownCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_iskeyup", &g_oIsKeyUpCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_enumwindows", &g_oEnumWindowsCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_enumchildwindows", &g_oEnumChildWindowsCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_sendmessage", &g_oSendMessageCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_postmessage", &g_oPostMessageCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_sendkeystrokes", &g_oSendKeyboardInputCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_sendmousestrokes", &g_oSendMouseInputCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_addkeyevent", &g_oAddKeyboardHookCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_addmouseevent", &g_oAddMouseHookCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_clpbsetstring", &g_oSetClipboardStringCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_clpbgetstring", &g_oGetClipboardStringCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_clpbclear", &g_oClearClipboardCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_addtimer", &g_oAddTimerCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_timerexists", &g_oTimerExistsCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_procevents", &g_oProcessHooksCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"aut_calctimers", &g_oProcessTimersCommandInterface, CT_VOID);

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	Automation::Release();

	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_findwindow");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_iswindow");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_getfgwindow");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_setfgwindow");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_setwndpos");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_getwndpos");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_getwndsize");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_setwndtext");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_getwndtext");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_getclassname");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_showwnd");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_getcursorpos");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_setcursorpos");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_run");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_iskeydown");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_iskeyup");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_enumwindows");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_enumchildwindows");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_sendmessage");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_postmessage");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_sendkeystrokes");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_sendmousestrokes");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_addkeyevent");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_addmouseevent");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_clpbsetstring");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_clpbgetstring");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_clpbclear");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_addtimer");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_timerexists");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_procevents");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"aut_calctimers");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}