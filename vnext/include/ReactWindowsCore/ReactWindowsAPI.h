// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#if defined(REACTWINDOWS_STATIC)
#define REACTWINDOWS_EXPORT
#elif defined(REACTWINDOWS_BUILD)
#define REACTWINDOWS_EXPORT __declspec(dllexport)
#else
#define REACTWINDOWS_EXPORT __declspec(dllimport)
#endif

#define REACTWINDOWS_API_(t) REACTWINDOWS_EXPORT t __stdcall
