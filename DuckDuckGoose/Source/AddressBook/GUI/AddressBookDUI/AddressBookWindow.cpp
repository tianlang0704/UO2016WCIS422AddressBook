#include "AddressBookWindow.h"
#include "OneInputWindow.h"
#include "OneDropdownWindow.h"
#include "EditWindow.h"
#include "CustomColumnsWindow.h"
#include "HelpWindow.h"
#include "../../Model/AddressBookModel.h"
#include <windows.h>
#include <shobjidl.h> 

#define CON_LIST_ID "ContactListMember"
#define ADDRESSBOOK_UNTITLED_NAME "Untitled"

AddressBookWindow::AddressBookWindow(CDuiString SkinFolder,
	CDuiString SkinFile,
	UILIB_RESOURCETYPE ResType,
	LPCTSTR ResID)
{
	m_SkinFolder = SkinFolder;
	m_SkinFile = SkinFile;
	m_ResType = ResType;
	m_ResID = ResID;
	m_currentTabIndex = -1;
}

AddressBookWindow::~AddressBookWindow()
{
	if (!m_TabContexts.empty())
	{
		for (auto &i : m_TabContexts)
		{
			if (i.pAddressBook)
			{
				delete i.pAddressBook;
				i.pAddressBook = NULL;
			}
		}
	}
}

LRESULT AddressBookWindow::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	PostQuitMessage(0);
	bHandled = TRUE;
	return 1;
};

void AddressBookWindow::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		Prepare();
	else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED)
	{
		if (msg.pSender->GetName() == "edit_search")
			Search(msg.pSender->GetText().GetData());
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMACTIVATE)
	{
		if (msg.pSender->GetUserData() == CON_LIST_ID)
			OnContactListActivate(msg);
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMCLICK)
	{
		if (msg.pSender->GetName() == "menu_btn_open")
			Open();
		else if (msg.pSender->GetName() == "menu_btn_new")
			AddTab();
		else if (msg.pSender->GetName() == "menu_btn_add_contact")
			AddContact();
		else if (msg.pSender->GetName() == "menu_btn_close")
			RemoveTab(m_currentTabIndex);
		else if (msg.pSender->GetName() == "menu_btn_save")
			Save();
		else if (msg.pSender->GetName() == "menu_btn_save_as")
			SaveAs();
		else if (msg.pSender->GetName() == "menu_btn_exit")
			Exit();
		else if (msg.pSender->GetName() == "menu_btn_export")
			Export();
		else if (msg.pSender->GetName() == "menu_btn_import")
			Import();
		else if (msg.pSender->GetName() == "menu_btn_delete")
			DeleteAddressbook();
		else if (msg.pSender->GetName() == "menu_btn_sort_zip")
			Sort(ZIPCODE_HEADER_NAME);
		else if (msg.pSender->GetName() == "menu_btn_sort_lastname")
			Sort(LASTNAME_HEADER_NAME);
		else if (msg.pSender->GetName() == "menu_list_btn_delete")
			DeleteContact(((CListUI *)msg.wParam)->GetCurSel());
		else if (msg.pSender->GetName() == "menu_list_btn_edit")
			Edit(((CListUI *)msg.wParam)->GetCurSel());
		else if (msg.pSender->GetName() == "menu_tab_delete")
			RemoveTab(m_TabBar->GetItemIndex((CControlUI *)msg.wParam));
		else if (msg.pSender->GetUserData() == CON_LIST_ID)
			OnContactListSelect(msg);
	}
	else if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == "w_close")
			Exit();
		if (msg.pSender->GetName() == "w_min")
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		else if (msg.pSender->GetName() == "btn_file")
			ShowMenu(CDuiPoint(msg.pSender->GetPos().left, msg.pSender->GetPos().bottom), "menu_file.xml", msg.pSender);
		else if (msg.pSender->GetName() == "btn_edit")
			ShowMenu(CDuiPoint(msg.pSender->GetPos().left, msg.pSender->GetPos().bottom), "menu_edit.xml", msg.pSender);
		else if (msg.pSender->GetName() == "btn_view")
			ShowMenu(CDuiPoint(msg.pSender->GetPos().left, msg.pSender->GetPos().bottom), "menu_view.xml", msg.pSender);
		else if (msg.pSender->GetName() == "btn_help")
			Help();
		else if (msg.pSender->GetName() == "tab_new")
			AddTab();
		else if (msg.pSender->GetName() == "tool_btn_add")
			AddContact();
		else if (msg.pSender->GetName() == "tool_btn_save")
			Save();
		else if (msg.pSender->GetName() == "tool_btn_close")
			RemoveTab(m_currentTabIndex);
		else if (msg.pSender->GetName() == "tool_btn_export")
			Export();
		else if (msg.pSender->GetName() == "tool_btn_import")
			Import();
	}
	else if (msg.sType == _T("selectchanged"))
	{
		OnTabSelect(msg);
	}
	else if (msg.sType == DUI_MSGTYPE_MENU)
	{
		if (msg.pSender->GetName() == "contact_list")
			ShowMenu(msg.ptMouse, "menu_list.xml", msg.pSender);
		else if (msg.pSender->GetName() == "addressbook_tab")
			ShowMenu(msg.ptMouse, "menu_tab.xml", msg.pSender);
	}
};

