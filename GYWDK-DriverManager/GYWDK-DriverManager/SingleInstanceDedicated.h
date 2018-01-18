#pragma once
#include "SingleInstanceMacro.h"
#include "GYDriverManager.h"

class CSingleInstanceDedicated
{
public:
  CSingleInstanceDedicated()
  {
    CREATE_SINGLE(CGYDriverManager)
  }

  virtual ~CSingleInstanceDedicated()
  {
    RELEASE_SINGLE(CGYDriverManager)
  }
};

