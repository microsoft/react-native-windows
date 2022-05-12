#include "pch.h"
#include "CoreApp.h"

int __stdcall wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR /*server*/, _In_ int /*showCommand*/) {
  RNStartCoreAppFromConfigJson(L"app.config.json");
}
