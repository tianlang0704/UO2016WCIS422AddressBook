#include "AddressBookWindow.h"
#include "OneInputWindow.h"
#include "OneDropdownWindow.h"
#include "EditWindow.h"
#include "CustomColumnsWindow.h"
#include "../../Model/AddressBookModel.h"
#include <windows.h>
#include <shobjidl.h> 

#define CON_LIST_ID "ContactListMember"

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
			RemoveTab();
		else if (msg.pSender->GetName() == "menu_btn_save")
			Save();
		else if (msg.pSender->GetName() == "menu_btn_exit")
			Exit();
		else if (msg.pSender->GetName() == "menu_btn_export")
			Export();
		else if (msg.pSender->GetName() == "menu_btn_import")
			Import();
		else if (msg.pSender->GetName() == "menu_btn_sort_zip")
			Sort(ZIPCODE_HEADER_NAME);
		else if (msg.pSender->GetName() == "menu_btn_sort_lastname")
			Sort(LASTNAME_HEADER_NAME);
		else if (msg.pSender->GetUserData() == CON_LIST_ID)
			OnContactListSelect(msg);
	}
	else if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == "w_close")
			Exit();
		else if (msg.pSender->GetName() == "btn_file")
			OpenMenu(CDuiPoint(msg.pSender->GetPos().left, msg.pSender->GetPos().bottom), "menu_file.xml");
		else if (msg.pSender->GetName() == "btn_edit")
			OpenMenu(CDuiPoint(msg.pSender->GetPos().left, msg.pSender->GetPos().bottom), "menu_edit.xml");
		else if (msg.pSender->GetName() == "btn_view")
			OpenMenu(CDuiPoint(msg.pSender->GetPos().left, msg.pSender->GetPos().bottom), "menu_view.xml");
		else if (msg.pSender->GetName() == "tab_new")
			AddTab();
		else if (msg.pSender->GetName() == "tool_btn_add")
			AddContact();
		else if (msg.pSender->GetName() == "tool_btn_save")
			Save();
		else if (msg.pSender->GetName() == "tool_btn_close")
			RemoveTab();
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

	}
};

void AddressBookWindow::OpenMenu(CDuiPoint p, STRINGorID fileName)
{
	CMenuWnd* pMenu = new CMenuWnd(NULL, &m_PaintManager);
	CDuiPoint point = p;
	ClientToScreen(m_hWnd, &point);
	pMenu->Init(NULL, fileName, NULL, point);
};

int AddressBookWindow::OpenFileDialog(string *out, DWORD flagAdd, DWORD flagRemove)
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
	pFileOpen->SetOptions(dwFlags | FOS_PATHMUSTEXIST | flagAdd & ~flagRemove);

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
	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	addressBook->GetContact(index, &res);

	EditWindow* pEditName = new EditWindow(&res);
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

	string res;
	OneDropdownWindow* pOpen = new OneDropdownWindow(&res, "Address book name: ", "Input a name to open an address book", names);
	pOpen->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
	pOpen->CenterWindow();
	if (pOpen->ShowModal())
		return;

	AddressBookModel *addressBook = new AddressBookModel();
	if (addressBook->Open(res))
	{
		delete addressBook;
		MessageBox(NULL, "Address book name does not exist", "Notice", MB_OK);
		return;
	}

	AddTab(addressBook);
}

void AddressBookWindow::Save()
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

void AddressBookWindow::Exit()
{
	if (CheckUnsave())
		return;
	this->Close();
}

void AddressBookWindow::Export()
{
	if (m_currentTabIndex < 0)
		return;

	string fileName;
	int res = OpenFileDialog(&fileName, FOS_CREATEPROMPT | FOS_OVERWRITEPROMPT, FOS_FILEMUSTEXIST);

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
	int res = OpenFileDialog(&fileName);

	if (res == 3)
		return;

	AddressBookModel *addressBook = new AddressBookModel();
	if (addressBook->Import(fileName))
	{
		MessageBox(NULL, "Import failed", "Warning", MB_OK);
	}

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
	if (CheckUnsave())
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
		vector<string> res;
		CustomColumnsWindow* pCosCal = new CustomColumnsWindow(&res);
		pCosCal->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
		pCosCal->CenterWindow();
		if (pCosCal->ShowModal())
			return;

		newAD = new AddressBookModel(res);
		newAD->SetName("Untitled");
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

void AddressBookWindow::RemoveTab()
{
	if (m_TabContexts.size() <= 0)
		return;

	m_TabBar->RemoveAt(m_currentTabIndex);
	CTabLayoutUI *tabContentContainer = static_cast<CTabLayoutUI *>(m_PaintManager.FindControl("tab_content_container"));
	tabContentContainer->RemoveAt(m_currentTabIndex);
	vector<TabContext>::iterator it = m_TabContexts.begin();
	for (int i = 0; i < m_currentTabIndex; i++)
		++it;
	if ((*it).pAddressBook != NULL)
		delete (*it).pAddressBook;
	m_TabContexts.erase(it);

	if (m_TabContexts.size() <= 0)
	{
		m_currentTabIndex = -1;
		return;
	}
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

bool AddressBookWindow::CheckUnsave()
{
	if (m_currentTabIndex < 0)
		return false;

	AddressBookModel *addressBook = m_TabContexts[m_currentTabIndex].pAddressBook;
	if (addressBook->IsChanged())
	{
		if(MessageBox(NULL, "There are unsaved changes, do you want to discard the changes?", "Unsaved changeds", MB_OKCANCEL) != 1)
			return true;
	}

	return false;
}