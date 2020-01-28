// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUNCTIONAL_FUNCTORREF_H
#define MSO_FUNCTIONAL_FUNCTORREF_H

//! Mso::FunctorRef is a lightweight type erased reference to a function
//! object such as lambda.
//!
//! In general, function object is any class that implements call operator "()".
//! Lambdas simplify creation of function objects.
//! Function objects are also called functors.
//!
//! FunctorRef is mainly targeted to be used as a parameter for functions which
//! need type-erased function object, but do not need to store or own it.
//! It can be considered as a replacement for the std::function optimized for
//! specific scenarios. FunctorRef only keeps a reference to the provided function
//! object and does not do any copy or move. The fact that it only keeps a reference
//! makes support for copy or move semantic impossible and FunctorRef must never be
//! stored as a field in an object.
//!
//! Mso::FunctorRef has the following semantics:
//!
//! - Only keeps a reference to the function object.
//! - Never copies or moves the provided function object.
//! - Does not do a heap allocation and has size of two pointers (one is
//!   for internal v-table for type erasure, and another is a reference to
//!   the function object.
//! - Must not be copied or moved.
//! - Must not be stored outside of call stack.
//! - Supports move-only function objects (e.g. a lambda capturing a unique_ptr),
//!   unlike std::function<>.
//! - Supports optional semantic by allowing to be initialized with nullptr, and
//!   then to be checked for non-null by the explicit bool operator.
//!
//! The Mso::FunctorRef only supports noexcept function objects.
//! Use Mso::FunctorRefThrow for throwing function objects.
//!
//! In case if you want to store function object or transfer its ownership, you can use:
//! - Mso::Functor that is an Mso::CntPtr to a ref counted heap-allocated object.
//! - Mso::SmallFunctor that uses in-place storage.
//!
//! Usage example:
//! In your interface you may have a function which needs a function object, but we
//! cannot use a template because this function is virtual. The function needs a predicate
//! to do some work, but does not store it.
//!
//!   virtual MyObject FindInCache(const Mso::FunctorRef<bool(MyObject&)>& predicate) = 0;
//!
//! This method can be called as following:
//!
//!   myCache->FindInCache([this](MyObject& obj) noexcept {	return this->CheckObj(obj); });
//!
//! Note that we can simply pass lambda as a parameter because we allow use of
//! FunctorRef implicit constructor.
//!
//! WARNING WARNING WARNING:
//!
//! Avoid storing Mso::FunctorRef in a variable because it often leads
//! to having a pointer to a deleted object.
//! Instead, always pass lambda directly to a function to call
//! Mso::FunctorRef constructor implicitly.
//!
//! For example, if we have a function:
//! void Sort(const FunctorRef<bool(int, int)>& comparer);
//!
//! Then we can pass lambda to it using one of the two ways:
//!
//! Sort([](int x, int y) noexcept { return x - y; });
//!
//! or
//!
//! auto comparer = [](int x, int y) noexcept { return x - y; };
//! Sort(comparer);
//!
//! In both cases the lambda object is alive while Sort function is called.
//! Otherwise, if we store Mso::FunctorRef in a local variable we can get a pointer
//! to deleted lambda object when lambda is not store in a variable.
//! BAD CODE:
//! BAD CODE: Mso::FunctorRef<bool(int, int)> comparer = [](int x, int y) noexcept { return x - y; };
//! BAD CODE: Sort(comparer);
//! BAD CODE:
//! In the bad example above the lambda object is deleted right after
//! the comparer variable is assigned, and thus the comparer variable
//! will point to a deleted lambda object. Usually it results in crashes in
//! optimized builds, while sometimes unoptimized builds seem to work.
//!

#include <compilerAdapters/cppMacros.h>
#include <crash/verifyElseCrash.h>

#include <memory>
#include <type_traits>
#include <utility>

