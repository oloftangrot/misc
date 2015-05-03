// Controller.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Controller.h"
#include "ControllerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char* CmdLine;

/////////////////////////////////////////////////////////////////////////////
// CControllerApp

BEGIN_MESSAGE_MAP(CControllerApp, CWinApp)
	//{{AFX_MSG_MAP(CControllerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControllerApp construction

CControllerApp::CControllerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CControllerApp object

CControllerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CControllerApp initialization

BOOL CControllerApp::InitInstance()
{
	//if (!AfxSocketInit())
	//{
	//	AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
	//	return FALSE;
	//}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

    CmdLine = m_lpCmdLine;
	CControllerDlg dlg;
	m_pMainWnd = &dlg;

    dlg.DoModal();

    // Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
