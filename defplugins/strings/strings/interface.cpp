#include "dnyas_sdk.h"

IShellPluginAPI* g_pShellPluginAPI;
ICVar<dnyInteger>* g_pCommonArrayIndex = nullptr;

dnyString FmtStrHexadecimal(const dnyString& wszInput)
{
	dnyString wszResult;
	dnyString wszEscSeq;
	bool bInEscapeSeq = false;

	for (size_t i = 0; i < wszInput.length(); i++) {
		if (wszInput[i] == '\\') {
			bInEscapeSeq = true;
			wszEscSeq.clear();
			continue;
		}
		else if (wszInput[i] == ';') {
			bInEscapeSeq = false;
			wszResult += (wchar_t)std::stoul(wszEscSeq, nullptr, 16);
			continue;
		}

		if (!bInEscapeSeq)
			wszResult += wszInput[i];
		else
			wszEscSeq += wszInput[i];
	}

	return wszResult;
}

class IGetLenCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	IGetLenCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		std::wstring wszStr = pCodeContext->GetPartString(1);

		IResultCommandInterface<dnyInteger>::SetResult(wszStr.length());

		return true;
	}
} g_oGetLenCommand;

class IGetCharCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IGetCharCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		std::wstring wszStr = pCodeContext->GetPartString(1);
		dnyInteger iIndex = pCodeContext->GetPartInt(2);

		std::wstring wszResult = L"";

		if ((iIndex > -1) && (iIndex < (dnyInteger)wszStr.length())) {
			wszResult = wszStr[iIndex];
		}

		IResultCommandInterface<dnyString>::SetResult(wszResult);

		return true;
	}
} g_oGetCharCommand;

class IAppendCommandInterface : public IVoidCommandInterface {
public:
	IAppendCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		std::wstring wszVar = pCodeContext->GetPartString(1);
		std::wstring wszStr = pCodeContext->GetPartString(2);

		ICVar<dnyString>* pCVar = (ICVar<dnyString>*)g_pShellPluginAPI->Cv_FindCVar(wszVar);
		if (!pCVar)
			return false;

		pCVar->SetValue(pCVar->GetValue() + wszStr);

		return true;
	}

} g_oAppendCommand;

class IFindCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	IFindCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		IResultCommandInterface<dnyInteger>::SetResult(pCodeContext->GetPartString(1).find(pCodeContext->GetPartString(2)));

		return true;
	}
} g_oFindCommand;

class ISubstrCommandInterface : public IResultCommandInterface<dnyString> {
public:
	ISubstrCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		std::wstring wszStr = pCodeContext->GetPartString(1);
		dnyInteger iStart = pCodeContext->GetPartInt(2);
		dnyInteger iEnd = pCodeContext->GetPartInt(3);

		std::wstring wszResult = L"";
	
		if ((wszStr.length()) && ((iStart > -1) && ((size_t)iStart < wszStr.length())) && ((iEnd > -1) && (iStart + iEnd > iStart) && ((size_t)iEnd < wszStr.length()))) {
			wszResult = wszStr.substr(iStart, iEnd);
		}
		
		IResultCommandInterface<dnyString>::SetResult(wszResult);

		return true;
	}
} g_oSubstrCommand;

class IReplaceCommandInterface : public IVoidCommandInterface {
public:
	IReplaceCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		std::wstring wszVar = pCodeContext->GetPartString(1);
		std::wstring wszOld = pCodeContext->GetPartString(2);
		std::wstring wszNew = pCodeContext->GetPartString(3);

		ICVar<dnyString>* pCVar = (ICVar<dnyString>*)g_pShellPluginAPI->Cv_FindCVar(wszVar);
		if (!pCVar)
			return false;

		std::wstring wszVarValue = pCVar->GetValue();
		size_t uiFound = wszVarValue.find(wszOld);

		while (uiFound != std::wstring::npos) {
			wszVarValue.replace(uiFound, wszOld.length(), wszNew);
			uiFound = wszVarValue.find(wszOld);
		}

		pCVar->SetValue(wszVarValue);

		return true;
	}

} g_oReplaceCommand;

class ITokenizeCommandInterface : public IVoidCommandInterface {
public:
	ITokenizeCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		std::wstring wszStr = pCodeContext->GetPartString(1);
		std::wstring wszSplit = pCodeContext->GetPartString(2);
		std::wstring wszVar = pCodeContext->GetPartString(3);

		std::vector<std::wstring> vList;
		wchar_t* pContext;
		wchar_t* pToken = wcstok_s((wchar_t*)wszStr.c_str(), wszSplit.c_str(), &pContext);
		while (pToken) {
			vList.push_back(pToken);
			pToken = wcstok_s(nullptr, wszSplit.c_str(), &pContext);
		}

		ICVar<dnyInteger>* pCountVar = (ICVar<dnyInteger>*)g_pShellPluginAPI->Cv_RegisterCVar(wszVar + L".count", CT_INT);
		if (!pCountVar)
			return false;

		pCountVar->SetValue(vList.size());

		for (size_t i = 0; i < vList.size(); i++) {
			ICVar<dnyString>* pItemVar = (ICVar<dnyString>*)g_pShellPluginAPI->Cv_RegisterCVar(wszVar + L"[" + std::to_wstring(i) + L"]", CT_STRING);
			if (!pItemVar)
				return false;

			pItemVar->SetValue(vList[i]);
		}

		return true;
	}

} g_oTokenizeCommand;

