// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_GUID_MSOGUID_H
#define MSO_GUID_MSOGUID_H

/**
  Support for multi-platform GUID implementation.
*/

#include <guiddef.h>
#include "oacr/oacr.h"

#ifdef __cplusplus
#include "guid/msoGuidDetails.h"

/**

MSO_STRUCT_GUID or MSO_CLASS_GUID are used to associate a GUID with a type in platform independent way:

  MSO_STRUCT_GUID(IMyInterface, "8665E4CE-50ED-4C12-A96D-13BD1432C219")
  struct DECLSPEC_NOVTABLE IMyInterface : IBaseInterface
  {
    virtual ReturnType1 Method1() = 0;
    virtual ReturnType2 Method1(ArgType1 arg1) = 0;
  };

  MSO_CLASS_GUID(MyClass, "8665E4CE-50ED-4C12-A96D-13BD1432C219")
  class MyClass : public IBaseInterface
  {
  };


NOTE: GUID must not use curly braces around it.

To retrieve GUID for a type use the __uuidof():


  const GUID& myTypeGuid = __uuidof(MyType);

  template <typename T, typename TSource>
  T* qi_cast(TSource* s, const GUID& riid = __uuidof(T));


The __uuidof() is a Visual C++ specific operator that accepts a variety of inputs.
For platforms that do not support the __uuidof() operator we define a __uuidof() macro.
This macro can only accept a type. We recommend to change code that uses an expression in the following way:
  - See if you can simply replace expression with a type.
  - Replace __uuidof(NULL) with GUID_NULL
  - Replace __uuidof(expr) with __uuidof_expr(expr)
*/

/**
++ FAQ ++

Q: Why to use macro to associate a GUID with a type?
A: There is no C++ standard way to assign a GUID attribute to a type. Any solution that we provide at this point
   is a workaround to this limitation. Macro can hide any such platform specific workarounds.

Q: How does it work?
A: For Visual C++ we use the __declspec(uuid) attribute.
   For Clang we use an overload of a function MsoGetGuid(T*).
   This function is used to initialize GuidOf<T>::Value static field returned by __uuidof() macro.
   The main benefit of using function is that it allows us to associate GUID with a type in the type's namespace.
   The function is discovered with help of C++ argument-dependent name lookup
http://en.wikipedia.org/wiki/Argument_dependent_name_lookup .

Q: Why not to use a template specialization for GUID association?
A: This is what we tried first, but a template cannot be specialized in a different namespace. In our case we want to
   be able associate GUID in a namespace where we declare a type, and not just in the Mso namespace.

Q: Where are the implementation details for the MSO_STRUCT_GUID/MSO_CLASS_GUID macro?
A: See the msoGuidDetails.h

*/

namespace Mso {
OACR_WARNING_PUSH
OACR_WARNING_DISABLE(VAR_IN_HEADER, "False Positive for variable in header with template.  (see Bug 1689514) ")

/// To return false in TypeHasGuid<T>::Value if type has no GUID defined by MSO_STRUCT_GUID/MSO_CLASS_GUID
template <typename T>
::Mso::Details::GuidUtils::FalseType MsoTypeHasGuid(T *);

/// Type traits to check if type has a GUID defined by MSO_STRUCT_GUID/MSO_CLASS_GUID
template <typename T>
struct TypeHasGuid {
  typedef decltype(MsoTypeHasGuid(static_cast<T *>(nullptr))) Type;
  enum { Value = Type::Value };
};

/**
  This template is to address an issue with NDK linker which cannot properly compile
  a template that uses a pointer to GUID as a template parameter.
  E.g. the " struct TCntQIPtr<T, const GUID* piid = &__uuidof(T)> " will fail to compile
  because linker cannot find a static field which we use for __uuidof(T) implementation.
  Instead, this line must be changed to " struct TCntQIPtr<T, const GUID* piid = nullptr> "
  and we should use Mso::ResolveGuidPtr<T, piid>::Guid instead of piid inside of the template.
  The Mso::ResolveGuidPtr has a specialization for nullptr that does all the "magic".
*/
template <typename T, const GUID *guidPtr>
struct ResolveGuidPtr {
  static const GUID *Guid;
};

template <typename T, const GUID *guidPtr>
const GUID *ResolveGuidPtr<T, guidPtr>::Guid = guidPtr;

template <typename T>
struct ResolveGuidPtr<T, static_cast<const GUID *>(nullptr)> {
  static const GUID *Guid;
};

template <typename T>
const GUID *ResolveGuidPtr<T, static_cast<const GUID *>(nullptr)>::Guid = &__uuidof(T);

OACR_WARNING_POP
} // namespace Mso

#endif // __cplusplus
#endif // MSO_GUID_MSOGUID_H
