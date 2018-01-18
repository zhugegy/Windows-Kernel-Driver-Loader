#pragma once
#include "SingleInstanceMacro.h"

#include <Winsvc.h>
#include "atlstr.h"
#include <stdlib.h>
#include <stdio.h>

class CGYDriverManager
{
public:
  SINGLE_INSTANCE(CGYDriverManager)

private:
  CGYDriverManager();
  virtual ~CGYDriverManager();

private:
  TCHAR m_szDriverPath[MAX_PATH];
  CString     m_strDriverName;

  CString m_strFeedBackInfo;

  bool m_bIsDriverIntalled;
  bool m_bIsDriverStarted;

public:
  int SetDriverPath(const TCHAR *szDriverPath);
  int InstallDriver();
  int StartDriver();
  int StopDriver();
  int UninstallDriver();

  int FlushFeedBackInfo();
  int GetFeedBackInfo(CString& strGiveBackToUser);
};

