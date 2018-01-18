#include "stdafx.h"
#include "GYDriverManager.h"

//调试用
// void ShowErrorMsg()
// {
//   LPVOID lpMsgBuf;
//   FormatMessage(
//     FORMAT_MESSAGE_ALLOCATE_BUFFER |
//     FORMAT_MESSAGE_FROM_SYSTEM |
//     FORMAT_MESSAGE_IGNORE_INSERTS,
//     NULL,
//     GetLastError(),
//     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//     (LPTSTR)&lpMsgBuf,
//     0,
//     NULL
//   );
//   // Process any inserts in lpMsgBuf.
//   // ...
//   // Display the string.
//   MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK | MB_ICONINFORMATION);
//   // Free the buffer.
//   LocalFree(lpMsgBuf);
// }

CGYDriverManager::CGYDriverManager()
{
  memset(m_szDriverPath, 0, MAX_PATH);
  m_strDriverName = TEXT("");

  m_strFeedBackInfo = TEXT("");

  m_bIsDriverIntalled = false;
  m_bIsDriverStarted = false;
}


CGYDriverManager::~CGYDriverManager()
{
  int nTryCount = 5;
  while (m_bIsDriverIntalled == true)
  {
    UninstallDriver();
    nTryCount--;

    if (nTryCount < 0)
    {
      break;
    }
  }

}

int CGYDriverManager::SetDriverPath(const TCHAR *szDriverPath)
{
  int nTryCount = 5;
  while (m_bIsDriverIntalled == true)
  {
    UninstallDriver();
    nTryCount--;

    if (nTryCount < 0)
    {
      FlushFeedBackInfo();
      m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("驱动路径设置失败：无法卸载原驱动。"));
      return -1;
    }
  }

  FlushFeedBackInfo();

  wcscpy(m_szDriverPath, szDriverPath);

  m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("驱动路径设置成功。"));

  TCHAR szDrive[_MAX_DRIVE];
  TCHAR szDir[_MAX_DIR];
  TCHAR szFname[_MAX_FNAME];
  TCHAR szExt[_MAX_EXT];

  _tsplitpath(szDriverPath, szDrive, szDir, szFname, szExt);
  
  m_strDriverName.Format(TEXT("%s%s"), szFname, szExt);
  m_strFeedBackInfo.AppendFormat(TEXT("驱动名：%s。"), m_strDriverName.GetBuffer(0));

  m_bIsDriverIntalled = false;  //冗余，本来就是false
  m_bIsDriverStarted = false;

  return 0;
}

int CGYDriverManager::InstallDriver()
{
  SC_HANDLE schSCManager = NULL;
  SC_HANDLE schService = NULL;

  if (wcscmp(m_szDriverPath, TEXT("")) == 0)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("驱动安装失败：未设置驱动路径。"));
    goto LABLE_FAILURE_IN_InstallDriver;
  }

  if (m_bIsDriverIntalled == true)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("安装失败：无需再次安装。"));
    goto LABLE_FAILURE_IN_InstallDriver;
  }

  schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("安装失败：SCManager创建失败。"));
    goto LABLE_FAILURE_IN_InstallDriver;
  }

  const TCHAR *szDriveName = m_strDriverName;

  schService = CreateService(
    schSCManager,              // SCManager database 
    szDriveName,               // name of service 
    szDriveName,               // service name to display 
    SERVICE_ALL_ACCESS,        // desired access 
    SERVICE_KERNEL_DRIVER,     // service type 
    SERVICE_DEMAND_START,      // start type 
    SERVICE_ERROR_NORMAL,      // error control type 
    m_szDriverPath,            // service's binary 
    NULL,                      // no load ordering group 
    NULL,                      // no tag identifier 
    NULL,                      // no dependencies 
    NULL,                      // LocalSystem account 
    NULL);                     // no password 
  if (schService == NULL)
  {
    //int nError = GetLastError();
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s%d"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("安装失败：服务创建失败。")/*, nError*/);

    //ShowErrorMsg();
    goto LABLE_FAILURE_IN_InstallDriver;
  }

  m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
    m_strDriverName, TEXT("安装成功。"));

  m_bIsDriverIntalled = true;

  CloseServiceHandle(schSCManager);
  CloseServiceHandle(schService);

  return 0;
LABLE_FAILURE_IN_InstallDriver:
  if (schService != NULL)
  {
    CloseServiceHandle(schService);
  }

  if (schSCManager != NULL)
  {
    CloseServiceHandle(schSCManager);
  }

  return -1;
}