namespace Mso {

//! Reference to a non-throwing function object.
template <typename TSignature>
class FunctorRef;

//! Reference to a throwing function object.
template <typename TSignature>
class FunctorRefThrow;

namespace Details {

// FunctorRefStorage should be big enough to contain a v-table pointer and a reference.
// We use 8-byte alignment. On 32-bit platform the storage will be 8 bytes, and for 64-bit - 16 bytes.
using FunctorRefStorage = std::aligned_storage<sizeof(uintptr_t) * 2, 8>::type;

} // namespace Details

//! Reference to a non-throwing function object.
template <typename TResult, typename... TArgs>
class FunctorRef<TResult(TArgs...)>
#if !defined(__cpp_noexcept_function_type) && (_HAS_NOEXCEPT_FUNCTION_TYPES != 1)
    final
#endif
{
 public:
  //! Creates an empty FunctorRef
  _Allow_implicit_ctor_ FunctorRef(std::nullptr_t) noexcept {}

  //! Creates a non-empty FunctorRef
  template <typename T>
  _Allow_implicit_ctor_ FunctorRef(T &&func) noexcept {
    using WrapperType = FunctorRefWrapper<std::remove_reference_t<T>>;

    // Make sure that WrapperType storage requirements match the FunctorRefStorage.
    static_assert(
        sizeof(WrapperType) <= sizeof(Details::FunctorRefStorage),
        "WrapperType is too big to fit in FunctorRefStorage.");
    static_assert(
        std::alignment_of<WrapperType>::value <= std::alignment_of<Details::FunctorRefStorage>::value,
        "WrapperType alignment does not match to FunctorRefStorage.");

    ::new (std::addressof(m_storage)) WrapperType{&func};
  }

  //! Delete copy and move constructors and assignment operators.
  FunctorRef(FunctorRef const &other) = delete;
  FunctorRef &operator=(FunctorRef const &other) = delete;

  //! Calls referenced function object.
  //! Crash if referenced function object is nullptr.
  TResult operator()(TArgs... args) const noexcept {
    // It's important that we don't use '&&' on TArgs here. Since TArgs is a class
    // template parameter, the '&&' would force each argument to become an rvalue
    // (or, subject to ref-collapsing, an lvalue ref) which could prevent us from
    // being able to correctly invoke the underlying function object. With
    // that said, it is important that the wrapper's 'Invoke' uses '&&' to prevent
    // a second copy of any non-ref-qualified parameter since a copy would already
    // have been made here.

    VerifyElseCrashSzTag(*this, "FunctorRef must not be empty", 0x025d9804 /* tag_cxz6e */);
    return reinterpret_cast<const IFunctorRef *>(std::addressof(m_storage))->Invoke(std::forward<TArgs>(args)...);
  }

  //! True if wrapped function object is not nullptr.
  explicit operator bool() const noexcept {
    return *reinterpret_cast<const uintptr_t *>(std::addressof(m_storage)) != 0;
  }

 private:
  //! The interface to be implemented by FunctorRefWrapper<T>.
  struct IFunctorRef {
    virtual TResult Invoke(TArgs &&...) const noexcept = 0;
  };

  //! A wrapper for a function object reference.
  //! We do not call destructor for this class because it does not need to free any resources.
  template <typename T>
  struct FunctorRefWrapper final : IFunctorRef {
    FunctorRefWrapper(T *func) noexcept : m_func(func) {}
    FunctorRefWrapper(FunctorRefWrapper const &) = delete;
    FunctorRefWrapper &operator=(FunctorRefWrapper const &) = delete;
    ~FunctorRefWrapper() = delete;

    TResult Invoke(TArgs &&... args) const noexcept override {
      // If you see OACR warning "Nothrow Func Throws" here then it means that the
      // provided lambda or function object's operator() are not marked as noexcept.

      // We use const_cast to enable support for mutable lambdas
      return (*const_cast<FunctorRefWrapper *>(this)->m_func)(std::forward<TArgs>(args)...);
    }

   private:
    T *m_func;
  };

 private:
  // Storage for 2 pointers: v_table + reference
  Details::FunctorRefStorage m_storage{0};
};

#if defined(__cpp_noexcept_function_type) || (_HAS_NOEXCEPT_FUNCTION_TYPES == 1)

// Treat the noexcept in function signature the same way as if it was not there.

template <typename TResult, typename... TArgs>
class FunctorRef<TResult(TArgs...) noexcept> : public FunctorRef<TResult(TArgs...)> {
 public:
  using FunctorRef<TResult(TArgs...)>::FunctorRef;
};

#endif

//! Reference to a throwing function object.
template <typename TResult, typename... TArgs>
class FunctorRefThrow<TResult(TArgs...)> final {
 public:
  //! Creates an empty FunctorRefThrow
  _Allow_implicit_ctor_ FunctorRefThrow(std::nullptr_t) noexcept {}