void AddressBookWindow::ShowMenu(CDuiPoint p, STRINGorID fileName, CControlUI *pSender)
{
	CMenuWnd* pMenu = new CMenuWnd(NULL, &m_PaintManager, pSender);
	CDuiPoint point = p;
	ClientToScreen(m_hWnd, &point);
	pMenu->Init(NULL, fileName, NULL, point);
};

int AddressBookWindow::ShowSaveFileDialog(string *out, DWORD flagAdd, DWORD flagRemove)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (!SUCCEEDED(hr))
		return 1;

	IFileSaveDialog *pFileSave;
	

	// Create the FileOpenDialog object.
	hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
		IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
	if (!SUCCEEDED(hr))
		return 2;

	// Setting options
	DWORD dwFlags;
	pFileSave->GetOptions(&dwFlags);
	pFileSave->SetOptions(dwFlags | flagAdd & ~flagRemove);

	COMDLG_FILTERSPEC rgSpec[] =
	{
		{ L"Addres book files", L"*.abk" },
		{ L"Text", L"*.txt" }
	};
	hr = pFileSave->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
			// Set the default extension to be ".doc" file.
	hr = pFileSave->SetDefaultExtension(L"abk");

	// Show the Open dialog box.
	hr = pFileSave->Show(NULL);
	if (hr != S_OK)
		return 3;

	// Get the file name from the dialog box.
	IShellItem *pItem;
	hr = pFileSave->GetResult(&pItem);
	if (!SUCCEEDED(hr))
		return 4;
	PWSTR pszFilePath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	if (!SUCCEEDED(hr))
		return 5;

	size_t retNum;
	int len = wcslen(pszFilePath);
	char *outBuff = (char *)malloc(len + 1);
	wcstombs_s(&retNum, outBuff, (size_t)((len + 1) * sizeof(char)), pszFilePath, (size_t)((len + 1) * sizeof(WCHAR)));

	(*out) = outBuff;

	free(outBuff);
	CoTaskMemFree(pszFilePath);

	pItem->Release();
	pFileSave->Release();
	CoUninitialize();

	return 0;
}

int AddressBookWindow::ShowOpenFileDialog(string *out, DWORD flagAdd, DWORD flagRemove)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (!SUCCEEDED(hr))
		return 1;

	IFileOpenDialog *pFileOpen;

	// Create the FileOpenDialog object.
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
		IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
	if (!SUCCEEDED(hr))
		return 2;
	
	// Setting options
	DWORD dwFlags;
	pFileOpen->GetOptions(&dwFlags);
	pFileOpen->SetOptions(dwFlags | flagAdd & ~flagRemove);

	COMDLG_FILTERSPEC rgSpec[] =
	{
		{ L"Addres book files", L"*.abk" },
		{ L"Text", L"*.txt" }
	};
	hr = pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);

	// Show the Open dialog box.
	hr = pFileOpen->Show(NULL);
	if (hr != S_OK)
		return 3;
	
	// Get the file name from the dialog box.
	IShellItem *pItem;
	hr = pFileOpen->GetResult(&pItem);
	if (!SUCCEEDED(hr))
		return 4;
	PWSTR pszFilePath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	if (!SUCCEEDED(hr))
		return 5;

	size_t retNum;
	int len = wcslen(pszFilePath);
	char *outBuff = (char *)malloc(len + 1);
	wcstombs_s(&retNum, outBuff, (size_t)((len + 1) * sizeof(char)), pszFilePath, (size_t)((len + 1) * sizeof(WCHAR)));

	(*out) = outBuff;
	
	free(outBuff);
	CoTaskMemFree(pszFilePath);

	pItem->Release();
	pFileOpen->Release();
	CoUninitialize();

	return 0;
}

