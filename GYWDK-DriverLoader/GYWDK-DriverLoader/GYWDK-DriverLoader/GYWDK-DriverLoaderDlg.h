
// GYWDK-DriverLoaderDlg.h : header file
//

#pragma once

#define DLL_NAME _T("GYWDK-DriverManager.dll")

typedef int(*pfnDllManagerSetPath)(const TCHAR *szDriverPath, CString& strFeedBack);
typedef int(*pfnDllManagerOperation)(int nOpCode, CString& strFeedBack);


// CGYWDKDriverLoaderDlg dialog
class CGYWDKDriverLoaderDlg : public CDialogEx
{
// Construction
public:
	CGYWDKDriverLoaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GYWDKDRIVERLOADER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButtonChooseFile();

private:
  pfnDllManagerSetPath m_pfnSetPath;
  pfnDllManagerOperation m_pfnOperation;
public:
  CString m_strEditFeedBack;
  afx_msg void OnDropFiles(HDROP hDropInfo);
  CString m_strEditPath;
  afx_msg void OnBnClickedButtonInstall();
  afx_msg void OnBnClickedButtonStart();
  afx_msg void OnBnClickedButtonStop();
  afx_msg void OnBnClickedButtonUninstall();
  afx_msg void OnClose();
};
