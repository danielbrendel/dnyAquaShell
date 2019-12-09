#pragma once

#include "dnyas_sdk.h"
#include <fstream>
#include <Windows.h>
namespace FileIO {
	IShellPluginAPI* pShellPluginAPI;
	std::wstring wszBasePath;

	class CFileContext {
	private:
		std::wfstream m_hFile;
		std::wstring m_wszFileName;

		bool FileExists(const std::wstring& wszFile)
		{
			std::wifstream hFile;
			hFile.open(wszFile, std::wifstream::in);
			if (hFile.is_open()) {
				hFile.close();
				return true;
			}

			return false;
		}

		bool EnsureFileIsThere(const std::wstring& wszFile)
		{
			if (!wszFile.length())
				return false;

			if (!this->FileExists(wszFile)) {
				std::wofstream hFile;
				hFile.open(wszFile, std::wofstream::out);
				hFile.close();
			}

			return true;
		}
	public:
		CFileContext() : m_wszFileName(L"") {}
		CFileContext(const std::wstring& wszFileName) : m_wszFileName(L"") { this->Load(wszFileName); }
		CFileContext(const std::wstring& wszFileName, bool bAppend) : m_wszFileName(L"") { this->Load(wszFileName, bAppend); }
		~CFileContext() { this->Close(); }

		bool Load(const std::wstring& wszFileName, bool bAppend = false)
		{
			if (!wszFileName.length())
				return false;
			
			if (!this->EnsureFileIsThere(wszFileName))
				return false;

			int iMode = std::wfstream::in | std::wfstream::out;
			if (bAppend)
				iMode |= std::wfstream::app;

			this->m_hFile.open(wszFileName, iMode);
			
			if (this->m_hFile.is_open()) {
				this->m_wszFileName = wszFileName;
				return true;
			}

			return false;
		}

		bool IsOpen(void) { return this->m_hFile.is_open(); }
		bool Eof(void) { return this->m_hFile.eof(); }
		std::wstring FileName(void) { return this->m_wszFileName; }

		bool Seek(const size_t uiPos)
		{
			if (!this->m_hFile.is_open())
				return false;

			this->m_hFile.seekg(uiPos, std::wfstream::beg);

			return true;
		}

		std::wstring ReadLine(void)
		{
			wchar_t wszLine[2048];

			this->m_hFile.getline(wszLine, sizeof(wszLine));

			return wszLine;
		}

		bool WriteText(const std::wstring& wszText)
		{
			if (!this->m_hFile.is_open())
				return false;

			this->m_hFile << wszText;

			return true;
		}

		bool WriteLine(const std::wstring& wszTextLine)
		{
			if (!this->m_hFile.is_open())
				return false;

			this->m_hFile << wszTextLine << std::endl;

			return true;
		}

		void Close(void)
		{
			this->m_hFile.close();
		}
	};

	typedef size_t FHANDLE;
	class CFileIO {
	private:
		struct file_context_s {
			std::wstring wszHandleVar;
			ICVar<dnyInteger>* pVar;
			CFileContext* pContext;
		};

		std::vector<file_context_s> m_vFileContexts;

		inline bool IsValidHandle(const size_t uiId) { return (uiId < this->m_vFileContexts.size()); }
	public:
		CFileIO() {}
		~CFileIO() {}

		bool LoadFile(const std::wstring& wszFileName, const std::wstring& wszHandleVarName, bool bAppend, FHANDLE* phFileOut = nullptr)
		{
			if ((!wszFileName.length()) || (!wszHandleVarName.length()))
				return false;

			file_context_s sContext;
			sContext.pContext = new CFileContext(wszFileName, bAppend);
			if (!sContext.pContext)
				return false;
			
			if (!sContext.pContext->IsOpen()) {
				delete sContext.pContext; 
				return false;
			}
			
			sContext.wszHandleVar = wszHandleVarName;

			sContext.pVar = (ICVar<dnyInteger>*)pShellPluginAPI->Cv_RegisterCVar(wszHandleVarName, CT_INT);
			if (!sContext.pVar) {
				delete sContext.pContext;
				return false;
			}

			sContext.pVar->SetValue((dnyInteger)this->m_vFileContexts.size());

			this->m_vFileContexts.push_back(sContext);

			if (phFileOut)
				*phFileOut = this->m_vFileContexts.size() - 1;

			return true;
		}