void AddressBookWindow::OnTabSelect(TNotifyUI &msg)
{
	m_currentTabIndex = m_TabBar->GetItemIndex(msg.pSender);
	CTabLayoutUI *tabContentContainer = static_cast<CTabLayoutUI *>(m_PaintManager.FindControl("tab_content_container"));
	tabContentContainer->SelectItem(m_currentTabIndex);
}

void AddressBookWindow::OnContactListSelect(TNotifyUI &msg)
{
	CHorizontalLayoutUI *pTabContent = m_TabContexts[m_currentTabIndex].pTabContent;
	CListUI *contactList = static_cast<CListUI *>(pTabContent->FindSubControl("contact_list"));
	ShowDetail(contactList->GetItemIndex(msg.pSender));
}

void AddressBookWindow::OnContactListActivate(TNotifyUI &msg)
{
	CHorizontalLayoutUI *pTabContent = m_TabContexts[m_currentTabIndex].pTabContent;
	CListUI *contactList = static_cast<CListUI *>(pTabContent->FindSubControl("contact_list"));
	Edit(contactList->GetItemIndex(msg.pSender));
}

void AddressBookWindow::Edit(int index)
{
	map<string, string> res;
	vector<string> headers;
	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	addressBook->GetContact(index, &res);
	headers = addressBook->GetHeaders();

	EditWindow* pEditName = new EditWindow(&res, headers);
	pEditName->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
	pEditName->CenterWindow();
	if (pEditName->ShowModal())
		return;

	addressBook->UpdateContact(index, res);

	UpdateContactList();
	ShowDetail(index);
}

void AddressBookWindow::Open()
{
	vector<string> names;
	AddressBookModel::GetAllAddressBookNames(&names);

	string res = "";
	OneDropdownWindow* pOpen = new OneDropdownWindow(&res, "Address book name: ", "Input a name to open an address book", names);
	pOpen->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
	pOpen->CenterWindow();
	if (pOpen->ShowModal())
		return;

	//Check if tab name is empty
	if (res == "")
		return;

	//Check if tab name is already exist
	int existingTabIndex;
	if (IsTabDup(res, &existingTabIndex))
	{
		COptionUI *tab = static_cast<COptionUI *>(m_TabBar->GetItemAt(existingTabIndex));
		tab->Selected(true);
		return;
	}

	AddressBookModel *addressBook = new AddressBookModel();
	if (addressBook->Open(res))
	{
		delete addressBook;
		MessageBox(NULL, "Address book name does not exist", "Notice", MB_OK);
		return;
	}

	AddTab(addressBook);
}

void AddressBookWindow::SaveAs()
{
	if ((int)m_TabContexts.size() <= m_currentTabIndex || m_currentTabIndex < 0)
		return;
	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	string name = addressBook->GetName();

	OneInputWindow* pSave = new OneInputWindow(&name, "Address book name:\n  1.Must start with a letter\n  2.Must not contain space ", "Input a name to save an address book to");
	pSave->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
	pSave->CenterWindow();
	if (pSave->ShowModal())
		return;

	addressBook->SetName(name);

	if (addressBook->IsDup())
	{
		string msg = "Address book name \"" + name + "\" already exists, do you want to overwrite?";
		if (MessageBox(NULL, msg.c_str(), "Warning!", MB_OKCANCEL) != 1)
			return;
	}

	if (addressBook->Save())
	{
		MessageBox(NULL, "Failed saving address book", "Warning!", MB_OK);
		return;
	}

	COptionUI *tab = static_cast<COptionUI *>(m_TabBar->GetItemAt(m_currentTabIndex));
	tab->SetText(name.c_str());
}

void AddressBookWindow::Save()
{
	if ((int)m_TabContexts.size() <= m_currentTabIndex || m_currentTabIndex < 0)
		return;
	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	//string name = addressBook->GetName();
	
	//OneInputWindow* pSave = new OneInputWindow(&name, "Address book name:\n  1.Must start with a letter\n  2.Must not contain space ", "Input a name to save an address book to");
	//pSave->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
	//pSave->CenterWindow();
	//if (pSave->ShowModal())
	//	return;

	//addressBook->SetName(name);

	//if (addressBook->IsDup())
	//{
	//	string msg = "Address book name \"" + name + "\" already exists, do you want to overwrite?";
	//	if (MessageBox(NULL, msg.c_str(), "Warning!", MB_OKCANCEL) != 1)
	//		return;
	//}

	if (addressBook->Save())
	{
		MessageBox(NULL, "Failed saving address book", "Warning!", MB_OK);
		return;
	}

	//COptionUI *tab = static_cast<COptionUI *>(m_TabBar->GetItemAt(m_currentTabIndex));
	//tab->SetText(name.c_str());
}

