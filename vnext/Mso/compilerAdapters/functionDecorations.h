// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_COMPILERADAPTERS_FUNCTIONDECORATIONS_H
#define MSO_COMPILERADAPTERS_FUNCTIONDECORATIONS_H

/**
  Cross-platform/language macros to decorate APIs
*/

/**
  The Liblet::PublicApi attribute is used to mark a function, class or
  class method as part of your public API. These are automatically
  added to your export file and shim DLL.

  Note: Your liblet must be configured to process PublicApi.
  Note: PublicApi replaces all MSOAPI/MSOCPPAPI-style macros.
  Note: See http://aka.ms/cppdef

  Platform specific APIs are filtered using standard strings:
    win, win32, win32client, win32server, winrt, android

  Usage: an API is exported for all targets
    LIBLET_PUBLICAPI void MyApi() noexcept;

  Usage: exporting static methods on a class/interface
    struct MyInterface
    {
      LIBLET_PUBLICAPI static MyInterface* Create();
      ...
    };

  Usage: platform-specific APIs
    LIBLET_PUBLICAPI_EX("win32") void MyDesktop();
    LIBLET_PUBLICAPI_EX("winrt") void MyStore();
    LIBLET_PUBLICAPI_EX("win32", "android") void MyApi();

  Note: LIBLET_PUBLICAPI_EX(...) is not supported on Apple platforms,
    Use LIBLET_PUBLICAPI_APPLE instead.
    LIBLET_PUBLICAPI_EX("winrt") LIBLET_PUBLICAPI_APPLE void MyApi();

  Usage: export a whole class (heavily discouraged)
    struct LIBLET_PUBLICAPI MyExportedClass
    {
      // override default class export
      LIBLET_PUBLICAPI_EX("win32") void MyDesktop();

      void AlsoExported();
    };

  Usage: export special data types from a class (android + apple only)
    Values: "typeinfo", "typeinfoname", "vtable", "thunks"
    struct LIBLET_PUBLICAPI_CLASSDATA("typeinfo") MyClass
    {
    };

    struct LIBLET_PUBLICAPI_CLASSDATA("typeinfo", "vtable") MyClass
    {
    };
*/
#if defined(__clangpluginrunner__)
#define LIBLET_PUBLICAPI [[Liblet::PublicApi]]
#define LIBLET_PUBLICAPI_EX(...) [[Liblet::PublicApi(__VA_ARGS__)]]
#define LIBLET_PUBLICAPI_CLASSDATA(...) [[Liblet::PublicClassData(__VA_ARGS__)]]
#define LIBLET_PUBLICAPI_APPLE [[Liblet::PublicApi("apple")]]
#define LIBLET_PUBLICAPI_APPLE_DATA
#define LIBLET_PUBLICTYPE_APPLE
#define WEAK_SYMBOL
#elif defined(MS_TARGET_APPLE)
#define LIBLET_PUBLICAPI __attribute__((visibility("default")))
#define LIBLET_PUBLICAPI_EX(...)
#define LIBLET_PUBLICAPI_CLASSDATA(...) LIBLET_PUBLICTYPE_APPLE
#define LIBLET_PUBLICAPI_APPLE LIBLET_PUBLICAPI
#define LIBLET_PUBLICAPI_APPLE_DATA __attribute__((visibility("default")))
#define LIBLET_PUBLICTYPE_APPLE __attribute__((type_visibility("default")))
#define WEAK_SYMBOL __attribute__((weak))
#else
#define LIBLET_PUBLICAPI
#define LIBLET_PUBLICAPI_EX(...)
#define LIBLET_PUBLICAPI_CLASSDATA(...)
#define LIBLET_PUBLICAPI_APPLE
#define LIBLET_PUBLICTYPE_APPLE
#define LIBLET_PUBLICAPI_APPLE_DATA
#define WEAK_SYMBOL
#endif

/**
  The Liblet::MockName macro enables renaming of the generated mock names.
  This is useful when the function / interface method is overloaded.

  struct IBank
  {
    // Generate mock_TryWithdrawlUint32
    LIBLET_MOCKNAME("TryWithdrawlUint32")
    virtual bool TryWithdrawl(uint32_t amount) = 0;

    // Generate mock_TryWithdrawlFloat
    LIBLET_MOCKNAME("TryWithdrawlFloat")
    virtual bool TryWithdrawl(float amount) = 0;
  };
*/
#if defined(__clangpluginrunner__)
#define LIBLET_MOCKNAME(name) [[Liblet::MockName(name)]]
#else
#define LIBLET_MOCKNAME(name)
#endif

/**
  Macros for extern "C" that support C & C++
*/
#if defined(__cplusplus)
#define MSOEXTERN_C extern "C"
#define MSOEXTERN_C_BEGIN extern "C" {
#define MSOEXTERN_C_END }
#else
#define MSOEXTERN_C
#define MSOEXTERN_C_BEGIN
#define MSOEXTERN_C_END
#endif

/**
  Macro for constexpr
  FUTURE: VC14 supports constexpr
*/
#if defined(__cplusplus) && defined(__clang__)
#define MSOCONSTEXPR constexpr
#else
#define MSOCONSTEXPR
#endif

/**
  Macros to use nothrow/throw not on clang
*/
#if defined(__cplusplus) && !defined(__clang__)
/* SSS_WARNINGS_OFF */
#define MSONOTHROW throw()
/* SSS_WARNINGS_ON */
#else
#define MSONOTHROW
#endif // C++

/**
  Macro to enable noexcept only for C++
*/
#if defined(__cplusplus)
#define MSONOEXCEPT noexcept
#else
#define MSONOEXCEPT
#endif // C++

/**
  Macro to mark a method as const for C & C++
*/
#if defined(__cplusplus) && !defined(CINTERFACE)
#define CONST_METHOD const
#else
#define CONST_METHOD
#endif

/**
  Utility macro to make writing NonConst versions of functions easier.
  In C, you can't overload functions; thus the two flavors of a function will be
    DoSomething
    DoSomethingNonConst
  In C++, you can overload functions, so the two flavors of a function will have
  the same name (but with different arguments).

  To work around this, declare the second function as NON_CONST_VER(DoSomething)
  This also can be used when you explicitly want to call the nonConst version from
  an inline function that can be compiled as either C or C++
*/
#if defined(__cplusplus)
#define NON_CONST_VER(Function) Function
#else
#define NON_CONST_VER(Function) Function##NonConst
#endif // C++
#define NON_CONST_VER_DECL(Function) NON_CONST_VER(Function)

/**
  Macro to use __forceinline with cl.exe
*/
/* SSS_WARNINGS_OFF */
#if defined(__cplusplus) && !defined(__clang__)
#define MSOFORCEINLINE __forceinline
#else
#define MSOFORCEINLINE __inline
#endif
/* SSS_WARNINGS_ON */

#if defined(__clang__)
#define MSO_NO_INLINE __attribute__((noinline))
#else
#define MSO_NO_INLINE __declspec(noinline)
#endif

/**
  Clang forbids the use of static with friend.
  msvc issues warning C4211: nonstandard extension used: redefined extern to static without it
*/
#if defined(__clang__)
#define MSO_STATIC_FRIEND friend
#else
#define MSO_STATIC_FRIEND friend static
#endif

#endif // MSO_COMPILERADAPTERS_FUNCTIONDECORATIONS_H
