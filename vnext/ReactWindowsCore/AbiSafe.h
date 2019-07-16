// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory>

// To ensure the same packing policy in all DLLs where this header is used.
#pragma pack(push, _CRT_PACKING)

// The purpose of this file is to define ABI safe primitives that can be used
// to pass parameters between different DLLs.
// The issue is that each DLL may have its own rules for memory allocation and
// deallocation. Thus, it is required to use primitives that ensure that
// object's memory is allocated and deallocated in the same DLL.
//
// To achieve it, we should ensure that object's memory is never freed
// with an explicit 'delete' operator call.
// Instead, we define the IAbiObject interface with a Destroy() method that
// should be called whenever object needs to be deleted. This method calls
// the 'delete' operator for the object.
//
// To simplify work with the IAbiObject, we define a smart pointer AbiPtr,
// and a few primitives to encapsulate string, vector, and a functor.

namespace AbiSafe {

// Base interface for ABI safe objects
struct IAbiObject {
  // Needs to be called to destroy object instead of explicit 'delete' operator.
  virtual void Destroy() noexcept = 0;
};

// To be used with std::unique_ptr to call Destroy() method.
struct AbiObjectDeleter {
  void operator()(IAbiObject *object) noexcept {
    if (object) {
      object->Destroy();
    }
  }
};

// Specialization of std::unique_ptr that uses AbiObjectDeleter instead of
// default deleter.
template <class T>
using AbiPtr = std::unique_ptr<T, AbiObjectDeleter>;

static_assert(
    sizeof(AbiPtr<int>) == sizeof(void *),
    "AbiPtr size must be the same as pointer size");

template <class T>
struct AbiSpan {
  const T *Data;
  size_t Size;
};

// To represent any array-like types such as arrays, vectors, and strings.
template <class T>
struct IAbiArray : IAbiObject {
  virtual AbiSpan<T> GetSpan() const noexcept = 0;
};

// Aliases for strings
using AbiCharSpan = AbiSpan<char>;
using IAbiString = IAbiArray<char>;
using AbiStringPtr = AbiPtr<IAbiString>;

// IAbiFunctor is a representation of a function object (functors)
// such as a lambda or std::function that must not throw exceptions (noexcept).
template <class TSignature>
struct IAbiFunctor; // Intentionally not implemented to allow the specialization
                    // for function signatures below.

template <class TResult, class... TArgs>
struct IAbiFunctor<TResult(TArgs...)> : IAbiObject {
  virtual TResult Invoke(TArgs &&... args) noexcept = 0;
};

// IAbiFunctorThrow is a representation of a function object (functors)
// such as a lambda or std::function that can throw exceptions.
template <class TSignature>
struct IAbiFunctorThrow; // Intentionally not implemented to allow the
                         // specialization for function signatures below.

template <class TResult, class... TArgs>
struct IAbiFunctorThrow<TResult(TArgs...)> : IAbiObject {
  virtual TResult Invoke(TArgs &&... args) = 0;
};

// A base class to implement ABI safe types.
// The base class is not copyable because it must be always managed by
// std::unique_ptr.
template <class TAbiInterface>
struct AbiObject : TAbiInterface {
  static_assert(
      std::is_base_of<IAbiObject, TAbiInterface>::value,
      "TAbiInterface must derive from IAbiObject");

  AbiObject() = default;
  AbiObject(const AbiObject &) = delete;
  AbiObject &operator=(const AbiObject &) = delete;

  void Destroy() noexcept override {
    delete this;
  }

 protected:
  virtual ~AbiObject() noexcept {}
};

// A helper method to create new instance of ABI safe object and return AbiPtr.
template <class T, class TResult = T, class... TArgs>
AbiPtr<TResult> AbiMake(TArgs &&... args) {
  return AbiPtr<TResult>{new T{std::forward<TArgs>(args)...}};
}

} // namespace AbiSafe

#pragma pack(pop)
