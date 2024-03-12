#pragma once

#pragma comment(lib, "Ws2_32.lib")

#include <locale>
#include <codecvt>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <vector>
#include "dnyas_sdk.h"

namespace IRCFramework {
	typedef void* NetworkObjectInstance;

	struct INetworkEvents {
	public:
		virtual void OnConnected(NetworkObjectInstance pInstance) = 0;
		virtual void OnDisconnected(NetworkObjectInstance pInstance) = 0;
		virtual void OnRecieve(NetworkObjectInstance pInstance, const void* pBuffer, const size_t uiBufferSize) = 0;
		virtual void OnError(NetworkObjectInstance pInstance, int iErrorMessage) = 0;
	};

	class CBaseClient {
	private:
		static const int MaximumRecvBufferSize = 4096;

		int m_iSockType;
		INetworkEvents* m_pEvents;
		SOCKET m_hSocket;
		sockaddr m_sSockAddr;

		inline void SetComponentData(int iSockType, INetworkEvents* pEvents) { this->m_iSockType = iSockType; this->m_pEvents = pEvents; }

		bool EstablishConnection(const std::wstring& wszAddress, const std::wstring& wszPort)
		{
			if ((!wszAddress.length()) || (!this->m_pEvents) || (!this->m_iSockType))
				return false;

			ADDRINFO sInitialAddrInfo;
			ADDRINFO* pAdressInfoData = nullptr;
			this->m_hSocket = INVALID_SOCKET;

			memset(&sInitialAddrInfo, 0x00, sizeof(sInitialAddrInfo));
			sInitialAddrInfo.ai_family = AF_UNSPEC;
			sInitialAddrInfo.ai_socktype = this->m_iSockType;
			
			if (getaddrinfo(this->ConvertToAnsiString(wszAddress).c_str(), (wszPort.length() > 0) ? this->ConvertToAnsiString(wszPort).c_str() : nullptr, &sInitialAddrInfo, &pAdressInfoData) != 0) {
				this->m_pEvents->OnError(this, WSAGetLastError());
				return false;
			}
			
			for (ADDRINFO* it = pAdressInfoData; it != nullptr; it = it->ai_next) {
				this->m_hSocket = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
				if (this->m_hSocket == INVALID_SOCKET) continue;

				if (this->m_iSockType == SOCK_STREAM) {
					if (connect(this->m_hSocket, it->ai_addr, (int)it->ai_addrlen) == SOCKET_ERROR) {
						closesocket(this->m_hSocket);
						this->m_hSocket = INVALID_SOCKET;
						continue;
					}
				}

				this->m_sSockAddr = *it->ai_addr;
			}
			
			bool bResult = this->m_hSocket != INVALID_SOCKET;

			if (!bResult)
				this->m_pEvents->OnError(this, WSAGetLastError());
			
			freeaddrinfo(pAdressInfoData);

			return bResult;
		}

		bool CanRead(void)
		{
			if (this->m_hSocket == INVALID_SOCKET)
				return false;

			FD_SET fdSockSet;
			timeval sTimeVal;

			sTimeVal.tv_sec = 1;
			sTimeVal.tv_usec = 0;

			FD_ZERO(&fdSockSet);
			FD_SET(this->m_hSocket, &fdSockSet);

			return select(0, &fdSockSet, nullptr, nullptr, &sTimeVal) == 1;
		}

		bool CanWrite(void)
		{
			if (this->m_hSocket == INVALID_SOCKET)
				return false;

			FD_SET fdSockSet;
			timeval sTimeVal;

			sTimeVal.tv_sec = 1;
			sTimeVal.tv_usec = 0;

			FD_ZERO(&fdSockSet);
			FD_SET(this->m_hSocket, &fdSockSet);

			return select(0, nullptr, &fdSockSet, nullptr, &sTimeVal) == 1;
		}

		inline bool SendAsTcp(const void* pBuffer, const size_t uiLength)
		{
			return send(this->m_hSocket, (char*)pBuffer, (int)uiLength, 0) == (int)uiLength;
		}

		inline bool SendAsUdp(const void* pBuffer, const size_t uiLength)
		{
			return sendto(this->m_hSocket, (char*)pBuffer, (int)uiLength, 0, &this->m_sSockAddr, sizeof(this->m_sSockAddr)) == (int)uiLength;
		}

