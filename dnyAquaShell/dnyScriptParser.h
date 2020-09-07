#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

/*
	dnyScriptParser developed by Daniel Brendel

	(C) 2017-2020 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace dnyScriptParser {
	//About information
	#define DNY_PRODUCT_NAME L"dynScriptParser"
	#define DNY_PRODUCT_VERSION L"1.0"
	#define DNY_PRODUCT_AUTHOR L"Daniel Brendel"
	#define DNY_PRODUCT_CONTACT L"dbrendel1988<at>gmail<dot>com"
	//Bracket start and end indices
	#define DNY_BRACKET_START 0
	#define DNY_BRACKET_END 1
	//Max line buffer size
	#define DNY_MAX_LINE_BUFFER_SIZE 2048

	//Var types
	typedef void* dnyVoid;
	typedef bool dnyBoolean;
	typedef __int64 dnyInteger;
	typedef double dnyFloat;
	typedef std::wstring dnyString;
	typedef std::wstring dnycustom;

	//Syntax relevant characters
	const wchar_t dnycommentChar = '#';
	const wchar_t dnySpaceCharacter = ' ';
	const wchar_t dnyTabCharacter = '\t';
	const wchar_t dnyVariableEntrance = '%';
	const wchar_t dnyFuncCallEntrance = '$';
	const wchar_t dnyListSeparator = ',';
	const wchar_t dnycodeContextDelimiter = ';';
	const wchar_t dnyArgumentContainer[] = { '(', ')' };
	const wchar_t dnycodeContainer[] = { '{', '}' };

	/* About functions */

	inline std::wstring dnyProduct(void)
	{
		return DNY_PRODUCT_NAME;
	}

	inline std::wstring dnyAuthor(void)
	{
		return DNY_PRODUCT_AUTHOR;
	}

	inline std::wstring dnyVersion(void)
	{
		return DNY_PRODUCT_VERSION;
	}

	inline std::wstring dnycontact(void)
	{
		return DNY_PRODUCT_CONTACT;
	}

	/* Error handler */

	enum ScriptErrorType_e {
		SET_NO_ERROR = 0, //Operation succeeded
		SET_OPEN_QUOTATION, //A quotation has not been finished by the required closing quotation character
		SET_OPEN_ARGBRACKETS, //There have been some argument brackets left open
		SET_OPEN_CODEBRACKETS, //There have been some code brackets left open
		SET_TOO_MANY_CLOSING_ARGBRACKETS, //There are too many closing argument brackets
		SET_TOO_MANY_CLOSING_CODEBRACKETS, //There are too many closing code brackets
		SET_REMAINING_UNHANDLED_CONTEXT_ITEMS, //Some unhandled context items have not been processed
		SET_INTERNAL_CMD_FAILURE, //An internal command has indicated a failure
		SET_EXTERNAL_CMD_FAILURE, //An external command has indicated a failure
		SET_UNKNOWN_COMMAND, //An expression identifier is neither an internal nor external command identifier
		SET_SCRIPTFILE_INPUT //An error occured during script input reading process
	};

	class CScriptErrorInformation {
	private:
		ScriptErrorType_e m_eErrorType;
		dnyString m_wszErrorMessage;
	public:
		CScriptErrorInformation() : m_eErrorType(SET_NO_ERROR), m_wszErrorMessage(L"") {}
		CScriptErrorInformation(const ScriptErrorType_e eType, const dnyString& wszErrorMessage) : m_eErrorType(eType), m_wszErrorMessage(wszErrorMessage) {}
		~CScriptErrorInformation() { this->m_eErrorType = SET_NO_ERROR; this->m_wszErrorMessage.clear(); }

		inline ScriptErrorType_e GetErrorCode(void) const { return this->m_eErrorType; }
		inline const dnyString& GetErrorText(void) const { return this->m_wszErrorMessage; }
	};

	void dnySetError(const ScriptErrorType_e eType, const dnyString& wszErrorMessage);
	void dnyclearError(void);
	const CScriptErrorInformation& GetErrorInformation(void);

	/* Container handlers */

	class CVarManager {
	public:
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
		struct custom_cvar_type_s {
			std::wstring wszName;
			struct cvar_type_event_table_s {
				TpfnDeclareVar pfnDeclareVar;
				TpfnAssignVarValue pfnAssignVarValue;
				TpfnGetReplacerString pfnGetReplacerString;
				TpfnRemoveVar pfnRemoveVar;
			} sEventTable;
		};
	private:
		struct cvar_s {
			cvarptr_t pcvar;
			cvartype_e eType;
			std::wstring wszName;
			std::wstring wszCustom;
			bool bConst;

			cvar_s() : pcvar(nullptr), wszName(L""), eType(CT_UNKNOWN) {}
			cvar_s(cvarptr_t p, cvartype_e ct, std::wstring& wsz, bool bc) : pcvar(p), eType(ct), wszName(wsz), bConst(bc) {}
		};

		std::vector<custom_cvar_type_s> m_vDataTypes;
		std::vector<cvar_s*> m_vCVars;

		bool FindDataType(const std::wstring& wszName, size_t* puiId = nullptr)
		{
			//Find data type item ID

			for (size_t i = 0; i < this->m_vDataTypes.size(); i++) {
				if (this->m_vDataTypes[i].wszName == wszName) {
					if (puiId)
						*puiId = i;

					return true;
				}
			}

			return false;
		}

		void Clear(void)
		{
			//Free allocated cvars

			for (size_t i = 0; i < this->m_vCVars.size(); i++) {
				delete this->m_vCVars[i]->pcvar;
				delete this->m_vCVars[i];
			}

			this->m_vCVars.clear();

			//Free data types
			this->m_vDataTypes.clear();
		}
	public:
		CVarManager() {}
		~CVarManager() { this->Clear(); }

		custom_cvar_type_s* GetDatatypeObjectData(const std::wstring& wszName)
		{
			//Acquire data type data pointer

			for (size_t i = 0; i < this->m_vDataTypes.size(); i++) {
				if (this->m_vDataTypes[i].wszName == wszName) {
					return &this->m_vDataTypes[i];
				}
			}

			return nullptr;
		}

		bool RegisterDataType(const std::wstring& wszDataType, custom_cvar_type_s::cvar_type_event_table_s* pEventTable)
		{
			//Register data type

			if ((!wszDataType.length()) || (!pEventTable))
				return false;

			//Check if name is already in use
			if (this->FindDataType(wszDataType))
				return false;

			//Setup data struct
			custom_cvar_type_s sCvarType;
			sCvarType.wszName = wszDataType;
			sCvarType.sEventTable = *pEventTable;

			//Add to list
			this->m_vDataTypes.push_back(sCvarType);

			return true;
		}

		cvarptr_t SpawnCVar(const std::wstring& wszName, const cvartype_e eType, bool bConst = false)
		{
			//Spawn new CVar instance of given type

			if (!wszName.length())
				return nullptr;

			cvarptr_t pCVar = nullptr;

			//Allocate memory for cvar and instantiate class object according to type
			switch (eType) {
			case CT_BOOL:
				pCVar = new ICVar<dnyBoolean>(CT_BOOL, bConst);
				break;
			case CT_INT:
				pCVar = new ICVar<dnyInteger>(CT_INT, bConst);
				break;
			case CT_FLOAT:
				pCVar = new ICVar<dnyFloat>(CT_FLOAT, bConst);
				break;
			case CT_STRING:
				pCVar = new ICVar<dnyString>(CT_STRING, bConst);
				break;
			default:
				return nullptr;
				break;
			}

			if (!pCVar)
				return nullptr;

			return pCVar;
		}

		cvarptr_t SpawnCustomCVar(const std::wstring& wszName, const std::wstring& wszType, bool bConst = false)
		{
			//Spawn new custom CVar instance of given type

			if (!wszName.length())
				return nullptr;

			cvarptr_t pCVar = nullptr;
			size_t uiCvarTypeId;

			//Find cvar type
			if (!this->FindDataType(wszType, &uiCvarTypeId))
				return nullptr;

			//Allocate memory for cvar and instantiate class object according to type
			pCVar = new ICVar<dnycustom>(CT_CUSTOM, bConst);
			if (!pCVar)
				return nullptr;

			//Call init function
			if (!this->m_vDataTypes[uiCvarTypeId].sEventTable.pfnDeclareVar(wszName, (ICVar<dnycustom>*)pCVar)) {
				delete pCVar;
				return nullptr;
			}

			return pCVar;
		}

		cvarptr_t RegisterCVar(const std::wstring& wszName, const cvartype_e eType, bool bConst, bool bAtBottom)
		{
			//Register CVar

			if (!wszName.length())
				return nullptr;

			//Check if name is already in use
			if (this->FindCVar(wszName))
				return nullptr;

			//Spawn CVar object
			cvarptr_t pCVar = SpawnCVar(wszName, eType, bConst);
			if (!pCVar)
				return nullptr;

			//Allocate cvar memory and add to list

			cvar_s* pCVarData = new cvar_s;
			if (!pCVarData) {
				delete pCVar;
				return nullptr;
			}

			pCVarData->pcvar = pCVar;
			pCVarData->eType = eType;
			pCVarData->wszName = wszName;
			pCVarData->wszCustom = L"";
			pCVarData->bConst = bConst;

			if (bAtBottom)
				this->m_vCVars.insert(this->m_vCVars.begin() + 0, pCVarData); //Insert as first entry
			else
				this->m_vCVars.push_back(pCVarData); //Append to list

			return pCVar;
		}

		cvarptr_t RegisterCustomCVar(const std::wstring& wszName, const std::wstring& wszType, bool bConst, bool bAtBottom)
		{
			//Register CVar

			if (!wszName.length())
				return nullptr;
			
			//Check if name is already in use
			if (this->FindCVar(wszName))
				return nullptr;
			
			//Spawn custom CVar object
			cvarptr_t pCVar = this->SpawnCustomCVar(wszName, wszType, bConst);
			if (!pCVar)
				return false;
			
			//Allocate cvar memory and set data

			cvar_s* pCVarData = new cvar_s;
			if (!pCVarData) {
				delete pCVar;
				return nullptr;
			}
			
			pCVarData->pcvar = pCVar;
			pCVarData->eType = CT_CUSTOM;
			pCVarData->wszName = wszName;
			pCVarData->wszCustom = wszType;
			pCVarData->bConst = bConst;

			//Add to list
			if (bAtBottom)
				this->m_vCVars.insert(this->m_vCVars.begin() + 0, pCVarData); //Insert as first entry
			else
				this->m_vCVars.push_back(pCVarData); //Append to list

			return pCVar;
		}

		bool AssignCustomVar(const std::wstring& wszName, const std::wstring& wszExpression)
		{
			//Assign custom variable value

			//Find CVar list ID
			size_t uiCVarId = this->FindCVarId(wszName);
			if (uiCVarId == std::wstring::npos)
				return false;

			//Setup approbriate accessor pointer
			ICVar<dnycustom>* pCVar = (ICVar<dnycustom>*)this->m_vCVars[uiCVarId]->pcvar;
			if (!pCVar)
				return false;

			//Find CVar custom data type
			size_t uiCVarDataType;
			if (!this->FindDataType(this->m_vCVars[uiCVarId]->wszCustom, &uiCVarDataType))
				return false;

			//Setup data content accessor object
			ICustomVarValue oCustomVarValue(wszExpression);

			//Call related function
			return this->m_vDataTypes[uiCVarDataType].sEventTable.pfnAssignVarValue(wszName, (ICVar<dnycustom>*)this->m_vCVars[uiCVarId]->pcvar, oCustomVarValue, this->m_vCVars[uiCVarId]->bConst);
		}

		bool AssignCustomVar(ICVar<dnycustom>* pCustomVar, const std::wstring& wszTypeName, const std::wstring& wszExpression)
		{
			//Assign custom variable value

			//Find CVar custom data type
			size_t uiCVarDataType;
			if (!this->FindDataType(wszTypeName, &uiCVarDataType))
				return false;

			//Setup data content accessor object
			ICustomVarValue oCustomVarValue(wszExpression);

			//Call related function
			return this->m_vDataTypes[uiCVarDataType].sEventTable.pfnAssignVarValue(L"", pCustomVar, oCustomVarValue, pCustomVar->IsConst());
		}

		cvarptr_t FindCVar(const std::wstring& wszName)
		{
			//Find cvar

			for (size_t i = 0; i < this->m_vCVars.size(); i++) {
				if (this->m_vCVars[i]->wszName == wszName) {
					return this->m_vCVars[i]->pcvar;
				}
			}

			return nullptr;
		}

		size_t FindCVarId(const std::wstring& wszName)
		{
			//Find ID

			for (size_t i = 0; i < this->m_vCVars.size(); i++) {
				if (this->m_vCVars[i]->wszName == wszName) {
					return i;
				}
			}

			return std::wstring::npos;
		}

		/*cvartype_e GetCVarType(cvarptr_t pCVar)
		{
			//Get cvar type

			for (size_t i = 0; i < this->m_vCVars.size(); i++) {
				if (this->m_vCVars[i]->pcvar == pCVar) {
					return this->m_vCVars[i]->eType;
				}
			}

			return CT_UNKNOWN;
		}*/
		cvartype_e GetCVarType(const std::wstring& wszName)
		{
			//Get cvar type

			for (size_t i = 0; i < this->m_vCVars.size(); i++) {
				if (this->m_vCVars[i]->wszName == wszName) {
					return this->m_vCVars[i]->eType;
				}
			}

			return CT_UNKNOWN;
		}

		bool FreeCVar(const std::wstring& wszName)
		{
			//Free CVar from memory and list

			for (size_t i = 0; i < this->m_vCVars.size(); i++) {
				if ((this->m_vCVars[i]->wszName == wszName) && (!this->m_vCVars[i]->bConst)) { //Compare if names are equal and if not a const
					//Handle stuff if custom cvar
					if (this->m_vCVars[i]->wszCustom.length()) {
						size_t uiId;
						if (this->FindDataType(this->m_vCVars[i]->wszCustom, &uiId)) {
							this->m_vDataTypes[uiId].sEventTable.pfnRemoveVar(this->m_vCVars[i]->wszName, (ICVar<dnycustom>*)this->m_vCVars[i]->pcvar);
						}
					}

					//Free memory of CVar class object and item list data entry
					delete this->m_vCVars[i]->pcvar;
					delete this->m_vCVars[i];

					//Remove from vector list
					this->m_vCVars.erase(this->m_vCVars.begin() + i);

					return true;
				}
			}

			return false;
		}

		std::wstring ReplaceVariables(const std::wstring& wszInputString)
		{
			//Replace variable identifiers with values

			if (!wszInputString.length())
				return L"";

			std::wstring wszResult = wszInputString;
			wchar_t wcsVarDerefIdent[] = { dnyVariableEntrance, 0x000 };

			//Iterate thorugh all variable items
			for (size_t i = 0; i < this->m_vCVars.size(); i++) {
				//Ignore command result variables
				if (this->m_vCVars[i]->wszName[0] == '$') continue;

				//Attempt to find all occurences of variable name with prefix as substring
				size_t uiSubstrPos = wszResult.find(std::wstring(wcsVarDerefIdent) + this->m_vCVars[i]->wszName);
				while (uiSubstrPos != std::wstring::npos) {
					//Assign replacer string with variables value
					std::wstring wszReplacerString = L"";
					switch (this->m_vCVars[i]->eType) {
					case CT_BOOL: {
						ICVar<dnyBoolean>* pBoolVar = (ICVar<dnyBoolean>*)this->m_vCVars[i]->pcvar;
						wszReplacerString = (pBoolVar->GetValue()) ? L"true" : L"false";
						break;
					}
					case CT_INT: {
						ICVar<dnyInteger>* pIntVar = (ICVar<dnyInteger>*)this->m_vCVars[i]->pcvar;
						wszReplacerString = std::to_wstring(pIntVar->GetValue());
						break;
					}
					case CT_FLOAT: {
						ICVar<dnyFloat>* pFloatVar = (ICVar<dnyFloat>*)this->m_vCVars[i]->pcvar;
						wszReplacerString = std::to_wstring(pFloatVar->GetValue());
						break;
					}
					case CT_STRING: {
						ICVar<dnyString>* pStringVar = (ICVar<dnyString>*)this->m_vCVars[i]->pcvar;
						wszReplacerString = pStringVar->GetValue();
						break;
					}
					case CT_CUSTOM: {
						size_t uiItemId;
						if (!this->FindDataType(this->m_vCVars[i]->wszCustom, &uiItemId)) return false;
						wszReplacerString = this->m_vDataTypes[uiItemId].sEventTable.pfnGetReplacerString(this->m_vCVars[i]->wszName, (ICVar<dnycustom>*)this->m_vCVars[i]->pcvar);
						break;
					}
					default:
						break;
					}

					//Replace var identifier with value
					wszResult = wszResult.replace(wszResult.begin() + uiSubstrPos, wszResult.begin() + uiSubstrPos + this->m_vCVars[i]->wszName.length() + 1, wszReplacerString);
				
					//Update position value
					uiSubstrPos = wszResult.find(std::wstring(wcsVarDerefIdent) + this->m_vCVars[i]->wszName);
				}
			}

			return wszResult;
		}

		cvartype_e GetTypeFromString(const std::wstring& wszTypeName)
		{
			if (wszTypeName == L"bool")
				return CT_BOOL;
			else if (wszTypeName == L"int")
				return CT_INT;
			else if (wszTypeName == L"float")
				return CT_FLOAT;
			else if (wszTypeName == L"string")
				return CT_STRING;
			else if (wszTypeName == L"void")
				return CT_VOID;

			for (size_t i = 0; i < this->m_vDataTypes.size(); i++) {
				if (this->m_vDataTypes[i].wszName == wszTypeName)
					return CT_CUSTOM;
			}

			return CT_UNKNOWN;
		}
	};
	
	class CCommandManager : public CVarManager {
	#define CMGR_RESULTVAR_PREFIX L"$__resultvar__"
	public:
		typedef void* cmdinterface_t;
		class IVoidCommandInterface {
		public:
			IVoidCommandInterface() {}

			virtual bool CommandCallback(void* pArg1, void* pArg2) = 0;
		};
		template <typename TResultVar>
		class IResultCommandInterface {
		private:
			CVarManager::ICVar<TResultVar> m_oResultVar;
			CVarManager::cvartype_e m_eCVarType;
		protected:
			virtual void SetType(CVarManager::cvartype_e ct) { this->m_eCVarType = ct; }
			virtual void SetResult(TResultVar vResultValue) { this->m_oResultVar.SetValue(vResultValue); }
		public:
			IResultCommandInterface() {}
			IResultCommandInterface(CVarManager::cvartype_e et) : m_eCVarType(et) {}

			virtual bool CommandCallback(void* pArg1, void* pArg2) = 0;
			virtual TResultVar GetResult(void) { return m_oResultVar.GetValue(); }
		};

		struct command_s {
			cmdinterface_t cmdCallback;
			CVarManager::cvarptr_t pResultVar;
			CVarManager::cvartype_e eResultVarType;
			std::wstring wszName;

			command_s() : cmdCallback(nullptr), pResultVar(nullptr), wszName(L"") {}
			command_s(cmdinterface_t p, std::wstring& wsz) : cmdCallback(p), wszName(wsz) {}
		};

	private:
		std::vector<command_s*> m_vCommands;

		void Clear(void)
		{
			//Clear commands from memory and empty list

			for (size_t i = 0; i < this->m_vCommands.size(); i++) {
				CVarManager::FreeCVar(CMGR_RESULTVAR_PREFIX + this->m_vCommands[i]->wszName);
				delete this->m_vCommands[i];
			}

			this->m_vCommands.clear();
		}
	public:
		CCommandManager() {}
		~CCommandManager() { this->Clear(); }

		bool RegisterCommand(const std::wstring& wszCmdName, cmdinterface_t pCmdInterface, CVarManager::cvartype_e eResultType)
		{
			//Register command

			if ((!wszCmdName.length()) || (!pCmdInterface))
				return false;

			//Check if name is already in use
			if (this->FindCommand(wszCmdName))
				return false;

			//Allocate object data
			command_s* pCmd = new command_s;
			if (!pCmd)
				return false;

			//Allocate variable according to var type if required
			if (eResultType != CT_VOID) {
				pCmd->pResultVar = CVarManager::RegisterCVar(CMGR_RESULTVAR_PREFIX + wszCmdName, eResultType, false, false);
				if (!pCmd->pResultVar)
					return false;
			} else { pCmd->pResultVar = nullptr; }

			//Store data
			pCmd->cmdCallback = pCmdInterface;
			pCmd->eResultVarType = eResultType;
			pCmd->wszName = wszCmdName;

			//Add to list
			this->m_vCommands.push_back(pCmd);

			return true;
		}

		command_s* FindCommand(const std::wstring& wszCmdName)
		{
			//Find command data from list

			for (size_t i = 0; i < this->m_vCommands.size(); i++) {
				if (this->m_vCommands[i]->wszName == wszCmdName) {
					return this->m_vCommands[i];
				}
			}

			return nullptr;
		}

		bool UnregisterCommand(const std::wstring& wszCmdName)
		{
			//Unregister command

			if (!wszCmdName.length())
				return false;

			for (size_t i = 0; i < this->m_vCommands.size(); i++) {
				if (this->m_vCommands[i]->wszName == wszCmdName) {
					CVarManager::FreeCVar(CMGR_RESULTVAR_PREFIX + wszCmdName);

					delete this->m_vCommands[i];

					this->m_vCommands.erase(this->m_vCommands.begin() + i);

					return true;
				}
			}

			return false;
		}
	};

	extern class CObjectMgr* pObjectManagerInstance;
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

		struct cvar_type_event_table_s {
			CVarManager::TpfnDeclareVar pfnDeclareVar;
			CVarManager::TpfnAssignVarValue pfnAssignVarValue;
			CVarManager::TpfnGetReplacerString pfnGetReplacerString;
			CVarManager::TpfnRemoveVar pfnRemoveVar;
		};
	private:
		std::vector<object_s> m_vObjects;
		objinstance_s* m_pCurrentInstance;
		class CScriptingInterface* m_pInterface;
		cvar_type_event_table_s m_sEventTable;

		friend bool CTOBJ_DeclareVar(const std::wstring& wszName, CVarManager::ICVar<dnycustom>* pCVar);
		friend bool CTOBJ_AssignVarValue(const std::wstring& wszName, CVarManager::ICVar<dnycustom>* pCVar, const CVarManager::ICustomVarValue& rCustomVarValue, bool bIsConst);
		friend dnyString CTOBJ_GetReplacerString(const std::wstring& wszName, CVarManager::ICVar<dnycustom>* pCVar);
		friend void CTOBJ_RemoveVar(const std::wstring& wszName, CVarManager::ICVar<dnycustom>* pCVar);

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

		dnyScriptParser::CVarManager::cvartype_e TypeByName(const std::wstring& wszName)
		{
			//Get type indicator by string

			dnyScriptParser::CVarManager::cvartype_e eResult = dnyScriptParser::CVarManager::cvartype_e::CT_UNKNOWN;

			if (wszName == L"bool") {
				eResult = dnyScriptParser::CVarManager::cvartype_e::CT_BOOL;
			}
			else if (wszName == L"int") {
				eResult = dnyScriptParser::CVarManager::cvartype_e::CT_INT;
			}
			else if (wszName == L"float") {
				eResult = dnyScriptParser::CVarManager::cvartype_e::CT_FLOAT;
			}
			else if (wszName == L"string") {
				eResult = dnyScriptParser::CVarManager::cvartype_e::CT_STRING;
			}

			return eResult;
		}

		inline void Free(void)
		{
			//Cleanup resources

			this->Clear();
		}
	public:
		CObjectMgr(class CScriptingInterface* pInterface);
		~CObjectMgr() { pObjectManagerInstance = nullptr; this->Free(); }

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

		bool AllocObject(const std::wstring& wszInstanceName, const std::wstring& wszObject);
		bool AddMemberToCurrentObject(class ICodeContext* pCodeContext, void* pInterfaceObject);
		bool AddMethodToCurrentObject(ICodeContext* pCodeContext, void* pInterfaceObject);
		bool FreeObject(const std::wstring& wszInstanceName);
	};

	/* Code context container */

	class ICodeContext {
	private:
		std::vector<dnyString> m_vCodeParts;
	public:
		ICodeContext() {}
		~ICodeContext() {}

		//Code context item list setter
		virtual void AddPart(const std::wstring& wszPartExpression) { this->m_vCodeParts.push_back(wszPartExpression); }
		virtual void Clear(void) { this->m_vCodeParts.clear(); }

		//Code context item list getters
		virtual const size_t GetPartCount(void) const { return this->m_vCodeParts.size(); }
		virtual std::wstring GetPartData(const size_t uiBlockId) const { if (uiBlockId < this->m_vCodeParts.size()) return this->m_vCodeParts[uiBlockId]; return L""; }
		virtual dnyBoolean GetPartBool(const size_t uiBlockId) const { return (this->GetPartData(uiBlockId) != L"false"); }
		virtual dnyInteger GetPartInt(const size_t uiBlockId) const { return (_wtoi64(this->GetPartData(uiBlockId).c_str())); }
		virtual dnyFloat GetPartFloat(const size_t uiBlockId) const { return (_wtof(this->GetPartData(uiBlockId).c_str())); }
		virtual dnyString GetPartString(const size_t uiBlockId) const { return this->GetPartData(uiBlockId); }
		//virtual std::vector<dnyString> GetPartArray(const size_t uiBlockId) { return CSyntaxParser::ParseArrayList(this->GetPartData(uiBlockId)); }
		//virtual std::vector<dnyString> GetPartParamlist(const size_t uiBlockId) { return CSyntaxParser::ParseParamList(this->GetPartData(uiBlockId)); }
		virtual std::vector<dnyString> GetPartArray(const size_t uiBlockId);
		virtual std::vector<dnyString> GetPartParamlist(const size_t uiBlockId);

		//Variable handlers
		virtual void ReplaceGlobalVariables(void* pInterfaceObject);
		virtual void ReplaceLocalVariables(void* pInterfaceObject);
		virtual void ReplaceAllVariables(void* pInterfaceObject);
		/*virtual void ReplaceGlobalVariables(void* pInterfaceObject)
		{
			//Attempt to replace all global variables

			if (!pInterfaceObject)
				return;

			class CScriptingInterface* pScriptingInterface = (class CScriptingInterface*)pInterfaceObject;

			for (size_t i = 0; i < this->m_vCodeParts.size(); i++) {
				this->m_vCodeParts[i] = pScriptingInterface->ReplaceVariables(this->m_vCodeParts[i]);
			}
		}
		virtual void ReplaceLocalVariables(void* pInterfaceObject)
		{
			//Attempt to replace all global variables

			if (!pInterfaceObject)
				return;

			class CScriptingInterface* pScriptingInterface = (class CScriptingInterface*)pInterfaceObject;

			for (size_t i = 0; i < this->m_vCodeParts.size(); i++) {
				this->m_vCodeParts[i] = pScriptingInterface->ReplaceLocalFunctionVariables(this->m_vCodeParts[i]);
			}
		}
		virtual void ReplaceAllVariables(void* pInterfaceObject)
		{
			//Attempt to replace all variables

			this->ReplaceLocalVariables(pInterfaceObject);
			this->ReplaceGlobalVariables(pInterfaceObject);
		}*/

		//Code context line getter
		virtual dnyString GetAsLine(void) const
		{
			dnyString result = L"";

			for (size_t i = 0; i < this->m_vCodeParts.size(); i++)
				result += L" [" + this->m_vCodeParts[i] + L"] ";

			return result;
		}
	};

	/* Syntax parser class */

	class CSyntaxParser : public CCommandManager {
	public:
		
	private:
		std::vector<ICodeContext> m_vBlocks;

		bool ShouldSplitOnChar(const std::wstring& wszString, const wchar_t wcSplitChar, const size_t uiCurrentIndex, const size_t uiMaxSize)
		{
			//Check if should split by the given char if not previous char or next following char are already indicating a split

			if (!wszString.length())
				return false;

			//Check if current char is even the given char
			if (wszString[uiCurrentIndex] != wcSplitChar)
				return false;

			//Check for previous split char
			if (uiCurrentIndex > 0) {
				if ((wszString[uiCurrentIndex - 1] == dnycodeContainer[DNY_BRACKET_END]) || (wszString[uiCurrentIndex - 1] == dnyArgumentContainer[DNY_BRACKET_END]) || (wszString[uiCurrentIndex - 1] == dnycodeContextDelimiter))
					return false;
			}

			//Check for following split char
			if (uiCurrentIndex + 1 < uiMaxSize) {
				if ((wszString[uiCurrentIndex + 1] == dnycodeContainer[DNY_BRACKET_START]) || (wszString[uiCurrentIndex + 1] == dnyArgumentContainer[DNY_BRACKET_START]) || (wszString[uiCurrentIndex + 1] == dnycodeContextDelimiter))
					return false;
			}

			return true;
		}

		virtual bool IsSuperficialSplitChar(const std::wstring& wszString, const size_t uiCurrentIndex)
		{
			//Check if current split char is superficial

			if (!wszString.length())
				return false;

			//Validate index
			if (uiCurrentIndex > wszString.length())
				return false;

			//Static array with all split chars
			static wchar_t wcaSplitChars[] = { dnySpaceCharacter, dnyTabCharacter, dnycodeContainer[DNY_BRACKET_START], dnyArgumentContainer[DNY_BRACKET_START] };
		
			for (size_t i = 0; i < (sizeof(wcaSplitChars) / sizeof(wchar_t)) - 2; i++) { //First loop for current char
				for (size_t j = 0; j < sizeof(wcaSplitChars) / sizeof(wchar_t); j++) { //Second loop for next char
					//Return true if current char is a split char and next following char is also a split char
					if ((wszString[uiCurrentIndex] == wcaSplitChars[i]) && (wszString[uiCurrentIndex + 1] == wcaSplitChars[j])) {
						return true;
					}
				}
			}

			return false;
		}
	public:
		CSyntaxParser() {}
		CSyntaxParser(const std::wstring& wszExpression)
		{
			this->ParseCode(wszExpression);
		}
		~CSyntaxParser() { this->Clear(); }

		virtual bool ParseCode(const std::wstring& wszExpression)
		{
			//Perform syntax code parsing

			if (!wszExpression.length())
				return false;

			ICodeContext oCurrentCodeContext;
			std::wstring wszCurrentExpression = L"";
			bool bInQuotes = false;
			size_t uiArgContCounter = 0;
			size_t uiCodeContCounter = 0;

			//Iterate through string
			for (size_t i = 0; i <= wszExpression.length(); i++) {
				//Add remaining expression to item list if end of string is reached
				if (i == wszExpression.length()) {
					oCurrentCodeContext.AddPart(wszCurrentExpression);
					break;
				}

				//Toggle in-quote flag value
				if (wszExpression[i] == '"') {
					bInQuotes = !bInQuotes;

					//Ignore copying only if not inside a bracket
					if ((!uiCodeContCounter) && (!uiArgContCounter))
						continue;
				}

				//Further stuff is only handled if not in quotes by start
				if (!bInQuotes) {
					//Break on comments
					if (wszExpression[i] == dnycommentChar)
						break;

					//Filter superficial split characters
					if (this->IsSuperficialSplitChar(wszExpression, i))
						continue;

					//Handle code containing brackets
					if (wszExpression[i] == dnycodeContainer[DNY_BRACKET_START]) {
						//Increase for each same bracket start character
						uiCodeContCounter++;

						//Ignore copying only for the first one
						if (uiCodeContCounter == 1) {
							//Check for split for the first bracket start
							if ((this->ShouldSplitOnChar(wszExpression, dnycodeContainer[DNY_BRACKET_START], i, wszExpression.length()))) {
								oCurrentCodeContext.AddPart(wszCurrentExpression);
								wszCurrentExpression.clear();
							}

							continue;
						}
					} else if (wszExpression[i] == dnycodeContainer[DNY_BRACKET_END]) {
						//Decrease for each same bracket start character
						uiCodeContCounter--;

						//Split if end of bracket area is reached
						if (uiCodeContCounter == 0) {
							oCurrentCodeContext.AddPart(wszCurrentExpression);
							wszCurrentExpression.clear();

							//Handle if next char is a spacing char
							if (i + 1 < wszExpression.length()) {
								if ((wszExpression[i + 1] == dnySpaceCharacter) || (wszExpression[i + 1] == dnyTabCharacter))
									i++;
							}

							continue;
						}
					}

					//Handle argument containing brackets
					if (!uiCodeContCounter) {
						if (wszExpression[i] == dnyArgumentContainer[DNY_BRACKET_START]) {
							//Increase for each same bracket start character
							uiArgContCounter++;

							//Ignore copying only for the first one
							if (uiArgContCounter == 1) {
								//Split for the first bracket start
								oCurrentCodeContext.AddPart(wszCurrentExpression);
								wszCurrentExpression.clear();
								continue;
							}
						} else if (wszExpression[i] == dnyArgumentContainer[DNY_BRACKET_END]) {
							//Decrease for each same bracket start character
							uiArgContCounter--;

							//Split if end of bracket area is reached
							if (uiArgContCounter == 0) {
								oCurrentCodeContext.AddPart(wszCurrentExpression);
								wszCurrentExpression.clear();

								//Handle if next char is a spacing char
								if (i + 1 < wszExpression.length()) {
									if ((wszExpression[i + 1] == dnySpaceCharacter) || (wszExpression[i + 1] == dnyTabCharacter))
										i++;
								}

								continue;
							}
						}
					}
				}

				//Do stuff if outside of code collector structures
				if ((!bInQuotes) && (!uiCodeContCounter) && (!uiArgContCounter)) {
					//Ignore spacings at begin of code context ident expression
					if (((wszExpression[i] == dnyTabCharacter) || (wszExpression[i] == dnySpaceCharacter)) && (!wszCurrentExpression.length()))
						continue;

					//Handle expression list split by space character if the next following character is not already another split char
					if ((wszExpression[i] == dnySpaceCharacter) && (this->ShouldSplitOnChar(wszExpression, dnySpaceCharacter, i, wszExpression.length()))) {
						//Add part to context container and clear current expression holder
						oCurrentCodeContext.AddPart(wszCurrentExpression);
						wszCurrentExpression.clear();
						continue;
					}
					//Handle code context split by delimiter character
					else if (wszExpression[i] == dnycodeContextDelimiter) {
						//Add last part to context container (if required) and, context container to list and then clear temporary buffers
						if (this->ShouldSplitOnChar(wszExpression, dnycodeContextDelimiter, i, wszExpression.length())) {
							oCurrentCodeContext.AddPart(wszCurrentExpression);
						}
						this->m_vBlocks.push_back(oCurrentCodeContext);
						oCurrentCodeContext.Clear();
						wszCurrentExpression.clear();
						continue;
					}
				}

				//Add current character to current expression item string
				wszCurrentExpression += wszExpression[i];
			}

			//Perform error handling
			if (bInQuotes) {
				dnySetError(SET_OPEN_QUOTATION, L"Quotation Error in: \n" + wszExpression + L"\n\n");
				return false;
			}

			if (uiArgContCounter) {
				dnySetError(SET_OPEN_ARGBRACKETS, L"Open argument bracket(s) in: \n" + wszExpression + L"\n\n");
				return false;
			} else if (uiArgContCounter < 0) {
				dnySetError(SET_TOO_MANY_CLOSING_ARGBRACKETS, L"Too many closing argument brackets in: \n" + wszExpression + L"\n\n");
				return false;
			}

			if (uiCodeContCounter) {
				dnySetError(SET_OPEN_CODEBRACKETS, L"Open code bracket(s) in: \n" + wszExpression + L"\n\n");
				return false;
			} else if (uiArgContCounter < 0) {
				dnySetError(SET_TOO_MANY_CLOSING_CODEBRACKETS, L"Too many closing code brackets in: \n" + wszExpression + L"\n\n");
				return false;
			}

			/*if (oCurrentCodeContext.GetPartCount()) {
				dnySetError(SET_REMAINING_UNHANDLED_CONTEXT_ITEMS, L"There are some remaining unhandled code context items in: \n" + wszExpression + L"\n\n");
				return false;
			}*/
			
			return true;
		}

		virtual std::wstring FilterLineFromStuff(const std::wstring& wszLine)
		{
			//Filter line from comments and tab chars

			if (!wszLine.length())
				return L"";

			std::wstring wszContent = L"";
			bool bInQuotes = false;

			//Iterate through string
			for (size_t i = 0; i < wszLine.length(); i++) {
				//Check for quotes
				if (wszLine[i] == '"') {
					bInQuotes = !bInQuotes;
				}

				if (!bInQuotes) { //Ensure that target chars are not inside quotations
					//Ignore tab chars
					if (wszLine[i] == dnyTabCharacter)
						continue;

					//Break on comments
					if (wszLine[i] == dnycommentChar)
						break;
				}

				//Append current char
				wszContent += wszLine[i];
			}

			return wszContent;
		}

		static std::vector<dnyString> ParseArrayList(const std::wstring& wszString)
		{
			//Parse array-styled list

			std::vector<dnyString> vResult;

			if (!wszString.length())
				return vResult;

			bool bInQuotes = false;
			std::wstring wszCurParamName = L"";

			//Iterate through string
			for (size_t i = 0; i <= wszString.length(); i++) {
				//Handle quotations
				if (wszString[i] == '"') {
					bInQuotes = !bInQuotes;
					continue;
				}

				//Ignore superficial spacing if required if not in quotes
				if (!bInQuotes) {
					if ((wszString[i] == dnySpaceCharacter) || (wszString[i] == dnyTabCharacter))
						continue;
				}

				//Check if list separator is found outside of quotations or end of string is reached
				if (((wszString[i] == dnyListSeparator) && (!bInQuotes)) || (!wszString[i])) {
					//Add current param name to list and clear temp buffer
					vResult.push_back(wszCurParamName);
					wszCurParamName.clear();
					continue; //Ignore copying the list separator
				}

				//Append char to string
				wszCurParamName += wszString[i];
			}

			return vResult;
		}

		static std::vector<dnyString> ParseParamList(const std::wstring& wszString)
		{
			//Parse array-styled list

			std::vector<dnyString> vResult;

			if (!wszString.length())
				return vResult;

			std::wstring wszCurParamName = L"";

			//Iterate through string
			for (size_t i = 0; i < wszString.length(); i++) {
				//Handle superficial spacings
				if (((wszString[i] == dnySpaceCharacter) || (wszString[i] == dnyTabCharacter)) && ((wszString[i + 1] == dnySpaceCharacter) || (wszString[i + 1] == dnyTabCharacter)))
					continue;

				//Check if list separator is found
				if (wszString[i] == dnyListSeparator) {
					//Add current param name to list and clear temp buffer
					vResult.push_back(wszCurParamName);
					wszCurParamName.clear();

					//Check for next char as a spacing char and if so ignore copying it
					if (i + 1 < wszString.length()) {
						if ((wszString[i + 1] == dnySpaceCharacter) || (wszString[i + 1] == dnyTabCharacter))
							i++;
					}

					continue; //Ignore copying the list separator
				}

				//Append char to string
				wszCurParamName += wszString[i];
			}

			return vResult;
		}

		//Expression item list accessors
		virtual const size_t GetCodeContextCount(void) const { return this->m_vBlocks.size(); }
		virtual ICodeContext GetCodeContextItem(const size_t uiBlockId) const { if (uiBlockId < this->m_vBlocks.size()) return this->m_vBlocks[uiBlockId]; return ICodeContext::ICodeContext(); }
		
		//Call clear method when done
		virtual void Clear(void) { this->m_vBlocks.clear(); }
	};

	//Script parsing summarizer
	class CScriptHandler : public CSyntaxParser {
	private:
		std::wstring m_wszFileBuffer;

	protected:
		bool Parse(const std::wstring& wszExpression) { return CSyntaxParser::ParseCode(wszExpression); }

		const size_t GetCodeContextCount(void) const { return CSyntaxParser::GetCodeContextCount(); }
		ICodeContext GetCodeContextItem(const size_t uiBlockId) const { return CSyntaxParser::GetCodeContextItem(uiBlockId); }

		bool ReadAllLines(const std::wstring& wszScriptFile)
		{
			//Parse script file

			this->m_wszFileBuffer.clear();

			//Open file in read-mode
			std::wifstream hFile;
			hFile.open(wszScriptFile, std::ifstream::in);
			if (hFile.is_open()) {
				wchar_t wszLineBuffer[DNY_MAX_LINE_BUFFER_SIZE] = { 0 };

				while (!hFile.eof()) {
					//Get current line
					hFile.getline(wszLineBuffer, DNY_MAX_LINE_BUFFER_SIZE, '\n');

					//Filter empty lines
					if (!wcslen(wszLineBuffer))
						continue;

					//Filter comments
					std::wstring wszLine = CSyntaxParser::FilterLineFromStuff(wszLineBuffer);

					//Add to buffer
					this->m_wszFileBuffer += wszLine;
				}

				hFile.close();

				return true;
			}

			return false;
		}

		std::wstring GetFileBuffer(void) { return this->m_wszFileBuffer; }
	public:
		CScriptHandler() {}
		CScriptHandler(const std::wstring& wszExpression)
		{
			this->Parse(wszExpression);
		}
		~CScriptHandler() {}
	};

	//Interface class for component user
	class CScriptingInterface : public CScriptHandler {
	private:
		struct IInternalCmdHandler {
			virtual bool InternalHandlerFunc(class CScriptingInterface* pThis, class ICodeContext* pContext) = 0;
		};
		struct internal_cmd_s {
			std::wstring wszName;
			IInternalCmdHandler* pHandler;
			bool bIsPrefixed;
		};

		struct funcarg_s {
			std::wstring wszName;
			std::wstring wszType;
			CVarManager::cvartype_e eType;
		};
		struct localvar_s {
			std::wstring wszName;
			cvarptr_t pCVar;
			cvartype_e eType;
			std::wstring wszType;
		};
		struct functions_s {
			std::wstring wszFuncName;
			std::vector<funcarg_s> vParameters;
			std::wstring wszFuncCode;
			std::wstring wszResVarType;
			CVarManager::cvartype_e eResVarType;
			CVarManager::cvarptr_t pCVar;
			std::vector<localvar_s> vLocalVars;
		};

		struct functioncall_user_s {
			bool bIsValid;
			CVarManager::cvarptr_t pResultVar;
			std::wstring wszFuncName;
			std::wstring wszFuncCode;
		};

		typedef void (*TpfnStandardOutput)(const dnyString& wszOutputText);

		std::wstring m_wszScriptDirectory;
		std::vector<internal_cmd_s> m_vInternalCmdList;
		std::vector<functions_s> m_vFunctions;
		TpfnStandardOutput m_pfnStandardOutput;
		std::vector<size_t> m_vCurrentFunctionContexts;
		functioncall_user_s m_sCurUserFunctionCall;
		bool m_bContinueScriptExecution;
		CObjectMgr* m_pObjectMgr;

		//Function management methods

		size_t FindFunction(const std::wstring& wszName)
		{
			//Find item list ID of a registered function

			if (!wszName.length())
				return std::wstring::npos;

			for (size_t i = 0; i < this->m_vFunctions.size(); i++) {
				if (this->m_vFunctions[i].wszFuncName == wszName)
					return i;
			}

			return std::wstring::npos;
		}

		bool RegisterFunction(const std::wstring& wszName, const std::wstring& wszResultVarType, const std::wstring& wszParamList, const std::wstring& wszCode)
		{
			//Register a function

			if ((!wszName.length()) || (!wszResultVarType.length()))
				return false;
			
			//If function with given name does already exist then delete it in order to replace it
			if (this->FindFunction(wszName) != std::wstring::npos) {
				if (!this->UnregisterFunction(wszName))
					return false;
			}
			
			//Setup data struct and save first data
			functions_s sFunctionData;
			sFunctionData.wszFuncName = wszName;
			sFunctionData.wszFuncCode = wszCode;
			
			//Get result var type and save it

			CVarManager::cvartype_e eType = CVarManager::GetTypeFromString(wszResultVarType);
			if (eType == CT_UNKNOWN)
				return false;
			
			sFunctionData.eResVarType = eType;
			sFunctionData.wszResVarType = wszResultVarType;

			//Create list of parameters
			
			std::vector<dnyString> vArrayItems = CSyntaxParser::ParseParamList(wszParamList + dnyListSeparator); //Get list of parameters
			
			//Add parameter with type to list
			for (size_t i = 0; i < vArrayItems.size(); i++) {
				funcarg_s sFuncArg;
				std::wstring szItem;

				//Query name and vartype from string stream
				std::wstringstream wStream(vArrayItems[i]);
				size_t uiCounter = 0;
				while (std::getline(wStream, szItem, dnySpaceCharacter)) {
					if (uiCounter == 0) {
						sFuncArg.wszName = szItem;
					} else if (uiCounter == 1) {
						sFuncArg.wszType = szItem;
						sFuncArg.eType = CVarManager::GetTypeFromString(sFuncArg.wszType);
						if (sFuncArg.eType == CT_UNKNOWN) {
							return false;
						}
					} else {
						return false;
					}
					
					uiCounter++;
				}

				sFunctionData.vParameters.push_back(sFuncArg);
			}

			//Add to list
			this->m_vFunctions.push_back(sFunctionData);
			
			return true;
		}

		bool CallFunction(const std::wstring& wszName, const std::wstring& wszArguments, cvarptr_t pResultVar)
		{
			//Call a given function

			if (!wszName.length())
				return false;
			
			//Attempt to find function by name
			size_t uiFuncListId = this->FindFunction(wszName);
			if (uiFuncListId == std::wstring::npos)
				return false;

			//Parse argument list
			std::vector<dnyString> vPassedArguments = CSyntaxParser::ParseArrayList(wszArguments);

			//Register temporary function argument variables if desired
			if (this->m_vFunctions[uiFuncListId].vParameters.size() == vPassedArguments.size()) {
				for (size_t i = 0; i < this->m_vFunctions[uiFuncListId].vParameters.size(); i++) {
					cvarptr_t pCVar = this->RegisterLocalVariable(this->m_vFunctions[uiFuncListId].vParameters[i].wszName, this->m_vFunctions[uiFuncListId].vParameters[i].wszType, false, uiFuncListId);
					if (!pCVar)
						return false;

					//Set initial value
					switch (this->m_vFunctions[uiFuncListId].vParameters[i].eType) {
					case CT_BOOL: {
						ICVar<dnyBoolean>* pBoolVar = (ICVar<dnyBoolean>*)pCVar;
						bool bValue = (!(_wtoi64(vPassedArguments[i].c_str()) == 0));
						pBoolVar->SetValue(bValue);
						break;
					}
					case CT_INT: {
						ICVar<dnyInteger>* pIntVar = (ICVar<dnyInteger>*)pCVar;
						__int64 iValue = _wtoi64(vPassedArguments[i].c_str());
						pIntVar->SetValue(iValue);
						break;
					}
					case CT_FLOAT: {
						ICVar<dnyFloat>* pFloatVar = (ICVar<dnyFloat>*)pCVar;
						double fValue = _wtof(vPassedArguments[i].c_str());
						pFloatVar->SetValue(fValue);
						break;
					}
					case CT_STRING: {
						ICVar<dnyString>* pStringVar = (ICVar<dnyString>*)pCVar;
						pStringVar->SetValue(vPassedArguments[i]);
						break;
					}
					case CT_CUSTOM: {
						this->AssignCustomVar(this->m_vFunctions[uiFuncListId].vParameters[i].wszName, vPassedArguments[i]);
						break;
					}
					default:
						return false;
					}
				}
			}
			
			//Create result variable if required
			if ((this->m_vFunctions[uiFuncListId].eResVarType != CT_UNKNOWN) && (this->m_vFunctions[uiFuncListId].eResVarType != CT_VOID)) {
				this->m_vFunctions[uiFuncListId].pCVar = this->RegisterLocalVariable(wszName, this->m_vFunctions[uiFuncListId].wszResVarType, false, uiFuncListId);
				if (!this->m_vFunctions[uiFuncListId].pCVar) {
					this->FreeFunctionVars(uiFuncListId);
					return false;
				}
			}

			//Add current function context
			this->m_vCurrentFunctionContexts.push_back(uiFuncListId);

			//Execute code with replaced variable values
			bool bResult = this->ExecuteCode(this->ReplaceVariables(this->m_vFunctions[uiFuncListId].wszFuncCode));

			//Query result var if required and save value
			if (pResultVar != nullptr) {
				switch (this->m_vFunctions[uiFuncListId].eResVarType) {
				case CT_BOOL: {
					ICVar<dnyBoolean>* pBoolVar = (ICVar<dnyBoolean>*)pResultVar;
					pBoolVar->SetValue(((ICVar<dnyBoolean>*)(this->m_vFunctions[uiFuncListId].pCVar))->GetValue());
					break;
				}
				case CT_INT: {
					ICVar<dnyInteger>* pIntVar = (ICVar<dnyInteger>*)pResultVar;
					pIntVar->SetValue(((ICVar<dnyInteger>*)(this->m_vFunctions[uiFuncListId].pCVar))->GetValue());
					break;
				}
				case CT_FLOAT: {
					ICVar<dnyFloat>* pFloatVar = (ICVar<dnyFloat>*)pResultVar;
					pFloatVar->SetValue(((ICVar<dnyFloat>*)(this->m_vFunctions[uiFuncListId].pCVar))->GetValue());
					break;
				}
				case CT_STRING: {
					ICVar<dnyString>* pStringVar = (ICVar<dnyString>*)pResultVar;
					pStringVar->SetValue(((ICVar<dnyString>*)(this->m_vFunctions[uiFuncListId].pCVar))->GetValue());
					break;
				}
				case CT_CUSTOM: {
					this->AssignCustomVar(wszName, ((ICVar<dnycustom>*)(this->m_vFunctions[uiFuncListId].pCVar))->GetValue());
					break;
				}
				default:
					return false;
				}
			}

			//Free result and argument variables
			this->FreeFunctionVars(uiFuncListId);

			//Clear function context
			this->m_vCurrentFunctionContexts.erase(this->m_vCurrentFunctionContexts.begin() + this->m_vCurrentFunctionContexts.size() - 1);

			return bResult;
		}

		bool FindLocalVariable(const std::wstring& wszName, const size_t uiFunctionId, size_t* puiIdOut = nullptr)
		{
			//Find local variable in function

			if (uiFunctionId == std::wstring::npos)
				return false;

			for (size_t i = 0; i < this->m_vFunctions[uiFunctionId].vLocalVars.size(); i++) {
				if (this->m_vFunctions[uiFunctionId].vLocalVars[i].wszName == wszName) {
					if (puiIdOut)
						*puiIdOut = i;

					return true;
				}
			}

			return false;
		}

		localvar_s* FindLocalVariablePtr(const std::wstring& wszName, const size_t uiFunctionId)
		{
			//Find local variable in function

			if (uiFunctionId == std::wstring::npos)
				return nullptr;
			
			for (size_t i = 0; i < this->m_vFunctions[uiFunctionId].vLocalVars.size(); i++) {
				if (this->m_vFunctions[uiFunctionId].vLocalVars[i].wszName == wszName) {
					return &this->m_vFunctions[uiFunctionId].vLocalVars[i];
				}
			}

			return nullptr;
		}

		cvarptr_t RegisterLocalVariable(const std::wstring& wszName, const std::wstring& wszType, bool bAtBottom, const size_t uiFunctionId)
		{
			//Register local variable to function

			if ((!wszName.length()) || (!wszType.length()))
				return nullptr;

			//Acquire CVar type
			cvartype_e eType = this->GetTypeFromString(wszType);
			if (eType == CT_UNKNOWN)
				return nullptr;

			//Spawn CVar object instance according to type
			cvarptr_t pCVar = ((eType == CT_CUSTOM) ? this->SpawnCustomCVar(wszName, wszType, false) : this->SpawnCVar(wszName, eType, false));
			if (!pCVar)
				return nullptr;

			//Setup data struct
			localvar_s sVarData;
			sVarData.wszName = wszName;
			sVarData.eType = eType;
			sVarData.pCVar = pCVar;
			sVarData.wszType = wszType;

			//Add to list
			if (bAtBottom)
				this->m_vFunctions[uiFunctionId].vLocalVars.insert(this->m_vFunctions[uiFunctionId].vLocalVars.begin() + 0, sVarData); //Insert as first entry
			else
				this->m_vFunctions[uiFunctionId].vLocalVars.push_back(sVarData); //Append to list

			return pCVar;
		}

		bool FreeLocalVariable(const std::wstring& wszName, const size_t uiFunctionId)
		{
			//Free local variable of function

			if (uiFunctionId >= this->m_vFunctions.size())
				return false;

			for (size_t i = 0; i < this->m_vFunctions[uiFunctionId].vLocalVars.size(); i++) {
				if (this->m_vFunctions[uiFunctionId].vLocalVars[i].wszName == wszName) {
					delete this->m_vFunctions[uiFunctionId].vLocalVars[i].pCVar;
					this->m_vFunctions[uiFunctionId].vLocalVars.erase(this->m_vFunctions[uiFunctionId].vLocalVars.begin() + i);
					return true;
				}
			}

			return false;
		}

		std::wstring ReplaceLocalFunctionVariables(const std::wstring& wszInputString, const size_t uiFunctionId)
		{
			//Replace variable identifiers with values

			if ((!wszInputString.length()) || (uiFunctionId == std::wstring::npos))
				return wszInputString;

			std::wstring wszResult = wszInputString;
			wchar_t wcsVarDerefIdent[] = { dnyVariableEntrance, 0x000 };

			//Iterate thorugh all variable items
			for (size_t i = 0; i < this->m_vFunctions[uiFunctionId].vLocalVars.size(); i++) {
				//Ignore command result variables
				if (this->m_vFunctions[uiFunctionId].vLocalVars[i].wszName[0] == '$') continue;

				//Attempt to find all occurences of variable name with prefix as substring
				size_t uiSubstrPos = wszResult.find(std::wstring(wcsVarDerefIdent) + this->m_vFunctions[uiFunctionId].vLocalVars[i].wszName);
				while (uiSubstrPos != std::wstring::npos) {
					//Assign replacer string with variables value
					std::wstring wszReplacerString = L"";
					switch (this->m_vFunctions[uiFunctionId].vLocalVars[i].eType) {
					case CT_BOOL: {
						ICVar<dnyBoolean>* pBoolVar = (ICVar<dnyBoolean>*)this->m_vFunctions[uiFunctionId].vLocalVars[i].pCVar;
						wszReplacerString = (pBoolVar->GetValue()) ? L"true" : L"false";
						break;
					}
					case CT_INT: {
						ICVar<dnyInteger>* pIntVar = (ICVar<dnyInteger>*)this->m_vFunctions[uiFunctionId].vLocalVars[i].pCVar;
						wszReplacerString = std::to_wstring(pIntVar->GetValue());
						break;
					}
					case CT_FLOAT: {
						ICVar<dnyFloat>* pFloatVar = (ICVar<dnyFloat>*)this->m_vFunctions[uiFunctionId].vLocalVars[i].pCVar;
						wszReplacerString = std::to_wstring(pFloatVar->GetValue());
						break;
					}
					case CT_STRING: {
						ICVar<dnyString>* pStringVar = (ICVar<dnyString>*)this->m_vFunctions[uiFunctionId].vLocalVars[i].pCVar;
						wszReplacerString = pStringVar->GetValue();
						break;
					}
					case CT_CUSTOM: {
						custom_cvar_type_s* pCustomCVarDataType = this->GetDatatypeObjectData(this->m_vFunctions[uiFunctionId].vLocalVars[i].wszType);
						if (!pCustomCVarDataType) return wszResult;
						wszReplacerString = pCustomCVarDataType->sEventTable.pfnGetReplacerString(this->m_vFunctions[uiFunctionId].vLocalVars[i].wszName, (ICVar<dnycustom>*)this->m_vFunctions[uiFunctionId].vLocalVars[i].pCVar);
						break;
					}
					default:
						break;
					}

					//Replace var identifier with value
					wszResult = wszResult.replace(wszResult.begin() + uiSubstrPos, wszResult.begin() + uiSubstrPos + this->m_vFunctions[uiFunctionId].vLocalVars[i].wszName.length() + 1, wszReplacerString);
				
					//Update position value
					uiSubstrPos = wszResult.find(std::wstring(wcsVarDerefIdent) + this->m_vFunctions[uiFunctionId].vLocalVars[i].wszName);
				}
			}

			return wszResult;
		}

		bool FreeFunctionVars(const size_t uiFunctionId)
		{
			//Free function vars

			if (uiFunctionId >= this->m_vFunctions.size())
				return false;

			for (size_t i = 0; i < this->m_vFunctions[uiFunctionId].vLocalVars.size(); i++) {
				delete this->m_vFunctions[uiFunctionId].vLocalVars[i].pCVar;
			}

			this->m_vFunctions[uiFunctionId].vLocalVars.clear();

			return true;
		}

		bool UnregisterFunction(const std::wstring& wszName)
		{
			//Unregister a function

			size_t uiListId = this->FindFunction(wszName);
			if (uiListId != std::wstring::npos) {
				this->m_vFunctions.erase(this->m_vFunctions.begin() + uiListId);
				return true;
			}

			return false;
		}

		//Internal command handler methods

		internal_cmd_s* FindCommandOrPrefix(const std::wstring& wszIdent)
		{
			//Find command or prefixed ident

			for (size_t i = 0; i < this->m_vInternalCmdList.size(); i++) {
				if (this->m_vInternalCmdList[i].bIsPrefixed) {
					if (this->m_vInternalCmdList[i].wszName[0] == wszIdent[0])
						return &this->m_vInternalCmdList[i];
				} else {
					if (this->m_vInternalCmdList[i].wszName == wszIdent)
						return &this->m_vInternalCmdList[i];
				}
			}

			return nullptr;
		}

		bool AddCommand(const std::wstring& wszCommand, IInternalCmdHandler* pHandler)
		{
			//Add internal command

			if ((!wszCommand.length()) || (!pHandler))
				return false;

			//Check if name is already in use
			if (this->FindCommandOrPrefix(wszCommand))
				return false;

			//Setup data struct and add to list
			internal_cmd_s sInternalCmd;
			sInternalCmd.bIsPrefixed = false;
			sInternalCmd.pHandler = pHandler;
			sInternalCmd.wszName = wszCommand;
			this->m_vInternalCmdList.push_back(sInternalCmd);

			return true;
		}

		bool AddCommandPrefix(const wchar_t wcPrefix, IInternalCmdHandler* pHandler)
		{
			//Add internal prefix ident

			if ((!wcPrefix) || (!pHandler))
				return false;

			wchar_t wszAsString[2] = { wcPrefix, 0x0000 };

			//Check if name is already in use
			if (this->FindCommandOrPrefix(wszAsString))
				return false;

			//Setup data struct and add to list
			internal_cmd_s sInternalCmd;
			sInternalCmd.bIsPrefixed = false;
			sInternalCmd.pHandler = pHandler;
			sInternalCmd.wszName = std::wstring(wszAsString);
			this->m_vInternalCmdList.push_back(sInternalCmd);

			return true;
		}

		bool CallHandlerFunction(const std::wstring& wszName, ICodeContext* pContext)
		{
			//Call handler of internal command

			internal_cmd_s* pInternalCmd = FindCommandOrPrefix(wszName);
			if (!pInternalCmd)
				return false;

			if (!pInternalCmd->pHandler->InternalHandlerFunc(this, pContext)) {
				dnySetError(SET_INTERNAL_CMD_FAILURE, L"Command execution failed for: \n" + wszName + L"\n" + pContext->GetAsLine() + L"\n\n");
				return false;
			}

			return true;
		}

		#define INTERNAL_COMMAND_HANDLER_METHOD(name, code) struct I##name : public IInternalCmdHandler { bool InternalHandlerFunc(class CScriptingInterface* pThis, class ICodeContext* pContext) { {code} } } o##name;
		#define CHECK_VALID_ARGUMENT_COUNT(cnt) if (pContext->GetPartCount() != cnt) return false;

		INTERNAL_COMMAND_HANDLER_METHOD(HandleConstantDeclaration,
			//Handle constant declaration

			CHECK_VALID_ARGUMENT_COUNT(5);

			CVarManager::cvartype_e eType = pThis->GetTypeFromString(pContext->GetPartData(2));
			if (eType == CT_UNKNOWN)
				return false;

			if (pContext->GetPartData(3) != L"<=")
				return false;

			cvarptr_t pCVar = nullptr;

			if (eType != CT_CUSTOM) {
				pCVar = pThis->RegisterCVar(pContext->GetPartData(1), eType, true, false);
				if (!pCVar)
					return false;
			} else {
				pCVar = pThis->RegisterCustomCVar(pContext->GetPartData(1), pContext->GetPartData(2), true, false);
				if (!pCVar)
					return false;
			}

			std::wstring wszValue = pThis->ReplaceAllVariables(pContext->GetPartData(4));

			switch (eType) {
			case CT_BOOL: {
				ICVar<dnyBoolean>* pBoolVar = (ICVar<dnyBoolean>*)pCVar;
				pBoolVar->SetValue(((wszValue == L"true") ? true : false));
				break;
			}
			case CT_INT: {
				ICVar<dnyInteger>* pIntVar = (ICVar<dnyInteger>*)pCVar;
				pIntVar->SetValue(_wtoi64(wszValue.c_str()));
				break;
			}
			case CT_FLOAT: {
				ICVar<dnyFloat>* pFloatVar = (ICVar<dnyFloat>*)pCVar;
				pFloatVar->SetValue(_wtof(wszValue.c_str()));
				break;
			}
			case CT_STRING: {
				ICVar<dnyString>* pStringVar = (ICVar<dnyString>*)pCVar;
				pStringVar->SetValue(wszValue);
				break;
			}
			case CT_CUSTOM: {
				if (!pThis->AssignCustomVar(pContext->GetPartData(1), wszValue))
					return false;
				break;
			}
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleVariableDeclaration, 
			//Handle variable declaration

			CHECK_VALID_ARGUMENT_COUNT(3);

			CVarManager::cvartype_e eType = pThis->GetTypeFromString(pContext->GetPartData(2));
			if (eType == CT_UNKNOWN)
				return false;

			return (eType != CT_CUSTOM) ? pThis->RegisterCVar(pContext->GetPartData(1), eType, false, false) != nullptr : pThis->RegisterCustomCVar(pContext->GetPartData(1), pContext->GetPartData(2), false, false) != nullptr;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleVariableAssignment, 
			//Handle variable assignment

			CHECK_VALID_ARGUMENT_COUNT(4);

			std::wstring wszVarName = pThis->ReplaceAllVariables(pContext->GetPartData(1));
			std::wstring wszVarAllocator = pContext->GetPartData(2);
			std::wstring wszVarValue = pThis->ReplaceAllVariables(pContext->GetPartData(3));

			if (wszVarAllocator != L"<=")
				return false;

			cvartype_e eType = CT_UNKNOWN;
			cvarptr_t pCvar = nullptr;

			localvar_s* pLocalVar = pThis->FindLocalVariablePtr(wszVarName, pThis->GetCurrentFunctionContext());
			if (!pLocalVar) {
				pCvar = pThis->FindCVar(wszVarName);
				if (!pCvar)
					return false;

				eType = pThis->GetCVarType(wszVarName);
			} else {
				pCvar = pLocalVar->pCVar;
				eType = pLocalVar->eType;
			}

			switch (eType) {
			case CT_BOOL: {
				ICVar<dnyBoolean>* pBoolVar = (ICVar<dnyBoolean>*)pCvar;
				bool bValue = (wszVarValue != L"false");
				pBoolVar->SetValue(bValue);
				break;
			}
			case CT_INT: {
				ICVar<dnyInteger>* pIntVar = (ICVar<dnyInteger>*)pCvar;
				__int64 iValue = _wtoi64(wszVarValue.c_str());
				pIntVar->SetValue(iValue);
				break;
			}
			case CT_FLOAT: {
				ICVar<dnyFloat>* pFloatVar = (ICVar<dnyFloat>*)pCvar;
				double fValue = _wtof(wszVarValue.c_str());
				pFloatVar->SetValue(fValue);
				break;
			}
			case CT_STRING: {
				ICVar<dnyString>* pStringVar = (ICVar<dnyString>*)pCvar;
				pStringVar->SetValue(wszVarValue);
				break;
			}
			case CT_CUSTOM: {
				if (!pThis->AssignCustomVar(wszVarName, wszVarValue))
					return false;
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleVariableRemoval,
			//Handle variable removal

			CHECK_VALID_ARGUMENT_COUNT(2);

			std::wstring wszVarName = pContext->GetPartData(1);

			return pThis->FreeCVar(wszVarName);
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleFunctionRegistration,
			//Handle function registration

			CHECK_VALID_ARGUMENT_COUNT(5);
		
			return pThis->RegisterFunction(pContext->GetPartData(1), pContext->GetPartData(2), pContext->GetPartData(3), pContext->GetPartData(4));
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleLocalVarRegistration,
			//Handle local variable registration of current function execution

			CHECK_VALID_ARGUMENT_COUNT(3);

			return pThis->RegisterLocalVariable(pContext->GetPartData(1), pContext->GetPartData(2), false, pThis->GetCurrentFunctionContext()) != nullptr;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleFunctionCall,
			//Handle function call

			CHECK_VALID_ARGUMENT_COUNT(5);
		
			if (pContext->GetPartData(3) != L"=>")
				return false;

			cvarptr_t pCVar = nullptr;

			if (pContext->GetPartData(4) != L"void") {
				pCVar = pThis->FindCVar(pContext->GetPartData(4));
			}
			
			return pThis->CallFunction(pThis->ReplaceAllVariables(pContext->GetPartData(1)), pThis->ReplaceAllVariables(pContext->GetPartData(2)), pCVar);
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleFunctionResult,
			//Handle function result storage

			CHECK_VALID_ARGUMENT_COUNT(2);

			CVarManager::cvartype_e eType = pThis->GetCurrentFunctionResultVarType();
			if (eType == CT_UNKNOWN)
				return false;

			switch (eType) {
			case CT_BOOL: {
				CVarManager::ICVar<dnyBoolean>* pBoolVar = (CVarManager::ICVar<dnyBoolean>*)pThis->GetCurrentFunctionResultVarPtr();
				bool bValue = false;
				if (pContext->GetPartData(1) == L"true") bValue = true; else if (pContext->GetPartData(1) == L"false") bValue = false;
				pBoolVar->SetValue(bValue);
				break;
			}
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pThis->GetCurrentFunctionResultVarPtr();
				pIntVar->SetValue(_wtoi64(pThis->ReplaceAllVariables(pContext->GetPartData(1)).c_str()));
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pThis->GetCurrentFunctionResultVarPtr();
				pFloatVar->SetValue(_wtof(pThis->ReplaceAllVariables(pContext->GetPartData(1)).c_str()));
				break;
			}
			case CT_STRING: {
				CVarManager::ICVar<dnyString>* pStringVar = (CVarManager::ICVar<dnyString>*)pThis->GetCurrentFunctionResultVarPtr();
				pStringVar->SetValue(pThis->ReplaceAllVariables(pContext->GetPartData(1)));
				break;
			}
			case CT_CUSTOM: {
				pThis->AssignCustomVar((CVarManager::ICVar<dnycustom>*)pThis->GetCurrentFunctionResultVarPtr(), pThis->GetCurrentFunctionResultVarTypeName(), pThis->ReplaceAllVariables(pContext->GetPartData(1)));
				break;
			}
			default:
				break;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(CreateClassDefinition,
			//Handle class registration

			CHECK_VALID_ARGUMENT_COUNT(3);

			return pThis->GetObjectMgr()->AddObject(pContext->GetPartString(1), pContext->GetPartString(2));
		);

		INTERNAL_COMMAND_HANDLER_METHOD(AddClassMember,
			//Add class member

			return pThis->GetObjectMgr()->AddMemberToCurrentObject(pContext, pThis);
		);

		INTERNAL_COMMAND_HANDLER_METHOD(AddClassMethod,
			//Add class method

			return pThis->GetObjectMgr()->AddMethodToCurrentObject(pContext, pThis);
		);

		INTERNAL_COMMAND_HANDLER_METHOD(AllocClassInstance,
			//Allocate class instance

			CHECK_VALID_ARGUMENT_COUNT(3);

			return pThis->GetObjectMgr()->AllocObject(pContext->GetPartString(1), pContext->GetPartString(2));
		);

		INTERNAL_COMMAND_HANDLER_METHOD(FreeClassInstance,
			//Free class instance

			return pThis->GetObjectMgr()->FreeObject(pContext->GetPartString(1));
		);

		//Elseif handler insertion macro
		#define INSERT_ELSEIF_HANDLER(cond1, operation, cond2, code, elseindicator) \
		if (operation == L"-eq") { \
			if (pThis->ReplaceAllVariables(cond1) == pThis->ReplaceAllVariables(cond2)) { \
				pThis->ExecuteCode(code); \
				elseindicator = false; \
			} \
		} else if (operation == L"-nt") { \
			if (pThis->ReplaceAllVariables(cond1) != pThis->ReplaceAllVariables(cond2)) { \
				pThis->ExecuteCode(code); \
				elseindicator = false; \
			} \
		} else if (operation == L"-ls") { \
			if (_wtof(pThis->ReplaceAllVariables(cond1).c_str()) < _wtof(pThis->ReplaceAllVariables(cond2).c_str())) { \
				pThis->ExecuteCode(code); \
				elseindicator = false; \
			} \
		} else if (operation == L"-gr") { \
			if (_wtof(pThis->ReplaceAllVariables(cond1).c_str()) > _wtof(pThis->ReplaceAllVariables(cond2).c_str())) { \
				pThis->ExecuteCode(code); \
				elseindicator = false; \
			} \
		} else if (operation == L"-lse") { \
			if (_wtof(pThis->ReplaceAllVariables(cond1).c_str()) <= _wtof(pThis->ReplaceAllVariables(cond2).c_str())) { \
				pThis->ExecuteCode(code); \
				elseindicator = false; \
			} \
		} else if (operation == L"-gre") { \
			if (_wtof(pThis->ReplaceAllVariables(cond1).c_str()) >= _wtof(pThis->ReplaceAllVariables(cond2).c_str())) { \
				pThis->ExecuteCode(code); \
				elseindicator = false; \
			} \
		} else { return false; }
		INTERNAL_COMMAND_HANDLER_METHOD(HandleIfElseIfElseStatement,
			//Handle if-elseif-else statements

			//Parse parameters
			std::vector<std::wstring> vParams = pThis->ParseArrayList(pContext->GetPartData(1));
			if (vParams.size() != 3)
				return false;

			//Query arguments
			std::wstring wszIfComp1 = vParams[0];
			std::wstring wszIfOperation = vParams[1];
			std::wstring wszIfComp2 = vParams[2];
			
			//Handle statement according operation type
			if (wszIfOperation == L"-eq") { //Equal
				//Execute code if statement is true
				if (pThis->ReplaceAllVariables(wszIfComp1) == pThis->ReplaceAllVariables(wszIfComp2)) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				} else {
					//Check for elseif's or else's and handle them

					bool bShallExecuteElse = true;

					for (size_t i = 3; i < pContext->GetPartCount(); i++) {
						if (pContext->GetPartData(i) == L"elseif") {
							//Parse elseif parameters
							std::vector<std::wstring> vElseifParams = pThis->ParseArrayList(pContext->GetPartData(i + 1));
							if (vElseifParams.size() != 3) return false;
							
							//Insert elseif handler code
							INSERT_ELSEIF_HANDLER(vElseifParams[0], vElseifParams[1], vElseifParams[2], pContext->GetPartData(i + 2), bShallExecuteElse);

							//Goto next part if exists
							i += 2;
						} else if (pContext->GetPartData(i) == L"else") {
							//Execute code if indicated
							if (bShallExecuteElse)
								pThis->ExecuteCode(pContext->GetPartData(i + 1));
								
							break; //Break out when else is reached
						} else { return false; }
					}
				}
			} else if (wszIfOperation == L"-nt") { //Not equal
				if (pThis->ReplaceAllVariables(wszIfComp1) != pThis->ReplaceAllVariables(wszIfComp2)) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				} else {
					//Check for elseif's or else's and handle them

					bool bShallExecuteElse = true;

					for (size_t i = 3; i < pContext->GetPartCount(); i++) {
						if (pContext->GetPartData(i) == L"elseif") {
							//Parse elseif parameters
							std::vector<std::wstring> vElseifParams = pThis->ParseArrayList(pContext->GetPartData(i + 1));
							if (vElseifParams.size() != 3) return false;

							//Insert elseif handler code
							INSERT_ELSEIF_HANDLER(vElseifParams[0], vElseifParams[1], vElseifParams[2], pContext->GetPartData(i + 2), bShallExecuteElse);

							//Goto next part if exists
							i += 2;
						} else if (pContext->GetPartData(i) == L"else") {
							//Execute code if indicated
							if (bShallExecuteElse)
								pThis->ExecuteCode(pContext->GetPartData(i + 1));

							break; //Break out when else is reached
						} else { return false; }
					}
				}
			} else if (wszIfOperation == L"-ls") { //Less than
				if (_wtof(pThis->ReplaceAllVariables(wszIfComp1).c_str()) < _wtof(pThis->ReplaceAllVariables(wszIfComp2).c_str())) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				} else {
					//Check for elseif's or else's and handle them

					bool bShallExecuteElse = true;

					for (size_t i = 3; i < pContext->GetPartCount(); i++) {
						if (pContext->GetPartData(i) == L"elseif") {
							//Parse elseif parameters
							std::vector<std::wstring> vElseifParams = pThis->ParseArrayList(pContext->GetPartData(i + 1));
							if (vElseifParams.size() != 3) return false;

							//Insert elseif handler code
							INSERT_ELSEIF_HANDLER(vElseifParams[0], vElseifParams[1], vElseifParams[2], pContext->GetPartData(i + 2), bShallExecuteElse);

							//Goto next part if exists
							i += 2;
						} else if (pContext->GetPartData(i) == L"else") {
							//Execute code if indicated
							if (bShallExecuteElse)
								pThis->ExecuteCode(pContext->GetPartData(i + 1));

							break; //Break out when else is reached
						} else { return false; }
					}
				}
			} else if (wszIfOperation == L"-gr") { //Greater than
				if (_wtof(pThis->ReplaceAllVariables(wszIfComp1).c_str()) > _wtof(pThis->ReplaceAllVariables(wszIfComp2).c_str())) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				} else {
					//Check for elseif's or else's and handle them

					bool bShallExecuteElse = true;

					for (size_t i = 3; i < pContext->GetPartCount(); i++) {
						if (pContext->GetPartData(i) == L"elseif") {
							//Parse elseif parameters
							std::vector<std::wstring> vElseifParams = pThis->ParseArrayList(pContext->GetPartData(i + 1));
							if (vElseifParams.size() != 3) return false;

							//Insert elseif handler code
							INSERT_ELSEIF_HANDLER(vElseifParams[0], vElseifParams[1], vElseifParams[2], pContext->GetPartData(i + 2), bShallExecuteElse);

							//Goto next part if exists
							i += 2;
						} else if (pContext->GetPartData(i) == L"else") {
							//Execute code if indicated
							if (bShallExecuteElse)
								pThis->ExecuteCode(pContext->GetPartData(i + 1));

							break; //Break out when else is reached
						} else { return false; }
					}
				}
			} else if (wszIfOperation == L"-lse") { //Less than or equal to
				
				if (_wtof(pThis->ReplaceAllVariables(wszIfComp1).c_str()) <= _wtof(pThis->ReplaceAllVariables(wszIfComp2).c_str())) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				} else {
					//Check for elseif's or else's and handle them
					
					bool bShallExecuteElse = true;

					for (size_t i = 3; i < pContext->GetPartCount(); i++) {
						if (pContext->GetPartData(i) == L"elseif") {
							//Parse elseif parameters
							std::vector<std::wstring> vElseifParams = pThis->ParseArrayList(pContext->GetPartData(i + 1));
							if (vElseifParams.size() != 3) return false;

							//Insert elseif handler code
							INSERT_ELSEIF_HANDLER(vElseifParams[0], vElseifParams[1], vElseifParams[2], pContext->GetPartData(i + 2), bShallExecuteElse);

							//Goto next part if exists
							i += 2;
						} else if (pContext->GetPartData(i) == L"else") { //Handle else statement if appropriate
							//Execute code if indicated
							if (bShallExecuteElse)
								pThis->ExecuteCode(pContext->GetPartData(i + 1));

							break; //Break out when else is reached
						} else { return false; }
					}
				}
			} else if (wszIfOperation == L"-gre") { //Greater than or equal to
				if (_wtof(pThis->ReplaceAllVariables(wszIfComp1).c_str()) >= _wtof(pThis->ReplaceAllVariables(wszIfComp2).c_str())) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				} else {
					//Check for elseif's or else's and handle them

					bool bShallExecuteElse = true;

					for (size_t i = 3; i < pContext->GetPartCount(); i++) {
						if (pContext->GetPartData(i) == L"elseif") {
							//Parse elseif parameters
							std::vector<std::wstring> vElseifParams = pThis->ParseArrayList(pContext->GetPartData(i + 1));
							if (vElseifParams.size() != 3) return false;

							//Insert elseif handler code
							INSERT_ELSEIF_HANDLER(vElseifParams[0], vElseifParams[1], vElseifParams[2], pContext->GetPartData(i + 2), bShallExecuteElse);

							//Goto next part if exists
							i += 2;
						} else if (pContext->GetPartData(i) == L"else") {
							//Execute code if indicated
							if (bShallExecuteElse)
								pThis->ExecuteCode(pContext->GetPartData(i + 1));

							break; //Break out when else is reached
						} else { return false; }
					}
				}
			} else { return false; }

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleForLoop,
			//Handle for-loop

			CHECK_VALID_ARGUMENT_COUNT(3);
		
			//Parse parameters
			std::vector<std::wstring> vParams = pThis->ParseArrayList(pContext->GetPartData(1));
			if (vParams.size() != 4)
				return false;

			//Query arguments
			std::wstring wszLoopVar = vParams[0];
			dnyInteger iLoopStart = _wtoi64(pThis->ReplaceAllVariables(vParams[1]).c_str());
			dnyInteger iLoopEnd = _wtoi64(pThis->ReplaceAllVariables(vParams[2]).c_str());
			std::wstring wszLoopStep = vParams[3];

			//Setup step-value
			dnyInteger iLoopStep = 0;
			if (wszLoopStep == L"-inc") {
				iLoopStep = 1;
			} else if (wszLoopStep == L"-dec") {
				iLoopStep = -1;
			} else {
				iLoopStep = _wtoi64(wszLoopStep.c_str());
			}

			//Register temporary loop variable

			ICVar<dnyInteger>* pVariable = nullptr;

			if (pThis->GetCurrentFunctionContext() != std::wstring::npos) {
				pVariable = (ICVar<dnyInteger>*)pThis->RegisterLocalVariable(wszLoopVar, L"int", true, pThis->GetCurrentFunctionContext());
			} else {
				pVariable = (ICVar<dnyInteger>*)pThis->RegisterCVar(wszLoopVar, CVarManager::CT_INT, false, true);
			}

			if (!pVariable)
				return false;

			//Set initial value
			pVariable->SetValue(iLoopStart);

			//Perform loop
			while (pVariable->GetValue() != iLoopEnd) { //Do while end value is not yet reached
				//Execute loop code
				pThis->ExecuteCode(pContext->GetPartData(2));

				//Add step-value to variable
				pVariable->SetValue(pVariable->GetValue() + iLoopStep);
			}

			//Remove variable
			if (pThis->GetCurrentFunctionContext() != std::wstring::npos)
				pThis->RemoveLocalVarFromCurrentFunctionContext(wszLoopVar);
			else
				pThis->FreeCVar(wszLoopVar);

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleWhileLoop,
			//Handle while-loop

			CHECK_VALID_ARGUMENT_COUNT(3);

			//Parse parameters
			std::vector<std::wstring> vParams = pThis->ParseArrayList(pContext->GetPartData(1));
			if (vParams.size() != 3)
				return false;

			//Query arguments
			std::wstring wszWhileComp1 = vParams[0];
			std::wstring wszWhileOperation = vParams[1];
			std::wstring wszWhileComp2 = vParams[2];

			//Handle loop according operation type
			if (wszWhileOperation == L"-eq") { //Equal
				while (pThis->ReplaceAllVariables(wszWhileComp1) == pThis->ReplaceAllVariables(wszWhileComp2)) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				}
			} else if (wszWhileOperation == L"-nt") { //Not equal
				while (pThis->ReplaceAllVariables(wszWhileComp1) != pThis->ReplaceAllVariables(wszWhileComp2)) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				}
			} else if (wszWhileOperation == L"-ls") { //Less than
				while (_wtof(pThis->ReplaceAllVariables(wszWhileComp1).c_str()) < _wtof(pThis->ReplaceAllVariables(wszWhileComp2).c_str())) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				}
			} else if (wszWhileOperation == L"-gr") { //Greater than
				while (_wtof(pThis->ReplaceAllVariables(wszWhileComp1).c_str()) > _wtof(pThis->ReplaceAllVariables(wszWhileComp2).c_str())) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				}
			} else if (wszWhileOperation == L"-lse") { //Less than or equal to
				while (_wtof(pThis->ReplaceAllVariables(wszWhileComp1).c_str()) <= _wtof(pThis->ReplaceAllVariables(wszWhileComp2).c_str())) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				}
			} else if (wszWhileOperation == L"-gre") { //Greater than or equal to
				while (_wtof(pThis->ReplaceAllVariables(wszWhileComp1).c_str()) >= _wtof(pThis->ReplaceAllVariables(wszWhileComp2).c_str())) {
					pThis->ExecuteCode(pContext->GetPartData(2));
				}
			} else { return false; }

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleScriptExecution,
			//Handle script execution

			CHECK_VALID_ARGUMENT_COUNT(2);

			std::wstring wszScriptFile = pThis->ReplaceAllVariables(pContext->GetPartData(1));

			return pThis->ExecuteScript(wszScriptFile);
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleStandardOutput,
			//Handle standard output

			if (!pThis->GetStandardOutput())
				return false;

			CHECK_VALID_ARGUMENT_COUNT(2);

			std::wstring wszTextToPrint = pThis->ReplaceAllVariables(pContext->GetPartData(1));

			(pThis->GetStandardOutput())(wszTextToPrint);

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleExitScriptCommand,
			//Handle script exit

			if (!pThis->GetStandardOutput())
				return false;

			pThis->AbortScriptExecution();

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleIncrementOp,
			//Handle increment operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;

			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue(pIntVar->GetValue() + 1);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(pFloatVar->GetValue() + 1);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleDecrementOp,
			//Handle decrement operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;

			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue(pIntVar->GetValue() - 1);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(pFloatVar->GetValue() - 1);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleIncreaseOp,
			//Handle increase operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));
			dnyFloat fTargetValue = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(1)).c_str());

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;

			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue(pIntVar->GetValue() + (dnyInteger)fTargetValue);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(pFloatVar->GetValue() + fTargetValue);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleDecreaseOp,
			//Handle decrease operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));
			dnyFloat fTargetValue = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(1)).c_str());

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;

			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue(pIntVar->GetValue() - (dnyInteger)fTargetValue);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(pFloatVar->GetValue() - fTargetValue);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleAddOp,
			//Handle add operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));
			dnyFloat fTargetValue1 = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(2)).c_str());
			dnyFloat fTargetValue2 = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(3)).c_str());

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;

			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue((dnyInteger)fTargetValue1 + (dnyInteger)fTargetValue2);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(fTargetValue1 + fTargetValue2);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleSubOp,
			//Handle sub operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));
			dnyFloat fTargetValue1 = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(2)).c_str());
			dnyFloat fTargetValue2 = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(3)).c_str());

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;

			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue((dnyInteger)fTargetValue1 - (dnyInteger)fTargetValue2);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(fTargetValue1 - fTargetValue2);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleMulOp,
			//Handle mul operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));
			dnyFloat fTargetValue1 = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(2)).c_str());
			dnyFloat fTargetValue2 = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(3)).c_str());

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;

			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue((dnyInteger)fTargetValue1 * (dnyInteger)fTargetValue2);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(fTargetValue1 * fTargetValue2);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleDivOp,
			//Handle div operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));
			dnyFloat fTargetValue1 = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(2)).c_str());
			dnyFloat fTargetValue2 = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(3)).c_str());

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;

			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue((dnyInteger)fTargetValue1 / (dnyInteger)fTargetValue2);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(fTargetValue1 / fTargetValue2);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleAddSelfOp,
			//Handle add self operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));
			dnyFloat fTargetValue = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(2)).c_str());

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;

			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue(pIntVar->GetValue() + (dnyInteger)fTargetValue);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(pFloatVar->GetValue() + fTargetValue);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleSubSelfOp,
			//Handle sub self operation


			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));
			dnyFloat fTargetValue = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(2)).c_str());

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;

			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue(pIntVar->GetValue() - (dnyInteger)fTargetValue);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(pFloatVar->GetValue() - fTargetValue);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleMulSelfOp,
			//Handle mul self operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));
			dnyFloat fTargetValue = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(2)).c_str());
			
			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;
			
			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue(pIntVar->GetValue() * (dnyInteger)fTargetValue);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(pFloatVar->GetValue() * fTargetValue);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleDivSelfOp,
			//Handle div self operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));
			dnyFloat fTargetValue = _wtof(pThis->ReplaceAllVariables(pContext->GetPartData(2)).c_str());

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if (!pVar)
				return false;

			switch (pThis->GetCVarType(wszTargetVar)) {
			case CT_INT: {
				CVarManager::ICVar<dnyInteger>* pIntVar = (CVarManager::ICVar<dnyInteger>*)pVar;
				pIntVar->SetValue(pIntVar->GetValue() / (dnyInteger)fTargetValue);
				break;
			}
			case CT_FLOAT: {
				CVarManager::ICVar<dnyFloat>* pFloatVar = (CVarManager::ICVar<dnyFloat>*)pVar;
				pFloatVar->SetValue(pFloatVar->GetValue() / fTargetValue);
				break;
			}
			default:
				return false;
			}

			return true;
		);

		INTERNAL_COMMAND_HANDLER_METHOD(HandleBoolNegOp,
			//Handle boolean negation operation

			std::wstring wszTargetVar = pThis->ReplaceAllVariables(pContext->GetPartData(1));

			CVarManager::cvarptr_t pVar = pThis->FindCVar(wszTargetVar);
			if ((!pVar) || (pThis->GetCVarType(wszTargetVar) != CVarManager::CT_BOOL))
				return false;

			CVarManager::ICVar<dnyBoolean>* pBoolVar = (CVarManager::ICVar<dnyBoolean>*)pVar;
			pBoolVar->SetValue(!pBoolVar->GetValue());

			return true;
		);

		bool HandleExternalCommands(const ICodeContext& oContext)
		{
			//Handle external commands

			if (!oContext.GetPartCount())
				return false;

			CCommandManager::command_s* pCommand = CCommandManager::FindCommand(oContext.GetPartData(0));
			if (!pCommand) {
				dnySetError(SET_UNKNOWN_COMMAND, L"Command is neither internal nor registered: \n" + oContext.GetAsLine() + L"\n\n");
				return false;
			}

			ICodeContext ctx = oContext;

			switch (pCommand->eResultVarType) {
			case CT_BOOL: {
				//Acquire and call handler
				IResultCommandInterface<dnyBoolean>* pCmdInt = (IResultCommandInterface<dnyBoolean>*)pCommand->cmdCallback;
				if (!pCmdInt->CommandCallback(&ctx, this)) return false;
				//Store result value
				CVarManager::ICVar<dnyBoolean>* pResultVar = (CVarManager::ICVar<dnyBoolean>*)this->FindCVar(oContext.GetPartData(oContext.GetPartCount() - 1));
				pResultVar->SetValue(pCmdInt->GetResult());
				break;
			}
			case CT_INT: {
				//Acquire and call handler
				IResultCommandInterface<dnyInteger>* pCmdInt = (IResultCommandInterface<dnyInteger>*)pCommand->cmdCallback;
				if (!pCmdInt->CommandCallback(&ctx, this)) return false;
				//Store result value
				CVarManager::ICVar<dnyInteger>* pResultVar = (CVarManager::ICVar<dnyInteger>*)this->FindCVar(oContext.GetPartData(oContext.GetPartCount() - 1));
				pResultVar->SetValue(pCmdInt->GetResult());
				break;
			}
			case CT_FLOAT: {
				//Acquire and call handler
				IResultCommandInterface<dnyFloat>* pCmdInt = (IResultCommandInterface<dnyFloat>*)pCommand->cmdCallback;
				if (!pCmdInt->CommandCallback(&ctx, this)) return false;
				//Store result value
				CVarManager::ICVar<dnyFloat>* pResultVar = (CVarManager::ICVar<dnyFloat>*)this->FindCVar(oContext.GetPartData(oContext.GetPartCount() - 1));
				pResultVar->SetValue(pCmdInt->GetResult());
				break;
			}
			case CT_STRING: {
				//Acquire and call handler
				IResultCommandInterface<dnyString>* pCmdInt = (IResultCommandInterface<dnyString>*)pCommand->cmdCallback;
				if (!pCmdInt->CommandCallback(&ctx, this)) return false;
				//Store result value
				CVarManager::ICVar<dnyString>* pResultVar = (CVarManager::ICVar<dnyString>*)this->FindCVar(oContext.GetPartData(oContext.GetPartCount() - 1));
				pResultVar->SetValue(pCmdInt->GetResult());
				break;
			}
			case CT_VOID: {
				IVoidCommandInterface* pCmdInt = (IVoidCommandInterface*)pCommand->cmdCallback;
				if (!pCmdInt->CommandCallback(&ctx, this)) return false;
				break;
			}
			default:
				return false;
			}

			return true;
		}

		bool RegisterInternalCommands(void)
		{
			//Register all internal commands

			#define REG_INTERNAL_CMD(name, pfn) if (!this->AddCommand(name, pfn)) return false;
			#define REG_INTERNAL_PREFIX(prefix, pfn) if (!this->AddCommandPrefix(prfix, pfn)) return false;

			REG_INTERNAL_CMD(L"const", &oHandleConstantDeclaration);
			REG_INTERNAL_CMD(L"declare", &oHandleVariableDeclaration);
			REG_INTERNAL_CMD(L"set", &oHandleVariableAssignment);
			REG_INTERNAL_CMD(L"undeclare", &oHandleVariableRemoval);
			REG_INTERNAL_CMD(L"function", &oHandleFunctionRegistration);
			REG_INTERNAL_CMD(L"local", &oHandleLocalVarRegistration);
			REG_INTERNAL_CMD(L"call", &oHandleFunctionCall);
			REG_INTERNAL_CMD(L"class", &oCreateClassDefinition);
			REG_INTERNAL_CMD(L"member", &oAddClassMember);
			REG_INTERNAL_CMD(L"method", &oAddClassMethod);
			//REG_INTERNAL_CMD(L"class_alloc", &oAllocClassInstance);
			//REG_INTERNAL_CMD(L"class_free", &oFreeClassInstance);
			REG_INTERNAL_CMD(L"result", &oHandleFunctionResult);
			REG_INTERNAL_CMD(L"if", &oHandleIfElseIfElseStatement);
			REG_INTERNAL_CMD(L"for", &oHandleForLoop);
			REG_INTERNAL_CMD(L"while", &oHandleWhileLoop);
			REG_INTERNAL_CMD(L"execute", &oHandleScriptExecution);
			REG_INTERNAL_CMD(L"print", &oHandleStandardOutput);
			REG_INTERNAL_CMD(L"exit", &oHandleExitScriptCommand);
			REG_INTERNAL_CMD(L"++", &oHandleIncrementOp);
			REG_INTERNAL_CMD(L"--", &oHandleDecrementOp);
			REG_INTERNAL_CMD(L"+", &oHandleAddOp);
			REG_INTERNAL_CMD(L"-", &oHandleSubOp);
			REG_INTERNAL_CMD(L"*", &oHandleMulOp);
			REG_INTERNAL_CMD(L"/", &oHandleDivOp);
			REG_INTERNAL_CMD(L"+=", &oHandleAddSelfOp);
			REG_INTERNAL_CMD(L"-=", &oHandleSubSelfOp);
			REG_INTERNAL_CMD(L"*=", &oHandleMulSelfOp);
			REG_INTERNAL_CMD(L"/=", &oHandleDivSelfOp);
			REG_INTERNAL_CMD(L"!", &oHandleBoolNegOp);

			return true;
		}
	public:
		CScriptingInterface() { this->RegisterInternalCommands(); this->m_sCurUserFunctionCall.bIsValid = false; this->m_pObjectMgr = new CObjectMgr(this);  }
		CScriptingInterface(const std::wstring& wszScriptDirectory, const TpfnStandardOutput pfnStandardOutput) : m_wszScriptDirectory(wszScriptDirectory), m_pfnStandardOutput(pfnStandardOutput) { this->RegisterInternalCommands(); this->m_sCurUserFunctionCall.bIsValid = false; this->m_pObjectMgr = new CObjectMgr(this);
		}
		~CScriptingInterface() { delete this->m_pObjectMgr; }

		//Parsing interface methods

		bool ExecuteCode(const std::wstring& wszCode)
		{
			//Execute code

			if (!wszCode.length())
				return false;

			CSyntaxParser oSyntaxParser;

			//Clear error status
			dnyclearError();

			//Perform syntax parsing
			if (!oSyntaxParser.ParseCode(wszCode))
				return false;

			//Set flag
			this->m_bContinueScriptExecution = true;

			//Handle code context objects
			for (size_t i = 0; i < oSyntaxParser.GetCodeContextCount(); i++) {
				//Check execution continuation flag and break out if unset
				if (!this->m_bContinueScriptExecution)
					break;

				//Get current code context item
				ICodeContext oCodeContext = oSyntaxParser.GetCodeContextItem(i);

				if (oCodeContext.GetPartCount()) {
					//Check for internal commands first
					if (!this->CallHandlerFunction(oCodeContext.GetPartData(0), &oCodeContext)) {
						if (GetErrorInformation().GetErrorCode() != SET_NO_ERROR) { //May be set by 'CallHandlerFunction'
							return false;
						}

						//No internal command found, handle external command
						if (!this->HandleExternalCommands(oCodeContext)) {
							if (GetErrorInformation().GetErrorCode() == SET_NO_ERROR) //May be set by 'HandleExternalCommands'
								dnySetError(SET_EXTERNAL_CMD_FAILURE, L"Command execution failed for: \n" + oCodeContext.GetAsLine() + L"\n\n");

							return false;
						}
					}
				}
			}

			return true;
		}

		bool ExecuteScript(const std::wstring& wszScriptFile)
		{
			//Execute script file

			if (!wszScriptFile.length())
				return false;

			//Clear error status
			dnyclearError();

			//Read entire script code
			if (!CScriptHandler::ReadAllLines(this->m_wszScriptDirectory + wszScriptFile)) {
				dnySetError(SET_SCRIPTFILE_INPUT, L"Error during script execution: " + m_wszScriptDirectory + wszScriptFile);
				return false;
			}
			
			//Execute code
			return this->ExecuteCode(CScriptHandler::GetFileBuffer());
		}

		//Directory methods
		inline void SetScriptBaseDirectory(const std::wstring& wszDirectory) { this->m_wszScriptDirectory = wszDirectory; }
		inline const std::wstring& GetScriptBaseDirectory(void) const { return this->m_wszScriptDirectory; }

		//Container interface methods
		cvarptr_t RegisterCVar(const std::wstring& wszName, const cvartype_e eType, bool bConst, bool bAtBottom) { return CVarManager::RegisterCVar(wszName, eType, bConst, bAtBottom); }
		bool FreeCVar(const std::wstring& wszName) { return CVarManager::FreeCVar(wszName); }
		bool RegisterCommand(const std::wstring& wszCmdName, CCommandManager::cmdinterface_t pCmdInterface, CVarManager::cvartype_e eType) { return CCommandManager::RegisterCommand(wszCmdName, pCmdInterface, eType); }
		bool UnregisterCommand(const std::wstring& wszCmdName) { return CCommandManager::UnregisterCommand(wszCmdName); }
		std::wstring ReplaceLocalFunctionVariables(const std::wstring& wszInputString) { return this->ReplaceLocalFunctionVariables(wszInputString, this->GetCurrentFunctionContext()); }
		std::wstring ReplaceAllVariables(const std::wstring& wszInputString)
		{
			std::wstring wszResult = wszInputString;
			wszResult = this->ReplaceLocalFunctionVariables(wszResult, this->GetCurrentFunctionContext());
			wszResult = this->ReplaceVariables(wszResult);
			return wszResult;
		}
		cvarptr_t FindCVar(const std::wstring& wszName)
		{
			cvarptr_t pResult = nullptr;
			
			localvar_s* pLocalVar = this->FindLocalVariablePtr(wszName, this->GetCurrentFunctionContext());
			if (pLocalVar) {
				pResult = pLocalVar->pCVar;
			} else {
				pResult = CVarManager::FindCVar(wszName);
			}

			return pResult;
		}
		cvartype_e GetCVarType(const std::wstring& wszName)
		{
			cvartype_e eResult = CT_UNKNOWN;

			localvar_s* pLocalVar = this->FindLocalVariablePtr(wszName, this->GetCurrentFunctionContext());
			if (pLocalVar) {
				eResult = pLocalVar->eType;
			}
			else {
				eResult = CVarManager::GetCVarType(wszName);
			}

			return eResult;
		}

		//User function calling

		bool BeginFunctionCall(const std::wstring& wszFuncName, const CVarManager::cvartype_e eResultType)
		{
			//Begin function call

			if (this->m_sCurUserFunctionCall.bIsValid)
				return false;

			if (eResultType == CT_UNKNOWN)
				return false;

			if (!wszFuncName.length())
				return false;

			//Check if function exists
			if (!this->FindFunction(wszFuncName))
				return false;
			
			//Register result variable if required
			if (eResultType != CT_VOID) {
				this->m_sCurUserFunctionCall.pResultVar = CVarManager::RegisterCVar(L"uc__resultvar__" + wszFuncName, eResultType, false, false);
			} else {
				this->m_sCurUserFunctionCall.pResultVar = nullptr;
			}
			
			//Initialize code string
			this->m_sCurUserFunctionCall.wszFuncCode = L"call " + wszFuncName + L"(";

			//Store function name
			this->m_sCurUserFunctionCall.wszFuncName = wszFuncName;

			return this->m_sCurUserFunctionCall.bIsValid = true; //Set as ready
		}

		bool BeginFunctionCall(const std::wstring& wszFuncName, const std::wstring& wszResultType)
		{
			//Begin function call

			if (this->m_sCurUserFunctionCall.bIsValid)
				return false;

			if (!wszFuncName.length())
				return false;

			//Check if function exists
			if (!this->FindFunction(wszFuncName))
				return false;

			//Acquire result type
			CVarManager::cvartype_e eResultType = CVarManager::GetTypeFromString(wszResultType);
			if (eResultType == CT_UNKNOWN)
				return false;

			//Register result variable if required
			if (eResultType != CT_VOID) {
				if (eResultType != CT_CUSTOM)
					this->m_sCurUserFunctionCall.pResultVar = CVarManager::RegisterCVar(L"uc__resultvar__" + wszFuncName, eResultType, false, false);
				else
					this->m_sCurUserFunctionCall.pResultVar = CVarManager::RegisterCustomCVar(L"uc__resultvar__" + wszFuncName, wszResultType, false, false);
			}
			else {
				this->m_sCurUserFunctionCall.pResultVar = nullptr;
			}

			//Initialize code string
			this->m_sCurUserFunctionCall.wszFuncCode = L"call " + wszFuncName + L"(";

			//Store function name
			this->m_sCurUserFunctionCall.wszFuncName = wszFuncName;

			return this->m_sCurUserFunctionCall.bIsValid = true; //Set as ready
		}

		bool PushFunctionParam(dnyBoolean value)
		{
			//Push function param: boolean

			if (!this->m_sCurUserFunctionCall.bIsValid)
				return false;

			if (this->m_sCurUserFunctionCall.wszFuncCode[this->m_sCurUserFunctionCall.wszFuncCode.length() - 1] != '(')
				this->m_sCurUserFunctionCall.wszFuncCode += L", ";

			this->m_sCurUserFunctionCall.wszFuncCode += ((value) ? L"true" : L"false");

			return true;
		}

		bool PushFunctionParam(dnyInteger value)
		{
			//Push function param: integer
			
			if (!this->m_sCurUserFunctionCall.bIsValid)
				return false;
			
			if (this->m_sCurUserFunctionCall.wszFuncCode[this->m_sCurUserFunctionCall.wszFuncCode.length() - 1] != '(')
				this->m_sCurUserFunctionCall.wszFuncCode += L", ";

			this->m_sCurUserFunctionCall.wszFuncCode += std::to_wstring(value);

			return true;
		}

		bool PushFunctionParam(dnyFloat value)
		{
			//Push function param: float

			if (!this->m_sCurUserFunctionCall.bIsValid)
				return false;

			if (this->m_sCurUserFunctionCall.wszFuncCode[this->m_sCurUserFunctionCall.wszFuncCode.length() - 1] != '(')
				this->m_sCurUserFunctionCall.wszFuncCode += L", ";

			this->m_sCurUserFunctionCall.wszFuncCode += std::to_wstring(value);

			return true;
		}

		bool PushFunctionParam(const dnyString& value)
		{
			//Push function param: string

			if (!this->m_sCurUserFunctionCall.bIsValid)
				return false;

			if (this->m_sCurUserFunctionCall.wszFuncCode[this->m_sCurUserFunctionCall.wszFuncCode.length() - 1] != '(')
				this->m_sCurUserFunctionCall.wszFuncCode += L", ";

			this->m_sCurUserFunctionCall.wszFuncCode += value;

			return true;
		}

		bool ExecuteFunction(void)
		{
			//Execute function

			if (!this->m_sCurUserFunctionCall.bIsValid)
				return false;

			//End function code string
			this->m_sCurUserFunctionCall.wszFuncCode += L") => " + ((this->m_sCurUserFunctionCall.pResultVar) ? L"uc__resultvar__" + this->m_sCurUserFunctionCall.wszFuncName : L"void") + L";";
			
			//Execute code
			return this->ExecuteCode(this->m_sCurUserFunctionCall.wszFuncCode);
		}

		dnyBoolean QueryFunctionResultAsBoolean(void)
		{
			//Query function result: boolean

			if (!this->m_sCurUserFunctionCall.bIsValid)
				return false;

			CVarManager::ICVar<dnyBoolean>* pVar = (CVarManager::ICVar<dnyBoolean>*)this->m_sCurUserFunctionCall.pResultVar;
			return pVar->GetValue();
		}

		dnyInteger QueryFunctionResultAsInteger(void)
		{
			//Query function result: integer

			if (!this->m_sCurUserFunctionCall.bIsValid)
				return false;

			CVarManager::ICVar<dnyInteger>* pVar = (CVarManager::ICVar<dnyInteger>*)this->m_sCurUserFunctionCall.pResultVar;
			return pVar->GetValue();
		}

		dnyFloat QueryFunctionResultAsFloat(void)
		{
			//Query function result: float

			if (!this->m_sCurUserFunctionCall.bIsValid)
				return false;

			CVarManager::ICVar<dnyFloat>* pVar = (CVarManager::ICVar<dnyFloat>*)this->m_sCurUserFunctionCall.pResultVar;
			return pVar->GetValue();
		}

		dnyString QueryFunctionResultAsString(void)
		{
			//Query function result: string

			if (!this->m_sCurUserFunctionCall.bIsValid)
				return false;

			CVarManager::ICVar<dnyString>* pVar = (CVarManager::ICVar<dnyString>*)this->m_sCurUserFunctionCall.pResultVar;
			return pVar->GetValue();
		}

		bool EndFunctionCall(void)
		{
			//End function call

			if (!this->m_sCurUserFunctionCall.bIsValid)
				return false;

			//Remove result variable if required
			if (this->m_sCurUserFunctionCall.pResultVar != nullptr) {
				CVarManager::FreeCVar(L"uc__resultvar__" + this->m_sCurUserFunctionCall.wszFuncName);
				this->m_sCurUserFunctionCall.pResultVar = nullptr;
			}
			
			//Clear data
			this->m_sCurUserFunctionCall.wszFuncCode.clear();
			this->m_sCurUserFunctionCall.wszFuncName.clear();
			this->m_sCurUserFunctionCall.bIsValid = false;

			return true;
		}

		//Standard output
		inline void SetStandardOutput(const TpfnStandardOutput pfnStandardOutput) { this->m_pfnStandardOutput = pfnStandardOutput; }
		inline const TpfnStandardOutput GetStandardOutput(void) const { return this->m_pfnStandardOutput; }

		//Only for internal use
		CVarManager::cvarptr_t GetCurrentFunctionResultVarPtr(void) { if (this->m_vCurrentFunctionContexts.size()) return this->m_vFunctions[this->m_vCurrentFunctionContexts[this->m_vCurrentFunctionContexts.size() - 1]].pCVar; return nullptr; }
		CVarManager::cvartype_e GetCurrentFunctionResultVarType(void) { if (this->m_vCurrentFunctionContexts.size()) return this->m_vFunctions[this->m_vCurrentFunctionContexts[this->m_vCurrentFunctionContexts.size() - 1]].eResVarType; return CT_UNKNOWN; }
		dnyString GetCurrentFunctionResultVarTypeName(void) { if (this->m_vCurrentFunctionContexts.size()) return this->m_vFunctions[this->m_vCurrentFunctionContexts[this->m_vCurrentFunctionContexts.size() - 1]].wszResVarType; return L""; }
		size_t GetCurrentFunctionContext(void) { if (this->m_vCurrentFunctionContexts.size()) return this->m_vCurrentFunctionContexts[this->m_vCurrentFunctionContexts.size() - 1]; return std::wstring::npos; }
		cvarptr_t FindCurrentFunctionLocalVarPtr(const std::wstring& wszName) { size_t uiId; if (!this->FindLocalVariable(wszName, this->GetCurrentFunctionContext(), &uiId)) return nullptr; return this->m_vFunctions[this->GetCurrentFunctionContext()].vLocalVars[uiId].pCVar; }
		bool RemoveLocalVarFromCurrentFunctionContext(const std::wstring& wszVarName) { return this->FreeLocalVariable(wszVarName, this->GetCurrentFunctionContext()); }
		inline void AbortScriptExecution(void) { this->m_bContinueScriptExecution = false; }
		class dnyScriptParser::CObjectMgr* GetObjectMgr(void) { return this->m_pObjectMgr; }
	};
}
