#pragma once

#include "dnyas_sdk.h"
#include <Windows.h>
#include <time.h>
#include <fstream>

namespace Timer {
	IShellPluginAPI* pShellPluginAPI = nullptr;

	void Init(IShellPluginAPI* pInterface)
	{
		Timer::pShellPluginAPI = pInterface;
	}

	class CTimer {
	private:
		bool m_bActive;
		DWORD m_dwCurrent, m_dwInitial;
		DWORD m_dwTime;
	public:
		CTimer() : m_bActive(false) {}
		CTimer(DWORD dwTime) { this->SetTime(dwTime); this->Activate(); }
		~CTimer() {}

		void Activate()
		{
			if (!this->m_bActive) {
				this->m_dwCurrent = this->m_dwInitial = GetTickCount();

				this->m_bActive = true;
			}
		}

		void Process(void)
		{
			if (!this->m_bActive)
				return;

			this->m_dwCurrent = GetTickCount();
		}

		bool Elapsed(bool bReset = false)
		{
			if (!this->m_bActive)
				return false;

			bool bResult = this->m_dwCurrent >= this->m_dwInitial + this->m_dwTime;

			if (bResult) {
				if (bReset) {
					this->m_dwCurrent = this->m_dwInitial = GetTickCount();
				} else {
					this->m_bActive = false;
				}
			} 

			return bResult;
		}

		inline void SetTime(DWORD dwTime) { this->m_dwTime = dwTime; }
		inline const bool IsActive(void) const { return this->m_bActive; }
	};

	class CTimerMgr {
	private:
		struct timerinfo_s {
			std::wstring wszName;
			CTimer oTimer;
		};

		std::vector<timerinfo_s> m_vTimers;
	public:
		CTimerMgr() {}
		~CTimerMgr() { this->m_vTimers.clear(); }

		bool NameAlreadyInUse(const std::wstring& wszName)
		{
			for (size_t i = 0; i < this->m_vTimers.size(); i++) {
				if (this->m_vTimers[i].wszName == wszName)
					return true;
			}

			return false;
		}

		size_t FindTimer(const std::wstring& wszName)
		{
			for (size_t i = 0; i < this->m_vTimers.size(); i++) {
				if (this->m_vTimers[i].wszName == wszName)
					return i;
			}

			return std::string::npos;
		}

		bool AddTimer(const std::wstring& wszName, const dnyInteger iDelay)
		{
			if (!wszName.length())
				return false;

			if (this->NameAlreadyInUse(wszName))
				return false;

			timerinfo_s sInfo;
			sInfo.wszName = wszName;
			sInfo.oTimer.SetTime((DWORD)iDelay);
			sInfo.oTimer.Activate();

			this->m_vTimers.push_back(sInfo);

			return true;
		}

		bool Process(void)
		{
			for (size_t i = 0; i < this->m_vTimers.size(); i++) {
				this->m_vTimers[i].oTimer.Process();
				if (this->m_vTimers[i].oTimer.Elapsed()) {
					ICVar<dnyInteger>* pCvResult = (ICVar<dnyInteger>*)pShellPluginAPI->Cv_RegisterCVar(this->m_vTimers[i].wszName + L"_fnc_result_", CT_INT);
					if (!pCvResult)
						return false;

					if (!pShellPluginAPI->Scr_ExecuteCode(L"call " + this->m_vTimers[i].wszName + L"_OnElapsed() => " + this->m_vTimers[i].wszName + L"_fnc_result_;")) {
						pShellPluginAPI->Cv_FreeCVar(this->m_vTimers[i].wszName + L"_fnc_result_");
						return false;
					}

					std::wstring wszVarName = this->m_vTimers[i].wszName + L"_fnc_result_";
					
					if (pCvResult->GetValue()) {
						this->m_vTimers[i].oTimer.Activate();
					} else {
						this->m_vTimers.erase(this->m_vTimers.begin() + i);
					}

					pShellPluginAPI->Cv_FreeCVar(wszVarName);
				}
			}

			return true;
		}

		bool ClearTimer(const std::wstring& wszTimer)
		{
			size_t uiTimer = this->FindTimer(wszTimer);
			if ((uiTimer != std::string::npos) && (uiTimer < this->m_vTimers.size())) {
				this->m_vTimers.erase(this->m_vTimers.begin() + uiTimer);

				return true;
			}

			return false;
		}
	} oTimerMgr;

	bool AddTimer(const std::wstring& wszName, const dnyInteger iDelay)
	{
		return oTimerMgr.AddTimer(wszName, iDelay);
	}

	bool Process(void)
	{
		return oTimerMgr.Process();
	}

	bool TimerExists(const std::wstring& wszTimer)
	{
		return oTimerMgr.NameAlreadyInUse(wszTimer);
	}

	bool ClearTimer(const std::wstring& wszTimer)
	{
		return oTimerMgr.ClearTimer(wszTimer);
	}
}