void AddressBookWindow::Exit()
{
	int tabNum = m_TabBar->GetCount();
	for (int i = 0; i < tabNum; i++)
	{
		CControlUI *control = m_TabBar->GetItemAt(i);
		string className = control->GetClass();
		if (className == "OptionUI")
			if (IsUnsave(i))
				return;
	}

	this->Close();
}

void AddressBookWindow::Export()
{
	if (m_currentTabIndex < 0)
		return;

	string fileName;
	int res = ShowSaveFileDialog(&fileName);

	if (res == 3)
		return;

	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	if (addressBook->Export(fileName))
	{
		MessageBox(NULL, "Export failed", "Warning", MB_OK);
	}
}

void AddressBookWindow::Import()
{
	string fileName;
	int res = ShowOpenFileDialog(&fileName);

	if (res == 3)
		return;

	AddressBookModel *addressBook = new AddressBookModel();
	if (addressBook->Import(fileName))
	{
		MessageBox(NULL, "Import failed", "Warning", MB_OK);
	}

	addressBook->SetName(ADDRESSBOOK_UNTITLED_NAME);
	AddTab(addressBook);
}

void AddressBookWindow::Sort(string header)
{
	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	addressBook->SortBy(header);

	UpdateContactList();
}

void AddressBookWindow::Search(string searchStr)
{
	if (IsUnsave(m_currentTabIndex))
		return;

	if (m_currentTabIndex < 0)
		return;

	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	addressBook->Search(searchStr);

	UpdateContactList();
}

void AddressBookWindow::ShowDetail(int index)
{
	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	CHorizontalLayoutUI *pTabContent = m_TabContexts[m_currentTabIndex].pTabContent;

	if ((int)addressBook->GetData().size() <= index)
		return;

	CLabelUI *name = static_cast<CLabelUI *>(pTabContent->FindSubControl("contact_detail_name"));
	CTextUI *address = static_cast<CTextUI *>(pTabContent->FindSubControl("contact_detail_address"));
	CLabelUI *phone = static_cast<CLabelUI *>(pTabContent->FindSubControl("contact_detail_phone"));
	CTextUI *addressDetail = static_cast<CTextUI *>(pTabContent->FindSubControl("contact_detail_address_detail"));

	string nameStr = "", phoneStr = "", addressStr = "", addresDetailStr = "";

	//if index invalid, display nothing
	if (index == -1)
	{
		name->SetText(nameStr.c_str());
		phone->SetText(phoneStr.c_str());
		address->SetText(addressStr.c_str());
		addressDetail->SetText(addresDetailStr.c_str());
		return;
	}

	map<string, string> entry = addressBook->GetData()[index];
	auto it = entry.find(FIRSTNAME_HEADER_NAME);
	if (it != entry.end())
		nameStr += (*it).second;

	it = entry.find(LASTNAME_HEADER_NAME);
	if (it != entry.end())
		nameStr += " " + (*it).second;

	it = entry.find(PHONE_HEADER_NAME);
	if (it != entry.end())
		phoneStr += (*it).second;

	it = entry.find(ADDRESS1_HEADER_NAME);
	if (it != entry.end())
	{
		addressStr += (*it).second;
		addresDetailStr += (*it).second;
	}

	it = entry.find(ADDRESS2_HEADER_NAME);
	if (it != entry.end())
		addresDetailStr += "\n" + (*it).second;

	it = entry.find(CITY_HEADER_NAME);
	if (it != entry.end())
		addresDetailStr += "\n" + (*it).second;

	it = entry.find(STATE_HEADER_NAME);
	if (it != entry.end())
		addresDetailStr += " " + (*it).second;

	it = entry.find(ZIPCODE_HEADER_NAME);
	if (it != entry.end())
		addresDetailStr += " " + (*it).second;

	name->SetText(nameStr.c_str());
	phone->SetText(phoneStr.c_str());
	address->SetText(addressStr.c_str());
	addressDetail->SetText(addresDetailStr.c_str());

	//if there's any extra info
	vector<string> headers = addressBook->GetHeaders();
	int headerNum = headers.size();
	if (headerNum > 8)
	{
		CVerticalLayoutUI *customArea = static_cast<CVerticalLayoutUI *>(pTabContent->FindSubControl("contact_detail_custom"));
		customArea->RemoveAll();
		for (int i = 8; i < headerNum; i++)
		{
			string infoStr = entry[headers[i]];
			if (infoStr == "")
				continue;

			CLabelUI *label = new CLabelUI();
			label->SetAttribute("font", "2");
			label->SetAttribute("text", headers[i].c_str());
			label->SetManager(&m_PaintManager, customArea);

			CLabelUI *info = new CLabelUI();
			info->SetAttribute("font", "2");
			info->SetAttribute("text", entry[headers[i]].c_str());
			info->SetManager(&m_PaintManager, customArea);

			customArea->Add(label);
			customArea->Add(info);
		}
	}
}

