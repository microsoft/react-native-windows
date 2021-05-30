// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_OACR_OACR_H
#define MSO_OACR_OACR_H

/*****************************************************************************

   Module  : OACR

******************************************************************************

   Definitions and #defines for OACR support

*****************************************************************************/

// SAL annotations are #defined in this file
#include <sal.h>
#include <specstrings.h>
#include <stddef.h>

// Don't allow deprecated Pre Orcas style annotations
#undef OACR_DEPRECATED
#define OACR_DEPRECATED 0

#ifndef __oacr_noop
#define __oacr_noop __noop
#endif

#if defined(__cplusplus)
#define __extern_c extern "C"
#define __extern_cplus extern "C++"
#else
#define __extern_c
#define __extern_cplus
#endif

#ifndef __noreturn
#ifndef __clang__
#define __noreturn __declspec(noreturn)
#else
#define __noreturn __attribute__((noreturn))
#endif
#endif

// OACR keywords:
// annotations understood by PREfast plugins

#if !defined(_Sealed_method_)
// use _Sealed_method_ to specify C# style 'sealed' behavior for methods
#define _Sealed_method_ __oacr_sealed_method
#endif // _Sealed_method_

#if !defined(_Sealed_class_)
// use _Sealed_class_ to specify C# style 'sealed' behavior for classes
#define _Sealed_class_ __oacr_sealed_class
#endif // _Sealed_class_

#if !defined(_SA_deprecated_)
// use _SA_deprecated_ to mark functions or classes that should not be called any more
// pass the replacement function or class as an argument to the macro
#define _SA_deprecated_(NewFunctionName) __oacr_sa_deprecated(NewFunctionName)
#endif // _SA_deprecated_

#if (defined(OACR) && OACR)
// use OACR_MARK_CLASS_DEPRECATED to mark classes that should not be used any more
// this macro is a workaround for template classes.  Vanilla classes should use _SA_deprecated_
// pass the replacement class as argument to the macro
#define OACR_MARK_CLASS_DEPRECATED(NewFunctionName) _SA_deprecated_(NewFunctionName) void OACRMarkClass();
#else
#define OACR_MARK_CLASS_DEPRECATED(NewFunctionName)
#endif

#if !defined(_SA_deprecated_staging_)
// use _SA_deprecated_staging_ to mark functions or classes that should not be called any more
// pass the replacement function or class as an argument to the macro
// This variation allows for newly deprecated fuctions to be flagged at a potentially
// lower warning level before promotion to an error level
#define _SA_deprecated_staging_(NewFunctionName) __oacr_sa_deprecated_staging(NewFunctionName)
#endif // _SA_deprecated_staging_

#if (defined(OACR) && OACR)
// use OACR_MARK_CLASS_DEPRECATED_STAGING to mark classes that should not be used any more
// this macro is a workaround for template classes.  Vanilla classes should use _SA_deprecated_
// pass the replacement function as argument to the macro
#define OACR_MARK_CLASS_DEPRECATED_STAGING(NewFunctionName) \
  _SA_deprecated_staging_(NewFunctionName) void OACRMarkClass();
#else
#define OACR_MARK_CLASS_DEPRECATED_STAGING(NewFunctionName)
#endif

#if !defined(_Intl_deprecated_)
// annotation used by warning UNMARKED_INTL_DEPRECATED_FUNCTION (25114)
// to mark deprecated functions using LCID parameters
// The annotation is used by Office to cleanup the use of LCIDs
#define _Intl_deprecated_ __oacr_intl_deprecated
#endif // _Intl_deprecated_

#if !defined(_Oleo_deprecated_)
// Office specifix annotation used by the OLEO effort
#define _Oleo_deprecated_(NewFunctionName) __oacr_oleo_deprecated(NewFunctionName)
#endif // _Oleo_deprecated_

#if !defined(_Norefcapture_asynclambda_)
//
// use _Norefcapture_asynclambda_ on async function. This warns user of possible unsafe variable capture.
//
// Example:
//   void SubmitTimer(int x, int y, _Norefcapture_asynclambda_ Func& fn)
//
#define _Norefcapture_asynclambda_ __oacr_asynclambda
#endif // _Norefcapture_asynclambda_

