#pragma once

#include "dnyas_sdk.h"

namespace Objects {
	bool CTOBJ_DeclareVar(const std::wstring& wszName, ICVar<dnycustom>* pCVar);
	bool CTOBJ_AssignVarValue(const std::wstring& wszName, ICVar<dnycustom>* pCVar, const ICustomVarValue& rCustomVarValue, bool bIsConst);
	dnyString CTOBJ_GetReplacerString(const std::wstring& wszName, ICVar<dnycustom>* pCVar);
	void CTOBJ_RemoveVar(const std::wstring& wszName, ICVar<dnycustom>* pCVar);

	IShellPluginAPI* pShellPluginAPI = nullptr;
	class CObjectMgr* pObjectMgr = nullptr;
	cvar_type_event_table_s sDataTypeEventTable = {
		&CTOBJ_DeclareVar,
		&CTOBJ_AssignVarValue,
		&CTOBJ_GetReplacerString,
		&CTOBJ_RemoveVar
	};

	class CObjectMgr { //Manager for objects
		struct objinstance_s { //Data related to an object instance
			std::wstring wszName;
			std::vector<std::wstring> vMembers;
			std::vector<std::wstring> vMethods;
		};

		struct object_s { //Data related to an object scheme
			std::wstring wszName;
			std::wstring wszBody;
			std::vector<objinstance_s> vInstances;
		};

		struct idmap_s {
			size_t map[2];

			inline size_t operator[](int iIndex) const { return map[iIndex]; }
			inline size_t& operator[](int iIndex) { return map[iIndex]; }
		};
	private:
		std::vector<object_s> m_vObjects;
		objinstance_s* m_pCurrentInstance;

		bool FindObject(const std::wstring& wszName, size_t* puiIdOut = nullptr)
		{
			//Find object scheme in list

			for (size_t i = 0; i < this->m_vObjects.size(); i++) {
				if (this->m_vObjects[i].wszName == wszName) {
					if (puiIdOut)
						*puiIdOut = i;

					return true;
				}
			}

			return false;
		}

		bool FindInstance(const std::wstring& wszName, idmap_s* pIdMap = nullptr)
		{
			//Find object instance from object 

			for (size_t i = 0; i < this->m_vObjects.size(); i++) {
				for (size_t j = 0; j < this->m_vObjects[i].vInstances.size(); j++) {
					if (this->m_vObjects[i].vInstances[j].wszName == wszName) {
						if (pIdMap) {
							pIdMap->map[0] = i;
							pIdMap->map[1] = j;
						}

						return true;
					}
				}
			}

			return false;
		}

		void Clear(void)
		{
			//Clear data

			this->m_vObjects.clear();
		}

		std::wstring StrReplace(const std::wstring& wszStr, const std::wstring& wszToken, const std::wstring& wszNewStr)
		{
			//Replace all tokens in a string

			std::wstring wszResult(wszStr);
			size_t uiStrPos = wszResult.find(wszToken);

			while (uiStrPos != std::wstring::npos) {
				wszResult.replace(uiStrPos, wszToken.length(), wszNewStr);
				uiStrPos = wszResult.find(wszToken);
			}

			return wszResult;
		}

		cvartype_e TypeByName(const std::wstring& wszName)
		{
			//Get type indicator by string

			cvartype_e eResult = CT_UNKNOWN;

			if (wszName == L"bool") {
				eResult = CT_BOOL;
			} else if (wszName == L"int") {
				eResult = CT_INT;
			} else if (wszName == L"float") {
				eResult = CT_FLOAT;
			} else if (wszName == L"string") {
				eResult = CT_STRING;
			}

			return eResult;
		}

		inline void Free(void)
		{
			//Cleanup resources

			this->Clear();
		}
	public:
		CObjectMgr() : m_pCurrentInstance(nullptr) { pObjectMgr = this; }
		~CObjectMgr() { this->Free(); }

		bool AddObject(const std::wstring& wszName, const std::wstring& wszBody)
		{
			//Add an object scheme to list

			if ((!wszName.length()) || (!wszBody.length()))
				return false;

			//Check if name is already in use
			if (this->FindObject(wszName))
				return false;

			//Setup data struct
			object_s sObject;
			sObject.wszName = wszName;
			sObject.wszBody = wszBody;

			//Add scheme data to list
			this->m_vObjects.push_back(sObject);

			return true;
		}

