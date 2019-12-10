#include "dnyas_sdk.h"
#include "dnyWinForms.h"

IShellPluginAPI* g_pShellPluginAPI;

class ISpawnFormCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ISpawnFormCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::HFORM hForm = dnyWinForms::SpawnForm(pContext->GetPartString(1), pContext->GetPartString(2), (int)pContext->GetPartInt(3), (int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5), (int)pContext->GetPartInt(6), WS_MINIMIZEBOX);

		IResultCommandInterface<dnyInteger>::SetResult(hForm);

		return hForm != INVALID_FORM_HANDLE;
	}

} g_oSpawnFormCommand;

class ISetFormPosCommandInterface : public IVoidCommandInterface {
public:
	ISetFormPosCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;
		
		return pForm->SetPosition((int)pContext->GetPartInt(2), (int)pContext->GetPartInt(3));
	}

} g_oSetFormPosCommandInterface;

class ISetFormResCommandInterface : public IVoidCommandInterface {
public:
	ISetFormResCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		return pForm->SetResolution((int)pContext->GetPartInt(2), (int)pContext->GetPartInt(3));
	}

} g_oSetFormResCommandInterface;

class ISetCompPosCommandInterface : public IVoidCommandInterface {
public:
	ISetCompPosCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::IBaseComponent* pBaseComp = pForm->FindComponent(pContext->GetPartString(2), pContext->GetPartString(3));
		if (!pBaseComp)
			return false;

		return pBaseComp->SetPosition((int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5));
	}

} g_oSetCompPosCommandInterface;

class ISetCompResCommandInterface : public IVoidCommandInterface {
public:
	ISetCompResCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::IBaseComponent* pBaseComp = pForm->FindComponent(pContext->GetPartString(2), pContext->GetPartString(3));
		if (!pBaseComp)
			return false;

		return pBaseComp->SetResolution((int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5));
	}

} g_oSetCompResCommandInterface;

class ISetCompTextCommandInterface : public IVoidCommandInterface {
public:
	ISetCompTextCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::IBaseComponent* pBaseComp = pForm->FindComponent(pContext->GetPartString(2), pContext->GetPartString(3));
		if (!pBaseComp)
			return false;

		return pBaseComp->SetText(pContext->GetPartString(4));
	}

} g_oSetCompTextCommandInterface;

class IGetCompTextCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IGetCompTextCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::IBaseComponent* pBaseComp = pForm->FindComponent(pContext->GetPartString(2), pContext->GetPartString(3));
		if (!pBaseComp)
			return false;
		
		IResultCommandInterface<dnyString>::SetResult(pBaseComp->GetText());

		return true;
	}

} g_oGetCompTextCommandInterface;

class ISetCompFontCommandInterface : public IVoidCommandInterface {
public:
	ISetCompFontCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::IBaseComponent* pBaseComp = pForm->FindComponent(pContext->GetPartString(2), pContext->GetPartString(3));
		if (!pBaseComp)
			return false;
		
		return pBaseComp->SetFont(pContext->GetPartString(4), (int)pContext->GetPartInt(5), (int)pContext->GetPartInt(6), pContext->GetPartInt(7) != 0, pContext->GetPartInt(8) != 0, pContext->GetPartInt(9) != 0, pContext->GetPartInt(10) != 0);
	}

} g_oSetCompFontCommandInterface;

class ISpawnLabelCommandInterface : public IVoidCommandInterface {
public:
	ISpawnLabelCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return dnyWinForms::SpawnLabel(pContext->GetPartString(1), pContext->GetPartString(2), (int)pContext->GetPartInt(3), (int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5), (int)pContext->GetPartInt(6), pContext->GetPartString(7)) != nullptr;
	}

} g_oSpawnLabelCommandInterface;