#if !defined(_Asynclambda_)
//
// use _Asynclambda_ on async function. This warns user of possible unsafe variable capture.
//
// Example:
//   void SubmitTimer(int x, int y, _Asynclambda_ Func& fn)
//
#define _Asynclambda_ __oacr_asynclambda
#endif // _Asynclambda_

#if !defined(_Synclambda_)
//
// use _Synclambda_ indicate lambda is synchronous
//
// Example:
//   void Compute(int x, int y, _Synclambda_ Func& fn)
//
#define _Synclambda_ __oacr_synclambda
#endif // _Synclambda_

#if !defined(_Nodecl_lambda_)
// use _Nodecl_lambda_, this warns user about temporary lambda object declared on declaration.
//
// Example:
//   template <typename T>
//   _Nodecl_lambda_ FunctorRef(const T& func) noexcept { ... }
//
#define _Nodecl_lambda_ __oacr_nodecl_lambda
#endif // _Nodecl_lambda_

#if !defined(_Noexcept_maythrow_)
// _Noexcept_maythrow_ is deprecated use _Noexcept_mayterminate_.
#define _Noexcept_maythrow_ __oacr_noexcept_maythrow
#endif // _Noexcept_maythrow_

#if !defined(_Noexcept_mayterminate_)
// use _Noexcept_mayterminate_ on declaration of noexcept function when throw is expected.
//
// Example:
//   _Noexcept_mayterminate_ void Foo() noexcept;
#define _Noexcept_mayterminate_ __oacr_noexcept_mayterminate
#endif // _Noexcept_mayterminate_

#if !defined(_Noexcept_mayterminate_ignore_)
// use _Noexcept_mayterminate_ignore_ on declaration of noexcept function when you want to ignore function.
//
// Example:
//   _Noexcept_mayterminate_ignore_("what") void Foo() noexcept;
#define _Noexcept_mayterminate_ignore_(FunctionName) __oacr_noexcept_mayterminate_ignore(FunctionName)
#endif // _Noexcept_mayterminate_ignore_

#if !defined(_Dont_swap_)
// _Dont_swap_ allow for data member to be exempt for swap.
//
// Example:
//   _Dont_swap_ int m_foo;
#define _Dont_swap_ __oacr_dont_swap
#endif // _Dont_swap_

#if !defined(_RAII_)
// _RAII_ declare a class is RAII
//
// Example:
//   class _RAII_ CriticalSectionLocker {..}
#define _RAII_ __oacr_raii
#endif // _RAII_

#if !defined(_Rpc_)
// use _Rpc_ for functions that used as remote procedure calls, the keyword will silence
// various OACR checks on formal parameters of the marked functions
#define _Rpc_ __oacr_rpc
#endif // _Rpc_

#if !defined(_RequireNoThrow_)
// use _RequireNoThrow_ inside a compound statement to indicate that the rest of the block
// should not emit exceptions.
#define _RequireNoThrow_ __oacr_requireNoThrow
#endif // _RequireNoThrow_

#if !defined(_Canthrow_)
// use _Canthrow_ for method declarations of template classes that have throwing and non-throwing
// specializations to suppress warning FUNC_COULD_BE_NOTHROW (25307).
#define _Canthrow_ __oacr_canthrow
#endif // __requireNoThrow

#if !defined(_Genericfunctype_)
// use _Genericfunctype_ for function typedefs used for arrays of functions of different function types.
// if the typedef is marked as _Genericfunctype_, OACR will not generate DIFFERENT_CALLING_CONVENTION (25018) warnings
// e.g. typedef _Genericfunctype_ void (*FUNCPTR)();
#if (defined(OACR) && OACR)
#define _Genericfunctype_ __oacr_genericfunctype
#else
#define _Genericfunctype_
#endif
#endif // _Genericfunctype_

#if !defined(_Nothrowfunctype_)
// use _Nothrowfunctype_ for function typedefs of non exception throwing function pointers
// e.g. typedef _Nothrowfunctype_ void (*FUNCPTR)();
// only assign non throwing functions to typedefs so marked
#define _Nothrowfunctype_ __oacr_nothrowfunctype
#endif // _Nothrowfunctype_

