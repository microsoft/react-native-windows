#include "pch.h"

#include "CoreApp.h"

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
#ifdef _DEBUG
  RNCoreAppStartFromConfigJson(L"app.config.json", nullptr, nullptr);
#else
  RNCoreAppStartFromConfigJson(L"app.config.release.json", nullptr, nullptr);
#endif
}
