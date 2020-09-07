#pragma once

/*
	dnyAquaShell developed by Daniel Brendel

	(C) 2017 by Daniel Brendel

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

#define DNY_AS_PRODUCT_VERSION_W MAKEWORD(1, 0)

typedef WORD dnyVersionInfo;

typedef void* dnyVoid;
typedef bool dnyBoolean;
typedef __int64 dnyInteger;
typedef double dnyFloat;
typedef std::wstring dnyString;
typedef std::wstring dnycustom;

typedef void* cvarptr_t;
enum cvartype_e { CT_UNKNOWN = -1, CT_VOID = 0, CT_BOOL, CT_INT, CT_FLOAT, CT_STRING, CT_CUSTOM };
template <typename VarType>
class ICVar {
private:
	VarType m_vtVariable;
	cvartype_e m_eType;
	bool m_bConst;
	bool m_bInitial;
	void* m_pCustomData;
public:
	ICVar() : m_bInitial(false), m_pCustomData(nullptr) {}
	ICVar(cvartype_e ct) : m_eType(ct), m_bConst(false), m_bInitial(false), m_pCustomData(nullptr) {}
	ICVar(cvartype_e ct, bool bc) : m_eType(ct), m_bConst(bc), m_bInitial(false), m_pCustomData(nullptr) {}
	ICVar(cvartype_e ct, bool bc, VarType v) : m_eType(ct), m_bConst(bc), m_vtVariable(v), m_bInitial(false), m_pCustomData(nullptr) {}
	~ICVar() { if (this->m_pCustomData) free(this->m_pCustomData); }

	inline void SetValue(VarType v) { if (this->m_bConst) { if (!this->m_bInitial) this->m_bInitial = true; else return; } this->m_vtVariable = v; }
	inline VarType GetValue(void) { return this->m_vtVariable; }
	inline bool IsConst(void) const { return this->m_bConst; }

	inline bool IsCustom(void) const { return this->m_eType == CT_CUSTOM; }
	inline bool AcquireCustomSpace(const size_t uiMemSize)
	{
		//Acquire custom memory space

		if ((this->m_eType != CT_CUSTOM) || (!uiMemSize))
			return false;

		if (this->m_pCustomData)
			free(this->m_pCustomData);

		this->m_pCustomData = malloc(uiMemSize);

		return this->m_pCustomData != nullptr;
	}
	inline void* CustomData(void) const { return this->m_pCustomData; }
};

class ICustomVarValue {
private:
	bool m_bAssigned;
	std::wstring m_wszExpression;
public:
	ICustomVarValue() : m_bAssigned(false) {}
	ICustomVarValue(const std::wstring& wszExpression) { this->SetExpression(wszExpression); }
	~ICustomVarValue() {}

	//Initialization
	inline void SetExpression(const std::wstring& wszExpression) { this->m_wszExpression = wszExpression; this->m_bAssigned = true; }
	inline bool IsValid(void) const { return this->m_bAssigned; }

	//Getters - dny data types
	dnyBoolean QueryAsDnyBoolean(void) const { return ((this->m_wszExpression == L"false") ? false : true); }
	dnyInteger QueryAsDnyInteger(void) const { return _wtoi64(this->m_wszExpression.c_str()); }
	dnyFloat QueryAsDnyFloat(void) const { return _wtof(this->m_wszExpression.c_str()); }
	dnyString QueryAsDnyString(void) const { return this->m_wszExpression; }
	//Getters - native data types
	signed char QueryAsByte(void) const { return (signed char)_wtoi64(this->m_wszExpression.c_str()); }
	unsigned char QueryAsUByte(void) const { return (unsigned char)_wtoi64(this->m_wszExpression.c_str()); }
	signed short QueryAsShort(void) const { return (signed short)_wtoi64(this->m_wszExpression.c_str()); }
	unsigned short QueryAsUShort(void) const { return (unsigned short)_wtoi64(this->m_wszExpression.c_str()); }
	signed int QueryAsInt32(void) const { return (signed int)_wtoi64(this->m_wszExpression.c_str()); }
	unsigned int QueryAsUInt32(void) const { return (unsigned int)_wtoi64(this->m_wszExpression.c_str()); }
	signed __int64 QueryAsInt64(void) const { return _wtoi64(this->m_wszExpression.c_str()); }
	unsigned __int64 QueryAsUInt64(void) const { return _wtoi64(this->m_wszExpression.c_str()); }
	float QueryAsFloat(void) const { return (float)_wtof(this->m_wszExpression.c_str()); }
	double QueryAsDouble(void) const { return _wtof(this->m_wszExpression.c_str()); }
	const wchar_t* QueryAsWString(void) const { return this->m_wszExpression.c_str(); }
	std::wstring QueryAsStdString(void) const { return this->m_wszExpression; }
	//Getter - unknown
	const void* QueryAsUnknown(void) { return this->m_wszExpression.data(); }
};
typedef bool(*TpfnDeclareVar)(const std::wstring& wszName, ICVar<dnycustom>* pCVar);
typedef bool(*TpfnAssignVarValue)(const std::wstring& wszName, ICVar<dnycustom>* pCVar, const ICustomVarValue& rCustomVarValue, bool bIsConst);
typedef dnyString(*TpfnGetReplacerString)(const std::wstring& wszName, ICVar<dnycustom>* pCVar);
typedef void(*TpfnRemoveVar)(const std::wstring& wszName, ICVar<dnycustom>* pCVar);
struct cvar_type_event_table_s {
	TpfnDeclareVar pfnDeclareVar;
	TpfnAssignVarValue pfnAssignVarValue;
	TpfnGetReplacerString pfnGetReplacerString;
	TpfnRemoveVar pfnRemoveVar;
};

typedef void* cmdinterface_t;
class IVoidCommandInterface {
public:
	IVoidCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject) = 0;
};
template <typename TResultVar>
class IResultCommandInterface {
private:
	ICVar<TResultVar> m_oResultVar;
	cvartype_e m_eCVarType;
protected:
	virtual void SetType(cvartype_e ct) { this->m_eCVarType = ct; }
	virtual void SetResult(TResultVar vResultValue) { this->m_oResultVar.SetValue(vResultValue); }
public:
	IResultCommandInterface() {}
	IResultCommandInterface(cvartype_e et) : m_eCVarType(et) {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject) = 0;
	virtual TResultVar GetResult(void) { return m_oResultVar.GetValue(); }
};

struct ICodeContext {
	virtual void AddPart(const std::wstring& wszPartExpression) = 0;
	virtual void Clear(void) = 0;

	//Code context item list getters
	virtual const size_t GetPartCount(void) = 0;
	virtual std::wstring GetPartData(const size_t uiBlockId) = 0;
	virtual dnyBoolean GetPartBool(const size_t uiBlockId) = 0;
	virtual dnyInteger GetPartInt(const size_t uiBlockId) = 0;
	virtual dnyFloat GetPartFloat(const size_t uiBlockId) = 0;
	virtual dnyString GetPartString(const size_t uiBlockId) = 0;
	virtual std::vector<dnyString> GetPartArray(const size_t uiBlockId) = 0;
	virtual std::vector<dnyString> GetPartParamlist(const size_t uiBlockId) = 0;
	
	//Variable replacement
	virtual void ReplaceGlobalVariables(void* pInterfaceObject) = 0;
	virtual void ReplaceLocalVariables(void* pInterfaceObject) = 0;
	virtual void ReplaceAllVariables(void* pInterfaceObject) = 0;
	
	virtual dnyString GetAsLine(void) const = 0;
};

struct IShellPluginAPI { //Plugin API interface
	IShellPluginAPI() {}
	IShellPluginAPI(void* p1, void* p2) {}
	~IShellPluginAPI() { }

	virtual bool Scr_ExecuteCode(const std::wstring& wszCode) = 0;
	virtual bool Scr_ExecuteScript(const std::wstring& wszScriptFile) = 0;

	virtual bool Fnc_BeginFunctionCall(const std::wstring& wszFuncName, const cvartype_e eResultType) = 0;
	virtual bool Fnc_PushFunctionParam(dnyBoolean value) = 0;
	virtual bool Fnc_PushFunctionParam(dnyInteger value) = 0;
	virtual bool Fnc_PushFunctionParam(dnyFloat value) = 0;
	virtual bool Fnc_PushFunctionParam(const dnyString& value) = 0;
	virtual bool Fnc_ExecuteFunction(void) = 0;
	virtual dnyBoolean Fnc_QueryFunctionResultAsBoolean(void) = 0;
	virtual dnyInteger Fnc_QueryFunctionResultAsInteger(void) = 0;
	virtual dnyFloat Fnc_QueryFunctionResultAsFloat(void) = 0;
	virtual dnyString Fnc_QueryFunctionResultAsString(void) = 0;
	virtual bool Fnc_EndFunctionCall(void) = 0;

	virtual bool Cv_RegisterCVarType(const std::wstring& wszTypeName, cvar_type_event_table_s* pEventTable) = 0;
	virtual cvarptr_t Cv_RegisterCVar(const std::wstring& wszName, const cvartype_e eType, bool bConst = false) = 0;
	virtual cvarptr_t Cv_FindCVar(const std::wstring& wszName) = 0;
	virtual bool Cv_FreeCVar(const std::wstring& wszName) = 0;

	virtual bool Cmd_RegisterCommand(const std::wstring& wszCmdName, cmdinterface_t pCmdInterface, cvartype_e eType) = 0;
	virtual bool Cmd_UnregisterCommand(const std::wstring& wszCmdName) = 0;

	virtual bool Con_WriteLine(const std::wstring& wszTextLine) = 0;
	virtual std::wstring Con_QueryInput(void) = 0;
};

struct plugininfo_s {
	wchar_t wszName[MAX_PATH];
	wchar_t wszVersion[MAX_PATH];
	wchar_t wszAuthor[MAX_PATH];
	wchar_t wszContact[MAX_PATH];
	wchar_t wszDescription[MAX_PATH];
};