		inline bool SendAsTcp(const std::wstring& wszBuffer)
		{
			return send(this->m_hSocket, (char*)wszBuffer.data(), (int)(wszBuffer.length() * sizeof(wchar_t) + 2), 0) == wszBuffer.length() * sizeof(wchar_t) + 2;
		}

		inline bool SendAsUdp(const std::wstring& wszBuffer)
		{
			return sendto(this->m_hSocket, (char*)wszBuffer.data(), (int)(wszBuffer.length() * sizeof(wchar_t) + 2), 0, &this->m_sSockAddr, sizeof(this->m_sSockAddr)) == wszBuffer.length() * sizeof(wchar_t) + 2;
		}

		byte* RecvAsTcp(size_t& uiBufferLenOut, const int iBufferSize = MaximumRecvBufferSize)
		{
			byte* pRecvBuffer = new byte[iBufferSize];
			if (!pRecvBuffer)
				return nullptr;

			memset(pRecvBuffer, 0x00, iBufferSize);

			int iResult = recv(this->m_hSocket, (char*)pRecvBuffer, iBufferSize, 0);
			if (iResult <= 0) {
				delete[] pRecvBuffer;
				this->m_pEvents->OnError(this, WSAGetLastError());
				if (iResult == 0) {
					this->m_pEvents->OnDisconnected(this);
					this->Release();
				}
				return nullptr;
			}

			uiBufferLenOut = (size_t)iResult;

			return pRecvBuffer;
		}

		byte* RecvAsUdp(size_t& uiBufferLenOut, const int iBufferSize = MaximumRecvBufferSize)
		{
			byte* pRecvBuffer = new byte[iBufferSize];
			if (!pRecvBuffer)
				return nullptr;

			memset(pRecvBuffer, 0x00, iBufferSize);

			sockaddr sFromAddr;
			int iFromLen = sizeof(sFromAddr);

			int iResult = recvfrom(this->m_hSocket, (char*)pRecvBuffer, iBufferSize, 0, &sFromAddr, &iFromLen);
			if (iResult <= 0) {
				delete[] pRecvBuffer;
				this->m_pEvents->OnError(this, WSAGetLastError());
				if (iResult == 0) {
					this->m_pEvents->OnDisconnected(this);
					this->Release();
				}
				return nullptr;
			}

			if (memcmp(sFromAddr.sa_data, this->m_sSockAddr.sa_data, sizeof(SOCKADDR::sa_data))) {
				delete[] pRecvBuffer;
				return nullptr;
			}

			uiBufferLenOut = (size_t)iResult;

			return pRecvBuffer;
		}

		virtual void Release(void)
		{
			if (this->m_hSocket == INVALID_SOCKET)
				return;

			this->m_pEvents->OnDisconnected(this);

			shutdown(this->m_hSocket, 2);
			closesocket(this->m_hSocket);

			this->m_hSocket = INVALID_SOCKET;
		}
	public:
		CBaseClient() : m_iSockType(0), m_pEvents(nullptr), m_hSocket(INVALID_SOCKET) {}
		CBaseClient(int iSockType, INetworkEvents* pEvents) : m_iSockType(iSockType), m_pEvents(pEvents), m_hSocket(INVALID_SOCKET) {}
		CBaseClient(int iSockType, INetworkEvents* pEvents, const std::wstring& wszAddress, const std::wstring& wszPort) : CBaseClient(iSockType, pEvents) { this->Connect(wszAddress, wszPort); }
		~CBaseClient() { this->Release(); }

		virtual bool Connect(const std::wstring& wszAddress, const std::wstring& wszPort)
		{
			if (!wszAddress.length())
				return false;
			
			if ((this->m_iSockType != SOCK_STREAM) && (this->m_iSockType != SOCK_DGRAM))
				return false;
			
			bool bResult = this->EstablishConnection(wszAddress, wszPort);

			if (bResult)
				this->m_pEvents->OnConnected(this);
			
			return bResult;
		}

		virtual void Process(void)
		{
			if (!this->CanRead())
				return;

			byte* pRecievedBuffer = nullptr;
			size_t uiBufferLength = 0;

			switch (this->m_iSockType) {
			case SOCK_STREAM:
				pRecievedBuffer = this->RecvAsTcp(uiBufferLength);
				break;
			case SOCK_DGRAM:
				pRecievedBuffer = this->RecvAsUdp(uiBufferLength);
				break;
			default:
				return;
			}

			this->m_pEvents->OnRecieve(this, pRecievedBuffer, uiBufferLength);

			delete[] pRecievedBuffer;
		}