  //! Creates an non-empty FunctorRefThrow
  template <typename T>
  _Allow_implicit_ctor_ FunctorRefThrow(T &&func) noexcept {
    using WrapperType = FunctorRefThrowWrapper<std::remove_reference_t<T>>;

    // Make sure that WrapperType storage requirements match the FunctorRefStorage.
    static_assert(
        sizeof(WrapperType) <= sizeof(Details::FunctorRefStorage),
        "WrapperType is too big to fit in FunctorRefStorage.");
    static_assert(
        std::alignment_of<WrapperType>::value <= std::alignment_of<Details::FunctorRefStorage>::value,
        "WrapperType alignment does not match to FunctorRefStorage.");

    ::new (std::addressof(m_storage)) WrapperType{&func};
  }

  //! Delete copy and move constructors and assignment operators.
  FunctorRefThrow(FunctorRefThrow const &) = delete;
  FunctorRefThrow &operator=(FunctorRefThrow const &) = delete;

  //! Calls referenced function object.
  //! Crash if referenced function object is nullptr.
  TResult operator()(TArgs... args) const {
    // It's important that we don't use '&&' on TArgs here. Since TArgs is a class
    // template parameter, the '&&' would force each argument to become an rvalue
    // (or, subject to ref-collapsing, an lvalue ref) which could prevent us from
    // being able to correctly invoke the underlying function object. With
    // that said, it is important that the wrapper's 'Invoke' uses '&&' to prevent
    // a second copy of any non-ref-qualified parameter since a copy would already
    // have been made here.

    VerifyElseCrashSzTag(*this, "FunctorRefThrow must not be empty", 0x025d9805 /* tag_cxz6f */);
    return reinterpret_cast<const IFunctorRefThrow *>(std::addressof(m_storage))->Invoke(std::forward<TArgs>(args)...);
  }

  //! True if wrapped function object is not nullptr.
  explicit operator bool() const noexcept {
    return *reinterpret_cast<const uintptr_t *>(std::addressof(m_storage)) != 0;
  }

 private:
  //! The interface to be implemented by FunctorRefThrowWrapper<T>.
  struct IFunctorRefThrow {
    virtual TResult Invoke(TArgs &&...) const = 0;
  };

  //! A wrapper for a function object reference.
  //! We do not call destructor for this class because it does not need to free any resources.
  template <typename T>
  struct FunctorRefThrowWrapper final : IFunctorRefThrow {
    FunctorRefThrowWrapper(T *func) noexcept : m_func(func) {}
    FunctorRefThrowWrapper(FunctorRefThrowWrapper const &) = delete;
    FunctorRefThrowWrapper &operator=(FunctorRefThrowWrapper const &) = delete;
    ~FunctorRefThrowWrapper() = delete;

    TResult Invoke(TArgs &&... args) const override {
      OACR_POSSIBLE_THROW;
      // We use const_cast to enable support for mutable lambdas
      return (*const_cast<FunctorRefThrowWrapper *>(this)->m_func)(std::forward<TArgs>(args)...);
    }

   private:
    T *m_func;
  };

 private:
  // Storage for 2 pointers: v_table + reference
  Details::FunctorRefStorage m_storage{0};
};

/// Aliases for the most common function object type "void()".
using VoidFunctorRef = FunctorRef<void()>;
using VoidFunctorRefThrow = FunctorRefThrow<void()>;

} // namespace Mso

#endif // MSO_FUNCTIONAL_FUNCTORREF_H
