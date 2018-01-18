
// GYWDK-DriverLoaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GYWDK-DriverLoader.h"
#include "GYWDK-DriverLoaderDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGYWDKDriverLoaderDlg dialog



CGYWDKDriverLoaderDlg::CGYWDKDriverLoaderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GYWDKDRIVERLOADER_DIALOG, pParent)
  , m_strEditFeedBack(_T(""))
  , m_strEditPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGYWDKDriverLoaderDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_FEEDBACK_INFO, m_strEditFeedBack);
  DDX_Text(pDX, IDC_EDIT_PATH, m_strEditPath);
}

BEGIN_MESSAGE_MAP(CGYWDKDriverLoaderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_BUTTON_CHOOSE_FILE, &CGYWDKDriverLoaderDlg::OnBnClickedButtonChooseFile)
  ON_WM_DROPFILES()
  ON_BN_CLICKED(IDC_BUTTON_INSTALL, &CGYWDKDriverLoaderDlg::OnBnClickedButtonInstall)
  ON_BN_CLICKED(IDC_BUTTON_START, &CGYWDKDriverLoaderDlg::OnBnClickedButtonStart)
  ON_BN_CLICKED(IDC_BUTTON_STOP, &CGYWDKDriverLoaderDlg::OnBnClickedButtonStop)
  ON_BN_CLICKED(IDC_BUTTON_UNINSTALL, &CGYWDKDriverLoaderDlg::OnBnClickedButtonUninstall)
  ON_WM_CLOSE()
END_MESSAGE_MAP()


// CGYWDKDriverLoaderDlg message handlers

BOOL CGYWDKDriverLoaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

  HINSTANCE hDll = LoadLibrary(DLL_NAME);

  if (hDll == NULL)
  {
    //TRACE(_T("DLLº”‘ÿ ß∞‹"));
    return 0;
  }

  m_pfnSetPath = (pfnDllManagerSetPath)GetProcAddress(hDll, "GYWDKSetDllPath");
  m_pfnOperation = (pfnDllManagerOperation)GetProcAddress(hDll, "GYWDKOperation");


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGYWDKDriverLoaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGYWDKDriverLoaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGYWDKDriverLoaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGYWDKDriverLoaderDlg::OnBnClickedButtonChooseFile()
{
  // TODO: Add your control notification handler code here
  CFileDialog dlg(TRUE);

  dlg.m_ofn.lpstrTitle = _T("—°‘Ò«˝∂Ø");
  dlg.m_ofn.lpstrFilter = _T("Driver Files(*.sys)\0*.sys\0All Files(*.*)\0*.*\0\0");
  dlg.m_ofn.lpstrDefExt = _T("sys");

  if (IDOK == dlg.DoModal())
  {
    CString strPath = dlg.GetPathName();
    m_strEditPath = strPath;

    CString strFeedBack = _T("");
    m_pfnSetPath(strPath, strFeedBack);
    m_strEditFeedBack = strFeedBack;

    UpdateData(FALSE);
  }
}


void CGYWDKDriverLoaderDlg::OnDropFiles(HDROP hDropInfo)
{
  // TODO: Add your message handler code here and/or call default
  TCHAR filePath[200];
 
  DragQueryFile(hDropInfo, 0, filePath, sizeof(filePath));
  DragFinish(hDropInfo);

  CString strPath;
  strPath.Format(_T("%s"), filePath);

  m_strEditPath = strPath;

  CString strFeedBack = _T("");
  m_pfnSetPath(filePath, strFeedBack);

  m_strEditFeedBack = strFeedBack;
  UpdateData(FALSE);
      
  CDialogEx::OnDropFiles(hDropInfo);
}


void CGYWDKDriverLoaderDlg::OnBnClickedButtonInstall()
{
  // TODO: Add your control notification handler code here
  CString strFeedBack = _T("");
  m_pfnOperation(0, strFeedBack);

  m_strEditFeedBack = strFeedBack;
  UpdateData(FALSE);
}


void CGYWDKDriverLoaderDlg::OnBnClickedButtonStart()
{
  // TODO: Add your control notification handler code here
  CString strFeedBack = _T("");
  m_pfnOperation(1, strFeedBack);

  m_strEditFeedBack = strFeedBack;
  UpdateData(FALSE);
}


void CGYWDKDriverLoaderDlg::OnBnClickedButtonStop()
{
  // TODO: Add your control notification handler code here
  CString strFeedBack = _T("");
  m_pfnOperation(2, strFeedBack);

  m_strEditFeedBack = strFeedBack;
  UpdateData(FALSE);
}


void CGYWDKDriverLoaderDlg::OnBnClickedButtonUninstall()
{
  // TODO: Add your control notification handler code here
  CString strFeedBack = _T("");
  m_pfnOperation(3, strFeedBack);

  m_strEditFeedBack = strFeedBack;
  UpdateData(FALSE);
}


void CGYWDKDriverLoaderDlg::OnClose()
{
  // TODO: Add your message handler code here and/or call default
  CString strFeedBack = _T("");
  m_pfnOperation(3, strFeedBack);

  CDialogEx::OnClose();
}
