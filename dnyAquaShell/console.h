#pragma once

/*
	AquaShell (dnyAquaShell) developed by Daniel Brendel

	(C) 2017 - 2024 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include "constants.h"

namespace Console {
	class CConInterface {
	private:
		std::wstring m_wszCurrentDir;
		std::wstring m_wszCmdPrefix;
		std::vector<std::wstring> m_vAppArgs;
		bool m_bReady;

		bool Initialize(const std::wstring& wszWorkingDir, int argc, wchar_t* argv[])
		{
			//Initialize component

			if (this->m_bReady)
				return true;

			//Set console title
			if (!SetConsoleTitle(DNY_AS_PRODUCT_NAME))
				return false;

			this->m_wszCurrentDir = wszWorkingDir;

			//Set input prefix
			this->m_wszCmdPrefix = DNY_AS_PRODUCT_NAME L" " + this->m_wszCurrentDir + L" > ";

			//Store application arguments
			for (int i = 1; i < argc; i++) {
				this->m_vAppArgs.push_back(argv[i]);
			}

			return this->m_bReady = true;
		}

		std::wstring QueryInput(void)
		{
			//Query console input

			if (!this->m_bReady)
				return L"";

			std::wstring wszResult;

			std::wcout << this->m_wszCmdPrefix;
			std::getline(std::wcin, wszResult);

			return wszResult;
		}

		bool WriteLine(const std::wstring& wszTextLine)
		{
			//Write textline to console

			if (!this->m_bReady)
				return false;

			std::wcout << wszTextLine << std::endl;

			return true;
		}
	public:
		CConInterface() : m_bReady(false) {}
		CConInterface(const std::wstring& wszWorkingDir, int argc, wchar_t* argv[]) : m_bReady(false) { this->Initialize(wszWorkingDir, argc, argv); }
		~CConInterface() { this->m_vAppArgs.clear(); this->m_bReady = false; }

		void SetColor(const std::string& color)
		{
			system(std::string("color " + color).c_str());
		}

		bool Int_WriteLine(const std::wstring& wszTextLine) { return this->WriteLine(wszTextLine); }
		std::wstring Int_QueryInput(void) { return this->QueryInput(); }

		std::wstring& operator >> (std::wstring& wszStorageString) { wszStorageString = this->QueryInput(); return wszStorageString; }
		void operator<<(const std::wstring& wszTextLine) { this->WriteLine(wszTextLine); }
	};
}