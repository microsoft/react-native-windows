// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <RuntimeOptions.h>

// Windows API
#include <delayimp.h>

static bool flag = false;
FARPROC WINAPI ChakraDelayLoadHook(unsigned dliNotify, DelayLoadInfo *pdli) {
  switch (dliNotify) {
    case dliNotePreLoadLibrary:
      if (0 == lstrcmpA(pdli->szDll, "ChakraCore.dll") && Microsoft::React::GetRuntimeOptionBool("ForceSystemChakra")) {
        return reinterpret_cast<FARPROC>(LoadLibrary(L"Chakra.dll"));
      }
      break;

    case dliStartProcessing:
    case dliNotePreGetProcAddress:
    case dliFailLoadLib:
    case dliFailGetProc:
    case dliNoteEndProcessing:
      break;
  }

  return NULL;
}

decltype(__pfnDliNotifyHook2) __pfnDliNotifyHook2 = ChakraDelayLoadHook;