void Btn_CommonOnClick(dnyWinForms::IBaseComponent* pThis, void* pDataPtr)
{
	dnyWinForms::CButton* pButton = (dnyWinForms::CButton*)pThis;
	
	g_pShellPluginAPI->Scr_ExecuteCode(L"call " + pButton->GetName() + L"_OnClick() => void;");
}
class ISpawnButtonCommandInterface : public IVoidCommandInterface {
public:
	ISpawnButtonCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return dnyWinForms::SpawnButton(pContext->GetPartString(1), pContext->GetPartString(2), (int)pContext->GetPartInt(3), (int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5), (int)pContext->GetPartInt(6), pContext->GetPartString(7), &Btn_CommonOnClick) != nullptr;
	}

} g_oSpawnButtonCommandInterface;

void Txt_CommonOnChange(dnyWinForms::IBaseComponent* pThis, void* pDataPtr)
{
	dnyWinForms::CTextbox* pTextbox = (dnyWinForms::CTextbox*)pThis;

	g_pShellPluginAPI->Scr_ExecuteCode(L"call " + pTextbox->GetName() + L"_OnChange(\"" + pTextbox->GetText() + L"\") => void;");
}
class ISpawnTextboxCommandInterface : public IVoidCommandInterface {
public:
	ISpawnTextboxCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CTextbox* pTextbox = dnyWinForms::SpawnTextbox(pContext->GetPartString(1), pContext->GetPartString(2), (int)pContext->GetPartInt(3), (int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5), (int)pContext->GetPartInt(6), pContext->GetPartString(7));
		if (!pTextbox)
			return false;

		return pTextbox->AddOnChangeEventHandler(&Txt_CommonOnChange);
	}

} g_oSpawnTextboxCommandInterface;

class IGetTextboxTextCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IGetTextboxTextCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CTextbox* pTextbox = (dnyWinForms::CTextbox*)pForm->FindComponent(L"CTextbox", pContext->GetPartString(2));
		if (!pTextbox)
			return false;

		IResultCommandInterface<dnyString>::SetResult(pTextbox->GetText());

		return true;
	}

} g_oGetTextboxTextCommandInterface;

class ISetTextboxTextCommandInterface : public IVoidCommandInterface {
public:
	ISetTextboxTextCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CTextbox* pTextbox = (dnyWinForms::CTextbox*)pForm->FindComponent(L"CTextbox", pContext->GetPartString(2));
		if (!pTextbox)
			return false;

		return pTextbox->SetText(pContext->GetPartString(3));
	}

} g_oSetTextboxTextCommandInterface;

class ISpawnCheckboxCommandInterface : public IVoidCommandInterface {
public:
	ISpawnCheckboxCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return dnyWinForms::SpawnCheckbox(pContext->GetPartString(1), pContext->GetPartString(2), (int)pContext->GetPartInt(3), (int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5), (int)pContext->GetPartInt(6), pContext->GetPartString(7), pContext->GetPartBool(8)) != nullptr;
	}

} g_oSpawnCheckboxCommandInterface;

class ICbGetValueCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	ICbGetValueCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CCheckbox* pCheckbox = (dnyWinForms::CCheckbox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pCheckbox)
			return false;

		IResultCommandInterface<dnyBoolean>::SetResult(pCheckbox->IsChecked());

		return true;
	}

} g_oCbGetValueCommandInterface;

class ICbSetValueCommandInterface : public IVoidCommandInterface {
public:
	ICbSetValueCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CCheckbox* pCheckbox = (dnyWinForms::CCheckbox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pCheckbox)
			return false;

		pCheckbox->SetCheckValue(pContext->GetPartBool(3));

		return true;
	}

} g_oCbSetValueCommandInterface;

void Cb_CommonOnSelect(dnyWinForms::IBaseComponent* pThis, void* pDataPtr)
{
	dnyWinForms::CCombobox* pCombobox = (dnyWinForms::CCombobox*)pThis;

	g_pShellPluginAPI->Scr_ExecuteCode(L"call " + pCombobox->GetName() + L"_OnSelect(\"" + pCombobox->Selection() + L"\") => void;");
}
class ISpawnComboboxCommandInterface : public IVoidCommandInterface {
public:
	ISpawnComboboxCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CCombobox* pCombobox = dnyWinForms::SpawnCombobox(pContext->GetPartString(1), pContext->GetPartString(2), (int)pContext->GetPartInt(3), (int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5), (int)pContext->GetPartInt(6), &Cb_CommonOnSelect);

