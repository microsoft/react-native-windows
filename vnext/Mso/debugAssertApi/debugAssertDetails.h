// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_DEBUGASSERTAPI_DEBUGASSERTDETAILS_H
#define MSO_DEBUGASSERTAPI_DEBUGASSERTDETAILS_H

/**
  Private implementation details for debug assert macros
  This header should not be included directly - only through debugAssertApi.h

   Note that C4706 is purposely left enabled. Assignments in inside Assert
  statements are almost always a bug (e.g. Assert(m_foo = fooBar))
*/

#include <cstdarg>
#include <cstdint>

/**
Mappings for core Debug asserts.
*/
#pragma warning(suppress : 4005) // macro redefinition :(
#define AssertTag(f, tag) \
  AssertAnnotatedSzNTagImpl(f, L## #f, InlineMsoAssertParams(#f, tag, __FILE__, __LINE__), "%s", #f)
#define AssertSzTag(f, sz, tag) \
  AssertSzNTagImpl(f, InlineMsoAssertParams(#f, tag, __FILE__, __LINE__), "%s", AssertDetails_SzCast(sz))
#define AssertSz1Tag(f, sz, a, tag) AssertSzNTagImpl(f, InlineMsoAssertParams(#f, tag, __FILE__, __LINE__), sz, a)
#define AssertSz2Tag(f, sz, a, b, tag) AssertSzNTagImpl(f, InlineMsoAssertParams(#f, tag, __FILE__, __LINE__), sz, a, b)
#define AssertSz3Tag(f, sz, a, b, c, tag) \
  AssertSzNTagImpl(f, InlineMsoAssertParams(#f, tag, __FILE__, __LINE__), sz, a, b, c)
#define AssertSz4Tag(f, sz, a, b, c, d, tag) \
  AssertSzNTagImpl(f, InlineMsoAssertParams(#f, tag, __FILE__, __LINE__), sz, a, b, c, d)
#define AssertSz5Tag(f, sz, a, b, c, d, e, tag) \
  AssertSzNTagImpl(f, InlineMsoAssertParams(#f, tag, __FILE__, __LINE__), sz, a, b, c, d, e)

#define FAssertDoTag(f, tag) FAssertDoSzNTagImpl(f, tag, __FILE__, __LINE__, L## #f, "%s", #f)
#define FAssertDoSzTag(f, sz, tag) \
  FAssertDoSzNTagImpl(f, tag, __FILE__, __LINE__, L##sz, "%s", AssertDetails_SzCast(sz))
#define FAssertDoSz1Tag(f, sz, a, tag) FAssertDoSzNTagImpl(f, tag, __FILE__, __LINE__, L##sz, sz, a)
#define FAssertDoSz2Tag(f, sz, a, b, tag) FAssertDoSzNTagImpl(f, tag, __FILE__, __LINE__, L##sz, sz, a, b)

/**
Return values from MsoAssertSzTagProc(Inline)(2)
*/
static const uint32_t c_assertIgnore = 0;
static const uint32_t c_assertDebugBreak = 1;
static const uint32_t c_assertAlwaysIgnore = 2;

#ifdef __cplusplus
#ifdef DEBUG
namespace Mso {
namespace DebugAsserts {

/**
  Return values from MsoAssertSzTagProc(Inline)(2)
*/
__pragma(warning(suppress : 4472)) enum class AssertResult : uint32_t {
  Ignore = c_assertIgnore,
  Break = c_assertDebugBreak,
  AlwaysIgnore = c_assertAlwaysIgnore,
};

} // namespace DebugAsserts
} // namespace Mso
#endif // DEBUG
#endif // C++

// TODO: move this abstraction into compilerAdapters? Except it depends on Windows - hmm
#if defined(__clang__) || defined(__cplusplus_cli) || defined(__INTELLISENSE__)
#define AssertBreak(wzMsg) __debugbreak()
#elif defined(_DBGRAISEASSERTIONFAILURE_)
#define AssertBreak(wzMsg) (__annotation(L"Debug", L"AssertFail", wzMsg), DbgRaiseAssertionFailure())
#else
#define AssertBreak(wzMsg) (__annotation(L"Debug", L"AssertFail", wzMsg), __debugbreak())
#endif

#if DEBUG
/**
  MsoAssertSzTagProcInline

  Converts variable arguments into the arg list format
*/
#ifndef __cplusplus
static
#endif
    __inline int32_t
    MsoAssertSzTagProcInline(MsoAssertParamsType params, _Printf_format_string_ const char *szFmt, ...) MSONOEXCEPT {
  va_list argList;
  va_start(argList, szFmt);
  AccessMsoAssertParams(params)->framesToSkip++;
  return MsoAssertSzTagProc(params, szFmt, argList);
}

#ifdef __cplusplus
/**
  MsoAssertSzTagProcInline2

  Converts variable arguments into the arg list format

  Use a lamba function to generate a unique function providing storage of
  _fIgnore_ (for in-proc based assert management) and unique break address.
*/
#define MsoAssertSzTagProcInline2(dwTag, szFile, iLine, wzAnnotation, szFmt, ...)                               \
  [&]() -> int32_t {                                                                                            \
    __pragma(warning(suppress : 4456)) /* declaration of '_fIgnore_' hides previous local declaration */        \
        static int32_t _fIgnore_ = false;                                                                       \
    if (!_fIgnore_) {                                                                                           \
      __pragma(warning(suppress : 4456)) /* declaration of 'params' hides previous local declaration */         \
          DeclareMsoAssertParams(dwTag, szFile, iLine);                                                         \
      params.framesToSkip++;                                                                                    \
      const int32_t _assertResult_ = MsoAssertSzTagProcInline(PassMsoAssertParams(params), szFmt, __VA_ARGS__); \
      __pragma(warning(suppress : 4700)) /* MSVC is unhappy with this used in a loop conditional */             \
          if (_assertResult_ == c_assertDebugBreak) {                                                           \
        AssertBreak(wzAnnotation);                                                                              \
      }                                                                                                         \
      _fIgnore_ = (_assertResult_ == c_assertAlwaysIgnore);                                                     \
    }                                                                                                           \
    return FALSE;                                                                                               \
  }()
#endif // __cplusplus

#else
#define MsoAssertSzTagProcInline(params, szFmt, ...) (0)
#endif // DEBUG

/**
  Hide the debug assert 'f' condition from OACR so it acts more like a ship build
*/
#if OACR
#define AssertDetails_ShouldRaiseDebugAssert(f, fIgnore) 0
#else
#define AssertDetails_ShouldRaiseDebugAssert(f, fIgnore) (!(f) && !(fIgnore))
#endif

#if DEBUG

/**
  AssertSzNTagImpl - all debug asserts funnel into here
  TODO: need to move the Ignore logic, should it be on params?
  Params is MsoAssertParams, only C callers need to do something weird.

  Note: be aware that ... requires at least one argument
*/
#define AssertSzNTagImpl(f, params, sz, ...) AssertAnnotatedSzNTagImpl(f, L## #f, params, sz, __VA_ARGS__)

#define AssertAnnotatedSzNTagImpl(f, wzAnnotation, params, sz, ...)                                        \
  AssertDetails_Statement_Begin static int32_t _fIgnore_ = 0;                                              \
  if (AssertDetails_ShouldRaiseDebugAssert(f, _fIgnore_)) {                                                \
    DeclareInlineMsoAssertParams(params);                                                                  \
    const int32_t _assertResult_ = MsoAssertSzTagProcInline(PassMsoAssertParams(params), sz, __VA_ARGS__); \
    if (_assertResult_ == c_assertDebugBreak)                                                              \
      AssertBreak(wzAnnotation);                                                                           \
    _fIgnore_ = (_assertResult_ == c_assertAlwaysIgnore);                                                  \
  }                                                                                                        \
  AssertDetails_Statement_End

#else

#define AssertSzNTagImpl(f, params, _sz, ...) __noop()
#define AssertAnnotatedSzNTagImpl(f, annotation, params, sz, ...) __noop()

#endif // DEBUG

#if DEBUG && !OACR // OACR can't understand FAssertDoSzNTagImpl

/**
  FAssertDoSzNTagImpl - all debug FAsserts funnel into here
  <condition> || <assertResult && false>

  TODO: need to move the Ignore logic, handle these differently?
  Note: be aware that ... requires at least one argument
*/

#define FAssertDoSzNTagImpl(f, dwTag, szFile, iLine, wzAnnotation, sz, ...) \
  ((f) || (MsoAssertSzTagProcInline2(dwTag, szFile, iLine, wzAnnotation, sz, __VA_ARGS__)))

#else

// REVIEW: (f) because sometimes f defines to nothing. But assuming here seems wrong.
#define FAssertDoSzNTagImpl(f, dwTag, szFile, iLine, wzAnnotation, _sz, ...) (f)

#endif // DEBUG && !OACR

// Note: AssertDo may contain assignments so I'm using the != 0 syntax which allows that.
#define AssertDoTag(f, tag) Statement(if ((f) == 0) { AssertSzTag(0, #f, tag); })
#define AssertDoSzTag(f, sz, tag) Statement(if ((f) == 0) { AssertSzTag(0, sz, tag); })
#define AssertDoSz1Tag(f, sz, a, tag) Statement(if ((f) == 0) { AssertSz1Tag(0, sz, a, tag); })
#define AssertDoSz2Tag(f, sz, a, b, tag) Statement(if ((f) == 0) { AssertSz2Tag(0, sz, a, b, tag); })
#define AssertDoSz3Tag(f, sz, a, b, c, tag) Statement(if ((f) == 0) { AssertSz3Tag(0, sz, a, b, c, tag); })
#define AssertDoSz4Tag(f, sz, a, b, c, d, tag) Statement(if ((f) == 0) { AssertSz4Tag(0, sz, a, b, c, d, tag); })
#define AssertDoSz5Tag(f, sz, a, b, c, d, e, tag) Statement(if ((f) == 0) { AssertSz5Tag(0, sz, a, b, c, d, e, tag); })

/**
  Helpers for debug HRESULT asserts.

  Ship versions are not supported; consider VerifySucceededElseCrash
*/
#if DEBUG

#define AssertDoSucceededTag(expr, tag)                                       \
  AssertDetails_Statement_Begin HRESULT _hr_ = (expr);                        \
  AssertAnnotatedSzNTagImpl(                                                  \
      SUCCEEDED(_hr_),                                                        \
      L#expr L" success. Check local variable _hr_.",                         \
      InlineMsoAssertParams("SUCCEEDED(" #expr ")", tag, __FILE__, __LINE__), \
      "%s failed with 0x%08x",                                                \
      #expr,                                                                  \
      _hr_);                                                                  \
  AssertDetails_Statement_End

#define FAssertDoSucceededTag(expr, tag)                                              \
  [&]() noexcept -> bool {                                                            \
    HRESULT _hr_ = (expr);                                                            \
    if (FAILED(_hr_)) {                                                               \
      if (MsoAssertSzTagProcInline(                                                   \
              InlineMsoAssertParams("SUCCEEDED(" #expr ")", tag, __FILE__, __LINE__), \
              "%s failed with 0x%08x",                                                \
              #expr,                                                                  \
              _hr_) == c_assertDebugBreak) {                                          \
        AssertBreak(L#expr L" success. Check local variable _hr_.");                  \
      }                                                                               \
    }                                                                                 \
    return SUCCEEDED(_hr_);                                                           \
    OACR_WARNING_SUPPRESS(NOEXCEPT_FUNC_THROWS, "Ignore whether expr throws.");       \
  }()

#else

#define AssertDoSucceededTag(expr, tag) ((void)(expr))
#define FAssertDoSucceededTag(expr, tag) (SUCCEEDED(expr))

#endif // DEBUG

/**
  Random other deprecated assert macros
*/
#define AssertImpliesTag(a, b, tag) AssertSzTag(FImplies(a, b), #a " => " #b, tag)
#define AssertBiImpliesTag(a, b, tag) AssertSzTag(FBiImplies(a, b), #a " <==> " #b, tag)

#endif // MSO_DEBUGASSERTAPI_DEBUGASSERTDETAILS_H
