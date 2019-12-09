#pragma once

#include <string>
#include <Windows.h>
#include <sapi.h>
#include <sphelper.h>

namespace SpeechInt {
	ISpVoice* pVoiceInstance = nullptr;
	ISpObjectToken* pCurVoice = nullptr;
	std::wstring wszVoiceName = L"Default";
	USHORT usPitch = 0;
	USHORT usVolume = 0;
	USHORT usSpeed = 0;

	bool Initialize(void)
	{
		//Initialize speech interface

		if (pVoiceInstance)
			return false;

		//Instantiate COM
		if (FAILED(CoInitialize(NULL)))
			return false;
		
		//Aquire an interface object for MS SAPI via COM
		return SUCCEEDED(CoCreateInstance(__uuidof(SpVoice), NULL, CLSCTX_ALL, IID_ISpVoice, (LPVOID*)&pVoiceInstance));
	}

	//Setters
	void SetPitch(USHORT pitch) { usPitch = pitch; }
	void SetVolume(USHORT volume) { usVolume = volume; }
	void SetSpeed(USHORT speed) { usSpeed = speed; }
	bool SetVoice(const std::wstring& wszVoice)
	{
		//Set language voice

		pCurVoice = nullptr;

		if (!wszVoice.length())
			return false;

		IEnumSpObjectTokens* pTokens = nullptr;

		//Enum all available voices
		if (FAILED(SpEnumTokens(SPCAT_VOICES, NULL, NULL, &pTokens)))
			return false;

		ULONG ulCount = 0;

		//Acquire amount of voices
		if (FAILED(pTokens->GetCount(&ulCount))) {
			pTokens->Release();
			return false;
		}

		ISpObjectToken* pVoice = nullptr;
		BOOL bAttrMatched = FALSE;
		for (ULONG i = 0; i < ulCount; i++) { //Loop through voice count
			pTokens->Next(1, &pVoice, NULL); //Acquire next voice object

			if (SUCCEEDED(pVoice->MatchesAttributes((L"Name=" + wszVoice).c_str(), &bAttrMatched))) { //Check for desired voice via name of current voice object
				if (bAttrMatched) {
					pCurVoice = pVoice;
					//Set found voice object
					pVoiceInstance->SetVoice(pCurVoice);
					wszVoiceName = wszVoice;
					break;
				}
			}
		}

		pTokens->Release(); //Release enum object

		return pCurVoice != nullptr;
	}

	//Getters
	USHORT GetPitch(void) { return usPitch; }
	USHORT GetVolume(void) { return usVolume; }
	USHORT GetSpeed(void) { return usSpeed; }
	std::wstring GetVoice(void) { return wszVoiceName; }

	bool Speak(const std::wstring& wszText)
	{
		//Speak synchronously

		if (!pVoiceInstance)
			return false;
		
		std::wstring wszSpeakBuffer = L"<volume level=\"" + std::to_wstring((_Longlong)usVolume) + L"\"><pitch absmiddle=\"" + std::to_wstring((_Longlong)usPitch) + L"\"><rate speed=\"" + std::to_wstring((_Longlong)usSpeed) + L"\">" + wszText + L"</volume></pitch></rate>";
	
		return SUCCEEDED(pVoiceInstance->Speak(wszSpeakBuffer.c_str(), SPF_IS_XML, nullptr));
	}

	bool SpeakAsync(const std::wstring& wszText)
	{
		//Speak asynchronously

		if (!pVoiceInstance)
			return false;

		std::wstring wszSpeakBuffer = L"<volume level=\"" + std::to_wstring((_Longlong)usVolume) + L"\"><pitch absmiddle=\"" + std::to_wstring((_Longlong)usPitch) + L"\"><rate speed=\"" + std::to_wstring((_Longlong)usSpeed) + L"\">" + wszText + L"</volume></pitch></rate>";

		return SUCCEEDED(pVoiceInstance->Speak(wszSpeakBuffer.c_str(), SPF_IS_XML | SPF_ASYNC, nullptr));
	}

	void Free(void)
	{
		//Free resources

		if (!pVoiceInstance)
			return;

		//Free SAPI interface object and uninitialize COM
		pVoiceInstance->Release();
		CoUninitialize();

		pVoiceInstance = nullptr;
	}


}