		bool AllocObject(const std::wstring& wszInstanceName, const std::wstring& wszObject)
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
			this->m_pCurrentInstance = &this->m_vObjects[uiObjId].vInstances[this->m_vObjects[uiObjId].vInstances.size()-1];

			//Replace all this accessor tokens with object instance name token
			this->m_vObjects[uiObjId].wszBody = this->StrReplace(this->m_vObjects[uiObjId].wszBody, L"%this.", wszInstanceName + L".");

			//Execute body code in order to add members and methods
			pShellPluginAPI->Scr_ExecuteCode(this->m_vObjects[uiObjId].wszBody);

			//Call constructor method initiallly
			pShellPluginAPI->Scr_ExecuteCode(L"call " + wszInstanceName + L".construct() => void;");

			//Clear current instance pointer
			this->m_pCurrentInstance = nullptr;

			return true;
		}

		bool AddMemberToCurrentObject(ICodeContext* pCodeContext, void* pInterfaceObject)
		{
			//Add member to current object instance

			if ((!pCodeContext) || (!this->m_pCurrentInstance))
				return false;

			//Replace all variables
			pCodeContext->ReplaceAllVariables(pInterfaceObject);

			//Setup member variable name string with object instance name
			std::wstring wszVarName = this->m_pCurrentInstance->wszName + L"." + pCodeContext->GetPartString(1);
			
			//Attempt to register member variable
			cvarptr_t pCVar = pShellPluginAPI->Cv_RegisterCVar(wszVarName, this->TypeByName(pCodeContext->GetPartString(2)));
			if (!pCVar)
				return false;

			//Add to list
			this->m_pCurrentInstance->vMembers.push_back(wszVarName);

			return true;
		}

		bool AddMethodToCurrentObject(ICodeContext* pCodeContext, void* pInterfaceObject)
		{
			//Add method to current object instance

			if ((!pCodeContext) || (!this->m_pCurrentInstance))
				return false;

			//Setup method name string with object instance name
			std::wstring wszFuncName = this->m_pCurrentInstance->wszName + L"." + pCodeContext->GetPartString(1);

			//Setup registration code in order to register the object method
			std::wstring wszRegCode = L"function " + wszFuncName + L" " + pCodeContext->GetPartString(2) + L"(" + pCodeContext->GetPartString(3) + L"){" + pCodeContext->GetPartString(4) + L"};";
			
			//Execute registration code
			if (!pShellPluginAPI->Scr_ExecuteCode(wszRegCode))
				return false;

			//Add to list
			this->m_pCurrentInstance->vMethods.push_back(wszFuncName);

			return true;
		}

