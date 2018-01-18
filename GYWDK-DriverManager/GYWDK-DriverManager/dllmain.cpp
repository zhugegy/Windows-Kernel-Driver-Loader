// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "atlstr.h"
#include "SingleInstanceMacro.h"
#include "GYDriverManager.h"
#include "SingleInstanceDedicated.h"

CSingleInstanceDedicated DoNotUse;  //单例类辅助对象，不要使用

extern "C" {
  __declspec(dllexport) int GYWDKSetDllPath(const TCHAR *szDriverPath, CString& strFeedBack);
  __declspec(dllexport) int GYWDKOperation(int nOperationCode, CString& strFeedBack);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

int GYWDKSetDllPath(const TCHAR *szDriverPath, CString& strFeedBack)
{
  CGYDriverManager* pManager = GET_SINGLE(CGYDriverManager);

  pManager->FlushFeedBackInfo();

  int nRet = pManager->SetDriverPath(szDriverPath);
  pManager->GetFeedBackInfo(strFeedBack);

  return nRet;
}

int GYWDKOperation(int nOperationCode, CString& strFeedBack)
{
  CGYDriverManager* pManager = GET_SINGLE(CGYDriverManager);
  pManager->FlushFeedBackInfo();

  int nRet = -1;

  switch (nOperationCode)
  {
  case 0:
    nRet = pManager->InstallDriver();
    break;
  case 1:
    nRet = pManager->StartDriver();
    break;
  case 2:
    nRet = pManager->StopDriver();
    break;
  case 3:
    nRet = pManager->UninstallDriver();
    break;
  default:
    break;
  }

  pManager->GetFeedBackInfo(strFeedBack);

  return nRet;
}