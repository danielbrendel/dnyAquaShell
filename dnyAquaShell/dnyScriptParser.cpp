#include "dnyScriptParser.h"

/*
	dnyScriptParser developed by Daniel Brendel

	(C) 2017-2018 by Daniel Brendel

	Version: 0.1
	Contact: Daniel Brendel<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel

	Licence: Creative Commons Attribution-NoDerivatives 4.0 International
*/

namespace dnyScriptParser {

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
}