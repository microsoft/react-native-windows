#include "pch.h"

#define DELAYIMP_INSECURE_WRITABLE_HOOKS
#include <delayimp.h>

static bool g_dliNotifyHookCalled = false;
// Depending on the flight, load either reac-native-win32.dll or reac-native-win32.chakra.dll
extern "C" FARPROC WINAPI ChakraDelayLoadHook(unsigned dliNotify, DelayLoadInfo *pdli) {
  // if (fLoadJsiChakra && dliNotify == dliNotePreLoadLibrary) {
  //  if (0 == ::_stricmp(pdli->szDll, "JSI.Desktop.ChakraCore.dll")) {
  //    return reinterpret_cast<FARPROC>(LoadLibrary(L"JSI.Desktop.Chakra.dll"));
  //  }
  //}

  (void)dliNotify;
  (void)pdli;

  switch (dliNotify)
  {
    case dliStartProcessing:
      break;

      case dliNotePreLoadLibrary:
      break;

      case dliNotePreGetProcAddress:
        break;

        case dliFailLoadLib:
        break;

        case dliFailGetProc:
          break;

          case dliNoteEndProcessing:
          break;
  }

  //return nullptr; // MsoDelayLoadHook(dliNotify, pdli);
  g_dliNotifyHookCalled = true;
  return 0;
}

// Install the handler for delay-loading mso binaries
//extern PfnDliHook
ExternC PfnDliHook __pfnDliNotifyHook2 = ChakraDelayLoadHook;


BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
  if (!__pfnDliNotifyHook2)
    //__pfnDliNotifyHook2 = ChakraDelayLoadHook;
    return FALSE;

  return TRUE;
}