		bool FreeObject(const std::wstring& wszInstanceName)
		{
			//Free an instantiated object

			idmap_s sIdMap;

			//Attempt to find instance
			if (!this->FindInstance(wszInstanceName, &sIdMap))
				return false;

			//Call destructor
			pShellPluginAPI->Scr_ExecuteCode(L"call " + this->m_vObjects[sIdMap[0]].vInstances[sIdMap[1]].wszName + L".destruct() => void;");

			//Free all member  variables
			for (size_t i = 0; i < this->m_vObjects[sIdMap[0]].vInstances[sIdMap[1]].vMembers.size(); i++) {
				pShellPluginAPI->Cv_FreeCVar(this->m_vObjects[sIdMap[0]].vInstances[sIdMap[1]].vMembers[i]);
			}

			//Free all methods
			for (size_t i = 0; i < this->m_vObjects[sIdMap[0]].vInstances[sIdMap[1]].vMethods.size(); i++) {
				pShellPluginAPI->Scr_ExecuteCode(L"function " + this->m_vObjects[sIdMap[0]].vInstances[sIdMap[1]].vMethods[i] + L" void(){};");
			}

			//Remove from list
			this->m_vObjects[sIdMap[0]].vInstances.erase(this->m_vObjects[sIdMap[0]].vInstances.begin() + sIdMap[1]);

			return true;
		}


	} oObjectMgr;

	/* Script command handlers */

	class IAddObjectCommandInterface : public IVoidCommandInterface {
	public:
		IAddObjectCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			return oObjectMgr.AddObject(pContext->GetPartString(1), pContext->GetPartString(2));
		}
	} oAddObjectCommandInterface;

	class IAddMemberCommandInterface : public IVoidCommandInterface {
	public:
		IAddMemberCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			return oObjectMgr.AddMemberToCurrentObject(pContext, pInterfaceObject);
		}
	} oAddMemberCommandInterface;

	class IAddMethodCommandInterface : public IVoidCommandInterface {
	public:
		IAddMethodCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			return oObjectMgr.AddMethodToCurrentObject(pContext, pInterfaceObject);
		}
	} oAddMethodCommandInterface;

	class IAllocObjectCommandInterface : public IVoidCommandInterface {
	public:
		IAllocObjectCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			return oObjectMgr.AllocObject(pContext->GetPartString(1), pContext->GetPartString(2));
		}
	} oAllocObjectCommandInterface;

	class IFreeObjectCommandInterface : public IVoidCommandInterface {
	public:
		IFreeObjectCommandInterface() {}

		virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
		{
			ICodeContext* pContext = (ICodeContext*)pCodeContext;

			return oObjectMgr.FreeObject(pContext->GetPartString(1));
		}
	} oFreeObjectCommandInterface;

	bool CTOBJ_DeclareVar(const std::wstring& wszName, ICVar<dnycustom>* pCVar)
	{
		if (wszName[0] != '@')
			return false;

		return true;
	}

	bool CTOBJ_AssignVarValue(const std::wstring& wszName, ICVar<dnycustom>* pCVar, const ICustomVarValue& rCustomVarValue, bool bIsConst)
	{
		if (bIsConst)
			return false;

		return oObjectMgr.AllocObject(wszName.substr(1), rCustomVarValue.QueryAsDnyString());
	}

	dnyString CTOBJ_GetReplacerString(const std::wstring& wszName, ICVar<dnycustom>* pCVar)
	{
		return wszName;
	}

	void CTOBJ_RemoveVar(const std::wstring& wszName, ICVar<dnycustom>* pCVar)
	{
		oObjectMgr.FreeObject(wszName.substr(1));
	}

	bool Initialize(IShellPluginAPI* pPluginAPI)
	{
		//Initialize component

		if (!pPluginAPI)
			return false;

		//Store API pointer
		pShellPluginAPI = pPluginAPI;

		//Register datatype
		if (!pShellPluginAPI->Cv_RegisterCVarType(L"object", &sDataTypeEventTable))
			return false;

		//Register commands
		pShellPluginAPI->Cmd_RegisterCommand(L"object", &oAddObjectCommandInterface, CT_VOID);
		pShellPluginAPI->Cmd_RegisterCommand(L"class", &oAddObjectCommandInterface, CT_VOID);
		pShellPluginAPI->Cmd_RegisterCommand(L"member", &oAddMemberCommandInterface, CT_VOID);
		pShellPluginAPI->Cmd_RegisterCommand(L"method", &oAddMethodCommandInterface, CT_VOID);
		pShellPluginAPI->Cmd_RegisterCommand(L"obj_alloc", &oAllocObjectCommandInterface, CT_VOID);
		pShellPluginAPI->Cmd_RegisterCommand(L"obj_free", &oFreeObjectCommandInterface, CT_VOID);

		return true;
	}

	bool Release(void)
	{
		//Release resources

		if (!pShellPluginAPI)
			return false;

		//Unregister commands
		pShellPluginAPI->Cmd_UnregisterCommand(L"object");
		pShellPluginAPI->Cmd_UnregisterCommand(L"class");
		pShellPluginAPI->Cmd_UnregisterCommand(L"member");
		pShellPluginAPI->Cmd_UnregisterCommand(L"method");
		pShellPluginAPI->Cmd_UnregisterCommand(L"obj_alloc");
		pShellPluginAPI->Cmd_UnregisterCommand(L"obj_free");

		//Clear pointer
		pShellPluginAPI = nullptr;

		return true;
	}
}