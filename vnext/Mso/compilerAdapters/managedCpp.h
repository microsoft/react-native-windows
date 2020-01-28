// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_COMPILERADAPTERS_MANAGEDCPP_H
#define MSO_COMPILERADAPTERS_MANAGEDCPP_H

/**
  Support for managed C++ compilation available only in VC++.
*/

/**
  VC++ specific pragmas to indicate that code must not be compiled as managed.
  Functions are compiled as managed by default when /clr is used.
  The pragmas below allow explicitly indicate that code is unmanaged, but these
  pragmas are not recognized by Clang compiler.
*/
#if !defined(__clang__)

#define MSO_PRAGMA_MANAGED_PUSH_OFF __pragma(managed(push, off))
#define MSO_PRAGMA_MANAGED_POP __pragma(managed(pop))

#else // !defined(__clang__)

#define MSO_PRAGMA_MANAGED_PUSH_OFF
#define MSO_PRAGMA_MANAGED_POP

#endif // !defined(__clang__)

#endif // MSO_COMPILERADAPTERS_MANAGEDCPP_H
