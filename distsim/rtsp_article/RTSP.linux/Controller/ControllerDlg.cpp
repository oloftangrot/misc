// ControllerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "ControllerDlg.h"

#include <Parser.h>

// Create an RTSP object that is a controller (no name is necessary)
static RTSP* s_rtsp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char* CmdLine;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControllerDlg dialog

CControllerDlg::CControllerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CControllerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControllerDlg)
	m_FederationStatusString = _T("Stopped");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CControllerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControllerDlg)
	DDX_Control(pDX, IDC_LIST, m_ListBox);
	DDX_Text(pDX, IDC_FEDERATION_STATUS, m_FederationStatusString);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CControllerDlg, CDialog)
	//{{AFX_MSG_MAP(CControllerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_INITIALIZE, OnInitialize)
	ON_BN_CLICKED(ID_START, OnStart)
	ON_BN_CLICKED(ID_HALT, OnHalt)
	ON_BN_CLICKED(ID_PING, OnPing)
	ON_COMMAND(ID_OPEN, OnOpen)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControllerDlg message handlers

BOOL CControllerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
    int tab_stop = 120;
    m_ListBox.SetTabStops(tab_stop);

    if (strlen(CmdLine) > 0)
        LoadMDFFile(CmdLine);

	UINT timer_num = 1;
	UINT timeout_msec = 50;
	SetTimer(timer_num, timeout_msec, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CControllerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CControllerDlg::OnDestroy()
{
	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CControllerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CControllerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

static void ErrorMsgBox(const char * msg)
{
    AfxMessageBox(msg, MB_OK|MB_ICONINFORMATION);
}

void CControllerDlg::OnOpen() 
{
    bool do_open = true;
    char* def_ext = ".mdf";
    char* def_file_name = NULL;
    int flags = OFN_HIDEREADONLY;
    char* filter = "MDF files (*.mdf)|*.mdf||";

    CFileDialog file_dlg(do_open, def_ext, def_file_name, flags, filter);
    int result = file_dlg.DoModal();

    if (result == IDOK)
        LoadMDFFile(file_dlg.GetPathName());
}

void CControllerDlg::OnExit() 
{
	SendMessage(WM_CLOSE, 0, 0);
}

void CControllerDlg::OnAbout() 
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}

void CControllerDlg::OnPing() 
{
	if (!s_rtsp)
		AfxMessageBox("No MDF file has been loaded", MB_OK|MB_ICONINFORMATION);
	else
	{
		s_rtsp->CmdFederation(RTSP::Ping);
        UpdateListBox(RTSP::Ping);
	}
}

void CControllerDlg::OnInitialize() 
{
	if (!s_rtsp)
		AfxMessageBox("No MDF file has been loaded", MB_OK|MB_ICONINFORMATION);
	else
    {
		s_rtsp->CmdFederation(RTSP::Initialize, 2.0);
        UpdateListBox(RTSP::Initialize);
	}
}

void CControllerDlg::OnStart() 
{
	if (!s_rtsp)
		AfxMessageBox("No MDF file has been loaded", MB_OK|MB_ICONINFORMATION);
	else
	{
		s_rtsp->CmdFederation(RTSP::Start);
        UpdateListBox(RTSP::Start);
	}
}

void CControllerDlg::OnHalt() 
{
	if (!s_rtsp)
		AfxMessageBox("No MDF file has been loaded", MB_OK|MB_ICONINFORMATION);
	else
	{
		s_rtsp->CmdFederation(RTSP::Halt);
        UpdateListBox(RTSP::Halt);
	}
}

void CControllerDlg::UpdateListBox(RTSP::MsgType msg_type) 
{
    char* cmd;
    bool* status_array;
    switch (msg_type)
    {
    case RTSP::Ping:
        cmd = "Ping";
        status_array = &_FederatePinged[0];
        break;
    case RTSP::Initialize:
        cmd = "Initialize";
        status_array = &_FederateInitialized[0];
        break;
    case RTSP::Start:
        cmd = "Start";
        status_array = &_FederateRunning[0];
        break;
    case RTSP::Halt:
        cmd = "Halt";
        status_array = &_FederateHalted[0];
        break;
    }

    // Delete all items currently in the list box
    while (m_ListBox.DeleteString(0) != LB_ERR)
        ;

    // Add the federate names and their initial state to the list
	int index = 0;
    for (int i=0; i<_NumFederateTypes; i++)
        for (uint j=0; j<_FederateInfoList[i].dimension; j++)
		{
            std::string fed_name = " ";
            fed_name += _FederateInfoList[i].name;

            if (_FederateInfoList[i].dimension > 1)
            {
                char buf[20];
				sprintf(buf, "[%d]", j);
                fed_name += buf;
            }

            char* result;
            switch (_FederateMsgResult[index])
            {
            case Success:
                result = "Success";
                break;
            case UserRoutineFailure:
                result = "Failed";
                break;
            case InitFailed_Running:
                result = "Can't initialize (running)";
                break;
            case StartFailed_Running:
                result = "Can't start (running)";
                break;
            case StartFailed_NoInit:
                result = "Can't start (not initialized)";
                break;
            case HaltFailed_NotRunning:
                result = "Can't halt (not running)";
                break;
            }

            fed_name += "\t";
            fed_name += cmd;
            fed_name += ": ";

            if (status_array[index])
            {
                if (msg_type == RTSP::Ping && _FederateMsgResult[index] == Success)
                {
                    char buf[50];
	    		    sprintf(buf, "%.3lf mS", 0.001*_ResponseTime[index]);
                    fed_name += buf;
                }
                else
                    fed_name += result;
            }
            else
                fed_name += "No response";

            m_ListBox.AddString(fed_name.c_str());
			index++;
		}

    switch (s_rtsp->FederationState())
    {
    case RTSP::Stopped:
    	m_FederationStatusString = _T("Stopped");
        break;
    case RTSP::Initialized:
    	m_FederationStatusString = _T("Initialized");
        break;
    case RTSP::Running:
    	m_FederationStatusString = _T("Running");
        break;
    }

    UpdateData(false);
}

void CControllerDlg::OnTimer(UINT nIDEvent) 
{
    if (s_rtsp)
    {
	    static RTSP::StateType prev_state = RTSP::Stopped;
	    s_rtsp->PollRcv(true, 0);

	    RTSP::StateType new_state = s_rtsp->FederationState();

	    if (new_state == RTSP::Stopped && new_state != prev_state)
		    UpdateListBox(RTSP::Halt);

	    prev_state = new_state;
    }
	
	CDialog::OnTimer(nIDEvent);
}


void CControllerDlg::LoadMDFFile(CString file_name)
{
    ParseMDFFile(std::string(LPCTSTR(file_name)), ErrorMsgBox);
    GenerateMDFData();

	delete s_rtsp;
	s_rtsp = new RTSP;

    // Delete all items currently in the list box
    while (m_ListBox.DeleteString(0) != LB_ERR)
        ;

    if (ErrorCount == 0)
    {
        // Add the federate names and their initial state to the list
        for (int i=0; i<_NumFederateTypes; i++)
            for (uint j=0; j<_FederateInfoList[i].dimension; j++)
		    {
                std::string fed_name = " ";
                fed_name += _FederateInfoList[i].name;

                if (_FederateInfoList[i].dimension > 1)
                {
                    char buf[20];
				    sprintf(buf, "[%d]", j);
                    fed_name += buf;
                }

                fed_name += "\tNot Connected";
                m_ListBox.AddString(fed_name.c_str());
		    }
    }
}
