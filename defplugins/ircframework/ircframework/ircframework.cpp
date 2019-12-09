#include "ircframework.h"

namespace IRCFramework {
	IShellPluginAPI* pShellPluginAPI = nullptr;
	CIRCClientMgr oIRCClientMgr;

	void CIRCNetworkEvents::OnConnected(NetworkObjectInstance pInstance)
	{
		class CIrcClient* pIRCInstance = (class CIrcClient*)pInstance;
		pIRCInstance->OnConnected();
	}

	void CIRCNetworkEvents::OnDisconnected(NetworkObjectInstance pInstance)
	{
		CIrcClient* pIRCInstance = (CIrcClient*)pInstance;
		pIRCInstance->OnDisconnected();
	}

	void CIRCNetworkEvents::OnRecieve(NetworkObjectInstance pInstance, const void* pBuffer, const size_t uiBufferSize)
	{
		CIrcClient* pIRCInstance = (CIrcClient*)pInstance;

		char* pAsAnsiString = new char[uiBufferSize + 1];
		memcpy(pAsAnsiString, pBuffer, uiBufferSize);
		pAsAnsiString[uiBufferSize] = 0;

		this->m_szEntireBuffer += std::string(pAsAnsiString);
		if ((this->m_szEntireBuffer[this->m_szEntireBuffer.length() - 2] == '\r') && (this->m_szEntireBuffer[this->m_szEntireBuffer.length() - 1] == '\n')) {
			std::string szSingleLine;
			std::vector<std::wstring> vLines;

			for (size_t i = 0; i < this->m_szEntireBuffer.length(); i++) {
				if ((this->m_szEntireBuffer[i] == '\r') && (this->m_szEntireBuffer[i + 1] == '\n')) {
					vLines.push_back(ConvertToWideString(szSingleLine));
					szSingleLine.clear();
					i++;
					continue;
				}

				szSingleLine += this->m_szEntireBuffer[i];
			}

			pIRCInstance->HandleQueue(vLines);

			this->m_szEntireBuffer.clear();
		}

		delete[] pAsAnsiString;
	}

	void CIRCNetworkEvents::OnError(NetworkObjectInstance pInstance, int iErrorMessage)
	{
		CIrcClient* pIRCInstance = (CIrcClient*)pInstance;
		pIRCInstance->OnError(iErrorMessage);
	}

	void CMgrFrameworkAPI::OnConnected(NetworkObjectInstance pInstance)
	{
		ircclient_s* pData = this->m_pInstance->FindClient(pInstance);
		if (pData) {
			pShellPluginAPI->Scr_ExecuteCode(L"call " + pData->wszIdent + L"_OnConnected() => void;");
		}
	}
	void CMgrFrameworkAPI::OnDisconnected(NetworkObjectInstance pInstance)
	{
		ircclient_s* pData = this->m_pInstance->FindClient(pInstance);
		if (pData) {
			pShellPluginAPI->Scr_ExecuteCode(L"call " + pData->wszIdent + L"_OnDisonnected() => void;");
		}
	}
	void CMgrFrameworkAPI::OnError(NetworkObjectInstance pInstance, int iErrorMessage)
	{
		ircclient_s* pData = this->m_pInstance->FindClient(pInstance);
		if (pData) {
			pShellPluginAPI->Scr_ExecuteCode(L"call " + pData->wszIdent + L"_OnError(" + std::to_wstring(iErrorMessage) + L") => void;");
		}
	}
	void CMgrFrameworkAPI::HandleQueue(NetworkObjectInstance pInstance, const std::vector<std::wstring>& vMessages)
	{
		ircclient_s* pData = this->m_pInstance->FindClient(pInstance);
		if (pData) {
			for (size_t i = 0; i < vMessages.size(); i++) {
				pShellPluginAPI->Scr_ExecuteCode(L"call " + pData->wszIdent + L"_OnRecieve(\"" + vMessages[i] + L"\") => void;");
			}
		}
	}

	ISpawnClientCommandInterface oSpawnClientCommandInterface;
	IValidClientCommandInterface oValidClientCommandInterface;
	IProcessClientsCommandInterface oProcessClientsCommandInterface;
	ISendDataCommandInterface oSendDataCommandInterface;
	IRemoveClientCommandInterface oRemoveClientCommandInterface;

	bool Initialize(IShellPluginAPI* pInstance)
	{
		if (!pInstance)
			return false;

		pShellPluginAPI = pInstance;

		WSADATA sWSAData;
		WSAStartup(MAKEWORD(2, 2), &sWSAData);

		pShellPluginAPI->Cmd_RegisterCommand(L"irc_spawn", &oSpawnClientCommandInterface, CT_BOOL);
		pShellPluginAPI->Cmd_RegisterCommand(L"irc_isvalid", &oValidClientCommandInterface, CT_BOOL);
		pShellPluginAPI->Cmd_RegisterCommand(L"irc_process", &oProcessClientsCommandInterface, CT_VOID);
		pShellPluginAPI->Cmd_RegisterCommand(L"irc_send", &oSendDataCommandInterface, CT_VOID);
		pShellPluginAPI->Cmd_RegisterCommand(L"irc_release", &oRemoveClientCommandInterface, CT_VOID);

		return true;
	}

	void Release(void)
	{
		if (!pShellPluginAPI)
			return;

		pShellPluginAPI->Cmd_UnregisterCommand(L"irc_spawn");
		pShellPluginAPI->Cmd_UnregisterCommand(L"irc_isvalid");
		pShellPluginAPI->Cmd_UnregisterCommand(L"irc_process");
		pShellPluginAPI->Cmd_UnregisterCommand(L"irc_send");
		pShellPluginAPI->Cmd_UnregisterCommand(L"irc_release");

		WSACleanup();

		pShellPluginAPI = nullptr;
	}
}