		bool IsOpen(const FHANDLE hFile)
		{
			if (!this->IsValidHandle(hFile))
				return false;

			return this->m_vFileContexts[hFile].pContext->IsOpen();
		}

		bool IsAtEof(const FHANDLE hFile)
		{
			if (!this->IsValidHandle(hFile))
				return false;

			return this->m_vFileContexts[hFile].pContext->Eof();
		}

		bool Seek(const FHANDLE hFile, const size_t uiPos)
		{
			if (!this->IsValidHandle(hFile))
				return false;

			return this->m_vFileContexts[hFile].pContext->Seek(uiPos);
		}
		
		bool WriteText(const FHANDLE hFile, const std::wstring& wszText)
		{
			if (!this->IsValidHandle(hFile))
				return false;

			return this->m_vFileContexts[hFile].pContext->WriteText(wszText);
		}

		bool WriteLine(const FHANDLE hFile, const std::wstring& wszTextLine)
		{
			if (!this->IsValidHandle(hFile))
				return false;

			this->m_vFileContexts[hFile].pContext->WriteLine(wszTextLine);

			return true;
		}

		std::wstring ReadLine(const FHANDLE hFile)
		{
			if (!this->IsValidHandle(hFile))
				return L"";

			return this->m_vFileContexts[hFile].pContext->ReadLine();
		}

		bool CloseFile(const FHANDLE hFile)
		{
			if (!this->IsValidHandle(hFile))
				return false;

			this->m_vFileContexts[hFile].pContext->Close();
			
			delete this->m_vFileContexts[hFile].pContext;
			pShellPluginAPI->Cv_FreeCVar(this->m_vFileContexts[hFile].wszHandleVar);

			this->m_vFileContexts.erase(this->m_vFileContexts.begin() + hFile);

			return true;
		}
	} oFileIO;

	class CFolderEnum {
	#define FE_TEMPVAR L"__CFolderEnum::bResult__" + wszFolder + wszFilter
	private:
		WIN32_FIND_DATA m_sFindData;
		HANDLE m_hFileSearch;
	public:
		CFolderEnum() {}
		CFolderEnum(const std::wstring& wszFolder, const std::wstring& wszFilter, const std::wstring& wszFunctionName) { this->EnumFolder(wszFolder, wszFilter, wszFunctionName); }
		~CFolderEnum() {}

		bool EnumFolder(const std::wstring& wszFolder, const std::wstring& wszFilter, const std::wstring& wszFunctionName)
		{
			memset(&this->m_sFindData, 0x00, sizeof(WIN32_FIND_DATA));
			
			ICVar<dnyInteger>* pResultVar = (ICVar<dnyInteger>*)pShellPluginAPI->Cv_RegisterCVar(FE_TEMPVAR, CT_INT);
			if (!pResultVar)
				return false;

			this->m_hFileSearch = FindFirstFile((wszFolder + wszFilter).c_str(), &this->m_sFindData);
			if (this->m_hFileSearch == INVALID_HANDLE_VALUE) {
				pShellPluginAPI->Cv_FreeCVar(FE_TEMPVAR);
				return false;
			}

			while (FindNextFile(this->m_hFileSearch, &this->m_sFindData)) {
				if (this->m_sFindData.cFileName[0] == '.')
					continue;
				
				/*if (!pShellPluginAPI->Fnc_BeginFunctionCall(wszFunctionName, CT_BOOL)) {
					FindClose(this->m_hFileSearch);
					return false;
				}
				
				if (!pShellPluginAPI->Fnc_PushFunctionParam(L"\"" + wszFolder + L"\"")) {
					FindClose(this->m_hFileSearch);
					return false;
				}
				
				if (!pShellPluginAPI->Fnc_PushFunctionParam(L"\"" + dnyString(this->m_sFindData.cFileName) + L"\"")) {
					FindClose(this->m_hFileSearch);
					return false;
				}
				
				if (!pShellPluginAPI->Fnc_ExecuteFunction()) {
					FindClose(this->m_hFileSearch);
					return false;
				}
				
				dnyBoolean bResult = pShellPluginAPI->Fnc_QueryFunctionResultAsBoolean();
				
				if (!pShellPluginAPI->Fnc_EndFunctionCall()) {
					FindClose(this->m_hFileSearch);
					return false;
				}*/
				
				std::wstring wszCallCode = L"call " + wszFunctionName + L"(\"" + wszFolder + L"\", \"" + this->m_sFindData.cFileName + L"\") => \"" + (FE_TEMPVAR) + L"\";";
				if (!pShellPluginAPI->Scr_ExecuteCode(wszCallCode)) {
					FindClose(this->m_hFileSearch);
					pShellPluginAPI->Cv_FreeCVar(FE_TEMPVAR);
					return false;
				}

				if (!pResultVar->GetValue())
					break;

				//if (!bResult)
				//	break;
			}

			if (!FindClose(this->m_hFileSearch))
				return false;

			return pShellPluginAPI->Cv_FreeCVar(FE_TEMPVAR);
		}
	};

