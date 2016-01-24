#pragma 
#define UILIB_STATIC
#include "..\DuiLib\UIlib.h"
#include "Menu\UIMenu.h"
#include "..\..\Model\AddressBookModel.h"
#include "../../resource.h"

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\bin\\DuiLib_ud.lib")
#   else
#       pragma comment(lib, "..\\bin\\DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\bin\\DuiLib_u.lib")
#   else
#       pragma comment(lib, "..\\bin\\DuiLib.lib")
#   endif
#endif

using namespace DuiLib;

typedef struct
{
	COptionUI *pTab;
	CHorizontalLayoutUI *pTabContent;
	AddressBookModel *pAddressBook;
} TabContext;

class AddressBookWindow : public WindowImplBase
{
public:
	AddressBookWindow(CDuiString SkinFolder, CDuiString SkinFile, UILIB_RESOURCETYPE ResType = UILIB_FILE, LPCTSTR ResID = 0);
	~AddressBookWindow();
	virtual LPCTSTR    GetWindowClassName() const   { return _T("AddressBookDUIMainFrame"); }

	virtual UILIB_RESOURCETYPE GetResourceType() const { return m_ResType; }
	virtual LPCTSTR GetResourceID() const			{ return m_ResID; }
	virtual CDuiString GetSkinFolder()              { return m_SkinFolder; }
	virtual CDuiString GetSkinFile()                { return m_SkinFile; }

	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual void Notify(TNotifyUI& msg);

private:
	UILIB_RESOURCETYPE m_ResType;
	LPCTSTR m_ResID;
	CDuiString m_SkinFolder;
	CDuiString m_SkinFile;

	CContainerUI *m_TabBar;
	int m_currentTabIndex;
	vector<TabContext> m_TabContexts;

private:
	void Prepare();
	void OpenMenu(CDuiPoint p, STRINGorID fileName);

	void AddTab(AddressBookModel *ad = NULL);
	void RemoveTab();
	void AddContact();
	void ShowDetail(int index);
	void Edit(int index);
	void UpdateContactList();
	void Open();
	void Save();
	

	void OnTabSelect(TNotifyUI &msg);
	void OnContactListSelect(TNotifyUI &msg);
	void OnContactListActivate(TNotifyUI &msg);

private:
	//=======Templates========
	const string m_ListCententTemplate = "list_content_template.xml";
	//Tab
	const string m_TabTemplate = "tab_template.xml";
	//Tab content
	const string m_TabContentTemplate = "tab_content_template.xml";
};