int CGYDriverManager::StartDriver()
{
  SC_HANDLE schSCManager = NULL;
  SC_HANDLE schService = NULL;

  if (wcscmp(m_szDriverPath, TEXT("")) == 0)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("驱动启动失败：未设置驱动路径。"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  if (m_bIsDriverIntalled == false)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
             m_strDriverName.GetBuffer(0), TEXT("启动失败：请先安装驱动。"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  if (m_bIsDriverStarted == true)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("启动失败：服务已经启动，无法再次启动。"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("启动失败：SCManager创建失败。"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  const TCHAR *szDriveName = m_strDriverName;

  schService = OpenService(schSCManager, szDriveName, SERVICE_ALL_ACCESS);
  if (schService == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s%d"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("启动失败：服务打开失败。"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  BOOL bRet = StartService(schService, 0, NULL);
  if (!bRet)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("启动失败：请重试。"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
    m_strDriverName.GetBuffer(0), TEXT("启动成功。"));

  m_bIsDriverStarted = true;

  CloseServiceHandle(schService);
  CloseServiceHandle(schSCManager);

  return 0;

LABLE_FAILURE_IN_StartDriver:
  if (schService != NULL)
  {
    CloseServiceHandle(schService);
  }

  if (schSCManager != NULL)
  {
    CloseServiceHandle(schSCManager);
  }

  return -1;
}

int CGYDriverManager::StopDriver()
{
  SC_HANDLE schSCManager = NULL;
  SC_HANDLE schService = NULL;
  SERVICE_STATUS ss = { 0 };

  if (wcscmp(m_szDriverPath, TEXT("")) == 0)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("驱动停止失败：未设置驱动路径。"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  if (m_bIsDriverIntalled == false)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("停止失败：请先安装驱动。"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  if (m_bIsDriverStarted == false)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("停止失败：服务尚未启动。"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("停止失败：SCManager创建失败。"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  const TCHAR *szDriveName = m_strDriverName;

  schService = OpenService(schSCManager, szDriveName, SERVICE_ALL_ACCESS);
  if (schService == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s%d"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("停止失败：服务打开失败。"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  BOOL bRet = ControlService(schService, SERVICE_CONTROL_STOP, &ss);
  if (!bRet)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("停止失败：请重试。"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
    m_strDriverName.GetBuffer(0), TEXT("已停止。"));

  m_bIsDriverStarted = false;

  CloseServiceHandle(schService);
  CloseServiceHandle(schSCManager);

  return 0;

LABLE_FAILURE_IN_StopDriver:
  if (schService != NULL)
  {
    CloseServiceHandle(schService);
  }

  if (schSCManager != NULL)
  {
    CloseServiceHandle(schSCManager);
  }

  return -1;
}

int CGYDriverManager::UninstallDriver()
{
  SC_HANDLE schSCManager = NULL;
  SC_HANDLE schService = NULL;

  if (wcscmp(m_szDriverPath, TEXT("")) == 0)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("驱动卸载失败：尚未设置驱动路径。"));
    goto LABLE_FAILURE_IN_UninstallDriver;
  }

  if (m_bIsDriverIntalled == false)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("卸载失败：尚未安装驱动。"));
    goto LABLE_FAILURE_IN_UninstallDriver;
  }

  if (m_bIsDriverStarted == true)
  {
    StopDriver();
    FlushFeedBackInfo();
  }

  schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("卸载失败：SCManager创建失败。"));
    goto LABLE_FAILURE_IN_UninstallDriver;
  }

  const TCHAR *szDriveName = m_strDriverName;

  schService = OpenService(schSCManager, szDriveName, SERVICE_ALL_ACCESS);
  if (schService == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s%d"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("卸载失败：服务打开失败。"));
    goto LABLE_FAILURE_IN_UninstallDriver;
  }

  BOOL bRet = DeleteService(schService);
  if (!bRet)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
      m_strDriverName.GetBuffer(0), TEXT("卸载失败。请重试。"));
    goto LABLE_FAILURE_IN_UninstallDriver;
  }

  m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("驱动"),
    m_strDriverName.GetBuffer(0), TEXT("已卸载。"));

  m_bIsDriverIntalled = false;

  CloseServiceHandle(schService);
  CloseServiceHandle(schSCManager);

  return 0;

LABLE_FAILURE_IN_UninstallDriver:
  if (schService != NULL)
  {
    CloseServiceHandle(schService);
  }

  if (schSCManager != NULL)
  {
    CloseServiceHandle(schSCManager);
  }

  return -1;
}

int CGYDriverManager::FlushFeedBackInfo()
{
  m_strFeedBackInfo = TEXT("");

  return 0;
}

int CGYDriverManager::GetFeedBackInfo(CString& strGiveBackToUser)
{
  strGiveBackToUser = m_strFeedBackInfo;

  return 0;
}