		return pCombobox != nullptr;
	}

} g_oSpawnComboboxCommandInterface;

class ICbAddCommandInterface : public IVoidCommandInterface {
public:
	ICbAddCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CCombobox* pCombobox = (dnyWinForms::CCombobox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pCombobox)
			return false;

		return pCombobox->Add(pContext->GetPartString(3));
	}

} g_oCbAddCommandInterface;

class ICbRemoveCommandInterface : public IVoidCommandInterface {
public:
	ICbRemoveCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CCombobox* pCombobox = (dnyWinForms::CCombobox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pCombobox)
			return false;

		return pCombobox->Remove(pContext->GetPartInt(3));
	}

} g_oCbRemoveCommandInterface;

class ICbCountCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ICbCountCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CCombobox* pCombobox = (dnyWinForms::CCombobox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pCombobox)
			return false;

		IResultCommandInterface<dnyInteger>::SetResult((dnyInteger)pCombobox->Count());

		return true;
	}

} g_CbCountCommandInterface;

class ICbSelectCommandInterface : public IVoidCommandInterface {
public:
	ICbSelectCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CCombobox* pCombobox = (dnyWinForms::CCombobox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pCombobox)
			return false;

		return pCombobox->SelectItem(pContext->GetPartInt(3));
	}

} g_oCbSelectCommandInterface;

class ICbSelectionCommandInterface : public IResultCommandInterface<dnyString> {
public:
	ICbSelectionCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CCombobox* pCombobox = (dnyWinForms::CCombobox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pCombobox)
			return false;

		IResultCommandInterface<dnyString>::SetResult(pCombobox->Selection());

		return true;
	}

} g_oCbSelectionCommandInterface;

class ICbSelectionIdCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ICbSelectionIdCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CCombobox* pCombobox = (dnyWinForms::CCombobox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pCombobox)
			return false;

		IResultCommandInterface<dnyInteger>::SetResult(pCombobox->SelectionId());

		return true;
	}

} g_oCbSelectionIdCommandInterface;

class ICbTextCommandInterface : public IResultCommandInterface<dnyString> {
public:
	ICbTextCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CCombobox* pCombobox = (dnyWinForms::CCombobox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pCombobox)
			return false;

		IResultCommandInterface<dnyString>::SetResult(pCombobox->GetText((dnyWinForms::COMBOBOXITEM)pContext->GetPartInt(3)));

		return true;
	}

} g_oCbTextCommandInterface;

void Lb_CommonOnSelectionChange(dnyWinForms::IBaseComponent* pThis, void* pDataPtr)
{
	dnyWinForms::CListbox* pListbox = (dnyWinForms::CListbox*)pThis;

	g_pShellPluginAPI->Scr_ExecuteCode(L"call " + pListbox->GetName() + L"_OnSelectionChange(" + std::to_wstring(pListbox->Selection()) + L") => void;");
}
void Lb_CommonOnDblClick(dnyWinForms::IBaseComponent* pThis, void* pDataPtr)
{
	dnyWinForms::CListbox* pListbox = (dnyWinForms::CListbox*)pThis;

	g_pShellPluginAPI->Scr_ExecuteCode(L"call " + pListbox->GetName() + L"_OnDoubleClick(" + std::to_wstring(pListbox->Selection()) + L") => void;");
}
class ISpawnListboxCommandInterface : public IVoidCommandInterface {
public:
	ISpawnListboxCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CListbox* pListbox = dnyWinForms::SpawnListbox(pContext->GetPartString(1), pContext->GetPartString(2), (int)pContext->GetPartInt(3), (int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5), (int)pContext->GetPartInt(6));
		if (!pListbox)
			return false;