#if !defined(_BindReturn_)
// stronger than __checkReturn, typically used by functions that return
// a pointer to an allocated object
// e.g _BindReturn_ void* malloc( size_t size );
// void Bar()
// {
//    void* pv;
//    if( ( pv == malloc( 20 ) ) == NULL )      <<<< typo !!
//    ...
// }
#define _BindReturn_ __oacr_bindReturn
#endif // _BindReturn_

#if !defined(_Memberinitializer_)
// use _Memberinitializer_ for init functions that initialize all members of a class
// e.g.:
// class X
// {
//    int m_i:
//    int m_j:
//    _Memberinitializer_ void Init(){ m_i = m_j = 0; }
// public:
//    X(){ Init(); }
// };
#define _Memberinitializer_ __oacr_memberinitializer
#endif // _Memberinitializer_

#if !defined(_Noheap_)
// use _Noheap_ classes that should not be instantiated on the heap
// e.g.:
// class _Noheap_ CriticalSection
// {
// public:
//    CriticalSection();
//    ~CriticalSection();
// };
#define _Noheap_ __oacr_noheap
#endif // _Noheap_

#if (defined(OACR) && OACR)
// use OACR_MARK_CLASS_NOHEAP to mark classes that should not be instantiated on the heap
// this macro is a workaround for template classes.  Vanilla classes should use _Noheap_
#define OACR_MARK_CLASS_NOHEAP _Noheap_ void OACRMarkClass();
#else
#define OACR_MARK_CLASS_NOHEAP
#endif

#if !defined(_Unsafe_string_api_)
// use _Unsafe_string_api_ to phase out functions that pass unbound writable buffers
// e.g.
// _Unsafe_string_api_ void MyStrCpy( char* szTo, const char* szFrom );
#define _Unsafe_string_api_ __oacr_unsafe_string_api
#endif // _Unsafe_string_api_

#if !defined(_Needsreview_)
// use _Needsreview_ to mark functions whose calls need to be reviewed for a
// special reason.
// e.g. a wrapper function to another function that needs to be reviewed
//_Needsreview_ __inline BOOL MsoGetStringTypeExW(LCID Locale, DWORD dwInfoType, LPCWSTR lpSrcStr, int cchSrc, LPWORD
// lpCharType)
//{
//   return OACR_REVIEWED_CALL("hannesr", GetStringTypeExW(Locale, dwInfoType, lpSrcStr, cchSrc, lpCharType));
//}
// use the OACR_REVIEWED_CALL macro to silent the warning after making sure that the function is used properly
#define _Needsreview_ __oacr_needsreview
#endif // _Needsreview_

#if !defined(_Notrunccast_)
// use _Notrunccast_ to find unsafe truncating cast on allocating functions taking
// a 16 bit size parameter
// e.g.
// void* MyMalloc( _Notrunccast_ ushort size );
#define _Notrunccast_ __oacr_notrunccast
#endif // _Notrunccast_

#if !defined(_Noinference_)
// keyword to disable (wrong) Hungarian inference of __count annotations
// inference is disabled for the all formals of the function
// e.g. _Noinference_ void FreePv( void* pv, size_t cb );
#define _Noinference_ __oacr_noinference
#endif // _Noinference_

#if !defined(_Allow_implicit_ctor_)
// use _Allow_implicit_ctor_ when a constructors is expected to be implicit.
//
// e.g.
//
//  _Allow_implicit_ctor_ CPoint( const POINT& );
//
#define _Allow_implicit_ctor_ __oacr_allow_implicit_ctor
#endif // _Allow_implicit_ctor_

// Pre Orcas style annotations are deprecated
#if (OACR_DEPRECATED)

