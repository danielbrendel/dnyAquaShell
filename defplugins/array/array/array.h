#pragma once

#include "dnyas_sdk.h"

namespace Array {
	IShellPluginAPI* pShellPluginAPI;
	ICVar<dnyInteger>* pCommonArrayIndex = nullptr;

	/* Static array manager */
	class CStaticArray {
	private:
		struct static_array_object {
			cvartype_e eType;
			std::wstring wszBaseName;
			std::vector<cvarptr_t> vpCVars;
		};

		std::vector<static_array_object> m_vStaticArrays;

		cvarptr_t RegisterArrayItem(const std::wstring& wszBaseName, const size_t uiIndex, const cvartype_e eType, const std::wstring& wszValue)
		{
			//Register an array item

			if (!wszBaseName.length())
				return nullptr;

			//Setup item var name
			std::wstring wszItemVarName = wszBaseName + L"[" + std::to_wstring(uiIndex) + L"]";

			//Check if name is already in use
			if (pShellPluginAPI->Cv_FindCVar(wszItemVarName))
				return nullptr;

			//Register CVar to engine
			cvarptr_t pCVar = pShellPluginAPI->Cv_RegisterCVar(wszItemVarName, eType);
			if (!pCVar)
				return nullptr;

			//Set var value
			switch (eType) {
			case CT_BOOL: {
				ICVar<dnyBoolean>* pBoolCVar = (ICVar<dnyBoolean>*)pCVar;
				pBoolCVar->SetValue((wszValue == L"true") ? true : false);
				break;
			}
			case CT_INT: {
				ICVar<dnyInteger>* pIntCVar = (ICVar<dnyInteger>*)pCVar;
				pIntCVar->SetValue(_wtoi64(wszValue.c_str()));
				break;
			}
			case CT_FLOAT: {
				ICVar<dnyFloat>* pFloatCVar = (ICVar<dnyFloat>*)pCVar;
				pFloatCVar->SetValue(_wtof(wszValue.c_str()));
				break;
			}
			case CT_STRING: {
				ICVar<dnyString>* pStringCVar = (ICVar<dnyString>*)pCVar;
				pStringCVar->SetValue(wszValue);
				break;
			}
			default: {
				pShellPluginAPI->Cv_FreeCVar(wszItemVarName);
				return nullptr;
				break;
			}
			}

			return pCVar;
		}

		bool FindArray(const std::wstring& wszBaseName, size_t* puiIndexOut = nullptr)
		{
			//Find array in list

			for (size_t i = 0; i < this->m_vStaticArrays.size(); i++) {
				if (this->m_vStaticArrays[i].wszBaseName == wszBaseName) {
					if (puiIndexOut)
						*puiIndexOut = i;

					return true;
				}
			}

			return false;
		}
	public:
		CStaticArray() {}
		~CStaticArray() { this->Clear(); }

		bool RegisterArray(const std::wstring& wszBaseName, const cvartype_e eType, const std::vector<std::wstring>& vItemValueList)
		{
			//Register array

			if ((!wszBaseName.length()) || (eType == CT_VOID) || (!vItemValueList.size()))
				return false;

			//Check if an array with given name already exists
			if (this->FindArray(wszBaseName))
				return false;

			//Setup data struct
			static_array_object sObject;
			sObject.wszBaseName = wszBaseName;
			sObject.eType = eType;

			//Register array items
			for (size_t i = 0; i < vItemValueList.size(); i++) {
				cvarptr_t pArrayItem = this->RegisterArrayItem(wszBaseName, i, eType, vItemValueList[i]);
				if (!pArrayItem)
					return false;

				sObject.vpCVars.push_back(pArrayItem);
			}

			//Register array length variable

			ICVar<dnyInteger>* pLenVar = (ICVar<dnyInteger>*)pShellPluginAPI->Cv_RegisterCVar(wszBaseName + L".length", CT_INT);
			if (!pLenVar)
				return false;

			pLenVar->SetValue(vItemValueList.size());

			//Add object to list
			this->m_vStaticArrays.push_back(sObject);

			return true;

		}

