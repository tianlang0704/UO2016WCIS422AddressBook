#include "GUI/AddressBookDUI/AddressBookWindow.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);

	AddressBookWindow duiFrame(_T(""), _T("skin.xml"), UILIB_ZIPRESOURCE, (LPCTSTR)IDR_ZIPRES1);
	//AddressBookWindow duiFrame(_T("skin"), _T("skin.xml"));
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();
	duiFrame.ShowWindow();
	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();
	return 0;
}