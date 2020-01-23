// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_OBJECT_QUERYCAST_H
#define MSO_OBJECT_QUERYCAST_H

#include <unknwn.h>
#include <type_traits>
#include <utility>
#include "crash/verifyElseCrash.h"
#include "debugAssertApi/debugAssertApi.h"
#include "guid/msoGuid.h"

/**
To implement IUnknown::QueryInterface use the QueryCastTraits to implement sequence of probing for a requested
type by a GUID.

QueryCastTraits have two base cases:
Case #1: Type implements a QueryCast method.
Case #2: Type has an associated GUID. It is used when there is no QueryCast for a type.

If type does not have QueryCast or a GUID then it will be a compilation error to apply the QueryCastTraits to it.
This is intentional because we want developers to be explicit about their intents. Use the QueryCastHidden template
to avoid compilation errors in case if interface is not intended to be queried.


Here we provide a number of helper templates to implement QueryCast for a type:
- QueryCastChain - Query for a base interface by its GUID if class implements an interface derived from it.
- QueryCastDerived - Casting to a derived type by its GUID.
- QueryCastGuid - Query type by specified GUID.
- QueryCastHidden - Prevents querying for a type.
- QueryCastList - Calls QueryCastTraits for all provided types.
*/

#pragma warning(push)
// Disable warnings 4625 and 4626 because templates below may have or may not have default copy constructor and
// copy assignment operator depending on the template type parameter. We do not want just to delete them because
// they could be useful in some scenarios when generated.
#pragma warning(disable : 4625) // copy constructor could not be generated because a base class copy constructor is
                                // inaccessible or deleted
#pragma warning(disable : 4626) // assignment operator could not be generated because a base class assignment operator
                                // is inaccessible or deleted
#pragma warning(disable : 4995) // 'IsDebuggerPresent': name was marked as #pragma deprecated. It is part of
                                // VerifyElseCrash macro.

namespace Mso {

/// A helper class for QueryCast to either use type's Guid or a QueryCast method.
/// Note that we do not call AddRef() here. It is a responsibility of a method that implements IUnknown::QueryInterface.
struct QueryCastHelper {
  /// Implements QueryCast similar to the classic IUnknown::QueryInterface implementation.
  template <typename T>
  static void *QueryCast(_In_ void *obj, const GUID &riid) noexcept {
    // It has lower priority than the next overload because we use void* instead of T* for the obj.
    return (riid == __uuidof(T)) ? obj : nullptr;
  }

  /// Delegates the QueryCast implementation to the object if it has a QueryCast method.
  /// If QueryCast fails then we check type's GUID if it is assigned by MSO_STRUCT_GUID or MSO_CLASS_GUID.
  template <typename T>
  static auto QueryCast(_In_ T *obj, const GUID &riid) noexcept -> decltype(obj->QueryCast(riid)) {
    void *result = obj->QueryCast(riid);
    return (result) ? result : GuidQueryCast<T>(obj, riid, static_cast<typename Mso::TypeHasGuid<T>::Type *>(nullptr));
  }

  /// Recursively calls itself while we have two or more base types.
  template <typename TSource, typename TBase0, typename TBase1, typename... TBases>
  static void *QueryCastList(TSource &obj, const GUID &riid) noexcept {
    void *result = QueryCastList<TSource, TBase0>(obj, riid);
    return (result) ? result : QueryCastList<TSource, TBase1, TBases...>(obj, riid);
  }

  /// An implementation for a type list with just one type.
  template <typename TSource, typename TBase>
  static void *QueryCastList(TSource &obj, const GUID &riid) noexcept {
    return QueryCast<TBase>(static_cast<TBase *>(&obj), riid);
  }

 private:
  template <typename T>
  static void *GuidQueryCast(_In_ T * /*obj*/, const GUID & /*riid*/, Mso::Details::GuidUtils::FalseType *) noexcept {
    // Type has no GUID: do nothing
    return nullptr;
  }

  template <typename T>
  static void *GuidQueryCast(_In_ T *obj, const GUID &riid, Mso::Details::GuidUtils::TrueType *) noexcept {
    // Typecast to void* to force choosing QueryCast overload that checks type's GUID.
    return QueryCast<T>(static_cast<void *>(obj), riid);
  }
};

/// Helps to implement static cast for the cases when multiple base types implement the same interface.
/// E.g. we want to cast to IUnknown but there are multiple base interfaces inherited from it.
/// static_cast<IUnknown*> fails because it does not know which IUnknown to choose.
/// This implementation chooses the first one to resolve the conflict.
template <typename TTarget>
struct StaticCastHelper;

template <typename TTarget>
struct StaticCastHelper<TTarget *> {
  /// Try to cast the first base type if it is inherited from TTarget.
  template <typename TSource, typename TBase0, typename... TBases>
  static TTarget *CastFirst(TSource *source) noexcept {
    return CastIf<TSource, TBase0, TBases...>(typename std::is_base_of<TTarget, TBase0>::type(), source);
  }

