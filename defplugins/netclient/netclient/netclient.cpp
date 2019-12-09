#include "netclient.h"

namespace NetClient {
	IShellPluginAPI* pShellPluginAPI = nullptr;
	CClientMgr* pClientMgrInstance = nullptr;
	CClientMgr oNetClientMgr;
	ISpawnNetClientCommandInterface oSpawnNetClientCommandInterface;
	IIsValidCommandInterface oIsValidCommandInterface;
	IProcessSocketsCommandInterface oProcessSocketsCommandInterface;
	ISendDataCommandInterface oSendDataCommandInterface;
	IReleaseNetClientCommandInterface oReleaseNetClientCommandInterface;

	inline std::wstring ConvertToWideString(const std::string& szString)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> oConverter;
		return oConverter.from_bytes(szString);
	}

	void ISharedClientEvents::OnConnected(NetworkObjectInstance pInstance)
	{
		std::wstring wszIdent = pClientMgrInstance->GetName((HNETCLIENT)pInstance);
		if (!wszIdent.length()) return;

		pShellPluginAPI->Scr_ExecuteCode(L"call " + wszIdent + L"_OnConnected() => void;");
	}

	void ISharedClientEvents::OnDisconnected(NetworkObjectInstance pInstance)
	{
		std::wstring wszIdent = pClientMgrInstance->GetName((HNETCLIENT)pInstance);
		if (!wszIdent.length()) return;

		pShellPluginAPI->Scr_ExecuteCode(L"call " + wszIdent + L"_OnDisconnected() => void;");
	}

	void ISharedClientEvents::OnRecieve(NetworkObjectInstance pInstance, const void* pBuffer, const size_t uiBufferSize)
	{
		std::wstring wszIdent = pClientMgrInstance->GetName((HNETCLIENT)pInstance);
		if (!wszIdent.length()) return;
		CClientMgr::DataType_e eDataType = pClientMgrInstance->GetType((HNETCLIENT)pInstance);
		if (eDataType == CClientMgr::DataType_e::DT_UNKNOWN) return;

		std::wstring wszArgument;
		
		if (eDataType == CClientMgr::DataType_e::DT_ANSI) {
			char* pChar = new char[uiBufferSize + 1];
			memset(pChar, 0x00, uiBufferSize + 1);
			memcpy(pChar, pBuffer, uiBufferSize);
			wszArgument = ConvertToWideString(pChar);
			delete[] pChar;
		} else if (eDataType == CClientMgr::DataType_e::DT_UNICODE) {
			wchar_t* pWChar = new wchar_t[uiBufferSize / 2 + 1];
			memset(pWChar, 0x00, uiBufferSize + 1);
			memcpy(pWChar, pBuffer, uiBufferSize);
			wszArgument = pWChar;
			delete[] pWChar;
		}

		ICVar<dnyString>* pContentCVar = (ICVar<dnyString>*)pShellPluginAPI->Cv_RegisterCVar(L"NetClient::ISharedClientEvents::OnRecieve", CT_STRING);
		pContentCVar->SetValue(wszArgument);
		pShellPluginAPI->Scr_ExecuteCode(L"call " + wszIdent + L"_OnRecieve(\"%NetClient::ISharedClientEvents::OnRecieve\") => void;");
		pShellPluginAPI->Cv_FreeCVar(L"NetClient::ISharedClientEvents::OnRecieve");
	}

	void ISharedClientEvents::OnError(NetworkObjectInstance pInstance, int iErrorMessage)
	{
		std::wstring wszIdent = pClientMgrInstance->GetName((HNETCLIENT)pInstance);
		if (!wszIdent.length()) return;

		pShellPluginAPI->Scr_ExecuteCode(L"call " + wszIdent + L"_OnError(" + std::to_wstring(iErrorMessage) + L") => void;");
	}

	bool Initialize(IShellPluginAPI* pInstance)
	{
		if (!pInstance)
			return false;

		WSADATA sWSAData;
		if (WSAStartup(MAKEWORD(2, 2), &sWSAData) == SOCKET_ERROR)
			return false;

		pShellPluginAPI = pInstance;

		pShellPluginAPI->Cmd_RegisterCommand(L"net_spawnclient", &oSpawnNetClientCommandInterface, CT_VOID);
		pShellPluginAPI->Cmd_RegisterCommand(L"net_isvalid", &oIsValidCommandInterface, CT_BOOL);
		pShellPluginAPI->Cmd_RegisterCommand(L"net_process", &oProcessSocketsCommandInterface, CT_VOID);
		pShellPluginAPI->Cmd_RegisterCommand(L"net_sendbuffer", &oSendDataCommandInterface, CT_VOID);
		pShellPluginAPI->Cmd_RegisterCommand(L"net_releaseclient", &oReleaseNetClientCommandInterface, CT_VOID);

		return true;
	}

	void Release(void)
	{
		pShellPluginAPI->Cmd_UnregisterCommand(L"net_spawnclient");
		pShellPluginAPI->Cmd_UnregisterCommand(L"net_isvalid");
		pShellPluginAPI->Cmd_UnregisterCommand(L"net_process");
		pShellPluginAPI->Cmd_UnregisterCommand(L"net_sendbuffer");
		pShellPluginAPI->Cmd_UnregisterCommand(L"net_releaseclient");

		pShellPluginAPI = nullptr;

		WSACleanup();
	}
}