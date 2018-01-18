#include "stdafx.h"
#include "GYDriverManager.h"

//������
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
      m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("����·������ʧ�ܣ��޷�ж��ԭ������"));
      return -1;
    }
  }

  FlushFeedBackInfo();

  wcscpy(m_szDriverPath, szDriverPath);

  m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("����·�����óɹ���"));

  TCHAR szDrive[_MAX_DRIVE];
  TCHAR szDir[_MAX_DIR];
  TCHAR szFname[_MAX_FNAME];
  TCHAR szExt[_MAX_EXT];

  _tsplitpath(szDriverPath, szDrive, szDir, szFname, szExt);
  
  m_strDriverName.Format(TEXT("%s%s"), szFname, szExt);
  m_strFeedBackInfo.AppendFormat(TEXT("��������%s��"), m_strDriverName.GetBuffer(0));

  m_bIsDriverIntalled = false;  //���࣬��������false
  m_bIsDriverStarted = false;

  return 0;
}

int CGYDriverManager::InstallDriver()
{
  SC_HANDLE schSCManager = NULL;
  SC_HANDLE schService = NULL;

  if (wcscmp(m_szDriverPath, TEXT("")) == 0)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("������װʧ�ܣ�δ��������·����"));
    goto LABLE_FAILURE_IN_InstallDriver;
  }

  if (m_bIsDriverIntalled == true)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("��װʧ�ܣ������ٴΰ�װ��"));
    goto LABLE_FAILURE_IN_InstallDriver;
  }

  schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("��װʧ�ܣ�SCManager����ʧ�ܡ�"));
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
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s%d"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("��װʧ�ܣ����񴴽�ʧ�ܡ�")/*, nError*/);

    //ShowErrorMsg();
    goto LABLE_FAILURE_IN_InstallDriver;
  }

  m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
    m_strDriverName, TEXT("��װ�ɹ���"));

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
    m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("��������ʧ�ܣ�δ��������·����"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  if (m_bIsDriverIntalled == false)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
             m_strDriverName.GetBuffer(0), TEXT("����ʧ�ܣ����Ȱ�װ������"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  if (m_bIsDriverStarted == true)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("����ʧ�ܣ������Ѿ��������޷��ٴ�������"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("����ʧ�ܣ�SCManager����ʧ�ܡ�"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  const TCHAR *szDriveName = m_strDriverName;

  schService = OpenService(schSCManager, szDriveName, SERVICE_ALL_ACCESS);
  if (schService == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s%d"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("����ʧ�ܣ������ʧ�ܡ�"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  BOOL bRet = StartService(schService, 0, NULL);
  if (!bRet)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("����ʧ�ܣ������ԡ�"));
    goto LABLE_FAILURE_IN_StartDriver;
  }

  m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
    m_strDriverName.GetBuffer(0), TEXT("�����ɹ���"));

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
    m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("����ֹͣʧ�ܣ�δ��������·����"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  if (m_bIsDriverIntalled == false)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("ֹͣʧ�ܣ����Ȱ�װ������"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  if (m_bIsDriverStarted == false)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("ֹͣʧ�ܣ�������δ������"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("ֹͣʧ�ܣ�SCManager����ʧ�ܡ�"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  const TCHAR *szDriveName = m_strDriverName;

  schService = OpenService(schSCManager, szDriveName, SERVICE_ALL_ACCESS);
  if (schService == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s%d"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("ֹͣʧ�ܣ������ʧ�ܡ�"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  BOOL bRet = ControlService(schService, SERVICE_CONTROL_STOP, &ss);
  if (!bRet)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("ֹͣʧ�ܣ������ԡ�"));
    goto LABLE_FAILURE_IN_StopDriver;
  }

  m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
    m_strDriverName.GetBuffer(0), TEXT("��ֹͣ��"));

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
    m_strFeedBackInfo.AppendFormat(TEXT("%s"), TEXT("����ж��ʧ�ܣ���δ��������·����"));
    goto LABLE_FAILURE_IN_UninstallDriver;
  }

  if (m_bIsDriverIntalled == false)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("ж��ʧ�ܣ���δ��װ������"));
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
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("ж��ʧ�ܣ�SCManager����ʧ�ܡ�"));
    goto LABLE_FAILURE_IN_UninstallDriver;
  }

  const TCHAR *szDriveName = m_strDriverName;

  schService = OpenService(schSCManager, szDriveName, SERVICE_ALL_ACCESS);
  if (schService == NULL)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s%d"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("ж��ʧ�ܣ������ʧ�ܡ�"));
    goto LABLE_FAILURE_IN_UninstallDriver;
  }

  BOOL bRet = DeleteService(schService);
  if (!bRet)
  {
    m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
      m_strDriverName.GetBuffer(0), TEXT("ж��ʧ�ܡ������ԡ�"));
    goto LABLE_FAILURE_IN_UninstallDriver;
  }

  m_strFeedBackInfo.AppendFormat(TEXT("%s%s%s"), TEXT("����"),
    m_strDriverName.GetBuffer(0), TEXT("��ж�ء�"));

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
