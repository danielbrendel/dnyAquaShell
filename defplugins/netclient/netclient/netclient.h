#pragma once

#include <locale>
#include <codecvt>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "dnyas_sdk.h"

namespace NetClient {
	extern IShellPluginAPI* pShellPluginAPI;

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
				delete [] pRecvBuffer;
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
				delete [] pRecvBuffer;
				this->m_pEvents->OnError(this, WSAGetLastError());
				if (iResult == 0) {
					this->m_pEvents->OnDisconnected(this);
					this->Release();
				}
				return nullptr;
			}

			if (memcmp(sFromAddr.sa_data, this->m_sSockAddr.sa_data, sizeof(SOCKADDR::sa_data))) {
				delete [] pRecvBuffer;
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

	class CTcpClient : public CBaseClient {
	public:
		CTcpClient(INetworkEvents* pEvents) : CBaseClient(SOCK_STREAM, pEvents) {}
		~CTcpClient() {}

		virtual bool Connect(const std::wstring& wszAddress, const std::wstring& wszPort) { return CBaseClient::Connect(wszAddress, wszPort); }
		virtual void Process(void) { CBaseClient::Process(); }
		virtual bool Send(const std::wstring& wszBuffer) { return CBaseClient::Send(wszBuffer); }
	};

	class CUdpClient : public CBaseClient {
	public:
		CUdpClient(INetworkEvents* pEvents) : CBaseClient(SOCK_DGRAM, pEvents) {}
		~CUdpClient() {}

		virtual bool Connect(const std::wstring& wszAddress, const std::wstring& wszPort) { return CBaseClient::Connect(wszAddress, wszPort); }
		virtual void Process(void) { CBaseClient::Process(); }
		virtual bool Send(const std::wstring& wszBuffer) { return CBaseClient::Send(wszBuffer); }
	};

	extern class CClientMgr* pClientMgrInstance;

	struct ISharedClientEvents : public INetworkEvents {
	public:
		virtual void OnConnected(NetworkObjectInstance pInstance);
		virtual void OnDisconnected(NetworkObjectInstance pInstance);
		virtual void OnRecieve(NetworkObjectInstance pInstance, const void* pBuffer, const size_t uiBufferSize);
		virtual void OnError(NetworkObjectInstance pInstance, int iErrorMessage);
	};
	typedef CBaseClient* HNETCLIENT;
	class CClientMgr {
	public:
		enum SocketType_e { ST_UNKNOWN = 0, ST_TCP, ST_UDP };
		enum DataType_e { DT_UNKNOWN = 0, DT_ANSI, DT_UNICODE };
	private:
		struct netclients_s { CBaseClient* pClient; std::wstring wszIdent; DataType_e eDataType; };

		ISharedClientEvents m_oSharedEvents;
		std::vector<netclients_s> m_vClients;

		SocketType_e GetSocketType(const std::wstring& wszNamedType)
		{
			if (wszNamedType == L"tcp")
				return ST_TCP;
			else if (wszNamedType == L"udp")
				return ST_UDP;

			return ST_UNKNOWN;
		}

		DataType_e GetDataType(const std::wstring& wszNamedType)
		{
			if (wszNamedType == L"ansi")
				return DT_ANSI;
			else if (wszNamedType == L"unicode")
				return DT_UNICODE;

			return DT_UNKNOWN;
		}

		void Release(void)
		{
			for (size_t i = 0; i < this->m_vClients.size(); i++)
				delete this->m_vClients[i].pClient;

			this->m_vClients.clear();
		}

		const size_t FindClient(HNETCLIENT hNetClient)
		{
			for (size_t i = 0; i < this->m_vClients.size(); i++) {
				if (this->m_vClients[i].pClient == hNetClient)
					return i;
			}

			return std::wstring::npos;
		}

		const size_t FindClient(const std::wstring& wszIdent)
		{
			for (size_t i = 0; i < this->m_vClients.size(); i++) {
				if (this->m_vClients[i].wszIdent == wszIdent)
					return i;
			}

			return std::wstring::npos;
		}
	public:
		CClientMgr() { pClientMgrInstance = this; }
		~CClientMgr() { this->Release(); }

		HNETCLIENT SpawnClient(const std::wstring& wszIdent, const std::wstring& wszAddress, const std::wstring& wszPort, const std::wstring& wszSockType, const std::wstring& wszDataType)
		{
			if (!wszIdent.length())
				return nullptr;

			CBaseClient* pClient = nullptr;
			
			SocketType_e eSockType = this->GetSocketType(wszSockType);
			if (eSockType == ST_UNKNOWN)
				return nullptr;

			DataType_e eDataType = this->GetDataType(wszDataType);
			if (eDataType == DT_UNKNOWN)
				return nullptr;
			
			switch (eSockType) {
			case ST_TCP: {
				pClient = new CTcpClient(&this->m_oSharedEvents);
				if (!pClient) return nullptr;
				break;
			}
			case ST_UDP: {
				pClient = new CUdpClient(&this->m_oSharedEvents);
				if (!pClient) return nullptr;
				break;
			}
			default:
				break;
			}
			
			netclients_s sClient;
			sClient.pClient = pClient;
			sClient.wszIdent = wszIdent;
			sClient.eDataType = eDataType;
			this->m_vClients.push_back(sClient);

			if (!pClient->Connect(wszAddress, wszPort)) {
				this->ReleaseClient(wszIdent);
				return nullptr;
			}

			return pClient;
		}

		std::wstring GetName(HNETCLIENT hNetClient)
		{
			size_t uiItemId = this->FindClient(hNetClient);
			if (uiItemId != std::wstring::npos) {
				return this->m_vClients[uiItemId].wszIdent;
			}

			return L"";
		}

		DataType_e GetType(HNETCLIENT hNetClient)
		{
			size_t uiItemId = this->FindClient(hNetClient);
			if (uiItemId != std::wstring::npos) {
				return this->m_vClients[uiItemId].eDataType;
			}

			return DT_UNKNOWN;
		}

		bool IsValid(const std::wstring& wszIdent)
		{
			return this->FindClient(wszIdent) != std::wstring::npos;
		}

		bool Send(HNETCLIENT hNetClient, const std::wstring& wszBuffer)
		{
			size_t uiItemId = this->FindClient(hNetClient);
			if (uiItemId != std::wstring::npos) {
				switch (this->m_vClients[uiItemId].eDataType) {
				case DT_ANSI: {
					std::string szData = CBaseClient::ConvertToAnsiString(wszBuffer);
					return this->m_vClients[uiItemId].pClient->Send(szData.data(), szData.length() + 1);
					break;
				}
				case DT_UNICODE: {
					return this->m_vClients[uiItemId].pClient->Send(wszBuffer);
					break;
				}
				default:
					return false;
				}
			}

			return false;
		}

		bool Send(const std::wstring& wszIdent, const std::wstring& wszBuffer)
		{
			size_t uiItemId = this->FindClient(wszIdent);
			if (uiItemId != std::wstring::npos) {
				switch (this->m_vClients[uiItemId].eDataType) {
				case DT_ANSI: {
					std::string szData = CBaseClient::ConvertToAnsiString(wszBuffer);
					return this->m_vClients[uiItemId].pClient->Send(szData.data(), szData.length());
					break;
				}
				case DT_UNICODE: {
					return this->m_vClients[uiItemId].pClient->Send(wszBuffer);
					break;
				}
				default:
					return false;
				}
			}

			return false;
		}

		void ProcessSockets(void)
		{
			for (size_t i = 0; i < this->m_vClients.size(); i++) {
				this->m_vClients[i].pClient->Process();
			}
		}

		void ProcessSockets(const std::wstring& wszIdent)
		{
			size_t uiItemId = this->FindClient(wszIdent);
			if (uiItemId != std::wstring::npos) {
				this->m_vClients[uiItemId].pClient->Process();
			}
		}

		bool ReleaseClient(HNETCLIENT hNetClient)
		{
			size_t uiItemId = this->FindClient(hNetClient);
			if (uiItemId != std::wstring::npos) {
				delete this->m_vClients[uiItemId].pClient;
				this->m_vClients.erase(this->m_vClients.begin() + uiItemId);

				return true;
			}

			return false;
		}

		bool ReleaseClient(const std::wstring& wszIdent)
		{
			size_t uiItemId = this->FindClient(wszIdent);
			if (uiItemId != std::wstring::npos) {
				delete this->m_vClients[uiItemId].pClient;
				this->m_vClients.erase(this->m_vClients.begin() + uiItemId);

				return true;
			}

			return false;
		}
	};

	extern CClientMgr oNetClientMgr;

	class ISpawnNetClientCommandInterface : public IVoidCommandInterface {
	public:
		ISpawnNetClientCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			std::vector<dnyString> vOptions = pContext->GetPartArray(4);
			if (vOptions.size() != 2) return false;

			return oNetClientMgr.SpawnClient(pContext->GetPartString(1), pContext->GetPartString(2), pContext->GetPartString(3), vOptions[0], vOptions[1]) != nullptr;
		}
	};

	class IIsValidCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		IIsValidCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			IResultCommandInterface<dnyBoolean>::SetResult(oNetClientMgr.IsValid(pContext->GetPartString(1)));

			return true;
		}

	};

	class IProcessSocketsCommandInterface : public IVoidCommandInterface {
	public:
		IProcessSocketsCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			std::wstring wszIdent = pContext->GetPartString(1);

			if (wszIdent.length() > 0) {
				oNetClientMgr.ProcessSockets(wszIdent);
			} else {
				oNetClientMgr.ProcessSockets();
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

			return oNetClientMgr.Send(pContext->GetPartString(1), pContext->GetPartString(2));
		}

	};

	class IReleaseNetClientCommandInterface : public IVoidCommandInterface {
	public:
		IReleaseNetClientCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			pContext->ReplaceAllVariables(pInterfaceObject);

			return oNetClientMgr.ReleaseClient(pContext->GetPartString(1));
		}

	};

	bool Initialize(IShellPluginAPI* pInstance);
	void Release(void);
}