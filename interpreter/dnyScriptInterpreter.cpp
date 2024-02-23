#include "dnyScriptInterpreter.h"

/*
	dnyScriptInterpreter developed by Daniel Brendel

	(C) 2017 - 2022 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace dnyScriptInterpreter {

	/* Error handling */

	CScriptErrorInformation gn_oErrorHandler;

	void dnySetError(const ScriptErrorType_e eType, const dnyString& wszErrorMessage)
	{
		gn_oErrorHandler = CScriptErrorInformation(eType, wszErrorMessage);
	}

	void dnyclearError(void)
	{
		gn_oErrorHandler = CScriptErrorInformation();
	}

	const CScriptErrorInformation& GetErrorInformation(void) { return gn_oErrorHandler; }

	/* Code contexting */

	std::vector<dnyString> ICodeContext::GetPartArray(const size_t uiBlockId) { return CSyntaxParser::ParseArrayList(this->GetPartData(uiBlockId)); }
	std::vector<dnyString> ICodeContext::GetPartParamlist(const size_t uiBlockId) { return CSyntaxParser::ParseParamList(this->GetPartData(uiBlockId)); }
	void ICodeContext::ReplaceGlobalVariables(void* pInterfaceObject)
	{
		//Attempt to replace all global variables

		if (!pInterfaceObject)
			return;

		class CScriptingInterface* pScriptingInterface = (class CScriptingInterface*)pInterfaceObject;

		for (size_t i = 0; i < this->m_vCodeParts.size(); i++) {
			this->m_vCodeParts[i] = pScriptingInterface->ReplaceVariables(this->m_vCodeParts[i]);
		}
	}
	void ICodeContext::ReplaceLocalVariables(void* pInterfaceObject)
	{
		//Attempt to replace all global variables

		if (!pInterfaceObject)
			return;

		class CScriptingInterface* pScriptingInterface = (class CScriptingInterface*)pInterfaceObject;

		for (size_t i = 0; i < this->m_vCodeParts.size(); i++) {
			this->m_vCodeParts[i] = pScriptingInterface->ReplaceLocalFunctionVariables(this->m_vCodeParts[i]);
		}
	}
	void ICodeContext::ReplaceAllVariables(void* pInterfaceObject)
	{
		//Attempt to replace all variables

		this->ReplaceLocalVariables(pInterfaceObject);
		this->ReplaceGlobalVariables(pInterfaceObject);
	}

	CObjectMgr::CObjectMgr(class CScriptingInterface* pInterface) : m_pInterface(pInterface)
	{
		pObjectManagerInstance = this;

		this->m_sEventTable.pfnDeclareVar = &CTOBJ_DeclareVar;
		this->m_sEventTable.pfnAssignVarValue = &CTOBJ_AssignVarValue;
		this->m_sEventTable.pfnGetReplacerString = &CTOBJ_GetReplacerString;
		this->m_sEventTable.pfnRemoveVar = &CTOBJ_RemoveVar;

		this->m_pInterface->RegisterDataType(L"class", (CVarManager::custom_cvar_type_s::cvar_type_event_table_s*) & this->m_sEventTable);
	}

	bool CObjectMgr::AllocObject(const std::wstring& wszInstanceName, const std::wstring& wszObject)
	{
		//Allocate an instance of an object

		if ((!wszInstanceName.length()) || (!wszObject.length()))
			return false;

		//Check if the given name is already in use
		if (this->FindInstance(wszInstanceName))
			return false;

		//Attempt to get object scheme ID
		size_t uiObjId;
		if (!this->FindObject(wszObject, &uiObjId))
			return false;

		//Setup data struct
		objinstance_s sInstance;
		sInstance.wszName = wszInstanceName;

		//Add instance data to list
		this->m_vObjects[uiObjId].vInstances.push_back(sInstance);

		//Set current instance data pointer
		this->m_pCurrentInstance = &this->m_vObjects[uiObjId].vInstances[this->m_vObjects[uiObjId].vInstances.size() - 1];

		//Replace all this accessor tokens with object instance name token
		this->m_vObjects[uiObjId].wszBody = this->StrReplace(this->m_vObjects[uiObjId].wszBody, L"%this.", std::wstring(&wszInstanceName[1]) + L".");

		//Execute body code in order to add members and methods
		this->m_pInterface->ExecuteCode(this->m_vObjects[uiObjId].wszBody);

		//Call constructor method initiallly
		this->m_pInterface->ExecuteCode(L"call " + wszInstanceName + L".construct() => void;");

		//Clear current instance pointer
		this->m_pCurrentInstance = nullptr;

		return true;
	}

	bool CObjectMgr::AddMemberToCurrentObject(class ICodeContext* pCodeContext, void* pInterfaceObject)
	{
		//Add member to current object instance

		if ((!pCodeContext) || (!this->m_pCurrentInstance))
			return false;

		//Replace all variables
		pCodeContext->ReplaceAllVariables(pInterfaceObject);

		//Setup member variable name string with object instance name
		std::wstring wszVarName = std::wstring(&this->m_pCurrentInstance->wszName[1]) + L"." + pCodeContext->GetPartString(1);
		
		//Attempt to register member variable
		dnyScriptInterpreter::CVarManager::cvarptr_t pCVar = this->m_pInterface->RegisterCVar(wszVarName, this->TypeByName(pCodeContext->GetPartString(2)), false, false);
		if (!pCVar)
			return false;

		//Add to list
		this->m_pCurrentInstance->vMembers.push_back(wszVarName);

		return true;
	}

	bool CObjectMgr::AddMethodToCurrentObject(ICodeContext* pCodeContext, void* pInterfaceObject)
	{
		//Add method to current object instance

		if ((!pCodeContext) || (!this->m_pCurrentInstance))
			return false;

		//Setup method name string with object instance name
		std::wstring wszFuncName = this->m_pCurrentInstance->wszName + L"." + pCodeContext->GetPartString(1);

		//Setup registration code in order to register the object method
		std::wstring wszRegCode = L"function " + wszFuncName + L" " + pCodeContext->GetPartString(2) + L"(" + pCodeContext->GetPartString(3) + L"){" + pCodeContext->GetPartString(4) + L"};";

		//Execute registration code
		if (!this->m_pInterface->ExecuteCode(wszRegCode))
			return false;

		//Add to list
		this->m_pCurrentInstance->vMethods.push_back(wszFuncName);

		return true;
	}

	bool CObjectMgr::FreeObject(const std::wstring& wszInstanceName)
	{
		//Free an instantiated object

		idmap_s sIdMap;

		//Attempt to find instance
		if (!this->FindInstance(wszInstanceName, &sIdMap))
			return false;

		//Call destructor
		this->m_pInterface->ExecuteCode(L"call " + this->m_vObjects[sIdMap[0]].vInstances[sIdMap[1]].wszName + L".destruct() => void;");

		//Free all member  variables
		for (size_t i = 0; i < this->m_vObjects[sIdMap[0]].vInstances[sIdMap[1]].vMembers.size(); i++) {
			this->m_pInterface->FreeCVar(this->m_vObjects[sIdMap[0]].vInstances[sIdMap[1]].vMembers[i]);
		}

		//Free all methods
		for (size_t i = 0; i < this->m_vObjects[sIdMap[0]].vInstances[sIdMap[1]].vMethods.size(); i++) {
			this->m_pInterface->ExecuteCode(L"function " + this->m_vObjects[sIdMap[0]].vInstances[sIdMap[1]].vMethods[i] + L" void(){};");
		}

		//Remove from list
		this->m_vObjects[sIdMap[0]].vInstances.erase(this->m_vObjects[sIdMap[0]].vInstances.begin() + sIdMap[1]);

		return true;
	}

	bool CTOBJ_DeclareVar(const std::wstring& wszName, CVarManager::ICVar<dnyCustom>* pCVar)
	{
		//Declare class variable

		return true;
	}

	bool CTOBJ_AssignVarValue(const std::wstring& wszName, CVarManager::ICVar<dnyCustom>* pCVar, const CVarManager::ICustomVarValue& rCustomVarValue, bool bIsConst)
	{
		//Allocate class object

		if (bIsConst)
			return false;

		return pObjectManagerInstance->AllocObject(wszName, rCustomVarValue.QueryAsDnyString());
	}

	dnyString CTOBJ_GetReplacerString(const std::wstring& wszName, CVarManager::ICVar<dnyCustom>* pCVar)
	{
		//Return replacer string

		return wszName;
	}

	void CTOBJ_RemoveVar(const std::wstring& wszName, CVarManager::ICVar<dnyCustom>* pCVar)
	{
		//Free class instance

		pObjectManagerInstance->FreeObject(wszName);
	}

	CObjectMgr* pObjectManagerInstance;
}