		pListbox->AddOnSelectionChangeEventHandler(&Lb_CommonOnSelectionChange);
		pListbox->AddOnDblClickEventHandler(&Lb_CommonOnDblClick);

		return true;
	}

} g_oSpawnListboxCommandInterface;

class ILbAddItemCommandInterface : public IVoidCommandInterface {
public:
	ILbAddItemCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListbox* pListbox = (dnyWinForms::CListbox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListbox)
			return false;

		return pListbox->Add(pContext->GetPartString(3));
	}

} g_oLbAddItemCommandInterface;

class ILbInsertItemCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ILbInsertItemCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);
		
		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;
		
		dnyWinForms::CListbox* pListbox = (dnyWinForms::CListbox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListbox)
			return false;
		
		IResultCommandInterface<dnyInteger>::SetResult(pListbox->Insert((dnyWinForms::LLISTBOXITEM)pContext->GetPartInt(3), pContext->GetPartString(4)));

		return true;
	}

} g_oLbInsertItemCommandInterface;

class ILbUpdateItemCommandInterface : public IVoidCommandInterface {
public:
	ILbUpdateItemCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);
		
		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;
		
		dnyWinForms::CListbox* pListbox = (dnyWinForms::CListbox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListbox)
			return false;
		
		return pListbox->Update((dnyWinForms::LLISTBOXITEM)pContext->GetPartInt(3), pContext->GetPartString(4));
	}

} g_oLbUpdateItemCommandInterface;

class ILbRemoveItemCommandInterface : public IVoidCommandInterface {
public:
	ILbRemoveItemCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListbox* pListbox = (dnyWinForms::CListbox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListbox)
			return false;

		return pListbox->Remove((dnyWinForms::LLISTBOXITEM)pContext->GetPartInt(3));
	}

} g_oLbRemoveItemCommandInterface;

class ILbSelectItemCommandInterface : public IVoidCommandInterface {
public:
	ILbSelectItemCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListbox* pListbox = (dnyWinForms::CListbox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListbox)
			return false;

		return pListbox->Select((dnyWinForms::LLISTBOXITEM)pContext->GetPartInt(3));
	}

} g_oLbSelectItemCommandInterface;

class ILbCountCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ILbCountCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListbox* pListbox = (dnyWinForms::CListbox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListbox)
			return false;

		IResultCommandInterface<dnyInteger>::SetResult(pListbox->Count());

		return true;
	}

} g_oLbCountCommandInterface;

class ILbSelectionCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ILbSelectionCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListbox* pListbox = (dnyWinForms::CListbox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListbox)
			return false;

		IResultCommandInterface<dnyInteger>::SetResult(pListbox->Selection());

		return true;
	}

} g_oLbSelectionCommandInterface;

class ILbGetTextCommandInterface : public IResultCommandInterface<dnyString> {
public:
	ILbGetTextCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListbox* pListbox = (dnyWinForms::CListbox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListbox)
			return false;

		IResultCommandInterface<dnyString>::SetResult(pListbox->GetText((dnyWinForms::LLISTBOXITEM)pContext->GetPartInt(3)));

		return true;
	}

} g_oLbGetTextCommandInterface;

void Lv_CommonOnSelect(dnyWinForms::IBaseComponent* pThis, void* pDataPtr)
{
	dnyWinForms::CListview* pListview = (dnyWinForms::CListview*)pThis;

	g_pShellPluginAPI->Scr_ExecuteCode(L"call " + pListview->GetName() + L"_OnSelect(" + std::to_wstring(pListview->Selection()) + L") => void;");
}
void Lv_CommonOnDblClick(dnyWinForms::IBaseComponent* pThis, void* pDataPtr)
{
	dnyWinForms::CListview* pListview = (dnyWinForms::CListview*)pThis;

	g_pShellPluginAPI->Scr_ExecuteCode(L"call " + pListview->GetName() + L"_OnDoubleClick(" + std::to_wstring(pListview->Selection()) + L") => void;");
}
class ISpawnListviewCommandInterface : public IVoidCommandInterface {
public:
	ISpawnListviewCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CListview* pListview = dnyWinForms::SpawnListview(pContext->GetPartString(1), pContext->GetPartString(2), (int)pContext->GetPartInt(3), (int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5), (int)pContext->GetPartInt(6), &Lv_CommonOnSelect, &Lv_CommonOnDblClick);
		
