// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_PLATFORMADAPTERS_WINDOWSFIRST_H
#define MSO_PLATFORMADAPTERS_WINDOWSFIRST_H

#if defined(MS_TARGET_POSIX)
typedef unsigned short WORD;
typedef unsigned long DWORD;
#else

#ifndef NOMINMAX
#define NOMINMAX
#endif

//#include <winsock2.h>
#include <windows.h>

#endif // defined(MS_TARGET_POSIX)

#endif // MSO_PLATFORMADAPTERS_WINDOWSFIRST_H
