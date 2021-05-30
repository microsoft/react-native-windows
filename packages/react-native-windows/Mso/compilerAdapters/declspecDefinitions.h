// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_COMPILERADAPTERS_DECLSPECDEFINITIONS_H
#define MSO_COMPILERADAPTERS_DECLSPECDEFINITIONS_H

/**
  Cross-compiler support for declspec things
  FUTURE: reconcile this with compilerFeatures.h
*/

#ifndef DECLSPEC_NORETURN
#if defined(__clang__) || defined(__GNUC__)
#define DECLSPEC_NORETURN __attribute__((noreturn))
#elif !defined(MIDL_PASS)
#define DECLSPEC_NORETURN __declspec(noreturn)
#else
#define DECLSPEC_NORETURN
#endif
#endif

#ifndef DECLSPEC_NOTHROW
#if defined(__clang__) || defined(__GNUC__)
#define DECLSPEC_NOTHROW __nothrow
#elif !defined(MIDL_PASS) && defined(__cplusplus)
#define DECLSPEC_NOTHROW __declspec(nothrow)
#else
#define DECLSPEC_NOTHROW
#endif
#endif

// Latest versions of clang supports this natively
#ifndef DECLSPEC_SELECTANY
#define DECLSPEC_SELECTANY __declspec(selectany)
#endif

// Mark the variable to be the first to be inited in the global init sequence.
// GCC/Clang does not have init_seg, rather has a priority 101-65535 where 101 is the highest
#ifndef DECLSPEC_INITFIRST
#if defined(__clang__) || defined(__GNUC__)
#define DECLSPEC_INITFIRST __attribute__((init_priority(101)))
#else
#define DECLSPEC_INITFIRST
#endif
#endif

#ifndef DECLSPEC_NOINLINE
#if defined(__clang__) || defined(__GNUC__)
// TODO: should be __attribute__((noinline)) but typedefs.h also defines this
#define DECLSPEC_NOINLINE
#else
#define DECLSPEC_NOINLINE __declspec(noinline)
#endif
#endif

#ifndef DECLSPEC_NOVTABLE
#if defined(__clang__) || !defined(__cplusplus) || defined(__GNUC__)
#define DECLSPEC_NOVTABLE
#else
#define DECLSPEC_NOVTABLE __declspec(novtable)
#endif
#endif

#ifndef DECLSPEC_DEPRECATED
#if defined(__clang__) || defined(__GNUC__)
#define DECLSPEC_DEPRECATED __attribute__((deprecated))
#elif !defined(MIDL_PASS)
#define DECLSPEC_DEPRECATED __declspec(deprecated)
#else
#define DECLSPEC_DEPRECATED
#endif
#endif

#endif // MSO_COMPILERADAPTERS_DECLSPECDEFINITIONS_H
