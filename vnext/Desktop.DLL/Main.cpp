#include "pch.h"

#define DELAYIMP_INSECURE_WRITABLE_HOOKS
#include <delayimp.h>

ExternC FARPROC WINAPI ChakraDelayLoadHook(unsigned dliNotify, DelayLoadInfo *pdli) {
  switch (dliNotify) {
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

  return NULL;
}

// Install the handler for delay-loading binaries
ExternC PfnDliHook __pfnDliNotifyHook2 = ChakraDelayLoadHook;
ExternC PfnDliHook __pfnDliFailureHook2 = ChakraDelayLoadHook;

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved) {
  if (!__pfnDliNotifyHook2)
    //__pfnDliNotifyHook2 = ChakraDelayLoadHook;
    return FALSE;

  return TRUE;
}