#if !defined(__sealed_method)
#define __sealed_method _Sealed_method_
#endif
#if !defined(__sealed_class)
#define __sealed_class _Sealed_class_
#endif
#if !defined(__sa_deprecated)
#define __sa_deprecated(NewFunctionName) _SA_deprecated_(NewFunctionName)
#endif
#if !defined(__intl_deprecated)
#define __intl_deprecated _Intl_deprecated_
#endif
#if !defined(__oleo_deprecated)
#define __oleo_deprecated(NewFunctionName) _Oleo_deprecated_(NewFunctionName)
#endif
#if !defined(__rpc)
#define __rpc _Rpc_
#endif
#if !defined(__requireNoThrow)
#define __requireNoThrow _RequireNoThrow_
#endif
#if !defined(__canthrow)
#define __canthrow _Canthrow_
#endif
#if !defined(__genericfunctype)
#define __genericfunctype _Genericfunctype_
#endif
#if !defined(__nothrowfunctype)
#define __nothrowfunctype _Nothrowfunctype_
#endif
#if !defined(__bindReturn)
#define __bindReturn _BindReturn_
#endif
#if !defined(__memberinitializer)
#define __memberinitializer _Memberinitializer_
#endif
#if !defined(__noheap)
#define __noheap _Noheap_
#endif
#if !defined(__nostack)
#define __nostack // not supported
#endif
#if !defined(__unsafe_string_api)
#define __unsafe_string_api _Unsafe_string_api_
#endif
#if !defined(__needsreview)
#define __needsreview _Needsreview_
#endif
#if !defined(__notrunccast)
#define __notrunccast _Notrunccast_
#endif
#if !defined(__noinference)
#define __noinference _Noinference_
#endif
#if !defined(__min_function)
#define __min_function // deprecated, use range annotations
#endif
#if !defined(__max_function)
#define __max_function // deprecated, use range annotations
#endif
#if !defined(__printf_format_string)
#define __printf_format_string _Printf_format_string_
#endif

#undef __callback
#define __callback _Callback_

#if !defined(__sa_sealed) // use __sealed_method and __sealed_class instead
#if (defined(OACR) && OACR)
#define __sa_sealed __declspec("_Sealed_")
#else
#define __sa_sealed
#endif
#endif // __sa_sealed

#endif // OACR_DEPRECATED

//-----------------

// OACR helper macros to suppress particular warnings

#if !defined(OACR_USE_PTR)
// use to suppress constness and related warnings:
// NONCONST_LOCAL (25003), NONCONST_PARAM( 25004), NONCONST_FUNCTION (25005),
// NONCONST_LOCAL_BUFFERPTR (25032), NONCONST_BUFFER_PARAM (25033)
#if (defined(OACR) && OACR)
__extern_c void OACRUsePtr(void *p);
#define OACR_USE_PTR(p) OACRUsePtr(p)
#elif defined(__clang__)
#define OACR_USE_PTR(p) __oacr_noop()
#else
#define OACR_USE_PTR(p) (p)
#endif
#endif // OACR_USE_PTR

#if !defined(RC_INVOKED) // Identifier is too long
#if !defined(OACR_MEMBER_IS_INITIALIZED_IN_CTOR)
// Use to suppress warning UNINITIALIZED_DATAMEMBER (25070) (as issued from a _Memberinitializer_)
// for data members which are correctly initialized in the ctor.
// (Warning 25070 cannot automatically handle member initialization that
// is spread across both ctor initializer list and a _Memberinitializer_ method).
// NOTES: This is functionally to OACR_USE_PTR(&m).
//        Contrast with OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER.
#define OACR_MEMBER_IS_INITIALIZED_IN_CTOR(m) OACR_USE_PTR(&m)
#endif // OACR_MEMBER_IS_INITIALIZED_IN_CTOR

#if !defined(OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER)
// Use to suppress warning UNINITIALIZED_DATAMEMBER (25070) (as issued from a ctor)
// for data members which are correctly initialized in the _Memberinitializer_ method.
// (Warning 25070 cannot automatically handle member initialization that
// is spread across both ctor initializer list and a _Memberinitializer_ method).
// NOTES: This is functionally equivalent to OACR_USE_PTR(&m).
//        Contrast with OACR_MEMBER_IS_INITIALIZED_IN_CTOR.
#define OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER(m) OACR_USE_PTR(&m)
#endif // OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER
#endif // !RC_INVOKED

