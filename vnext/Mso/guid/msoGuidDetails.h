// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_GUID_MSOGUIDDETAILS_H
#define MSO_GUID_MSOGUIDDETAILS_H

/****************************************************************************
Core definitions to support multi-platform GUID implementation.
This file is intended to be used by msoGuid.h and guiddef.h files only.
See the msoGuid.h for the usage guidelines.
****************************************************************************/

#ifndef __cplusplus
#error Must be included in C++ context only.
#endif

// Clang compiler must target C++11 or up to support MSO*GUIDs because we rely on constexpr.
//#if (__cplusplus >= 201103L) && !defined(MSO_GUID_DISABLED)

#include "compilerAdapters/compilerFeatures.h"
#if MS_TARGET_POSIX
#include <guiddef.h>
#endif // MS_TARGET_POSIX

// Private macro for MSO_STRUCT_GUID/MSO_CLASS_GUID macro implementations.
#if COMPILER_SUPPORTS_UUID // For Visual C++
#define MSO_GUID_IMPL(keyword, type, guidString)                                               \
  static_assert(                                                                               \
      sizeof(guidString) == 37,                                                                \
      "GUID string must have the following format: \"XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX\""); \
  keyword __declspec(uuid(guidString)) type;                                                   \
  __pragma(warning(suppress : 25352)) extern "C++" ::Mso::Details::GuidUtils::TrueType MsoTypeHasGuid(type *);
#else // For Clang
#define MSO_GUID_IMPL(keyword, type, guidString)                   \
  keyword type;                                                    \
  extern "C++" inline constexpr GUID MsoGetGuid(type *) noexcept { \
    return ::Mso::Details::GuidUtils::StringToGuid(guidString);    \
  }                                                                \
  extern "C++" ::Mso::Details::GuidUtils::TrueType MsoTypeHasGuid(type *);
#endif // COMPILER_SUPPORTS_UUID

/// Associates type with a GUID. To be used before type declaration.
#define MSO_STRUCT_GUID(type, guidString) MSO_GUID_IMPL(struct, type, guidString)
#define MSO_CLASS_GUID(type, guidString) MSO_GUID_IMPL(class, type, guidString)
#define MSO_GUID(type, guidString) MSO_STRUCT_GUID(type, guidString)

//==========================================================================================================================
// GUID token is a special type that can be used to define and access GUIDs using only header files.
// For example, to define a GUID in a header file without declaring a type, you can write:
//
// MSO_DEFINE_GUID_TOKEN(MyToken, "d807b970-b875-4491-a88e-99038236d60f")
//
// Then to access the GUID:
//
// auto myGuid = __uuidof_token(MyToken);
//
// The MSO_GET_GUID_TOKEN_TYPE macro can be used in some more advanced scenarios.
// For example, when we want to associate a GUID with a class template:
//
// template <class T, class TGuidToken>
// struct MyTemplate {
//     using GuidType = TGuidToken;
//
//     static bool IsMyGuid(const GUID& guid) noexcept {
//         return __uuidof(GuidType) == guid;
//     }
// };
//
// Then we can define the type aliases as the following:
//
// MSO_DEFINE_GUID_TOKEN(MyIntTemplate, "edf68ab1-90c3-4dfe-83c6-0044455626b3")
// using MyIntTemplate = MyTemplate<int, MSO_GET_GUID_TOKEN_TYPE(MyIntTemplate)>;
//
// MSO_DEFINE_GUID_TOKEN(MyFloatTemplate, "bd2ce2ad-2926-4229-8abd-c333531b99db")
// using MyFloatTemplate = MyTemplate<int, MSO_GET_GUID_TOKEN_TYPE(MyFloatTemplate)>;
//
// Each alias will have its own unique GUID associated with it.
// To get GUID of an alias:
//
// const GUID& intGuid = __uuidof(MyIntTemplate::GuidType);
// const GUID& floatGuid = __uuidof(MyFloatTemplate::GuidType);
//

/// Gets type from the GUID token.
#define MSO_GET_GUID_TOKEN_TYPE(guidToken) guidToken##_GuidToken

/// Defines a GUID token type.
#define MSO_DEFINE_GUID_TOKEN(guidToken, guidString) MSO_STRUCT_GUID(MSO_GET_GUID_TOKEN_TYPE(guidToken), guidString)

