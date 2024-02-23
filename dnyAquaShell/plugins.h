#pragma once

/*
	AquaShell (dnyAquaShell) developed by Daniel Brendel

	(C) 2017 - 2024 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include <string>
#include <vector>
#include <Windows.h>
#include "constants.h"

namespace Plugins {
	class CPluginInt {
	private:
		typedef WORD dnyVersionInfo;
		typedef bool(*TpfnPluginLoadFunc)(dnyVersionInfo version, void* pInterfaceData, void* pPluginInfos);
		typedef void(*TpfnPluginProcessFunc)(void);
		typedef void(*TpfnPluginUnloadFunc)(void);

		struct plugin_s {
			HMODULE hModule;
			std::wstring wszPluginFile;
			TpfnPluginLoadFunc pfnLoadFunc;
			TpfnPluginProcessFunc pfnProcessFunc;
			TpfnPluginUnloadFunc pfnUnloadFunc;
			struct plugininfo_s {
				wchar_t wszName[MAX_PATH];
				wchar_t wszVersion[MAX_PATH];
				wchar_t wszAuthor[MAX_PATH];
				wchar_t wszContact[MAX_PATH];
				wchar_t wszDescription[MAX_PATH];
			} sInfo;
		};

		std::vector<plugin_s> m_vPlugins;

		void Free(void)
		{
			//Free resources

			//Call unload function and free module from memory for each plugin
			for (size_t i = 0; i < this->m_vPlugins.size(); i++) {
				this->m_vPlugins[i].pfnUnloadFunc();
				FreeLibrary(this->m_vPlugins[i].hModule);
			}

			//Clear list
			this->m_vPlugins.clear();
		}
	public:
		CPluginInt() {}
		CPluginInt(const std::wstring& wszBaseDir, void* pInterfaceData) { this->LoadAllPlugins(wszBaseDir, pInterfaceData); }
		~CPluginInt() { this->Free(); }

		bool LoadAllPlugins(const std::wstring& wszBaseDir, void* pInterfaceData)
		{
			//Load all plugins from base directory

			WIN32_FIND_DATA sFindData;

			//Initiate file search
			HANDLE hFileSearch = FindFirstFile(std::wstring(wszBaseDir + L"\\*.dll").c_str(), &sFindData);
			if (hFileSearch == INVALID_HANDLE_VALUE)
				return false;

			//Loop through all DLL files
			do {
				if (!this->LoadPlugin(wszBaseDir + L"\\" + sFindData.cFileName, pInterfaceData, DNY_AS_PRODUCT_VERSION_W)) { //Attempt to load plugin
					std::wcout << L"Failed to load plugin \"" << wszBaseDir << L"\\" << sFindData.cFileName << L"\"" << std::endl;
				}
			} while (FindNextFile(hFileSearch, &sFindData));

			//Cleanup search data
			FindClose(hFileSearch);

			return true;
		}

		bool LoadPlugin(const std::wstring& wszPluginFile, void* pInterfaceData, dnyVersionInfo versionInfo)
		{
			//Load a given plugin

			if ((!wszPluginFile.length()) || (!pInterfaceData))
				return false;

			//Check if already loaded
			if (this->PluginLoaded(wszPluginFile))
				return false;

			plugin_s sPluginData;
			sPluginData.wszPluginFile = wszPluginFile;

			//Attempt to load module into memory
			sPluginData.hModule = LoadLibrary(wszPluginFile.c_str());
			if (!sPluginData.hModule)
				return false;

			//Query function pointers
			#define LP_QUERY_FUNC_PTR(storage, td, name) sPluginData.##storage = (td)GetProcAddress(sPluginData.hModule, name); if (!sPluginData.##storage) { FreeLibrary(sPluginData.hModule); return false; }
			LP_QUERY_FUNC_PTR(pfnLoadFunc, TpfnPluginLoadFunc, "dnyAS_PluginLoad");
			LP_QUERY_FUNC_PTR(pfnUnloadFunc, TpfnPluginUnloadFunc, "dnyAS_PluginUnload");

			//Call plugin load function
			if (!sPluginData.pfnLoadFunc(versionInfo, pInterfaceData, &sPluginData.sInfo)) {
				FreeLibrary(sPluginData.hModule);
				return false;
			}

			//Save plugin info data to list
			this->m_vPlugins.push_back(sPluginData);

			return true;
		}

		bool PluginLoaded(const std::wstring& wszPluginFile)
		{
			//Check whether a plugin has already been loaded

			for (size_t i = 0; i < this->m_vPlugins.size(); i++) {
				if (this->m_vPlugins[i].wszPluginFile == wszPluginFile)
					return true;
			}

			return false;
		}

		void ListPlugins(void)
		{
			//List all plugins

			std::wcout << "Total amount of plugins: " << this->m_vPlugins.size() << std::endl;

			for (size_t i = 0; i < this->m_vPlugins.size(); i++) {
				std::wcout << L"#" << (i + 1) << L" " << this->m_vPlugins[i].sInfo.wszName << " v" << this->m_vPlugins[i].sInfo.wszVersion << " by " << this->m_vPlugins[i].sInfo.wszAuthor << " (" << this->m_vPlugins[i].sInfo.wszContact << "): " << this->m_vPlugins[i].sInfo.wszDescription << std::endl;
			}
		}
	};
}