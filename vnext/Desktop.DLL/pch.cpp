// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <delayimp.h>

FARPROC WINAPI ChakraDelayLoadHook(unsigned dliNotify, DelayLoadInfo *pdli) {
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

decltype(__pfnDliNotifyHook2) __pfnDliNotifyHook2 = ChakraDelayLoadHook;