  /// A case when no base types are inherited from TTarget.
  template <typename TSource>
  static TTarget *CastFirst(TSource * /*source*/) noexcept {
    return nullptr;
  }

 private:
  /// First base type is inherited from TTarget. Cast it to TTarget.
  template <typename TSource, typename TBase0, typename... TBases>
  static TTarget *CastIf(std::true_type /*isConvertible?*/, TSource *source) noexcept {
    return Cast<TBase0>(/*tag:*/ 0, static_cast<TBase0 *>(source));
  }

  /// First base type is not inherited from TTarget. Recursively call CastFirst for the remaining base types.
  template <typename TSource, typename TBase0, typename... TBases>
  static TTarget *CastIf(std::false_type /*isConvertible?*/, TSource *source) noexcept {
    return CastFirst<TSource, TBases...>(source);
  }

  /// To have a lower priority when matching an int tag parameter.
  struct IntWrapper {
    IntWrapper(int) noexcept {}
  };

  /// If TSource has a StaticCastElseNull method then call it to do the static cast.
  template <typename TSource>
  static auto Cast(int /*tag*/, TSource *source) noexcept
      -> decltype(source->template StaticCastElseNull<TTarget *>()) {
    return source->template StaticCastElseNull<TTarget *>();
  }

  /// Cast using static_cast. It has a lower priority than previous method because we use IntWrapper tag instead of int.
  template <typename TSource>
  static TTarget *Cast(IntWrapper /*tag*/, TSource *source) noexcept {
    return static_cast<TTarget *>(source);
  }
};

//
// Helper templates to implement QueryCast.
//

/// Implements QueryCast for the case when object implements an interface derived from a base interface, and we want to
/// to query the base interface by its GUID.
/// E.g. struct IDerived : IBase {}
/// class MyClass : Mso::QueryCastChain&lt; IDerived, IBase &gt;
template <typename T, typename... TBaseTypes>
class DECLSPEC_NOVTABLE QueryCastChain : public T {
 public:
  void *QueryCast(const GUID &riid) noexcept {
    return InternalQueryCast<T, TBaseTypes...>(riid);
  }

 protected:
  template <typename... TArgs>
  QueryCastChain(TArgs &&... args) noexcept : T(std::forward<TArgs>(args)...) {}

 private:
  // Recursively calls itself while we have two or more base types.
  template <typename TBaseType0, typename TBaseType1, typename... TOtherBaseTypes>
  void *InternalQueryCast(const GUID &riid) noexcept {
    void *result = InternalQueryCast<TBaseType0>(riid);
    return (result) ? result : InternalQueryCast<TBaseType1, TOtherBaseTypes...>(riid);
  }

  template <typename TBaseType>
  void *InternalQueryCast(const GUID &riid) noexcept {
    static_assert(std::is_base_of<TBaseType, T>::value, "T must be inherited from TBaseType");
    return QueryCastHelper::QueryCast<TBaseType>(static_cast<TBaseType *>(this), riid);
  }
};

/// Implements the QueryCast that can return the derived type by its GUID.
/// It can be used to query for the most derived concrete type.
template <typename T>
class QueryCastDerived {
 public:
  void *QueryCast(const GUID &riid) noexcept {
    if (riid == __uuidof(T)) {
      return static_cast<T *>(this);
    }

    return nullptr;
  }
};

/// Allows to associate a GUID for a type that works only in the scope of the QueryCast operation.
/// This method can be used if type has no associated GUID, but we still want to query for it.
template <typename T, const GUID *piid = static_cast<const GUID *>(nullptr)>
class DECLSPEC_NOVTABLE QueryCastGuid : public T {
 public:
  void *QueryCast(const GUID &riid) noexcept {
    if (*Mso::ResolveGuidPtr<T, piid>::Guid == riid) {
      return static_cast<T *>(this);
    }

    return nullptr;
  }

 protected:
  template <typename... TArgs>
  QueryCastGuid(TArgs &&... args) noexcept : T(std::forward<TArgs>(args)...) {}
};

/// Implements the QueryCast that does not do any query cast for a type.
/// It can be useful when we want to pass a base type as a part of base type list to the QueryCastList, but do
/// not want to query for this type.
template <typename T>
class DECLSPEC_NOVTABLE QueryCastHidden : public T {
 public:
  void *QueryCast(const GUID & /*riid*/) noexcept {
    (void)(this); // To avoid OACR warning that method can be static
    return nullptr;
  }

 protected:
  template <typename... TArgs>
  QueryCastHidden(TArgs &&... args) noexcept : T(std::forward<TArgs>(args)...) {}
};

/// Delegates the QueryCast implementation to the list of the base types.
/// If a constructor with a non-empty set of parameters then they are forwarded to the TBase0.
template <typename TBase0, typename... TBases>
class DECLSPEC_NOVTABLE QueryCastList : public TBase0, public TBases... {
  using CastList = int; // TODO: This is a temporary variable. We will have to expand this concept to implement shallow
                        // inheritance tree for UnknownObject.
                        // The goal is to have "using CastList = CastList<TBase0, TBases...>" in UnknownObject.
  using ThisType = QueryCastList;