		return pListview != nullptr;
	}

} g_oSpawnListviewCommandInterface;

class ILvAddCategoryCommandInterface : public IVoidCommandInterface {
public:
	ILvAddCategoryCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListview* pListview = (dnyWinForms::CListview*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListview)
			return false;

		return pListview->AddCategory(pContext->GetPartString(3), (int)pContext->GetPartInt(4), pContext->GetPartBool(5));
	}

} g_oLvAddCategoryCommandInterface;

class ILvSubItemCountCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ILvSubItemCountCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListview* pListview = (dnyWinForms::CListview*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListview)
			return false;

		IResultCommandInterface<dnyInteger>::SetResult(pListview->GetSubItemCount());

		return true;
	}

} g_oLvSubItemCountCommandInterface;

class ILvItemCountCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ILvItemCountCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListview* pListview = (dnyWinForms::CListview*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListview)
			return false;

		IResultCommandInterface<dnyInteger>::SetResult(pListview->GetItemCount());

		return true;
	}

} g_oLvItemCountCommandInterface;

class ILvSetItemTextCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ILvSetItemTextCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListview* pListview = (dnyWinForms::CListview*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListview)
			return false;

		IResultCommandInterface<dnyInteger>::SetResult(pListview->SetItemText(pContext->GetPartString(3), (int)pContext->GetPartInt(4)));

		return true;
	}

} g_oLvSetItemTextCommandInterface;

class ILvSetSubItemTextCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ILvSetSubItemTextCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListview* pListview = (dnyWinForms::CListview*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListview)
			return false;

		IResultCommandInterface<dnyInteger>::SetResult(pListview->SetSubItemText((int)pContext->GetPartInt(3), pContext->GetPartString(4), (int)pContext->GetPartInt(5)));

		return true;
	}

} g_oLvSetSubItemTextCommandInterface;

class ILvGetItemTextCommandInterface : public IResultCommandInterface<dnyString> {
public:
	ILvGetItemTextCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListview* pListview = (dnyWinForms::CListview*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListview)
			return false;

		int iBufferCount = (int)pContext->GetPartInt(5);
		if (iBufferCount <= 0) iBufferCount = LV_MAXITEMBUFFERSIZE;

		IResultCommandInterface<dnyString>::SetResult(pListview->GetItemText((int)pContext->GetPartInt(3), (int)pContext->GetPartInt(4), iBufferCount));
		
		return true;
	}

} g_oLvGetItemTextCommandInterface;

class ILvGetSelectionCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	ILvGetSelectionCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListview* pListview = (dnyWinForms::CListview*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListview)
			return false;

		IResultCommandInterface<dnyInteger>::SetResult(pListview->Selection());

		return true;
	}

} g_oLvGetSelectionCommandInterface;

class ILvDeleteItemCommandInterface : public IVoidCommandInterface {
public:
	ILvDeleteItemCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CListview* pListview = (dnyWinForms::CListview*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pListview)
			return false;

		return pListview->Delete((int)pContext->GetPartInt(3));
	}

} g_oLvDeleteItemCommandInterface;

class ISpawnProgressbarCommandInterface : public IVoidCommandInterface {
public:
	ISpawnProgressbarCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return dnyWinForms::SpawnProgressbar(pContext->GetPartString(1), pContext->GetPartString(2), (int)pContext->GetPartInt(3), (int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5), (int)pContext->GetPartInt(6), (int)pContext->GetPartInt(7), (int)pContext->GetPartInt(8)) != nullptr;
	}

} g_oSpawnProgressbarCommandInterface;

