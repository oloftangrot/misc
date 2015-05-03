// Controller.h : main header file for the CONTROLLER application
//

#if !defined(AFX_CONTROLLER_H__A92FC848_D0C3_11D1_B00D_000000000000__INCLUDED_)
#define AFX_CONTROLLER_H__A92FC848_D0C3_11D1_B00D_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CControllerApp:
// See Controller.cpp for the implementation of this class
//

class CControllerApp : public CWinApp
{
public:
	CControllerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControllerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CControllerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLLER_H__A92FC848_D0C3_11D1_B00D_000000000000__INCLUDED_)
