// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_COMPILERADAPTERS_COMPILERFEATURES_H
#define MSO_COMPILERADAPTERS_COMPILERFEATURES_H

//=============================================================================
// Detect whether compiler features are present
//=============================================================================

// Check if Microsoft's Structured Exception Handling (SEH) are supported.
#ifndef MSO_HAS_SEH
#if defined(_MSC_VER) || defined(__BORLANDC__)
#define MSO_HAS_SEH 1
#else
#define MSO_HAS_SEH 0
#endif
#endif // MSO_HAS_SEH

// Check if compiler supports UUID
#ifndef COMPILER_SUPPORTS_UUID
#if defined(__clang__) || defined(__GNUC__)
#define COMPILER_SUPPORTS_UUID 0
#elif defined(_MSC_VER)
#define COMPILER_SUPPORTS_UUID 1
#else
#error Unsupported compiler type.
#endif
#endif

#endif // MSO_COMPILERADAPTERS_COMPILERFEATURES_H