#if !defined(OACR_POSSIBLE_THROW)
// use to suppress __nothrow related warnings NOTHROW_FUNC_THROWS (25306) and FUNC_COULD_BE_NOTHROW (25307)
#if (defined(OACR) && OACR)
__extern_cplus void OACRPossibleThrow();
#define OACR_POSSIBLE_THROW OACRPossibleThrow()
#else
#define OACR_POSSIBLE_THROW __oacr_noop()
#endif
#endif // OACR_POSSIBLE_THROW

#if !defined(OACR_ASSUME_NOTHROW_BEGIN)
// use with OACR_ASSUME_NOTHROW_END to suppress NOTHROW_FUNC_THROWS warnings around functions that don't throw in this
// context macro pair needs to be on the same scope
#if (defined(OACR) && OACR)
__extern_c __noreturn void OACRNoReturn();
#define OACR_ASSUME_NOTHROW_BEGIN try {
#define OACR_ASSUME_NOTHROW_END \
  }                             \
  catch (...) {                 \
    OACRNoReturn();             \
  }
#else
#define OACR_ASSUME_NOTHROW_BEGIN
#define OACR_ASSUME_NOTHROW_END
#endif
#endif // OACR_POSSIBLE_THROW

#if !defined(UNREFERENCED_OACR)
// use to function staticness and related warnings: STATIC_FUNCTION (25007)
#if (defined(OACR) && OACR)
#define UNREFERENCED_OACR(p) __assume(p == p)
#else
#define UNREFERENCED_OACR(p)
#endif
#endif // UNREFERENCED_OACR

#if !defined(OACR_OWN_PTR)
// can be used for objects that attach themselves to an owner
// in their constructors
#if (defined(OACR) && OACR)
__extern_c void OACROwnPtr(const void *p);
#define OACR_OWN_PTR(p) OACROwnPtr(p)
#else
#define OACR_OWN_PTR(p) __oacr_noop()
#endif
#endif // OACR_OWN_PTR

#if !defined(OACR_PTR_NOT_NULL)
// tells OACR that a pointer is not null at this point
#if (defined(OACR) && OACR)
#define OACR_PTR_NOT_NULL(p) OACR_ASSUME(0 != p)
#else
#define OACR_PTR_NOT_NULL(p) __oacr_noop()
#endif
#endif // OACR_PTR_NOT_NULL

#if !defined(OACR_NOT_IMPLEMENTED_MEMBER)
#if (defined(OACR) && OACR)
#define OACR_NOT_IMPLEMENTED_MEMBER OACR_USE_PTR((void *)this)
#else
#define OACR_NOT_IMPLEMENTED_MEMBER
#endif
#endif // OACR_NOT_IMPLEMENTED_MEMBER

#if !defined(OACR_DECLARE_FILLER)
#if (defined(OACR) && OACR)
#define OACR_DECLARE_FILLER(type, inst) type __filler##inst;
#else
#define OACR_DECLARE_FILLER(type, inst)
#endif
#endif // OACR_DECLARE_FILLER

// use this macro once you have inspected warnings FUNCTION_NEEDS_REVIEW (25028)
#if !defined(OACR_REVIEWED_CALL)
#if (defined(OACR) && OACR)
__extern_c void __OACRReviewedCall();
#define OACR_REVIEWED_CALL(reviewer, functionCall) (__OACRReviewedCall(), functionCall)
#else
#define OACR_REVIEWED_CALL(reviewer, functionCall) functionCall
#endif
#endif // OACR_REVIEWED_CALL

// use this macro once you have inspected warnings URL_NEEDS_TO_BE_REVIEWED (25085)
#if !defined(OACR_REVIEWED_URL)
#if (defined(OACR) && OACR)
__extern_c void __OACRReviewedUrl();
#define OACR_REVIEWED_URL(reviewer, reviewedUrl) (__OACRReviewedUrl(), reviewedUrl)
#else
#define OACR_REVIEWED_URL(reviewer, reviewedUrl) reviewedUrl
#endif
#endif // OACR_REVIEWED_URL