class ISetTokenIndexCommandInterface : public IVoidCommandInterface {
public:
	ISetTokenIndexCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);
		
		if (!g_pCommonArrayIndex)
			return false;

		g_pCommonArrayIndex->SetValue(pCodeContext->GetPartInt(1));

		return true;
	}
} g_oSetTokenIndexCommand;

class IClearTokensCommandInterface : public IVoidCommandInterface {
public:
	IClearTokensCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		std::wstring wszVar = pCodeContext->GetPartString(1);
		
		ICVar<dnyInteger>* pCountVar = (ICVar<dnyInteger>*)g_pShellPluginAPI->Cv_FindCVar(wszVar + L".count");
		if (!pCountVar)
			return false;

		for (size_t i = 0; i < (size_t)pCountVar->GetValue(); i++) {
			g_pShellPluginAPI->Cv_FreeCVar(wszVar + L"[" + std::to_wstring(i) + L"]");
		}

		g_pShellPluginAPI->Cv_FreeCVar(wszVar + L".count");

		return true;
	}

} g_oClearTokensCommand;

class IUpperCommandInterface : public IVoidCommandInterface {
public:
	IUpperCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		ICVar<dnyString>* pCVar = (ICVar<dnyString>*)g_pShellPluginAPI->Cv_FindCVar(pCodeContext->GetPartString(1));
		if (!pCVar)
			return false;

		std::wstring wszValue = pCVar->GetValue();

		for (size_t i = 0; i < wszValue.length(); i++) {
			wszValue[i] = toupper(wszValue[i]);
		}

		pCVar->SetValue(wszValue);

		return true;
	}
} g_oUpperCommand;

class ILowerCommandInterface : public IVoidCommandInterface {
public:
	ILowerCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		ICVar<dnyString>* pCVar = (ICVar<dnyString>*)g_pShellPluginAPI->Cv_FindCVar(pCodeContext->GetPartString(1));
		if (!pCVar)
			return false;

		std::wstring wszValue = pCVar->GetValue();

		for (size_t i = 0; i < wszValue.length(); i++) {
			wszValue[i] = tolower(wszValue[i]);
		}

		pCVar->SetValue(wszValue);

		return true;
	}
} g_oLowerCommand;

class IRTrimCommandInterface : public IVoidCommandInterface {
public:
	IRTrimCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		ICVar<dnyString>* pCVar = (ICVar<dnyString>*)g_pShellPluginAPI->Cv_FindCVar(pCodeContext->GetPartString(1));
		if (!pCVar)
			return false;

		std::wstring wszValue = pCVar->GetValue();
		size_t uiFound = std::wstring::npos;

		for (size_t i = wszValue.length() - 1; i >= 0; i--) {
			if (((wszValue[i] != ' ') && (wszValue[i] != '\t')) && (uiFound == std::wstring::npos)) {
				uiFound = i;
				break;
			}
		}

		pCVar->SetValue(wszValue.substr(0, uiFound));

		return true;
	}
} g_oRTrimCommand;

class ILTrimCommandInterface : public IVoidCommandInterface {
public:
	ILTrimCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		ICVar<dnyString>* pCVar = (ICVar<dnyString>*)g_pShellPluginAPI->Cv_FindCVar(pCodeContext->GetPartString(1));
		if (!pCVar)
			return false;

		std::wstring wszValue = pCVar->GetValue();
		std::wstring wszResult = L"";
		bool bNotSpacing = false;

		for (size_t i = 0; i < wszValue.length(); i++) {
			if (((wszValue[i] != ' ') && (wszValue[i] != '\t')) && (!bNotSpacing))
				bNotSpacing = true;

			if (bNotSpacing)
				wszResult += wszValue[i];
		}

		pCVar->SetValue(wszResult);

		return true;
	}
} g_oLTrimCommand;

class IFmtHexEscSeqCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IFmtHexEscSeqCommandInterface() {}

	virtual bool CommandCallback(void* pArg1, void* pArg2)
	{
		ICodeContext* pCodeContext = (ICodeContext*)pArg1;

		pCodeContext->ReplaceAllVariables(pArg2);

		IResultCommandInterface<dnyString>::SetResult(FmtStrHexadecimal(pCodeContext->GetPartString(1)));

		return true;
	}
} g_oFmtHexEscSeqCommandInterface;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Strings",
	L"0.1",
	L"Daniel Brendel",
	L"Daniel Brendel<at>gmail<dot>com",
	L"String handling helper"
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

	//Register commands
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_getlen", &g_oGetLenCommand, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_getchar", &g_oGetCharCommand, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_append", &g_oAppendCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_find", &g_oFindCommand, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_substr", &g_oSubstrCommand, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_replace", &g_oReplaceCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_tokenize", &g_oTokenizeCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_settokenindex", &g_oSetTokenIndexCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_cleartokens", &g_oClearTokensCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_upper", &g_oUpperCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_lower", &g_oLowerCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_rtrim", &g_oRTrimCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_ltrim", &g_oLTrimCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"s_fmtescseq", &g_oFmtHexEscSeqCommandInterface, CT_STRING);

	g_pCommonArrayIndex = (ICVar<dnyInteger>*)g_pShellPluginAPI->Cv_RegisterCVar(L"token_index", CT_INT);
	if (!g_pCommonArrayIndex)
		return false;

	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_getlen");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_getchar");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_append");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_find");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_substr");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_replace");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_tokenize");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_settokenindex");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_cleartokens");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_upper");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_lower");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_rtrim");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_ltrim");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"s_fmtescseq");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}