class ISetPbMaxValueCommandInterface : public IVoidCommandInterface {
public:
	ISetPbMaxValueCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CProgressbar* pProgressbar = (dnyWinForms::CProgressbar*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pProgressbar)
			return false;

		return pProgressbar->SetRange((int)pContext->GetPartInt(3));
	}

} g_oSetPbMaxValueCommandInterface;

class ISetPbPosValueCommandInterface : public IVoidCommandInterface {
public:
	ISetPbPosValueCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CProgressbar* pProgressbar = (dnyWinForms::CProgressbar*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pProgressbar)
			return false;

		return pProgressbar->SetPos((int)pContext->GetPartInt(3));
	}

} g_oSetPbPosValueCommandInterface;

class IGetPbMaxValueCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	IGetPbMaxValueCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CProgressbar* pProgressbar = (dnyWinForms::CProgressbar*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pProgressbar)
			return false;

		IResultCommandInterface<dnyInteger>::SetResult(pProgressbar->GetRange());

		return true;
	}

} g_oGetPbMaxValueCommandInterface;

class IGetPbPosValueCommandInterface : public IResultCommandInterface<dnyInteger> {
public:
	IGetPbPosValueCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CProgressbar* pProgressbar = (dnyWinForms::CProgressbar*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pProgressbar)
			return false;

		IResultCommandInterface<dnyInteger>::SetResult(pProgressbar->GetPos());

		return true;
	}

} g_oGetPbPosValueCommandInterface;

class ISpawnImageboxCommandInterface : public IVoidCommandInterface {
public:
	ISpawnImageboxCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return dnyWinForms::SpawnImagebox(pContext->GetPartString(1), pContext->GetPartString(2), (int)pContext->GetPartInt(3), (int)pContext->GetPartInt(4), (int)pContext->GetPartInt(5), (int)pContext->GetPartInt(6), pContext->GetPartString(7)) != nullptr;
	}

} g_oSpawnImageboxCommandInterface;

class ISetImageCommandInterface : public IVoidCommandInterface {
public:
	ISetImageCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CImagebox* pImagebox = (dnyWinForms::CImagebox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pImagebox)
			return false;

		return pImagebox->SetImage(pContext->GetPartString(3));
	}

} g_oSetImageCommandInterface;

class IGetImageCommandInterface : public IResultCommandInterface<dnyString> {
public:
	IGetImageCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::CForm* pForm = dnyWinForms::FindForm(pContext->GetPartString(1));
		if (!pForm)
			return false;

		dnyWinForms::CImagebox* pImagebox = (dnyWinForms::CImagebox*)pForm->FindComponent(pContext->GetPartString(2));
		if (!pImagebox)
			return false;

		IResultCommandInterface<dnyString>::SetResult(pImagebox->GetImage());

		return true;
	}

} g_oGetImageCommandInterface;

class IFormValidCommandInterface : public IResultCommandInterface<dnyBoolean> {
public:
	IFormValidCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		IResultCommandInterface<dnyBoolean>::SetResult(dnyWinForms::IsFormValid(pContext->GetPartString(1)));

		return true;
	}

} g_oFormValidCommand;

class IProcessFormsCommandInterface : public IVoidCommandInterface {
public:
	IProcessFormsCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		dnyWinForms::Process();

		return true;
	}

} g_oProcessFormsCommand;

class IFreeFormCommandInterface : public IVoidCommandInterface {
public:
	IFreeFormCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		pContext->ReplaceAllVariables(pInterfaceObject);

		return dnyWinForms::ReleaseForm(pContext->GetPartInt(1));
	}

} g_oFreeFormCommand;