void AddressBookWindow::UpdateContactList()
{
	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	CHorizontalLayoutUI *pTabContent = m_TabContexts[m_currentTabIndex].pTabContent;
	COptionUI *tab = m_TabContexts[m_currentTabIndex].pTab;
	CListUI *contactList = static_cast<CListUI *>(pTabContent->FindSubControl("contact_list"));

	contactList->RemoveAll();

	tab->SetText(addressBook->GetName().c_str());

	vector< map<string, string> > entryList;
	addressBook->GetAllContacts(&entryList);
	for (auto &i : entryList)
	{
		CDialogBuilder builder;
		CListContainerElementUI *newContact = static_cast<CListContainerElementUI *>(builder.Create(m_ListCententTemplate.c_str(), NULL, (UINT)0, &m_PaintManager));
		newContact->SetUserData(CON_LIST_ID);

		CLabelUI *name = static_cast<CLabelUI *>(newContact->FindSubControl("list_content_name"));
		CLabelUI *phone = static_cast<CLabelUI *>(newContact->FindSubControl("list_content_phone"));
		CLabelUI *zip = static_cast<CLabelUI *>(newContact->FindSubControl("list_content_zipcode"));

		string nameStr = "", phoneStr = "", zipStr = "";

		auto it = i.find(FIRSTNAME_HEADER_NAME);
		if (it != i.end())
			nameStr += (*it).second;

		it = i.find(LASTNAME_HEADER_NAME);
		if (it != i.end())
			nameStr += " " + (*it).second;

		it = i.find(PHONE_HEADER_NAME);
		if (it != i.end())
			phoneStr += (*it).second;

		it = i.find(ZIPCODE_HEADER_NAME);
		if (it != i.end())
			zipStr += (*it).second;

		name->SetText(nameStr.c_str());
		phone->SetText(phoneStr.c_str());
		zip->SetText(zipStr.c_str());

		contactList->Add(newContact);
	}
}

void AddressBookWindow::AddTab(AddressBookModel *ad)
{
	AddressBookModel  *newAD;
	if (ad == NULL)
	{
		vector<string> columns;
		string name;
		CustomColumnsWindow* pCosCal = new CustomColumnsWindow(&columns, &name);
		pCosCal->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
		pCosCal->CenterWindow();
		if (pCosCal->ShowModal())
			return;

		newAD = new AddressBookModel(columns);
		if (name == "")
			newAD->SetName(ADDRESSBOOK_UNTITLED_NAME);
		else
			newAD->SetName(name);
	}
	else
	{
		newAD = ad;
	}	

	CDialogBuilder builder;
	COptionUI *newTab = static_cast<COptionUI *>(builder.Create(m_TabTemplate.c_str(), NULL, (UINT)0, &m_PaintManager));
	CContainerUI *tabBar = static_cast<CContainerUI *>(m_PaintManager.FindControl("tab_bar"));
	tabBar->AddAt(newTab, 0);
	newTab->Selected(true);

	CDialogBuilder builder2;
	CHorizontalLayoutUI *newTabContent = static_cast<CHorizontalLayoutUI *>(builder2.Create(m_TabContentTemplate.c_str(), NULL, (UINT)0, &m_PaintManager));
	CTabLayoutUI *tabContentContainer = static_cast<CTabLayoutUI *>(m_PaintManager.FindControl("tab_content_container"));
	tabContentContainer->AddAt(newTabContent, 0);
	tabContentContainer->SelectItem(0);

	TabContext tabContext;
	tabContext.pTab = newTab;
	tabContext.pAddressBook = newAD;
	tabContext.pTabContent = newTabContent;

	m_TabContexts.insert(m_TabContexts.begin(), tabContext);

	UpdateContactList();
}

