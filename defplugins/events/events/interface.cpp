#include "dnyas_sdk.h"

IShellPluginAPI* g_pShellPluginAPI;

class CEventMgr { //Event manager
private:
	struct eventdata_s {
		std::wstring wszName;
		size_t uiArgCount;
		bool bMultipleHandlers;
		std::vector<std::wstring> vHandlers;
	};

	std::vector<eventdata_s> m_vEvents;

	//Get event data pointer of event
	eventdata_s* GetEventData(const std::wstring& wszName)
	{
		for (size_t i = 0; i < this->m_vEvents.size(); i++) {
			if (this->m_vEvents[i].wszName == wszName)
				return &this->m_vEvents[i];
		}

		return nullptr;
	}
public:
	CEventMgr() {}
	~CEventMgr() {}

	//Register event
	bool Register(const std::wstring& wszName, const size_t uiArgCount, bool bMultipleHandlers = true)
	{
		if (!wszName.length())
			return false;

		if (this->GetEventData(wszName))
			return false;

		eventdata_s sData;
		sData.wszName = wszName;
		sData.uiArgCount = uiArgCount;
		sData.bMultipleHandlers = bMultipleHandlers;
		this->m_vEvents.push_back(sData);

		return true;
	}

	//Add event handler
	bool Add(const std::wstring& wszName, const std::wstring& wszFunction)
	{
		if (!wszName.length())
			return false;

		eventdata_s* pData = this->GetEventData(wszName);
		if (!pData)
			return false;

		if ((!pData->bMultipleHandlers) && (pData->vHandlers.size()))
			return false;

		pData->vHandlers.push_back(wszFunction);

		return true;
	}

	//Raise event
	bool Raise(const std::wstring& wszName, const std::vector<std::wstring>& vArgs, const std::wstring& wszStorageVar)
	{
		if (!wszName.length())
			return false;

		eventdata_s* pData = this->GetEventData(wszName);
		if (!pData)
			return false;

		if (pData->uiArgCount != vArgs.size())
			return false;

		for (size_t i = 0; i < pData->vHandlers.size(); i++) {
			std::wstring wszCode = L"call " + pData->vHandlers[i] + L"(";
			for (size_t j = 0; j < vArgs.size(); j++) {
				wszCode += L"\"" + vArgs[j] + L"\"";
				if (j < vArgs.size() - 1) wszCode += L", ";
			}
			wszCode += L") => " + wszStorageVar + L";";
			
			g_pShellPluginAPI->Scr_ExecuteCode(wszCode);
		}
	}
} g_oEventMgr;

class IRegEventCommandInterface : public IVoidCommandInterface {
public:
	IRegEventCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return g_oEventMgr.Register(pContext->GetPartString(1), (size_t)pContext->GetPartInt(2), pContext->GetPartBool(3));
	}

} g_oRegEventCommandInterface;

class IAddEventHandlerCommandInterface : public IVoidCommandInterface {
public:
	IAddEventHandlerCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return g_oEventMgr.Add(pContext->GetPartString(1), pContext->GetPartString(2));
	}

} g_oAddEventHandlerCommandInterface;

class IRaiseEventCommandInterface : public IVoidCommandInterface {
public:
	IRaiseEventCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		std::wstring wszStorage = (pContext->GetPartCount() == 4) ? pContext->GetPartString(3) : L"void";

		return g_oEventMgr.Raise(pContext->GetPartString(1), pContext->GetPartArray(2), wszStorage);
	}

} g_oRaiseEventCommandInterface;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Events",
	L"0.1",
	L"Daniel Brendel",
	L"dbrendel1988<at>gmail<dot>com",
	L"Event handling provider"
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

	//Register example commands
	g_pShellPluginAPI->Cmd_RegisterCommand(L"events.register", &g_oRegEventCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"events.add", &g_oAddEventHandlerCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"events.raise", &g_oRaiseEventCommandInterface, CT_VOID);

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	g_pShellPluginAPI->Cmd_UnregisterCommand(L"events.register");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"events.add");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"events.raise");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}