#if !defined(OACR_DONT_SWAP)
// use to suppress warnings MISSING_MEMBER_SWAP (25146) for nonswappable data members like refcounts, critical sections,
// etc.
#if (defined(OACR) && OACR)
__extern_c void OACRDontSwap(void *p);
#define OACR_DONT_SWAP(m) OACRDontSwap(&(m))
#else
#define OACR_DONT_SWAP(m) __oacr_noop()
#endif
#endif // OACR_DONT_SWAP

#if (defined(OACR) && OACR && defined(_WINDEF_) && 0)

// redefine FALSE & TRUE for better HRESULT<->BOOL conversion detection
#if defined(FALSE)
#undef FALSE
#define FALSE ((BOOL)0)
#endif

#if defined(TRUE)
#undef TRUE
#define TRUE ((BOOL)1)
#endif

#endif

// Use the following macros to suppress and disable OACR warnings in the code

// macro to tell OACR to not issue a specific warning for the following line of code
// use to suppress false positives from OACR
// e.g.
// if( fPointerNotNull )
//    OACR_WARNING_SUPPRESS( DEREF_NULL_PTR, "pointer access is guarded by 'fPointerNotNull'" )
//    p->Foo();

#if !defined(OACR_WARNING_PUSH)
#if (defined(OACR) && OACR && !defined(OACR_NO_WARNING_MACROS))
#if (1400 <= _MSC_VER)
#define OACR_WARNING_PUSH __pragma(warning(push))
#else
#define OACR_WARNING_PUSH __pragma(prefast(push))
#endif
#else
#define OACR_WARNING_PUSH
#endif
#endif

#if !defined(OACR_WARNING_POP)
#if (defined(OACR) && OACR && !defined(OACR_NO_WARNING_MACROS))
#if (1400 <= _MSC_VER)
#define OACR_WARNING_POP __pragma(warning(pop))
#else
#define OACR_WARNING_POP __pragma(prefast(pop))
#endif
#else
#define OACR_WARNING_POP
#endif
#endif

#if !defined(OACR_WARNING_ENABLE)
#if (defined(OACR) && OACR && !defined(OACR_NO_WARNING_MACROS))
#if (1400 <= _MSC_VER)
#define OACR_WARNING_ENABLE(cWarning, comment) __pragma(warning(default : __WARNING_##cWarning))
#else
#define OACR_WARNING_ENABLE(cWarning, comment)
#endif
#else
#define OACR_WARNING_ENABLE(cWarning, comment)
#endif
#endif

#if !defined(OACR_WARNING_DISABLE)
#if (defined(OACR) && OACR && !defined(OACR_NO_WARNING_MACROS))
#if (1400 <= _MSC_VER)
#define OACR_WARNING_DISABLE(cWarning, comment) __pragma(warning(disable : __WARNING_##cWarning))
#else
#define OACR_WARNING_DISABLE(cWarning, comment) __pragma(prefast(disable : __WARNING_##cWarning, comment))
#endif
#else
#define OACR_WARNING_DISABLE(cWarning, comment)
#endif
#endif

#if !defined(OACR_WARNING_SUPPRESS)
#if (defined(OACR) && OACR && !defined(OACR_NO_WARNING_MACROS))
#if (1400 <= _MSC_VER)
#define OACR_WARNING_SUPPRESS(cWarning, comment) __pragma(warning(suppress : __WARNING_##cWarning))
#else
#define OACR_WARNING_SUPPRESS(cWarning, comment) __pragma(prefast(suppress : __WARNING_##cWarning, comment))
#endif
#else
#define OACR_WARNING_SUPPRESS(cWarning, comment)
#endif
#endif

// macro to tell OACR about conditions that are true.
// Use OACR_ASSUME instead of #pragma prefast(suppress,...) if possible
// e.g.:
// if( g_pRoot == NULL ) // global variable g_pRoot
// {
//    InitRoot();
//    OACR_ASSUME( NULL != g_pRoot );
// }
// g_pRoot->Traverse();    // without OACR_ASSUME this would cause warning 11
#if (defined(OACR) && OACR)
#define OACR_ASSUME(fCondition) __assume(fCondition)

#elif defined(Assert)
#define OACR_ASSUME(fCondition) Assert(fCondition)

#else
#define OACR_ASSUME(fCondition) __oacr_noop()

#endif