/// A macro to access GUID by a token defined by MSO_DEFINE_GUID_TOKEN
#define __uuidof_token(guidToken) __uuidof(MSO_GET_GUID_TOKEN_TYPE(guidToken))

//==========================================================================================================================

/// Replacement for the MIDL_INTERFACE based on MSO_STRUCT_GUID. To be used in MIDL generated files.
#define MIDL_INTERFACE2(type, guidString) \
  MSO_STRUCT_GUID(type, guidString)       \
  struct DECLSPEC_NOVTABLE

// __uuidof(type) implementation for Clang. In VC++ we use the native __uuidof() operator.
#if !COMPILER_SUPPORTS_UUID
#undef __uuidof
#define __uuidof(type)::Mso::Details::GuidUtils::GuidOf < type> ::Value
#endif

/// A macro to be used instead of __uuidof(expr). For types use __uuidof(type). E.g. __uuidof_expr(this).
#define __uuidof_expr(expr) __uuidof(typename std::remove_pointer<typename std::decay<decltype(expr)>::type>::type)

// We must explicitly set the C++ extern context for the MIDL generated files where it is set to "C".
extern "C++" {
namespace Mso::Details::GuidUtils {

// To indicate that type has or does not have GUID. We do not want to add dependency on "type_traits"
struct TrueType {
  enum { Value = true };
};
struct FalseType {
  enum { Value = false };
};

#if !COMPILER_SUPPORTS_UUID

/// Default implementation of the MsoGetGuid. It is used when we cannot find type specific implementation
/// provided by MSO_STRUCT_GUID/MSO_CLASS_GUID macro.
template <typename T>
inline GUID MsoGetGuid(T *) noexcept {
  static_assert(sizeof(T) == 0, "GUID is undefined for the type T");
  return {};
}

/// Statically associates a GUID with a type. It is used by __uuidof() macro implementation.
template <typename T>
struct GuidOf {
  static const GUID Value;
};

template <typename T>
const GUID GuidOf<T>::Value = MsoGetGuid(static_cast<T *>(nullptr));

/// To represent a GUID string without curly braces: "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXX"
typedef char GuidString[37];

/// Converts a hexadecimal ASCII character to an unsigned char.
constexpr const unsigned char H2U[256] = {
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
    0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wchar-subscripts"
#endif // __clang__

/// Converts string to a GUID at compile time. Expected format: "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXX"
constexpr GUID StringToGuid(const GuidString &g) noexcept {
  return {static_cast<unsigned int>(
              (H2U[g[0]] << 28) | (H2U[g[1]] << 24) | (H2U[g[2]] << 20) | (H2U[g[3]] << 16) | (H2U[g[4]] << 12) |
              (H2U[g[5]] << 8) | (H2U[g[6]] << 4) | H2U[g[7]]),
          static_cast<unsigned short>((H2U[g[9]] << 12) | (H2U[g[10]] << 8) | (H2U[g[11]] << 4) | H2U[g[12]]),
          static_cast<unsigned short>((H2U[g[14]] << 12) | (H2U[g[15]] << 8) | (H2U[g[16]] << 4) | H2U[g[17]]),
          {static_cast<unsigned char>((H2U[g[19]] << 4) | H2U[g[20]]),
           static_cast<unsigned char>((H2U[g[21]] << 4) | H2U[g[22]]),
           static_cast<unsigned char>((H2U[g[24]] << 4) | H2U[g[25]]),
           static_cast<unsigned char>((H2U[g[26]] << 4) | H2U[g[27]]),
           static_cast<unsigned char>((H2U[g[28]] << 4) | H2U[g[29]]),
           static_cast<unsigned char>((H2U[g[30]] << 4) | H2U[g[31]]),
           static_cast<unsigned char>((H2U[g[32]] << 4) | H2U[g[33]]),
           static_cast<unsigned char>((H2U[g[34]] << 4) | H2U[g[35]])}};
}

#if __clang__
#pragma clang diagnostic pop
#endif // __clang__

#endif // !COMPILER_SUPPORTS_UUID

} // namespace Mso::Details::GuidUtils
} // extern "C++"

#endif // MSO_GUID_MSOGUIDDETAILS_H