//Plugin infos
plugininfo_s g_sPluginInfos = {
	L"Forms",
	L"0.1",
	L"Daniel Brendel",
	L"dbrendel1988<at>yahoo<dot>com",
	L"Windows forms and controls provider"
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

	//Register example commands
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_spawnform", &g_oSpawnFormCommand, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_setformpos", &g_oSetFormPosCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_setformres", &g_oSetFormResCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_setcomppos", &g_oSetCompPosCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_setcompres", &g_oSetCompResCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_setcomptext", &g_oSetCompTextCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_setcompfont", &g_oSetCompFontCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_getcomptext", &g_oGetCompTextCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_spawnlabel", &g_oSpawnLabelCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_spawnbutton", &g_oSpawnButtonCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_spawntextbox", &g_oSpawnTextboxCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_gettextboxtext", &g_oGetTextboxTextCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_settextboxtext", &g_oSetTextboxTextCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_spawncheckbox", &g_oSpawnCheckboxCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_cbischecked", &g_oCbGetValueCommandInterface, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_cbsetvalue", &g_oCbSetValueCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_spawnlistbox", &g_oSpawnListboxCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lbadditem", &g_oLbAddItemCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lbinsertitem", &g_oLbInsertItemCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lbremoveitem", &g_oLbRemoveItemCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lbupdateitem", &g_oLbUpdateItemCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lbselectitem", &g_oLbSelectItemCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lbgetcount", &g_oLbCountCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lbgetselection", &g_oLbSelectionCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lbgettext", &g_oLbGetTextCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_spawncombobox", &g_oSpawnComboboxCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_cbadditem", &g_oCbAddCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_cbremoveitem", &g_oCbRemoveCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_cbgetcount", &g_CbCountCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_cbselectitem", &g_oCbSelectCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_cbgetselection", &g_oCbSelectionCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_cbgetselectionid", &g_oCbSelectionIdCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_cbgetitemtext", &g_oCbTextCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_spawnlistview", &g_oSpawnListviewCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lvaddcategory", &g_oLvAddCategoryCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lvgetsubitemcount", &g_oLvSubItemCountCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lvgetitemcount", &g_oLvItemCountCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lvsetitemtext", &g_oLvSetItemTextCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lvsetsubitemtext", &g_oLvSetSubItemTextCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lvgetitemtext", &g_oLvGetItemTextCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lvgetselection", &g_oLvGetSelectionCommandInterface, CT_INT);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_lvdeleteitem", &g_oLvDeleteItemCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_spawnprogressbar", &g_oSpawnProgressbarCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_setpbrange", &g_oSetPbMaxValueCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_setpbposition", &g_oSetPbPosValueCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_getpbrange", &g_oGetPbMaxValueCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_getpbposition", &g_oGetPbPosValueCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_spawnimagebox", &g_oSpawnImageboxCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_setimageboximage", &g_oSetImageCommandInterface, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_getimageboximage", &g_oGetImageCommandInterface, CT_STRING);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_isformvalid", &g_oFormValidCommand, CT_BOOL);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_process", &g_oProcessFormsCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"wnd_freeform", &g_oFreeFormCommand, CT_VOID);
	
	return true;
}

void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_spawnform");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_setformpos");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_setformres");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_setcomppos");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_setcompres");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_setcomptext");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_setcompfont");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_getcomptext");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_spawnlabel");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_spawnbutton");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_spawntextbox");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_gettextboxtext");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_settextboxtext");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_spawncheckbox");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_cbischecked");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_cbsetvalue");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_spawnlistbox");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lbadditem");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lbinsertitem");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lbremoveitem");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lbupdateitem");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lbselectitem");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lbgetcount");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lbgetselection");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lbgettext");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_spawncombobox");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_cbadditem");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_cbremoveitem");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_cbgetcount");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_cbselectitem");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_cbgetselection");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_cbgetselectionid");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_cbgetitemtext");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_spawnlistview");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lvaddcategory");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lvgetsubitemcount");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lvgetitemcount");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lvsetitemtext");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lvsetsubitemtext");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lvgetitemtext");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lvgetselection");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_lvdeleteitem");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_spawnprogressbar");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_setpbrange");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_setpbposition");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_getpbrange");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_getpbposition");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_spawnimagebox");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_setimageboximage");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_getimageboximage");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_isformvalid");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_process");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"wnd_freeform");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}