// macro to tell OACR that a string is null terminated at this point of execution
#if (defined(OACR) && OACR && defined(_Post_z_))
__extern_c void __OACRAssumeNullterminated(_Post_z_ const char *sz);
#define OACR_ASSUME_NULLTERMINATED(string) __OACRAssumeNullterminated((const char *)string)

#else
#define OACR_ASSUME_NULLTERMINATED(string) __oacr_noop()

#endif

// macro to tell OACR that a pointer is null valid at this point of execution
#if (defined(OACR) && OACR && defined(_Post_valid_))
__extern_c void __OACRAssumeValid(_Post_valid_ const void *pv);
#define OACR_ASSUME_VALID(ptr) __OACRAssumeValid(ptr)

#else
#define OACR_ASSUME_VALID(ptr) __oacr_noop()

#endif

// macro to tell OACR that a buffer has a certain readable extent at this point of execution
// it can be used to silent noisy espX INCORRECT_ANNOTATION warnings
#if (defined(OACR) && OACR && defined(_Post_bytecount_))
__extern_c void __OACRAssumeByteCount(_Post_bytecount_(cb) const void *pv, size_t cb);
#define OACR_ASSUME_BYTECOUNT(pv, cb) __OACRAssumeByteCount(pv, cb)

#else
#define OACR_ASSUME_BYTECOUNT(pv, cb) __oacr_noop()

#endif

// macro indicate lambda mayterminate
#if (defined(OACR) && OACR && defined(__cplusplus))
void _lambda_noexcept_mayterminate_() noexcept {};
#define OACR_LAMBDA_NOEXCEPT_MAYTERMINATE _lambda_noexcept_mayterminate_()
#define OACR_NOEXCEPT_MAYTERMINATE _lambda_noexcept_mayterminate_()
#else
#define OACR_LAMBDA_NOEXCEPT_MAYTERMINATE __oacr_noop
#define OACR_NOEXCEPT_MAYTERMINATE __oacr_noop
#endif

// macro indicate mayterminate ignore stl
#if (defined(OACR) && OACR && defined(__cplusplus))
void _noexcept_mayterminate_ignore_stl_() noexcept {};
#define OACR_NOEXCEPT_MAYTERMINATE_IGNORE_STL _noexcept_mayterminate_ignore_stl_()
#else
#define OACR_NOEXCEPT_MAYTERMINATE_IGNORE_STL __oacr_noop
#endif

// macro indicate mayterminate ignore
#if (defined(OACR) && OACR && defined(__cplusplus))
__extern_c void _noexcept_mayterminate_ignore_(const char *) noexcept;
#define OACR_NOEXCEPT_MAYTERMINATE_IGNORE(funcs) _noexcept_mayterminate_ignore_(funcs)
#else
#define OACR_NOEXCEPT_MAYTERMINATE_IGNORE(funcs) __oacr_noop
#endif

// OACR custom plugin specific extensions

//=======================================================================

#define _Callback_ __oacr_callback

//======================================================================
// OACR custom attributes

// OACR custom plugin specific extensions

#if (_USE_DECLSPECS_FOR_SAL)

#ifndef _SA_SPECSTRIZE
#define _SA_SPECSTRIZE(x) #x
#endif

__ANNOTATION(_Callback_(void));
__ANNOTATION(_Rpc_(void));
__ANNOTATION(_Sealed_(void));
__ANNOTATION(_Memberinitializer_(void));
__ANNOTATION(_Unsafe_string_api_(void));
__ANNOTATION(_Needsreview_(void));
__ANNOTATION(_Intl_deprecated_(void));
__ANNOTATION(_Noinference_(void));
__ANNOTATION(_Canthrow_(void));
__ANNOTATION(_BindReturn_(void));
__ANNOTATION(_SA_deprecated_(__AuToQuOtE char *));
__ANNOTATION(_Oleo_deprecated_(__AuToQuOtE char *));
__ANNOTATION(_Genericfunctype_(void));
__ANNOTATION(__nothrowfunctype(void));
__ANNOTATION(_Noheap_(void));
__ANNOTATION(_Notrunccast_(void));
__ANNOTATION(_Sealed_(void));
__ANNOTATION(_Allow_implicit_ctor_(void));
__ANNOTATION(_Norefcapture_asynclambda_(void));
__ANNOTATION(_Synclambda_(void));
__ANNOTATION(_Nodecl_lambda_(void));
__ANNOTATION(_Noexcept_maythrow_(void));
__ANNOTATION(_Noexcept_mayterminate_(void));
__ANNOTATION(_Noexcept_mayterminate_ignore_(__AuToQuOtE char *));
__ANNOTATION(_Dont_swap_(void));
__ANNOTATION(_RAII_(void));