void AddressBookWindow::RemoveTab(int index)
{
	if (((int)m_TabContexts.size()) <= 0 || ((int)m_TabContexts.size()) - 1 < index)
		return;

	if (IsUnsave(index))
		return;

	//remove tab
	m_TabBar->RemoveAt(index);
	//remove tab content
	CTabLayoutUI *tabContentContainer = static_cast<CTabLayoutUI *>(m_PaintManager.FindControl("tab_content_container"));
	tabContentContainer->RemoveAt(index);
	//remove tab context
	m_TabContexts.erase(m_TabContexts.begin() + index);

	//if there's no more tab, select none
	if (m_TabContexts.size() <= 0)
	{
		m_currentTabIndex = -1;
		return;
	}

	//if there is tab, select the right tab
	if (((int)m_TabContexts.size()) - 1 > index)
		static_cast<COptionUI *>(m_TabBar->GetItemAt(index))->Selected(true);
	else
		static_cast<COptionUI *>(m_TabBar->GetItemAt(0))->Selected(true);
}

void AddressBookWindow::AddContact()
{
	if (m_TabContexts.empty())
	{
		MessageBox(NULL, "Please open or create an address book first", "Notice", MB_OK);
		return;
	}

	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	addressBook->AddNewContact(map<string, string>());

	UpdateContactList();
}

void AddressBookWindow::DeleteContact(int index)
{
	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	addressBook->DeleteContact(index);
	UpdateContactList();
	if (addressBook->GetSize() - 1 > index)
		ShowDetail(index);
	else
		ShowDetail(-1);
}
void AddressBookWindow::DeleteAddressbook()
{
	vector<string> names;
	AddressBookModel::GetAllAddressBookNames(&names);

	string res = "";
	OneDropdownWindow* pOpen = new OneDropdownWindow(
		&res, 
		"Address book name: ", 
		"Input a name to DELETE an address book", 
		names, true, "It's gonna cry T^T!!!!");
	pOpen->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
	pOpen->CenterWindow();
	if (pOpen->ShowModal())
		return;

	//Check if tab name is empty
	if (res == "")
		return;

	if (AddressBookModel::DeleteAddressbook(res))
		MessageBox(NULL, "Failed deleting address book", "Warning", MB_OK);
}

void AddressBookWindow::Prepare()
{
	//List
	CListUI *ContactList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("contact_list")));
	if (ContactList != NULL)
		ContactList->RemoveAll();

	//Tab
	m_TabBar = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("tab_bar")));
	COptionUI *tabTemplate = static_cast<COptionUI *>(m_PaintManager.FindControl("tab_template"));
	if (m_TabBar != NULL && tabTemplate != NULL)
		m_TabBar->Remove(tabTemplate);

	//Tab content
	CTabLayoutUI *TabContentContainer = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tab_content_container")));
	if (TabContentContainer != NULL)
		TabContentContainer->RemoveAll();
}

bool AddressBookWindow::IsUnsave(int index)
{
	if (m_currentTabIndex < index)
		return false;

	AddressBookModel *addressBook = m_TabContexts[index].pAddressBook;
	if (addressBook->IsChanged())
	{
		if(MessageBox(NULL, "There are unsaved changes, do you want to discard the changes?", "Unsaved changeds", MB_OKCANCEL) != 1)
			return true;
	}

	return false;
}

bool AddressBookWindow::IsTabDup(string tabName, int *out)
{
	//Check if tab name already exists
	int tabNum = m_TabBar->GetCount();
	for (int i = 0; i < tabNum; i++)
	{
		CControlUI *tab = m_TabBar->GetItemAt(i);
		string tabClass = tab->GetClass();
		if (tabClass == "OptionUI")
		{
			string tabText = tab->GetText();
			if (tabName == tabText)
			{
				if (out != NULL)
					(*out) = i;
				return true;
			}
		}
	}

	return false;
}

void AddressBookWindow::Help()
{
	HelpWindow* pHelp = new HelpWindow();
	pHelp->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
	pHelp->CenterWindow();
	pHelp->ShowModal();
}