	bool CreateFolder(const std::wstring& wszFolder)
	{
		return CreateDirectory(wszFolder.c_str(), NULL) == TRUE;
	}

	bool RemoveFolder(const std::wstring& wszFolder)
	{
		return RemoveDirectory(wszFolder.c_str()) == TRUE;
	}

	bool RemoveFile(const std::wstring& wszFile)
	{
		return DeleteFile(wszFile.c_str()) == TRUE;
	}

	dnyInteger GetFileSize(const std::wstring& wszFile)
	{
		HANDLE hFile = CreateFile(wszFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile == INVALID_HANDLE_VALUE)
			return -1;
		
		dnyInteger iResult = (dnyInteger)::GetFileSize(hFile, NULL);

		CloseHandle(hFile);
		
		return iResult;
	}

	bool IsFolder(const std::wstring& wszObject)
	{
		return (GetFileAttributes(wszObject.c_str()) & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
	}

	dnyString GetCurDir(void)
	{
		wchar_t wszAppFile[MAX_PATH];

		if (!GetModuleFileName(NULL, wszAppFile, sizeof(wszAppFile)))
			return L"";

		for (size_t i = wcslen(wszAppFile); i >= 0; i--) {
			if (wszAppFile[i] == '\\')
				break;

			wszAppFile[i] = 0;
		}

		return wszAppFile;
	}

	bool DirExists(const std::wstring& wszFolder)
	{
		return IsFolder(wszFolder);
	}

	bool FileExists(const std::wstring& wszFile)
	{
		HANDLE hFile = CreateFile(wszFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		CloseHandle(hFile);

		return true;
	}

	bool FileCopy(const std::wstring& wszSrc, const std::wstring& wszDst)
	{
		return CopyFile(wszSrc.c_str(), wszDst.c_str(), TRUE) == TRUE;
	}

	bool FileMove(const std::wstring& wszSrc, const std::wstring& wszDst)
	{
		return MoveFile(wszSrc.c_str(), wszDst.c_str()) == TRUE;
	}

	class ILoadFileCommandInterface : public IVoidCommandInterface {
	public:
		ILoadFileCommandInterface() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			std::vector<std::wstring> vParams = pCodeContext->GetPartArray(1);
			if (vParams.size() != 3)
				return false;

			FHANDLE hFile;

			bool bAppend = (vParams[2] == L"true") ? true : false;

			if (!oFileIO.LoadFile(vParams[1], vParams[0], bAppend, &hFile))
				return false;
			
			std::wstring wszCode = pCodeContext->GetPartString(2);
			if (wszCode.length()) {
				//oFileIO.Seek(hFile, 0);
				pShellPluginAPI->Scr_ExecuteCode(wszCode);
				oFileIO.CloseFile(hFile);
			}

			return true;
		}

	} oLoadFileCommandInterface;

	class IIsOpenCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		IIsOpenCommandInterface() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			FHANDLE hFile = (FHANDLE)pCodeContext->GetPartInt(1);

			this->SetResult(oFileIO.IsOpen(hFile));

			return true;
		}

	} oIsOpenCommandInterface;

	class IEofCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		IEofCommandInterface() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			FHANDLE hFile = (FHANDLE)pCodeContext->GetPartInt(1);

			this->SetResult(oFileIO.IsAtEof(hFile));

			return true;
		}

	} oEofCommandInterface;

	class IWriteTextCommandInterface : public IVoidCommandInterface {
	public:
		IWriteTextCommandInterface() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			FHANDLE hFile = (FHANDLE)pCodeContext->GetPartInt(1);

			return oFileIO.WriteText(hFile, pCodeContext->GetPartString(2));
		}

	} oWriteTextCommandInterface;

	class IWriteLineCommandInterface : public IVoidCommandInterface {
	public:
		IWriteLineCommandInterface() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			FHANDLE hFile = (FHANDLE)pCodeContext->GetPartInt(1);

			return oFileIO.WriteLine(hFile, pCodeContext->GetPartString(2));
		}

	} oWriteLineCommandInterface;

	class IReadLineCommandInterface : public IResultCommandInterface<dnyString> {
	public:
		IReadLineCommandInterface() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			FHANDLE hFile = (FHANDLE)pCodeContext->GetPartInt(1);

			this->SetResult(oFileIO.ReadLine(hFile));

			return true;
		}

	} oReadLineCommandInterface;

	class ICloseFileCommandInterface : public IVoidCommandInterface {
	public:
		ICloseFileCommandInterface() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			FHANDLE hFile = (FHANDLE)pCodeContext->GetPartInt(1);

			return oFileIO.CloseFile(hFile);;
		}

	} oCloseFileCommandInterface;

	class ICreateFolderCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			IResultCommandInterface<dnyBoolean>::SetResult(CreateFolder(pCodeContext->GetPartString(1)));

			return true;
		}

	} oCreateFolderCommandInterface;

	class IRemoveFolderCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			IResultCommandInterface<dnyBoolean>::SetResult(RemoveFolder(pCodeContext->GetPartString(1)));

			return true;
		}

	} oRemoveFolderCommandInterface;

	class IRemoveFileCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			IResultCommandInterface<dnyBoolean>::SetResult(RemoveFile(pCodeContext->GetPartString(1)));

			return true;
		}

	} oRemoveFileCommandInterface;

	class IEnumFolderCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			CFolderEnum oFolderEnum;
		
			IResultCommandInterface<dnyBoolean>::SetResult(oFolderEnum.EnumFolder(pCodeContext->GetPartString(1), pCodeContext->GetPartString(2), pCodeContext->GetPartString(3)));

			return true;
		}

	} oEnumFolderCommandInterface;

	class ICurDirCommandInterface : public IResultCommandInterface<dnyString> {
	public:
		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			IResultCommandInterface<dnyString>::SetResult(GetCurDir());

			return true;
		}

	} oCurDirCommandInterface;

	class IGetFileSizeCommandInterface : public IResultCommandInterface<dnyInteger> {
	public:
		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			IResultCommandInterface<dnyInteger>::SetResult(GetFileSize(pCodeContext->GetPartString(1)));
			
			return true;
		}

	} oGetFileSizeCommandInterface;

	class IIsDirectoryCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			IResultCommandInterface<dnyBoolean>::SetResult(IsFolder(pCodeContext->GetPartString(1)));

			return true;
		}

	} oIsDirectoryCommandInterface;

	class IDirExistsCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			IResultCommandInterface<dnyBoolean>::SetResult(DirExists(pCodeContext->GetPartString(1)));

			return true;
		}

	} oDirExistsCommandInterface;

	class IFileExistsCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			IResultCommandInterface<dnyBoolean>::SetResult(FileExists(pCodeContext->GetPartString(1)));

			return true;
		}

	} oFileExistsCommandInterface;

	class ICopyFileCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			IResultCommandInterface<dnyBoolean>::SetResult(FileCopy(pCodeContext->GetPartString(1), pCodeContext->GetPartString(2)));

			return true;
		}

	} oCopyFileCommandInterface;

	class IMoveFileCommandInterface : public IResultCommandInterface<dnyBoolean> {
	public:
		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			IResultCommandInterface<dnyBoolean>::SetResult(FileMove(pCodeContext->GetPartString(1), pCodeContext->GetPartString(2)));

			return true;
		}

	} oMoveFileCommandInterface;

	bool Initialize(IShellPluginAPI* pInterface)
	{
		if (!pInterface)
			return false;

		pShellPluginAPI = pInterface;

		#define REG_CMD(n, ptr, type) if (!pShellPluginAPI->Cmd_RegisterCommand(n, ptr, type)) return false;
		REG_CMD(L"fopen", &oLoadFileCommandInterface, CT_VOID);
		REG_CMD(L"fisopen", &oIsOpenCommandInterface, CT_BOOL);
		REG_CMD(L"fateof", &oEofCommandInterface, CT_BOOL);
		REG_CMD(L"fwritetext", &oWriteTextCommandInterface, CT_VOID);
		REG_CMD(L"fwriteline", &oWriteLineCommandInterface, CT_VOID);
		REG_CMD(L"freadline", &oReadLineCommandInterface, CT_STRING);
		REG_CMD(L"fclose", &oCloseFileCommandInterface, CT_VOID);
		REG_CMD(L"dcreate", &oCreateFolderCommandInterface, CT_BOOL);
		REG_CMD(L"dremove", &oRemoveFolderCommandInterface, CT_BOOL);
		REG_CMD(L"fremove", &oRemoveFileCommandInterface, CT_BOOL);
		REG_CMD(L"denum", &oEnumFolderCommandInterface, CT_BOOL);
		REG_CMD(L"dgetcurrent", &oCurDirCommandInterface, CT_STRING);
		REG_CMD(L"fgetsize", &oGetFileSizeCommandInterface, CT_INT);
		REG_CMD(L"fdisdir", &oIsDirectoryCommandInterface, CT_BOOL);
		REG_CMD(L"fexists", &oFileExistsCommandInterface, CT_BOOL);
		REG_CMD(L"dexists", &oDirExistsCommandInterface, CT_BOOL);
		REG_CMD(L"fcopy", &oCopyFileCommandInterface, CT_BOOL);
		REG_CMD(L"fmove", &oMoveFileCommandInterface, CT_BOOL);

		wchar_t wcBasePath[MAX_PATH];
		
		if (!GetModuleFileName(NULL, wcBasePath, sizeof(wcBasePath)))
			return false;

		for (size_t i = wcslen(wcBasePath); i >= 0; i--) {
			if (wcBasePath[i] == '\\') break;
			wcBasePath[i] = 0x0000;
		}

		wszBasePath = wcBasePath;

		return true;
	}

	bool Free(void)
	{
		#define UNREG_CMD(n) if (!pShellPluginAPI->Cmd_UnregisterCommand(n)) return false;
		UNREG_CMD(L"fopen");
		UNREG_CMD(L"fisopen");
		UNREG_CMD(L"fateof");
		UNREG_CMD(L"fwritetext");
		UNREG_CMD(L"fwriteline");
		UNREG_CMD(L"freadline");
		UNREG_CMD(L"fclose");
		UNREG_CMD(L"dcreate");
		UNREG_CMD(L"dremove");
		UNREG_CMD(L"fremove");
		UNREG_CMD(L"denum");
		UNREG_CMD(L"dgetcurrent");
		UNREG_CMD(L"fgetsize");
		UNREG_CMD(L"fdisdir");
		UNREG_CMD(L"fexists");
		UNREG_CMD(L"dexists");
		UNREG_CMD(L"fcopy");
		UNREG_CMD(L"fmove");

		pShellPluginAPI = nullptr;

		return true;
	}
}