		virtual bool Send(const void* pBuffer, const size_t uiLength)
		{
			if (!this->CanWrite())
				return false;

			switch (this->m_iSockType) {
			case SOCK_STREAM:
				return this->SendAsTcp(pBuffer, uiLength);
				break;
			case SOCK_DGRAM:
				return this->SendAsUdp(pBuffer, uiLength);
				break;
			default:
				break;
			}

			return false;
		}

		virtual bool Send(const std::wstring& wszBuffer)
		{
			if (!this->CanWrite())
				return false;

			switch (this->m_iSockType) {
			case SOCK_STREAM:
				return this->SendAsTcp(wszBuffer);
				break;
			case SOCK_DGRAM:
				return this->SendAsUdp(wszBuffer);
				break;
			default:
				break;
			}

			return false;
		}

		static inline std::string ConvertToAnsiString(const std::wstring& wszString)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> oConverter;
			return oConverter.to_bytes(wszString);
		}
	};

	inline std::wstring ConvertToWideString(const std::string& szString)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> oConverter;
		return oConverter.from_bytes(szString);
	}

	inline std::string ConvertToAnsiString(const std::wstring& wszString)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> oConverter;
		return oConverter.to_bytes(wszString);
	}

	class CIRCNetworkEvents : public INetworkEvents {
	private:
		std::string m_szEntireBuffer;
	public:
		virtual void OnConnected(NetworkObjectInstance pInstance);
		virtual void OnDisconnected(NetworkObjectInstance pInstance);
		virtual void OnRecieve(NetworkObjectInstance pInstance, const void* pBuffer, const size_t uiBufferSize);
		virtual void OnError(NetworkObjectInstance pInstance, int iErrorMessage);
	};

	struct IIRCFrameworkAPI {
	public:
		virtual void OnConnected(NetworkObjectInstance pInstance) = 0;
		virtual void OnDisconnected(NetworkObjectInstance pInstance) = 0;
		virtual void OnError(NetworkObjectInstance pInstance, int iErrorMessage) = 0;
		virtual void HandleQueue(NetworkObjectInstance pInstance, const std::vector<std::wstring>& vMessages) = 0;
	};

	class CIrcClient : public CBaseClient {
	public:
		void OnConnected() { this->m_pFramework->OnConnected(this); }
		void OnDisconnected() { this->m_pFramework->OnDisconnected(this); }
		void OnError(int iErrorMessage) { this->m_pFramework->OnError(this, iErrorMessage); }
		void HandleQueue(const std::vector<std::wstring>& vMessages) { this->m_pFramework->HandleQueue(this, vMessages); }
	private:
		CIRCNetworkEvents m_oNetworkEvents;
		IIRCFrameworkAPI* m_pFramework;
	public:
		CIrcClient() : CBaseClient(0, nullptr), m_pFramework(nullptr) {}
		CIrcClient(IIRCFrameworkAPI* pFramework) : CBaseClient(SOCK_STREAM, &m_oNetworkEvents), m_pFramework(pFramework) {}
		~CIrcClient() {}

		virtual bool Connect(const std::wstring& wszAddress, const std::wstring& wszPort) { return CBaseClient::Connect(wszAddress, wszPort); }
		virtual void Process(void) { CBaseClient::Process(); }
		virtual bool Send(const std::wstring& wszBuffer)
		{ 
			std::string szMessage = ConvertToAnsiString(wszBuffer + L"\r\n");
			return CBaseClient::Send(szMessage.data(), szMessage.length());
		}
	};

	struct CMgrFrameworkAPI : public IIRCFrameworkAPI {
	private:
		class CIRCClientMgr* m_pInstance;
	public:
		CMgrFrameworkAPI(class CIRCClientMgr* pInstance) : m_pInstance(pInstance) {}
		~CMgrFrameworkAPI() { this->m_pInstance = nullptr; }

		virtual void OnConnected(NetworkObjectInstance pInstance);
		virtual void OnDisconnected(NetworkObjectInstance pInstance);
		virtual void OnError(NetworkObjectInstance pInstance, int iErrorMessage);
		virtual void HandleQueue(NetworkObjectInstance pInstance, const std::vector<std::wstring>& vMessages);
	};

	struct ircclient_s {
		CIrcClient* pClient;
		std::wstring wszIdent;
	};

	class CIRCClientMgr {
	private:
		std::vector<ircclient_s> m_vClients;
		CMgrFrameworkAPI m_oMgrFrameworkAPI;

		void Release(void)
		{
			for (size_t i = 0; i < this->m_vClients.size(); i++) {
				delete this->m_vClients[i].pClient;
			}

			this->m_vClients.clear();
		}

		ircclient_s* FindClient(const std::wstring& wszIdent)
		{
			for (size_t i = 0; i < this->m_vClients.size(); i++) {
				if (this->m_vClients[i].wszIdent == wszIdent)
					return &this->m_vClients[i];
			}
			return nullptr;
		}
	public:
		CIRCClientMgr() : m_oMgrFrameworkAPI(this) {}
		~CIRCClientMgr() { this->Release(); }

		ircclient_s* FindClient(NetworkObjectInstance pInstance)
		{
			for (size_t i = 0; i < this->m_vClients.size(); i++) {
				if (this->m_vClients[i].pClient == pInstance)
					return &this->m_vClients[i];
			}
			return nullptr;
		}

		bool Add(const std::wstring& wszIdent, const std::wstring& wszServer, const std::wstring& wszPort)
		{
			if (!wszIdent.length())
				return false;
			
			if (this->FindClient(wszIdent))
				return false;
			
			CIrcClient* pClient = new CIrcClient(&this->m_oMgrFrameworkAPI);
			if (!pClient)
				return false;

			ircclient_s sClientData;
			sClientData.pClient = pClient;
			sClientData.wszIdent = wszIdent;

			this->m_vClients.push_back(sClientData);
			if (!pClient->Connect(wszServer, wszPort)) {
				this->Remove(wszIdent);
				return false;
			}
			
			return true;
		}

		bool IsValid(const std::wstring& wszIdent)
		{
			return this->FindClient(wszIdent) != nullptr;
		}

		void Process(void)
		{
			for (size_t i = 0; i < this->m_vClients.size(); i++) {
				this->m_vClients[i].pClient->Process();
			}
		}

		void Process(const std::wstring& wszIdent)
		{
			ircclient_s* pClient = this->FindClient(wszIdent);
			if (pClient) {
				pClient->pClient->Process();
			}
		}

		bool Send(const std::wstring& wszIdent, const std::wstring& wszData)
		{
			ircclient_s* pClient = this->FindClient(wszIdent);
			return (pClient) ? pClient->pClient->Send(wszData) : false;
		}

		bool Remove(const std::wstring& wszIdent)
		{
			for (size_t i = 0; i < this->m_vClients.size(); i++) {
				if (this->m_vClients[i].wszIdent == wszIdent) {
					delete this->m_vClients[i].pClient;
					this->m_vClients.erase(this->m_vClients.begin() + i);
					return true;
				}
			}

			return false;
		}
	};

	extern CIRCClientMgr oIRCClientMgr;

	class ISpawnClientCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		ISpawnClientCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			IResultCommandInterface<dnyBoolean>::SetResult(oIRCClientMgr.Add(pContext->GetPartString(1), pContext->GetPartString(2), pContext->GetPartString(3)));

			return true;
		}

	};

	class IValidClientCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		IValidClientCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			IResultCommandInterface<dnyBoolean>::SetResult(oIRCClientMgr.IsValid(pContext->GetPartString(1)));

			return true;
		}

	};

	class IProcessClientsCommandInterface : public IVoidCommandInterface {
	public:
		IProcessClientsCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			std::wstring wszClient = pContext->GetPartString(1);

			if (wszClient.length() > 0) {
				oIRCClientMgr.Process(wszClient);
			} else {
				oIRCClientMgr.Process();
			}

			return true;
		}

	};

	class ISendDataCommandInterface : public IVoidCommandInterface {
	public:
		ISendDataCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			return oIRCClientMgr.Send(pContext->GetPartString(1), pContext->GetPartString(2));
		}

	};

	class IRemoveClientCommandInterface : public IVoidCommandInterface {
	public:
		IRemoveClientCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			return oIRCClientMgr.Remove(pContext->GetPartString(1));
		}

	};

	bool Initialize(IShellPluginAPI* pInstance);
	void Release(void);
}