#define __oacr_callback __declspec("_Callback_")
#define __oacr_rpc __declspec("_Rpc_")
#define __oacr_sealed_method __declspec("_Sealed_")
#define __oacr_memberinitializer __declspec("_Memberinitializer_")
#define __oacr_unsafe_string_api __declspec("_Unsafe_string_api_")
#define __oacr_needsreview __declspec("_Needsreview_")
#define __oacr_intl_deprecated __declspec("_Intl_deprecated_")
#define __oacr_noinference __declspec("_Noinference_")
#define __oacr_canthrow __declspec("_Canthrow_")
#define __oacr_bindReturn __declspec("_BindReturn_")
#define __oacr_sa_deprecated(NewFunctionName) __declspec("_SA_deprecated_("_SA_SPECSTRIZE(NewFunctionName) ")")
#define __oacr_sa_deprecated_staging(NewFunctionName) \
  __declspec("_SA_deprecated_staging_("_SA_SPECSTRIZE(NewFunctionName) ")")
#define __oacr_oleo_deprecated(NewFunctionName) __declspec("_Oleo_deprecated_("_SA_SPECSTRIZE(NewFunctionName) ")")
#define __oacr_genericfunctype __declspec("_Genericfunctype_")
#define __oacr_nothrowfunctype __declspec("__nothrowfunctype")
#define __oacr_noheap __declspec("_Noheap_")
#define __oacr_notrunccast __declspec("_Notrunccast_")
#define __oacr_sealed_class __declspec("_Sealed_")
#define __oacr_allow_implicit_ctor __declspec("_Allow_implicit_ctor_")
#define __oacr_asynclambda __declspec("_Norefcapture_asynclambda_")
#define __oacr_synclambda __declspec("_Synclambda_")
#define __oacr_nodecl_lambda __declspec("_Nodecl_lambda_")
#define __oacr_noexcept_maythrow __declspec("_Noexcept_maythrow_")
#define __oacr_noexcept_mayterminate __declspec("_Noexcept_mayterminate_")
#define __oacr_noexcept_mayterminate_ignore(FunctionName) \
  __declspec("_Noexcept_mayterminate_ignore_("_SA_SPECSTRIZE(FunctionName) ")")
#define __oacr_dont_swap __declspec("_Dont_swap_")
#define __oacr_raii __declspec("_RAII_")

__extern_c int __RequireNoThrow();
#define __oacr_requireNoThrow __RequireNoThrow();

#else // not __declspecs

#define __oacr_callback
#define __oacr_rpc
#define __oacr_sealed_method
#define __oacr_memberinitializer
#define __oacr_unsafe_string_api
#define __oacr_needsreview
#define __oacr_intl_deprecated
#define __oacr_noinference
#define __oacr_canthrow
#define __oacr_bindReturn
#define __oacr_sa_deprecated(NewFunctionName)
#define __oacr_sa_deprecated_staging(NewFunctionName)
#define __oacr_oleo_deprecated(NewFunctionName)
#define __oacr_genericfunctype
#define __oacr_nothrowfunctype
#define __oacr_noheap
#define __oacr_notrunccast
#define __oacr_sealed_class
#define __oacr_requireNoThrow
#define __oacr_allow_implicit_ctor
#define __oacr_asynclambda
#define __oacr_synclambda
#define __oacr_nodecl_lambda
#define __oacr_noexcept_maythrow
#define __oacr_noexcept_mayterminate
#define __oacr_noexcept_mayterminate_ignore(FunctionName)
#define __oacr_dont_swap
#define __oacr_raii

#endif

#endif // MSO_OACR_OACR_H