		bool StoreArrayItemValueToTarget(const std::wstring& wszBaseName, const size_t uiIndex, cvarptr_t pTargetVar)
		{
			//Store array item value to target variable

			if ((!wszBaseName.length()) || (!pTargetVar))
				return false;

			size_t uiArrayId;
			
			//Find array object
			if (!this->FindArray(wszBaseName, &uiArrayId))
				return false;

			//Check index with size
			if (uiIndex >= this->m_vStaticArrays[uiArrayId].vpCVars.size())
				return false;

			//Store value
			switch (this->m_vStaticArrays[uiArrayId].eType) {
			case CT_BOOL: {
				ICVar<dnyBoolean>* pTargetCVar = (ICVar<dnyBoolean>*)pTargetVar;
				ICVar<dnyBoolean>* pItemVar = (ICVar<dnyBoolean>*)this->m_vStaticArrays[uiArrayId].vpCVars[uiIndex];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_INT: {
				ICVar<dnyInteger>* pTargetCVar = (ICVar<dnyInteger>*)pTargetVar;
				ICVar<dnyInteger>* pItemVar = (ICVar<dnyInteger>*)this->m_vStaticArrays[uiArrayId].vpCVars[uiIndex];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_FLOAT: {
				ICVar<dnyFloat>* pTargetCVar = (ICVar<dnyFloat>*)pTargetVar;
				ICVar<dnyFloat>* pItemVar = (ICVar<dnyFloat>*)this->m_vStaticArrays[uiArrayId].vpCVars[uiIndex];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_STRING: {
				ICVar<dnyString>* pTargetCVar = (ICVar<dnyString>*)pTargetVar;
				ICVar<dnyString>* pItemVar = (ICVar<dnyString>*)this->m_vStaticArrays[uiArrayId].vpCVars[uiIndex];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			default:
				return false;
			}

			return true;
		}

		bool StoreArrayItemValueToTarget(const std::wstring& wszBaseName, const std::wstring& wszIndexVar, cvarptr_t pTargetVar)
		{
			//Store array item value to target variable

			if ((!wszBaseName.length()) || (!pTargetVar))
				return false;

			size_t uiArrayId;

			//Find array object
			if (!this->FindArray(wszBaseName, &uiArrayId))
				return false;

			//Get index variable
			ICVar<dnyInteger>* pIndexCVar = (ICVar<dnyInteger>*)pShellPluginAPI->Cv_FindCVar(wszIndexVar);
			if (!pIndexCVar)
				return false;

			//Check index with size
			if ((size_t)pIndexCVar->GetValue() >= this->m_vStaticArrays[uiArrayId].vpCVars.size())
				return false;

			//Store value
			switch (this->m_vStaticArrays[uiArrayId].eType) {
			case CT_BOOL: {
				ICVar<dnyBoolean>* pTargetCVar = (ICVar<dnyBoolean>*)pTargetVar;
				ICVar<dnyBoolean>* pItemVar = (ICVar<dnyBoolean>*)this->m_vStaticArrays[uiArrayId].vpCVars[(size_t)pIndexCVar->GetValue()];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_INT: {
				ICVar<dnyInteger>* pTargetCVar = (ICVar<dnyInteger>*)pTargetVar;
				ICVar<dnyInteger>* pItemVar = (ICVar<dnyInteger>*)this->m_vStaticArrays[uiArrayId].vpCVars[(size_t)pIndexCVar->GetValue()];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_FLOAT: {
				ICVar<dnyFloat>* pTargetCVar = (ICVar<dnyFloat>*)pTargetVar;
				ICVar<dnyFloat>* pItemVar = (ICVar<dnyFloat>*)this->m_vStaticArrays[uiArrayId].vpCVars[(size_t)pIndexCVar->GetValue()];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_STRING: {
				ICVar<dnyString>* pTargetCVar = (ICVar<dnyString>*)pTargetVar;
				ICVar<dnyString>* pItemVar = (ICVar<dnyString>*)this->m_vStaticArrays[uiArrayId].vpCVars[(size_t)pIndexCVar->GetValue()];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			default:
				return false;
			}

			return true;
		}

		bool RemoveArray(const std::wstring& wszBaseName)
		{
			//Remove array

			size_t uiListId;

			//Find in list
			if (!this->FindArray(wszBaseName, &uiListId))
				return false;

			//Remove engine variables
			for (size_t i = 0; i < this->m_vStaticArrays[uiListId].vpCVars.size(); i++)
				pShellPluginAPI->Cv_FreeCVar(this->m_vStaticArrays[uiListId].wszBaseName + L"[" + std::to_wstring(i) + L"]");

			//Remove length variable
			pShellPluginAPI->Cv_FreeCVar(this->m_vStaticArrays[uiListId].wszBaseName + L".length");

			//Remove from list
			this->m_vStaticArrays.erase(this->m_vStaticArrays.begin() + uiListId);

			return true;
		}

		void Clear(void)
		{
			//Clear resources

			for (size_t i = 0; i < this->m_vStaticArrays.size(); i++) {
				for (size_t j = 0; j < this->m_vStaticArrays[i].vpCVars.size(); j++)
					pShellPluginAPI->Cv_FreeCVar(this->m_vStaticArrays[i].wszBaseName + L"[" + std::to_wstring(j) + L"]");

				pShellPluginAPI->Cv_FreeCVar(this->m_vStaticArrays[i].wszBaseName + L".length");
			}

			this->m_vStaticArrays.clear();
		}
	} oStaticArray;

	class IAddStaticArrayCommand : public IVoidCommandInterface {
	public:
		IAddStaticArrayCommand() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			std::wstring wszArrayName = pCodeContext->GetPartString(1);
			if (!wszArrayName.length())
				return false;

			std::wstring wszDataType = pCodeContext->GetPartString(2);
			if (!wszDataType.length())
				return false;

			std::vector<std::wstring> vItemList = pCodeContext->GetPartArray(3);
			if (!vItemList.size())
				return false;

			cvartype_e eType = CT_VOID;

			if (wszDataType == L"bool") {
				eType = CT_BOOL;
			}
			else if (wszDataType == L"int") {
				eType = CT_INT;
			}
			else if (wszDataType == L"float") {
				eType = CT_FLOAT;
			}
			else if (wszDataType == L"string") {
				eType = CT_STRING;
			}
			else {
				return false;
			}

			return oStaticArray.RegisterArray(wszArrayName, eType, vItemList);
		}

	} oAddStaticArrayVoidCommand;

	class IStoreStaticArrayItemCommand : public IVoidCommandInterface {
	public:
		IStoreStaticArrayItemCommand() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			std::vector<dnyString> vParams = pCodeContext->GetPartArray(1);
			if (vParams.size() != 3)
				return false;

			cvarptr_t pTargetVar = pShellPluginAPI->Cv_FindCVar(vParams[2]);

			return oStaticArray.StoreArrayItemValueToTarget(vParams[0], vParams[1], pTargetVar);
		}

	} oStoreStaticArrayItemVoidCommand;

	class IRemoveStaticArrayCommand : public IVoidCommandInterface {
	public:
		IRemoveStaticArrayCommand() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			return oStaticArray.RemoveArray(pCodeContext->GetPartString(1));
		}

	} oRemoveStaticArrayVoidCommand;

	class CDynamicArray {
	public:
		struct dynamic_array_object {
			std::wstring wszBaseName;
			cvartype_e eType;
			std::vector<cvarptr_t> vArrayItems;
			ICVar<dnyInteger>* pLenVar;
		};
	private:
		std::vector<dynamic_array_object> m_vObjects;

		bool FindArray(const std::wstring& wszBaseName, size_t* pIdOut = nullptr)
		{
			//Find array in list

			for (size_t i = 0; i < this->m_vObjects.size(); i++) {
				if (this->m_vObjects[i].wszBaseName == wszBaseName) {
					if (pIdOut)
						*pIdOut = i;

					return true;
				}
			}

			return false;
		}

		void Clear(void)
		{
			//Clear resources

			//Free array resources
			for (size_t i = 0; i < this->m_vObjects.size(); i++) {
				for (size_t j = 0; j < this->m_vObjects[i].vArrayItems.size(); j++) {
					pShellPluginAPI->Cv_FreeCVar(this->m_vObjects[i].wszBaseName + L"[" + std::to_wstring(j) + L"]");
				}

				pShellPluginAPI->Cv_FreeCVar(this->m_vObjects[i].wszBaseName + L".length");
			}

			//Clear list
			this->m_vObjects.clear();
		}
	public:
		CDynamicArray() {}
		~CDynamicArray() { this->Clear(); }

		bool RegisterArray(const std::wstring& wszBaseName, const cvartype_e eType, const size_t uiBaseEntries, const std::vector<std::wstring>& vInitalList)
		{
			//Register an array
			
			if (!wszBaseName.size())
				return false;
			
			//Check if name is already in use
			if (this->FindArray(wszBaseName))
				return false;
			
			//Check cvar type
			if (eType == CT_VOID)
				return false;
			
			//Ensure that list of initial items equals the amount of initial array items if the initial item value list is provided
			if ((uiBaseEntries) && (vInitalList.size())) {
				if (uiBaseEntries != vInitalList.size()) {
					return false;
				}
			}

			//Setup data object
			dynamic_array_object sObject;
			sObject.wszBaseName = wszBaseName;
			sObject.eType = eType;

			//Register initial array items
			for (size_t i = 0; i < uiBaseEntries; i++) {
				cvarptr_t pCVar = pShellPluginAPI->Cv_RegisterCVar(wszBaseName + L"[" + std::to_wstring(i) + L"]", eType);
				if (!pCVar)
					return false;

				//Set initial item values if desired
				if (vInitalList.size()) {
					switch (eType) {
					case CT_BOOL: {
						ICVar<dnyBoolean>* pVar = (ICVar<dnyBoolean>*)pCVar;
						pVar->SetValue((vInitalList[i] == L"true") ? true : false);
						break;
					}
					case CT_INT: {
						ICVar<dnyInteger>* pVar = (ICVar<dnyInteger>*)pCVar;
						pVar->SetValue(_wtoi64(vInitalList[i].c_str()));
						break;
					}
					case CT_FLOAT: {
						ICVar<dnyFloat>* pVar = (ICVar<dnyFloat>*)pCVar;
						pVar->SetValue(_wtof(vInitalList[i].c_str()));
						break;
					}
					case CT_STRING: {
						ICVar<dnyString>* pVar = (ICVar<dnyString>*)pCVar;
						pVar->SetValue(vInitalList[i]);
						break;
					}
					default:
						return false;
					}
				}

				//Add to list
				sObject.vArrayItems.push_back(pCVar);
			}
			
			//Register array length variable

			ICVar<dnyInteger>* pLenVar = (ICVar<dnyInteger>*)pShellPluginAPI->Cv_RegisterCVar(wszBaseName + L".length", CT_INT);
			if (!pLenVar)
				return false;

			pLenVar->SetValue(uiBaseEntries);
			sObject.pLenVar = pLenVar;

			//Add to list
			this->m_vObjects.push_back(sObject);

			return true;
		}

		const dynamic_array_object* GetArrayData(const std::wstring& wszBaseName) const
		{
			//Find array in list

			for (size_t i = 0; i < this->m_vObjects.size(); i++) {
				if (this->m_vObjects[i].wszBaseName == wszBaseName) {
					return &this->m_vObjects[i];
				}
			}

			return nullptr;
		}

		bool StoreArrayItemValueToTarget(const std::wstring& wszBaseName, const size_t uiIndex, cvarptr_t pTargetVar)
		{
			//Store array item value to target variable

			if ((!wszBaseName.length()) || (!pTargetVar))
				return false;

			size_t uiArrayId;

			//Find array object
			if (!this->FindArray(wszBaseName, &uiArrayId))
				return false;

			//Check index with size
			if (uiIndex >= this->m_vObjects[uiArrayId].vArrayItems.size())
				return false;

			//Store value
			switch (this->m_vObjects[uiArrayId].eType) {
			case CT_BOOL: {
				ICVar<dnyBoolean>* pTargetCVar = (ICVar<dnyBoolean>*)pTargetVar;
				ICVar<dnyBoolean>* pItemVar = (ICVar<dnyBoolean>*)this->m_vObjects[uiArrayId].vArrayItems[uiIndex];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_INT: {
				ICVar<dnyInteger>* pTargetCVar = (ICVar<dnyInteger>*)pTargetVar;
				ICVar<dnyInteger>* pItemVar = (ICVar<dnyInteger>*)this->m_vObjects[uiArrayId].vArrayItems[uiIndex];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_FLOAT: {
				ICVar<dnyFloat>* pTargetCVar = (ICVar<dnyFloat>*)pTargetVar;
				ICVar<dnyFloat>* pItemVar = (ICVar<dnyFloat>*)this->m_vObjects[uiArrayId].vArrayItems[uiIndex];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_STRING: {
				ICVar<dnyString>* pTargetCVar = (ICVar<dnyString>*)pTargetVar;
				ICVar<dnyString>* pItemVar = (ICVar<dnyString>*)this->m_vObjects[uiArrayId].vArrayItems[uiIndex];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			default:
				return false;
			}

			return true;
		}

		bool StoreArrayItemValueToTarget(const std::wstring& wszBaseName, const std::wstring& wszIndexVar, cvarptr_t pTargetVar)
		{
			//Store array item value to target variable

			if ((!wszBaseName.length()) || (!pTargetVar))
				return false;

			size_t uiArrayId;

			//Find array object
			if (!this->FindArray(wszBaseName, &uiArrayId))
				return false;

			//Get index variable
			ICVar<dnyInteger>* pIndexCVar = (ICVar<dnyInteger>*)pShellPluginAPI->Cv_FindCVar(wszIndexVar);
			if (!pIndexCVar)
				return false;

			//Check index with size
			if ((size_t)pIndexCVar->GetValue() >= this->m_vObjects[uiArrayId].vArrayItems.size())
				return false;

			//Store value
			switch (this->m_vObjects[uiArrayId].eType) {
			case CT_BOOL: {
				ICVar<dnyBoolean>* pTargetCVar = (ICVar<dnyBoolean>*)pTargetVar;
				ICVar<dnyBoolean>* pItemVar = (ICVar<dnyBoolean>*)this->m_vObjects[uiArrayId].vArrayItems[(size_t)pIndexCVar->GetValue()];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_INT: {
				ICVar<dnyInteger>* pTargetCVar = (ICVar<dnyInteger>*)pTargetVar;
				ICVar<dnyInteger>* pItemVar = (ICVar<dnyInteger>*)this->m_vObjects[uiArrayId].vArrayItems[(size_t)pIndexCVar->GetValue()];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_FLOAT: {
				ICVar<dnyFloat>* pTargetCVar = (ICVar<dnyFloat>*)pTargetVar;
				ICVar<dnyFloat>* pItemVar = (ICVar<dnyFloat>*)this->m_vObjects[uiArrayId].vArrayItems[(size_t)pIndexCVar->GetValue()];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			case CT_STRING: {
				ICVar<dnyString>* pTargetCVar = (ICVar<dnyString>*)pTargetVar;
				ICVar<dnyString>* pItemVar = (ICVar<dnyString>*)this->m_vObjects[uiArrayId].vArrayItems[(size_t)pIndexCVar->GetValue()];
				pTargetCVar->SetValue(pItemVar->GetValue());
				break;
			}
			default:
				return false;
			}

			return true;
		}

		bool SaveToArrayItem(cvarptr_t pSourceVar, const std::wstring& wszBaseName, const size_t uiIndex)
		{
			//Store source value to array item

			if ((!wszBaseName.length()) || (!pSourceVar))
				return false;

			size_t uiArrayId;

			//Find array object
			if (!this->FindArray(wszBaseName, &uiArrayId))
				return false;

			//Check index with size
			if (uiIndex >= this->m_vObjects[uiArrayId].vArrayItems.size())
				return false;

			//Store value
			switch (this->m_vObjects[uiArrayId].eType) {
			case CT_BOOL: {
				ICVar<dnyBoolean>* pSourceCVar = (ICVar<dnyBoolean>*)pSourceVar;
				ICVar<dnyBoolean>* pItemVar = (ICVar<dnyBoolean>*)this->m_vObjects[uiArrayId].vArrayItems[uiIndex];
				pItemVar->SetValue(pSourceCVar->GetValue());
				break;
			}
			case CT_INT: {
				ICVar<dnyInteger>* pSourceCVar = (ICVar<dnyInteger>*)pSourceVar;
				ICVar<dnyInteger>* pItemVar = (ICVar<dnyInteger>*)this->m_vObjects[uiArrayId].vArrayItems[uiIndex];
				pItemVar->SetValue(pSourceCVar->GetValue());
				break;
			}
			case CT_FLOAT: {
				ICVar<dnyFloat>* pSourceCVar = (ICVar<dnyFloat>*)pSourceVar;
				ICVar<dnyFloat>* pItemVar = (ICVar<dnyFloat>*)this->m_vObjects[uiArrayId].vArrayItems[uiIndex];
				pItemVar->SetValue(pSourceCVar->GetValue());
				break;
			}
			case CT_STRING: {
				ICVar<dnyString>* pSourceCVar = (ICVar<dnyString>*)pSourceVar;
				ICVar<dnyString>* pItemVar = (ICVar<dnyString>*)this->m_vObjects[uiArrayId].vArrayItems[uiIndex];
				pItemVar->SetValue(pSourceCVar->GetValue());
				break;
			}
			default:
				return false;
			}

			return true;
		}

		bool CopyArrayItem(const std::wstring& wszBaseName1, const size_t uiIndex1, const std::wstring& wszBaseName2, const size_t uiIndex2)
		{
			//Copy array value from one array item to another array item

			if ((!wszBaseName1.length()) || (!wszBaseName2.length()))
				return false;

			size_t uiArrayId1, uiArrayId2;

			//Find first array object
			if (!this->FindArray(wszBaseName1, &uiArrayId1))
				return false;

			//Find second array object
			if (!this->FindArray(wszBaseName2, &uiArrayId2))
				return false;

			//Check indices with size
			if (uiIndex1 >= this->m_vObjects[uiArrayId1].vArrayItems.size()) return false;
			if (uiIndex2 >= this->m_vObjects[uiArrayId2].vArrayItems.size()) return false;

			//Check type
			if (this->m_vObjects[uiArrayId1].eType != this->m_vObjects[uiArrayId2].eType)
				return false;

			//Store value
			switch (this->m_vObjects[uiArrayId1].eType) {
			case CT_BOOL: {
				ICVar<dnyBoolean>* pSourceCVar = (ICVar<dnyBoolean>*)this->m_vObjects[uiArrayId1].vArrayItems[uiIndex1];
				ICVar<dnyBoolean>* pTargetCVar = (ICVar<dnyBoolean>*)this->m_vObjects[uiArrayId2].vArrayItems[uiIndex2];
				pTargetCVar->SetValue(pSourceCVar->GetValue());
				break;
			}
			case CT_INT: {
				ICVar<dnyInteger>* pSourceCVar = (ICVar<dnyInteger>*)this->m_vObjects[uiArrayId1].vArrayItems[uiIndex1];
				ICVar<dnyInteger>* pTargetCVar = (ICVar<dnyInteger>*)this->m_vObjects[uiArrayId2].vArrayItems[uiIndex2];
				pTargetCVar->SetValue(pSourceCVar->GetValue());
				break;
			}
			case CT_FLOAT: {
				ICVar<dnyFloat>* pSourceCVar = (ICVar<dnyFloat>*)this->m_vObjects[uiArrayId1].vArrayItems[uiIndex1];
				ICVar<dnyFloat>* pTargetCVar = (ICVar<dnyFloat>*)this->m_vObjects[uiArrayId2].vArrayItems[uiIndex2];
				pTargetCVar->SetValue(pSourceCVar->GetValue());
				break;
			}
			case CT_STRING: {
				ICVar<dnyString>* pSourceCVar = (ICVar<dnyString>*)this->m_vObjects[uiArrayId1].vArrayItems[uiIndex1];
				ICVar<dnyString>* pTargetCVar = (ICVar<dnyString>*)this->m_vObjects[uiArrayId2].vArrayItems[uiIndex2];
				pTargetCVar->SetValue(pSourceCVar->GetValue());
				break;
			}
			default:
				return false;
			}

			return true;
		}

		bool RemoveArray(const std::wstring& wszBaseName)
		{
			//Remove an array

			size_t uiArrayId;
			if (this->FindArray(wszBaseName, &uiArrayId)) {
				for (size_t i = 0; i < this->m_vObjects[uiArrayId].vArrayItems.size(); i++) {
					pShellPluginAPI->Cv_FreeCVar(this->m_vObjects[uiArrayId].wszBaseName + L"[" + std::to_wstring(i) + L"]");
				}

				pShellPluginAPI->Cv_FreeCVar(this->m_vObjects[uiArrayId].wszBaseName + L".length");

				this->m_vObjects.erase(this->m_vObjects.begin() + uiArrayId);

				return true;
			}

			return false;
		}

		bool Resize(const std::wstring& wszBaseName, const size_t uiSize)
		{
			//Resize arrray

			size_t uiArrayId;
			
			//Find array
			if (!this->FindArray(wszBaseName, &uiArrayId))
				return false;

			//Get array cvar type
			cvartype_e eType = this->m_vObjects[uiArrayId].eType;
			
			//Remove and re-register array with new count

			if (!this->RemoveArray(wszBaseName))
				return false;
			
			return this->RegisterArray(wszBaseName, eType, uiSize, std::vector<std::wstring>());
		}

		bool Insert(const std::wstring& wszBaseName, const dnyInteger iIndex, const std::wstring& wszExpression)
		{
			//Insert item into array

			size_t uiArrayId;

			//Find array
			if (!this->FindArray(wszBaseName, &uiArrayId))
				return false;
			
			//Create new CVar for last item
			cvarptr_t pItemCVar = pShellPluginAPI->Cv_RegisterCVar(wszBaseName + L"[" + std::to_wstring(this->m_vObjects[uiArrayId].vArrayItems.size()) + L"]", this->m_vObjects[uiArrayId].eType);
			if (!pItemCVar)
				return false;

			//Add cvar to list
			this->m_vObjects[uiArrayId].vArrayItems.push_back(pItemCVar);

			//Move items one step forward at index
			for (size_t i = this->m_vObjects[uiArrayId].vArrayItems.size() - 1; i > (size_t)iIndex; i--) {
				this->StoreArrayItemValueToTarget(wszBaseName, i - 1, this->m_vObjects[uiArrayId].vArrayItems[i]);
			}

			//Set new expression
			switch (this->m_vObjects[uiArrayId].eType) {
			case CT_BOOL: {
				ICVar<dnyBoolean>* pTargetCVar = (ICVar<dnyBoolean>*)this->m_vObjects[uiArrayId].vArrayItems[(size_t)iIndex];
				pTargetCVar->SetValue((wszExpression == L"true") ? true : false);
				break;
			}
			case CT_INT: {
				ICVar<dnyInteger>* pTargetCVar = (ICVar<dnyInteger>*)this->m_vObjects[uiArrayId].vArrayItems[(size_t)iIndex];
				pTargetCVar->SetValue(_wtoi64(wszExpression.c_str()));
				break;
			}
			case CT_FLOAT: {
				ICVar<dnyFloat>* pTargetCVar = (ICVar<dnyFloat>*)this->m_vObjects[uiArrayId].vArrayItems[(size_t)iIndex];
				pTargetCVar->SetValue(_wtof(wszExpression.c_str()));
				break;
			}
			case CT_STRING: {
				ICVar<dnyString>* pTargetCVar = (ICVar<dnyString>*)this->m_vObjects[uiArrayId].vArrayItems[(size_t)iIndex];
				pTargetCVar->SetValue(wszExpression);
				break;
			}
			default:
				return false;
			}

			//Update length value
			this->m_vObjects[uiArrayId].pLenVar->SetValue(this->m_vObjects[uiArrayId].pLenVar->GetValue() + 1);

			return true;
		}

		bool Append(const std::wstring& wszBaseName, const std::wstring& wszExpression)
		{
			//Append new value item

			size_t uiArrayId;

			//Find array
			if (!this->FindArray(wszBaseName, &uiArrayId))
				return false;

			//Append item
			return this->Insert(wszBaseName, this->m_vObjects[uiArrayId].vArrayItems.size(), wszExpression);
		}
	} oDynamicArray;

	class IRegisterDynamicArray : public IVoidCommandInterface {
	public:
		IRegisterDynamicArray() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			std::wstring wszArrayName = pCodeContext->GetPartString(1);
			if (!wszArrayName.length())
				return false;

			std::wstring wszDataType = pCodeContext->GetPartString(2);
			if (!wszDataType.length())
				return false;

			dnyInteger iInitialSize = pCodeContext->GetPartInt(3);

			std::vector<std::wstring> vItemList = pCodeContext->GetPartArray(4);
			if (!vItemList.size())
				return false;

			cvartype_e eType = CT_VOID;

			if (wszDataType == L"bool") {
				eType = CT_BOOL;
			}
			else if (wszDataType == L"int") {
				eType = CT_INT;
			}
			else if (wszDataType == L"float") {
				eType = CT_FLOAT;
			}
			else if (wszDataType == L"string") {
				eType = CT_STRING;
			}
			else {
				return false;
			}

			return oDynamicArray.RegisterArray(wszArrayName, eType, iInitialSize, vItemList);
		}

	} oRegisterDynamicArray;

	class ISetDynamicArrayItem : public IVoidCommandInterface {
	public:
		ISetDynamicArrayItem() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			cvarptr_t pTargetVar = pShellPluginAPI->Cv_FindCVar(pCodeContext->GetPartString(3));

			return oDynamicArray.StoreArrayItemValueToTarget(pCodeContext->GetPartString(1), pCodeContext->GetPartInt(2), pTargetVar);
		}

	} oSetDynamicArrayItem;

	class ISaveToDynamicArrayItem : public IVoidCommandInterface {
	public:
		ISaveToDynamicArrayItem() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			cvarptr_t pSourceVar = pShellPluginAPI->Cv_FindCVar(pCodeContext->GetPartString(1));

			return oDynamicArray.SaveToArrayItem(pSourceVar, pCodeContext->GetPartString(2), pCodeContext->GetPartInt(3));
		}

	} oSaveToDynamicArrayItem;

	class ICopyDynamicArrayItem : public IVoidCommandInterface {
	public:
		ICopyDynamicArrayItem() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			return oDynamicArray.CopyArrayItem(pCodeContext->GetPartString(1), pCodeContext->GetPartInt(2), pCodeContext->GetPartString(3), pCodeContext->GetPartInt(4));
		}

	} oCopyDynamicArrayItem;

	class IResizeDynamicArray : public IVoidCommandInterface {
	public:
		IResizeDynamicArray() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			return oDynamicArray.Resize(pCodeContext->GetPartString(1), pCodeContext->GetPartInt(2));
		}

	} oResizeDynamicArray;

	class IInsertItem : public IVoidCommandInterface {
	public:
		IInsertItem() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			return oDynamicArray.Insert(pCodeContext->GetPartString(1), pCodeContext->GetPartInt(2), pCodeContext->GetPartString(3));
		}

	} oInsertItem;

	class IAppendItem : public IVoidCommandInterface {
	public:
		IAppendItem() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			return oDynamicArray.Append(pCodeContext->GetPartString(1), pCodeContext->GetPartString(2));
		}

	} oAppendItem;

	class IRemoveDynamicArray : public IVoidCommandInterface {
	public:
		IRemoveDynamicArray() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			return oDynamicArray.RemoveArray(pCodeContext->GetPartString(1));
		}

	} oRemoveDynamicArray;

	class ICommonArraySetIndex : public IVoidCommandInterface {
	public:
		ICommonArraySetIndex() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			pCommonArrayIndex->SetValue(pCodeContext->GetPartInt(1));

			return true;
		}

	} oCommonArraySetIndex;

	class IForEach : public IVoidCommandInterface {
	public:
		IForEach() {}

		virtual bool CommandCallback(void* pArg1, void* pArg2)
		{
			ICodeContext* pCodeContext = (ICodeContext*)pArg1;

			pCodeContext->ReplaceAllVariables(pArg2);

			std::vector<dnyString> vArgs = pCodeContext->GetPartArray(1);
			if (vArgs.size() != 2)
				return false;

			const CDynamicArray::dynamic_array_object* pData = oDynamicArray.GetArrayData(vArgs[1]);
			if (!pData)
				return false;

			cvarptr_t pCvar = pShellPluginAPI->Cv_RegisterCVar(vArgs[0], pData->eType);
			if (!pCvar)
				return false;

			for (size_t i = 0; i < pData->vArrayItems.size(); i++) {
				switch (pData->eType) {
				case CT_BOOL: {
					ICVar<dnyBoolean>* pVar = (ICVar<dnyBoolean>*)pCvar;
					pVar->SetValue(((ICVar<dnyBoolean>*)pData->vArrayItems[i])->GetValue());
					break;
				}
				case CT_INT: {
					ICVar<dnyInteger>* pVar = (ICVar<dnyInteger>*)pCvar;
					pVar->SetValue(((ICVar<dnyInteger>*)pData->vArrayItems[i])->GetValue());
					break;
				}
				case CT_FLOAT: {
					ICVar<dnyFloat>* pVar = (ICVar<dnyFloat>*)pCvar;
					pVar->SetValue(((ICVar<dnyFloat>*)pData->vArrayItems[i])->GetValue());
					break;
				}
				case CT_STRING: {
					ICVar<dnyString>* pVar = (ICVar<dnyString>*)pCvar;
					pVar->SetValue(((ICVar<dnyString>*)pData->vArrayItems[i])->GetValue());
					break;
				}
				default:
					return false;
				}

				pShellPluginAPI->Scr_ExecuteCode(pCodeContext->GetPartString(2));
			}

			pShellPluginAPI->Cv_FreeCVar(vArgs[0]);

			return true;
		}

	} oForEach;

	bool Initialize(IShellPluginAPI* pInstance)
	{
		//Initialize Array interface

		if (!pInstance)
			return false;

		//Save instance pointer
		pShellPluginAPI = pInstance;

		//Register commands
		#define REG_CMD(n, ptr, type) if (!pShellPluginAPI->Cmd_RegisterCommand(n, ptr, type)) return false;
		REG_CMD(L"static_array", &oAddStaticArrayVoidCommand, CT_VOID);
		REG_CMD(L"store_array_item_s", &oStoreStaticArrayItemVoidCommand, CT_VOID);
		REG_CMD(L"free_array_s", &oRemoveStaticArrayVoidCommand, CT_VOID);
		REG_CMD(L"dynamic_array", &oRegisterDynamicArray, CT_VOID);
		REG_CMD(L"store_array_item_d", &oSetDynamicArrayItem, CT_VOID);
		REG_CMD(L"store_item_to_array_d", &oSaveToDynamicArrayItem, CT_VOID);
		REG_CMD(L"copy_array_item_d", &oCopyDynamicArrayItem, CT_VOID);
		REG_CMD(L"resize_array", &oResizeDynamicArray, CT_VOID);
		REG_CMD(L"item_insert", &oInsertItem, CT_VOID);
		REG_CMD(L"item_append", &oAppendItem, CT_VOID);
		REG_CMD(L"free_array_d", &oRemoveDynamicArray, CT_VOID);
		REG_CMD(L"asetindex", &oCommonArraySetIndex, CT_VOID);
		REG_CMD(L"foreach", &oForEach, CT_VOID);

		//Register index variable
		pCommonArrayIndex = (ICVar<dnyInteger>*)pShellPluginAPI->Cv_RegisterCVar(L"array_index", CT_INT);
		if (!pCommonArrayIndex)
			return false;

		return true;
	}

	bool Free(void)
	{
		//Free resources

		//Unegister commands
		#define UNREG_CMD(n) if (!pShellPluginAPI->Cmd_UnregisterCommand(n)) return false;
		UNREG_CMD(L"static_array");
		UNREG_CMD(L"store_array_item_s");
		UNREG_CMD(L"free_array_s");
		UNREG_CMD(L"dynamic_array");
		UNREG_CMD(L"store_array_item_d");
		UNREG_CMD(L"store_item_to_array_d");
		UNREG_CMD(L"copy_array_item_d");
		UNREG_CMD(L"resize_array");
		UNREG_CMD(L"item_insert");
		UNREG_CMD(L"item_append");
		UNREG_CMD(L"free_array_d");
		UNREG_CMD(L"asetindex");
		UNREG_CMD(L"foreach");

		//Undeclare index variable
		pShellPluginAPI->Cv_FreeCVar(L"array_index");

		//Clear class object resources
		oStaticArray.Clear();

		return true;
	}
}