 protected:
  template <typename... TArgs>
  QueryCastList(TArgs &&... args) noexcept : TBase0(std::forward<TArgs>(args)...) {}

 public:
  void *QueryCast(const GUID &riid) noexcept {
    return QueryCastHelper::QueryCastList<ThisType, TBase0, TBases...>(*this, riid);
  }

  template <typename TTarget>
  TTarget StaticCastElseNull() noexcept {
    return StaticCastHelper<TTarget>::template CastFirst<ThisType, TBase0, TBases...>(this);
  }

#pragma warning(suppress : 4265) // class has virtual functions, but destructor is not virtual
};

/**
  A helper class to call QueryCast method for an object.
  There are two supported cases:
  (1) type has a QueryCast instance method
  (2) type is inherited from IUnknown and the implementation supports initialization of a QueryCastBridge instance
    inside its IUnknown::QueryInterface() implementation.
  The QueryCast typecasting does not call AddRef.
*/
MSO_STRUCT_GUID(QueryCastBridge, "A39D5FC8-0641-4EEE-8C97-DDEF114D487D")
struct QueryCastBridge {
  // DECLARE_COPYCONSTR_AND_ASSIGNMENT(QueryCastBridge);
  void *Object; // It must be a first field. In that case **ppvObject in QueryInterface will have the same semantic as
                // with the normal QueryInterface calls.
  const GUID &ObjectId;
};

namespace Details {

/// Helper class for query_cast function to allow specializations based on TTarget.
template <typename TTarget>
struct QueryCastConverter; // Default case is undefined.

// Specialization for reference conversion
template <typename TTarget>
struct QueryCastConverter<TTarget &> {
  template <typename TSource>
  static TTarget &QueryCast(const TSource &source) noexcept {
    using SourceType = std::decay_t<TSource>;
    using TargetType = std::decay_t<TTarget>;
    static_assert(!std::is_pointer<SourceType>::value, "Cannot convert pointer to a reference.");

    SourceType *nonConstSource = const_cast<SourceType *>(&source);
    QueryCastBridge bridge = {nullptr, __uuidof(TargetType)};
    VerifySucceededElseCrashTag(
        nonConstSource->QueryInterface(__uuidof(QueryCastBridge), reinterpret_cast<void **>(&bridge)),
        0x0100370d /* tag_bad2n */);

    TargetType *target = static_cast<TargetType *>(bridge.Object);
    VerifyElseCrashSzTag(target, "Query cast failed for a reference type.", 0x0100370e /* tag_bad2o */);
    return static_cast<TTarget &>(*target);
  }
};

// Specialization for pointer conversion
template <typename TTarget>
struct QueryCastConverter<TTarget *> {
  template <typename TSource>
  static TTarget *QueryCast(const TSource &source) noexcept {
    using SourceType = std::remove_const_t<std::remove_pointer_t<std::decay_t<TSource>>>;
    using TargetType = std::decay_t<TTarget>;
    static_assert(std::is_pointer<TSource>::value, "Cannot convert non-pointer to a pointer.");

    if (source != nullptr) {
      SourceType *nonConstSource = const_cast<SourceType *>(source);
      QueryCastBridge bridge = {nullptr, __uuidof(TargetType)};
      if (nonConstSource->QueryInterface(__uuidof(QueryCastBridge), reinterpret_cast<void **>(&bridge)) == S_OK) {
        TargetType *target = static_cast<TargetType *>(bridge.Object);
        return static_cast<TTarget *>(target);
      }
    }

    return nullptr;
  }
};

} // namespace Details
} // namespace Mso

/**
  query_cast does a dynamic cast based on a type GUID.
  It does not call AddRef. Caller of this method is responsible for the object's lifetime management.
  TTarget is required to be either a pointer or a reference (similar to dynamic_cast).
  E.g. query_cast< IMyIntf* >( x ), or query_cast< IMyIntf& >( x ).

  If query_cast fails to convert to a pointer type then it returns nullptr.
  If query_cast fails to convert to a reference type then it crashes the app.

  For this method to work, the TSource class must support initialization of QueryCastBridge struct
  when queried for its GUID. UnknownObject in core\msoUnknownObject.h provides all necessary support.

  query_cast can be used when we want to do a dynamic cast and do not need to call AddRef to control object lifetime.
*/
template <typename TTarget, typename TSource>
TTarget query_cast(TSource &&source) noexcept {
  return Mso::Details::QueryCastConverter<TTarget>::QueryCast(std::forward<TSource>(source));
}

#pragma warning(pop)

#endif // MSO_OBJECT_QUERYCAST_H
