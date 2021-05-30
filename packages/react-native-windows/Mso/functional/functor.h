// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUNCTIONAL_FUNCTOR_H
#define MSO_FUNCTIONAL_FUNCTOR_H

/**
  Mso::Functor is a replacement for std::function that uses intrusive reference
  counting and is always non-throwing (even if it is wrapping a throwing function
  object). Mso::Functor has the following semantics:
  - Always performs a heap allocation when creating a new instance from a function object, unless the function object is
  stateless.
  - Are small (size of a CntPtr).
  - Cheap to copy and move.
  - There will only be one outstanding copy of the function object given to the Mso::Functor.
  - Supports move-only function objects (e.g. a lambda capturing a unique_ptr), unlike std::function<>.

  For throwing function objects you can use Mso::FunctorThrow.

  If you want to avoid the heap allocation overhead then you have two other choices:
  - If the functor is not long lived and won't outlive the function object, use Mso::FunctorRef.
  - If you need to keep the functor for longer, use Mso::SmallFunctor.
*/

#include <object/unknownObject.h>
#include <functional>
#include <type_traits>

//! These are set of macros to generate code that depends on
//! calling conventions and other function decorators.
#ifdef __clang__
#define MSO_EMIT_CDECL(func, cvOpt, refOpt, noexceptOpt) func(, cvOpt, refOpt, noexceptOpt)
#else
#define MSO_EMIT_CDECL(func, cvOpt, refOpt, noexceptOpt) func(__cdecl, cvOpt, refOpt, noexceptOpt)
#endif

#if !defined(__clang__) && defined(_M_CEE)
#define MSO_EMIT_CLRCALL(func, cvOpt, refOpt, noexceptOpt) func(__clrcall, cvOpt, refOpt, noexceptOpt)
#else
#define MSO_EMIT_CLRCALL(func, cvOpt, refOpt, noexceptOpt)
#endif

#if !defined(__clang__) && defined(_M_IX86) && !defined(_M_CEE)
#define MSO_EMIT_FASTCALL(func, cvOpt, refOpt, noexceptOpt) func(__fastcall, cvOpt, refOpt, noexceptOpt)
#else
#define MSO_EMIT_FASTCALL(func, cvOpt, refOpt, noexceptOpt)
#endif

#if !defined(__clang__) && defined(_M_IX86)
#define MSO_EMIT_STDCALL(func, cvOpt, refOpt, noexceptOpt) func(__stdcall, cvOpt, refOpt, noexceptOpt)
#else
#define MSO_EMIT_STDCALL(func, cvOpt, refOpt, noexceptOpt)
#endif

#if !defined(__clang__) && ((defined(_M_IX86) && _M_IX86_FP >= 2) || defined(_M_X64)) && !defined(_M_CEE)
#define MSO_EMIT_VECTORCALL(func, cvOpt, refOpt, noexceptOpt) func(__vectorcall, cvOpt, refOpt, noexceptOpt)
#else
#define MSO_EMIT_VECTORCALL(func, cvOpt, refOpt, noexceptOpt)
#endif

#define MSO_NON_MEMBER_CALL(func, noexceptOpt) \
  MSO_EMIT_CDECL(func, , , noexceptOpt)        \
  MSO_EMIT_CLRCALL(func, , , noexceptOpt)      \
  MSO_EMIT_FASTCALL(func, , , noexceptOpt)     \
  MSO_EMIT_STDCALL(func, , , noexceptOpt)      \
  MSO_EMIT_VECTORCALL(func, , , noexceptOpt)

#if defined(__cpp_noexcept_function_type) || (_HAS_NOEXCEPT_FUNCTION_TYPES == 1)
#define MSO_NON_MEMBER_CALL_NOEXCEPT(func) \
  MSO_NON_MEMBER_CALL(func, )              \
  MSO_NON_MEMBER_CALL(func, noexcept)
#else
#define MSO_NON_MEMBER_CALL_NOEXCEPT(func) MSO_NON_MEMBER_CALL(func, )
#endif

namespace Mso {

template <typename TSignature>
class Functor;
template <typename TSignature>
class FunctorThrow;

//! Base interface for Mso::Functor implementation.
template <typename TResult, typename... TArgs>
struct DECLSPEC_NOVTABLE IFunctor : IUnknown {
  virtual TResult Invoke(TArgs &&...) noexcept = 0;
};

//! Base interface for Mso::FunctorThrow implementation.
template <typename TResult, typename... TArgs>
struct DECLSPEC_NOVTABLE IFunctorThrow : IUnknown {
  virtual TResult Invoke(TArgs &&...) = 0;
};

namespace Details {

//! The library that we use for Android and Apple platforms
//! has a bug in std::is_function implementation where it does not recognize the
//! functions with 'noexcept' in signature as functions.
//! This code is to fix it for the non-MSVC platforms

#ifdef __clang__
template <typename T>
struct IsFunction : std::false_type {};

#define MSO_IS_FUNCTION(CALL_OPT, X1, X2, NOEXCEPT_OPT) \
  template <typename TResult, typename... TArgs>        \
  struct IsFunction<TResult CALL_OPT(TArgs...) NOEXCEPT_OPT> : std::true_type {};

MSO_NON_MEMBER_CALL_NOEXCEPT(MSO_IS_FUNCTION)
#undef MSO_IS_FUNCTION

template <typename T>
struct Decay {
  using NoRefType = std::remove_reference_t<T>;
  using Type =
      std::conditional_t<IsFunction<NoRefType>::value, std::add_pointer_t<NoRefType>, std::remove_cv_t<NoRefType>>;
};

template <typename T>
using Decay_t = typename Decay<T>::Type;

#else // __clang__

template <typename T>
using IsFunction = std::is_function<T>;

template <typename T>
using Decay_t = std::decay_t<T>;

#endif // __clang__

//! Base class for functor objects that need constexpr constructors
//! Mostly equivalent to Mso::UnknownObject<Mso::RefCountStrategy::NoRefCountNoQuery>.
//! The tradeoff is that it doesn't have a virtual destructor, which simplifies
//! construction of static instances, but means that anything using this base class
//! cannot have any non-trivial member variables.
template <typename TIFunctor>
struct ConstexprFunctorBase : public TIFunctor {
  _Success_(return == S_OK)
      STDMETHOD(QueryInterface)(const GUID & /*riid*/, _Outptr_ void ** /*ppvObject*/) noexcept override {
    return E_FAIL;
  }
  STDMETHOD_(ULONG, AddRef)() noexcept override {
    return 1;
  }
  STDMETHOD_(ULONG, Release)() noexcept override {
    return 1;
  }
};

//! Function object wrapper. It can be a lambda or a class implementing call operator().
template <typename TFunc, typename TResult, typename... TArgs>
class FunctionObjectWrapper final
    : public Mso::UnknownObject<Mso::RefCountStrategy::SimpleNoQuery, Mso::IFunctor<TResult, TArgs...>> {
 public:
  FunctionObjectWrapper() = delete;
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(FunctionObjectWrapper);

  template <typename T>
  FunctionObjectWrapper(T &&func) noexcept : m_func(std::forward<T>(func)) {}

  virtual TResult Invoke(TArgs &&... args) noexcept override {
    return m_func(std::forward<TArgs>(args)...);
  }

 private:
  TFunc m_func;
};

//! Throwing function object wrapper. It can be a lambda or a class implementing call operator().
template <typename TFunc, typename TResult, typename... TArgs>
class FunctionObjectWrapperThrow final
    : public Mso::UnknownObject<Mso::RefCountStrategy::SimpleNoQuery, Mso::IFunctorThrow<TResult, TArgs...>> {
 public:
  FunctionObjectWrapperThrow() = delete;
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(FunctionObjectWrapperThrow);

  template <typename T>
  FunctionObjectWrapperThrow(T &&func) noexcept : m_func(std::forward<T>(func)) {}

  virtual TResult Invoke(TArgs &&... args) override {
    return m_func(std::forward<TArgs>(args)...);
  }

 private:
  TFunc m_func;
};

//! Function pointer wrapper. For function pointers, we can avoid a heap allocation by making a
//! static wrapper object for each function that we want to use, because we don't need to store
//! per-Functor state.
template <typename TFunc, typename TResult, typename... TArgs>
class FunctionPointerWrapper final : public ConstexprFunctorBase<Mso::IFunctor<TResult, TArgs...>> {
  static_assert(std::is_pointer<TFunc>::value, "Must be a pointer");
  static_assert(Mso::Details::IsFunction<std::remove_pointer_t<TFunc>>::value, "Must be a function pointer");

 public:
  constexpr FunctionPointerWrapper(TFunc func) noexcept : m_func(func) {}

  TResult Invoke(TArgs &&... args) noexcept override {
    return (*m_func)(std::forward<TArgs>(args)...);
  }

 private:
  TFunc m_func;
};

//! Throwing function pointer wrapper
template <typename TFunc, typename TResult, typename... TArgs>
class FunctionPointerWrapperThrow final : public ConstexprFunctorBase<Mso::IFunctorThrow<TResult, TArgs...>> {
  static_assert(std::is_pointer<TFunc>::value, "Must be a pointer");
  static_assert(Mso::Details::IsFunction<std::remove_pointer_t<TFunc>>::value, "Must be a function pointer");

 public:
  constexpr FunctionPointerWrapperThrow(TFunc func) noexcept : m_func(func) {}

  TResult Invoke(TArgs &&... args) override {
    return (*m_func)(std::forward<TArgs>(args)...);
  }

 private:
  TFunc m_func;
};

//! Stateless function wrapper. For function objects that carry no state, we
//! can take a shortcut and reuse a static instance of IFunctor rather than
//! allocating a new wrapper object on the heap each time.
//! I'm not aware of this existing previously, so I'm going to take the
//! opportunity to name this technique the "Really Small Functor Optimization".
template <typename TFunc, typename TResult, typename... TArgs>
class StatelessFunctorWrapper final : public ConstexprFunctorBase<Mso::IFunctor<TResult, TArgs...>> {
 public:
  constexpr StatelessFunctorWrapper(TFunc &func) noexcept : m_func(func) {}

  TResult Invoke(TArgs &&... args) noexcept override {
    return m_func(std::forward<TArgs>(args)...);
  }

 private:
  TFunc &m_func;
};

//! Throwing stateless functor wrapper
template <typename TFunc, typename TResult, typename... TArgs>
class StatelessFunctorWrapperThrow final : public ConstexprFunctorBase<Mso::IFunctorThrow<TResult, TArgs...>> {
 public:
  constexpr StatelessFunctorWrapperThrow(TFunc &func) noexcept : m_func(func) {}

  TResult Invoke(TArgs &&... args) override {
    return m_func(std::forward<TArgs>(args)...);
  }

 private:
  TFunc &m_func;
};

template <typename TResult, typename... TArgs>
TResult DoNothingFunction(TArgs...) noexcept {
  return TResult();
}

template <typename TFunc, typename TResult, typename... TArgs>
struct IsFunctionObject {
 private:
  template <typename T>
  static auto CheckFunctionObject(T &&t, int)
      -> decltype(static_cast<TResult>(std::forward<T>(t)(std::declval<TArgs>()...)), std::true_type());

  template <typename T>
  static std::false_type CheckFunctionObject(T &&t, void *);

 public:
  //! IsFunctionObject is true when provided TFunc type is a function object with operator() that accepts
  //! arguments of type TArgs... and its result can be converted to TResult.
  constexpr static const bool Value = decltype(CheckFunctionObject<TFunc>(std::declval<TFunc>(), 0))::value;
};

//! Checks if T is an Mso::Functor
template <typename T>
struct IsMsoFunctor : std::false_type {};
template <typename T>
struct IsMsoFunctor<Mso::Functor<T>> : std::true_type {};

//! Checks if T is an Mso::FunctorThrow
template <typename T>
struct IsMsoFunctorThrow : std::false_type {};
template <typename T>
struct IsMsoFunctorThrow<Mso::FunctorThrow<T>> : std::true_type {};

template <typename TFunc, typename... TArgs>
struct IsNoExceptFunctionObject {
  constexpr static const bool Value = noexcept(std::declval<TFunc>()(std::declval<TArgs>()...));
};

} // namespace Details

//! Special pseudo-functor that can be passed to Mso::Functor or Mso::FunctorThrow that does nothing.
//! It is more efficient than empty lambda because we do not do heap allocations in such case.
struct DoNothingFunctor {};

//! To indicate that function object may throw an exception and it will cause a process termination.
//! Mso::Functor(std::function<>(), Mso::TerminateOnException);
struct TerminateOnExceptionTag {
  constexpr TerminateOnExceptionTag() noexcept {}
};
constexpr const TerminateOnExceptionTag TerminateOnException;

// For backward compatibility:
using ThrowingFunctorObjectTag = TerminateOnExceptionTag;
constexpr const ThrowingFunctorObjectTag ThrowingFunctorObject;

/**
  Functor is a smart pointer to a IFunctor instance.
  Its constructor accepts either a CntPtr<IFunctor> for custom implementations or a function object.
  Function object is a class with a call operator(). Lambda is a function object. Function object must be
  noexcept. For throwing function objects such as std::function use overload with Mso::CrashOnException value. Functor
  can work with function pointers without heap allocations using FromFunctionPtr.
*/
template <typename TResult, typename... TArgs>
class Functor<TResult(TArgs...)> {
 public:
  using IFunctor = Mso::IFunctor<TResult, TArgs...>;

 private:
  template <typename T>
  using EnableIfIFunctor = std::enable_if_t<std::is_convertible<T *, IFunctor *>::value, int>;
  template <typename T>
  using EnableIfFunctionObject = std::enable_if_t<
      Mso::Details::IsFunctionObject<T, TResult, TArgs...>::Value &&
          !std::is_convertible<Mso::Details::Decay_t<T> *, IFunctor *>::value &&
          !Mso::Details::IsMsoFunctor<Mso::Details::Decay_t<T>>::value,
      int>;
  template <typename T>
  using EnableIfNoThrow = std::enable_if_t<Mso::Details::IsNoExceptFunctionObject<T, TArgs...>::Value, int>;
  template <typename T>
  using EnableIfThrow = std::enable_if_t<!Mso::Details::IsNoExceptFunctionObject<T, TArgs...>::Value, int>;
  template <typename T>
  using EnableIfStateless = std::enable_if_t<std::is_empty<T>::value, int>;
  template <typename T>
  using EnableIfNotStateless = std::enable_if_t<!std::is_empty<T>::value, int>;

 public:
  Functor() noexcept {}

  _Allow_implicit_ctor_ Functor(std::nullptr_t) noexcept {}

  Functor(const Functor &other) noexcept : m_impl(other.m_impl) {}

  Functor(Functor &&other) noexcept : m_impl(std::move(other.m_impl)) {}

  _Allow_implicit_ctor_ Functor(DoNothingFunctor) noexcept : Functor(DoNothing()) {}

  template <typename T, EnableIfIFunctor<T> = 0>
  _Allow_implicit_ctor_ Functor(_In_ T *impl) noexcept : m_impl{impl} {}

  template <typename T, EnableIfIFunctor<T> = 0>
  _Allow_implicit_ctor_ Functor(_In_ T *impl, AttachTagType tag) noexcept : m_impl{impl, tag} {}

  template <typename T, EnableIfIFunctor<T> = 0>
  _Allow_implicit_ctor_ Functor(Mso::CntPtr<T> &impl) noexcept : m_impl(impl) {}

  template <typename T, EnableIfIFunctor<T> = 0>
  _Allow_implicit_ctor_ Functor(const Mso::CntPtr<T> &impl) noexcept : m_impl(impl) {}

  template <typename T, EnableIfIFunctor<T> = 0>
  _Allow_implicit_ctor_ Functor(Mso::CntPtr<T> &&impl) noexcept : m_impl(std::move(impl)) {}

  template <typename T, EnableIfFunctionObject<T> = 0, EnableIfNoThrow<T> = 0, EnableIfNotStateless<T> = 0>
  _Allow_implicit_ctor_ Functor(T &&func) noexcept
      : m_impl{MakeFunctionObjectWrapper(std::forward<T>(func)), AttachTag} {}

  template <typename T, EnableIfFunctionObject<T> = 0, EnableIfNoThrow<T> = 0, EnableIfStateless<T> = 0>
  _Allow_implicit_ctor_ Functor(T &&func) noexcept {
    using TFunc = Mso::Details::Decay_t<T>;
    static TFunc s_func{std::forward<T>(func)};
    static constexpr Mso::Details::StatelessFunctorWrapper<TFunc, TResult, TArgs...> s_impl{s_func};
    m_impl = Mso::CntPtr{const_cast<IFunctor *>(static_cast<const IFunctor *>(&s_impl)), AttachTag};
  }

  template <typename T, EnableIfFunctionObject<T> = 0, EnableIfThrow<T> = 0>
  _SA_deprecated_(
      lambda must be noexcept or use Functor constructor with TerminateOnException argument or use Mso::FunctorThrow)
      Functor(T &&func) noexcept
      : m_impl{MakeFunctionObjectWrapper(std::forward<T>(func)), AttachTag} {}

  //! Explicitly wraps up throwing objects such as std::function<>.
  //! It should be used only in places where we cannot make function object noexcept.
  template <typename T, EnableIfFunctionObject<T> = 0>
  Functor(T &&func, const Mso::TerminateOnExceptionTag &) noexcept
      : m_impl{MakeFunctionObjectWrapper(std::forward<T>(func)), AttachTag} {}

  Functor &operator=(const Functor &other) noexcept {
    m_impl = other.m_impl;
    return *this;
  }

  Functor &operator=(Functor &&other) noexcept {
    m_impl = std::move(other.m_impl);
    return *this;
  }

  TResult operator()(TArgs... args) const noexcept {
    // It's important that we don't use '&&' on TArgs here. Since TArgs is a class template parameter,
    // '&&' would force each argument to become an rvalue (or, subject to ref-collapsing, an lvalue ref)
    // which could prevent us from being able to correctly invoke the underlying function object. With
    // that said, it is important that the wrapper's 'Invoke' use '&&' to prevent a second copy of any
    // non-ref-qualified parameter since a copy would already have been made here.

    return m_impl->Invoke(std::forward<TArgs>(args)...);
  }

  bool IsEmpty() const noexcept {
    return m_impl.IsEmpty();
  }

  explicit operator bool() const noexcept {
    return !m_impl.IsEmpty();
  }

  void Swap(Functor &other) noexcept {
    using std::swap;
    swap(m_impl, other.m_impl);
  }

  IFunctor *Get() const noexcept {
    return m_impl.Get();
  }

  IFunctor *Detach() noexcept {
    return m_impl.Detach();
  }

  //! Wraps a function pointer in a Functor, without allocating memory.
  //! Note that the function is passed as a template argument, not as a function argument.
  template <TResult (*func)(TArgs...)>
  static Functor FromFunctionPtr() noexcept {
    static constexpr Mso::Details::FunctionPointerWrapper<TResult (*)(TArgs...), TResult, TArgs...> s_func{func};
    return {const_cast<IFunctor *>(static_cast<const IFunctor *>(&s_func)), AttachTag};
  }

  //! Returns a no-op Functor, without allocating memory.
  static Functor DoNothing() noexcept {
    return FromFunctionPtr<&Mso::Details::DoNothingFunction<TResult, TArgs...>>();
  }

 private:
  template <typename T>
  static IFunctor *MakeFunctionObjectWrapper(T &&func) noexcept {
    using TFunc = Mso::Details::Decay_t<T>;
    using Type = Mso::Details::FunctionObjectWrapper<TFunc, TResult, TArgs...>;
    void *objMemory = Type::RefCountPolicy::Allocator::Allocate(sizeof(Type));
    VerifyAllocElseCrashTag(objMemory, 0x0131f462 /* tag_bm5r8 */);
    return ::new (objMemory) Type(std::forward<T>(func));
  }

 private:
  Mso::CntPtr<IFunctor> m_impl;
};

#if defined(__cpp_noexcept_function_type) || (_HAS_NOEXCEPT_FUNCTION_TYPES == 1)

// Treat the noexcept in function signature the same way as if it was not there.

template <typename TResult, typename... TArgs>
class Functor<TResult(TArgs...) noexcept> : public Functor<TResult(TArgs...)> {
 public:
  using Functor<TResult(TArgs...)>::Functor;

  static Functor DoNothing() noexcept {
    return Functor(DoNothingFunctor());
  }
};

#endif

/**
  FunctorThrow is a smart pointer to a IFunctorThrow instance.
  Its constructor accepts either a CntPtr<IFunctorThrow> for custom implementations or a function object.
  Function object is a class with a call operator(). Lambda is a function object. Function object may throw.
  FunctorThrow can work with function pointers without heap allocations using FromFunctionPtr.
*/
template <typename TResult, typename... TArgs>
class FunctorThrow<TResult(TArgs...)> {
 public:
  using IFunctorThrow = Mso::IFunctorThrow<TResult, TArgs...>;

 private:
  template <typename T>
  using EnableIfIFunctorThrow = std::enable_if_t<std::is_convertible<T *, IFunctorThrow *>::value, int>;
  template <typename T>
  using EnableIfFunctionObject = std::enable_if_t<
      Mso::Details::IsFunctionObject<T, TResult, TArgs...>::Value &&
          !std::is_convertible<Mso::Details::Decay_t<T> *, IFunctorThrow *>::value &&
          !Mso::Details::IsMsoFunctorThrow<Mso::Details::Decay_t<T>>::value,
      int>;
  template <typename T>
  using EnableIfStateless = std::enable_if_t<std::is_empty<T>::value, int>;
  template <typename T>
  using EnableIfNotStateless = std::enable_if_t<!std::is_empty<T>::value, int>;

 public:
  FunctorThrow() noexcept {}

  _Allow_implicit_ctor_ FunctorThrow(std::nullptr_t) noexcept {}

  FunctorThrow(const FunctorThrow &other) noexcept : m_impl(other.m_impl) {}

  FunctorThrow(FunctorThrow &&other) noexcept : m_impl(std::move(other.m_impl)) {}

  _Allow_implicit_ctor_ FunctorThrow(DoNothingFunctor) noexcept : FunctorThrow(DoNothing()) {}

  template <typename T, EnableIfIFunctorThrow<T> = 0>
  _Allow_implicit_ctor_ FunctorThrow(_In_ T *impl) noexcept : m_impl{impl} {}

  template <typename T, EnableIfIFunctorThrow<T> = 0>
  _Allow_implicit_ctor_ FunctorThrow(_In_ T *impl, AttachTagType tag) noexcept : m_impl{impl, tag} {}

  template <typename T, EnableIfIFunctorThrow<T> = 0>
  _Allow_implicit_ctor_ FunctorThrow(Mso::CntPtr<T> &impl) noexcept : m_impl(impl) {}

  template <typename T, EnableIfIFunctorThrow<T> = 0>
  _Allow_implicit_ctor_ FunctorThrow(const Mso::CntPtr<T> &impl) noexcept : m_impl(impl) {}

  template <typename T, EnableIfIFunctorThrow<T> = 0>
  _Allow_implicit_ctor_ FunctorThrow(Mso::CntPtr<T> &&impl) noexcept : m_impl(std::move(impl)) {}

  template <typename T, EnableIfFunctionObject<T> = 0, EnableIfNotStateless<T> = 0>
  _Allow_implicit_ctor_ FunctorThrow(T &&func) noexcept
      : m_impl{MakeFunctionObjectWrapper(std::forward<T>(func)), AttachTag} {}

  template <typename T, EnableIfFunctionObject<T> = 0, EnableIfStateless<T> = 0>
  _Allow_implicit_ctor_ FunctorThrow(T &&func) noexcept {
    using TFunc = Mso::Details::Decay_t<T>;
    static TFunc s_func{std::forward<T>(func)};
    static constexpr Mso::Details::StatelessFunctorWrapperThrow<TFunc, TResult, TArgs...> s_impl{s_func};
    m_impl = Mso::CntPtr{const_cast<IFunctorThrow *>(static_cast<const IFunctorThrow *>(&s_impl)), AttachTag};
  }

  FunctorThrow &operator=(const FunctorThrow &other) noexcept {
    m_impl = other.m_impl;
    return *this;
  }

  FunctorThrow &operator=(FunctorThrow &&other) noexcept {
    m_impl = std::move(other.m_impl);
    return *this;
  }

  TResult operator()(TArgs... args) const {
    // It's important that we don't use '&&' on TArgs here. Since TArgs is a class template parameter,
    // '&&' would force each argument to become an rvalue (or, subject to ref-collapsing, an lvalue ref)
    // which could prevent us from being able to correctly invoke the underlying function object. With
    // that said, it is important that the wrapper's 'Invoke' use '&&' to prevent a second copy of any
    // non-ref-qualified parameter since a copy would already have been made here.

    return m_impl->Invoke(std::forward<TArgs>(args)...);
  }

  bool IsEmpty() const noexcept {
    return m_impl.IsEmpty();
  }

  explicit operator bool() const noexcept {
    return !m_impl.IsEmpty();
  }

  void Swap(FunctorThrow &other) noexcept {
    using std::swap;
    swap(m_impl, other.m_impl);
  }

  IFunctorThrow *Get() const noexcept {
    return m_impl.Get();
  }

  IFunctorThrow *Detach() noexcept {
    return m_impl.Detach();
  }

  //! Wraps a function pointer in a FunctorThrow, without allocating memory.
  //! Note that the function is passed as a template argument, not as a function argument.
  template <TResult (*func)(TArgs...)>
  static FunctorThrow FromFunctionPtr() noexcept {
    static constexpr Mso::Details::FunctionPointerWrapperThrow<TResult (*)(TArgs...), TResult, TArgs...> s_func{func};
    return {const_cast<IFunctorThrow *>(static_cast<const IFunctorThrow *>(&s_func)), AttachTag};
  }

  //! Returns a no-op FunctorThrow, without allocating memory.
  static FunctorThrow DoNothing() noexcept {
    return FromFunctionPtr<&Mso::Details::DoNothingFunction<TResult, TArgs...>>();
  }

 private:
  template <typename T>
  static IFunctorThrow *MakeFunctionObjectWrapper(T &&func) noexcept {
    using TFunc = Mso::Details::Decay_t<T>;
    using Type = Mso::Details::FunctionObjectWrapperThrow<TFunc, TResult, TArgs...>;
    void *objMemory = Type::RefCountPolicy::Allocator::Allocate(sizeof(Type));
    VerifyAllocElseCrashTag(objMemory, 0x0131f463 /* tag_bm5r9 */);
    return ::new (objMemory) Type(std::forward<T>(func));
  }

 private:
  Mso::CntPtr<IFunctorThrow> m_impl;
};

template <typename T>
inline bool operator==(const Functor<T> &left, const Functor<T> &right) noexcept {
  return left.Get() == right.Get();
}

template <typename T>
inline bool operator==(std::nullptr_t, const Functor<T> &right) noexcept {
  return nullptr == right.Get();
}

template <typename T>
inline bool operator==(const Functor<T> &left, std::nullptr_t) noexcept {
  return left.Get() == nullptr;
}

template <typename T>
inline bool operator!=(const Functor<T> &left, const Functor<T> &right) noexcept {
  return left.Get() != right.Get();
}

template <typename T>
inline bool operator!=(std::nullptr_t, const Functor<T> &right) noexcept {
  return nullptr != right.Get();
}

template <typename T>
inline bool operator!=(const Functor<T> &left, std::nullptr_t) noexcept {
  return left.Get() != nullptr;
}

template <typename T>
inline bool operator==(const FunctorThrow<T> &left, const FunctorThrow<T> &right) noexcept {
  return left.Get() == right.Get();
}

template <typename T>
inline bool operator==(std::nullptr_t, const FunctorThrow<T> &right) noexcept {
  return nullptr == right.Get();
}

template <typename T>
inline bool operator==(const FunctorThrow<T> &left, std::nullptr_t) noexcept {
  return left.Get() == nullptr;
}

template <typename T>
inline bool operator!=(const FunctorThrow<T> &left, const FunctorThrow<T> &right) noexcept {
  return left.Get() != right.Get();
}

template <typename T>
inline bool operator!=(std::nullptr_t, const FunctorThrow<T> &right) noexcept {
  return nullptr != right.Get();
}

template <typename T>
inline bool operator!=(const FunctorThrow<T> &left, std::nullptr_t) noexcept {
  return left.Get() != nullptr;
}

// Aliases for the most common function object type "void()".
using IVoidFunctor = IFunctor<void>;
using VoidFunctor = Functor<void()>;
using VoidFunctorImpl = Mso::UnknownObject<Mso::RefCountStrategy::SimpleNoQuery, IVoidFunctor>;

using IVoidFunctorThrow = IFunctorThrow<void>;
using VoidFunctorThrow = FunctorThrow<void()>;
using VoidFunctorThrowImpl = Mso::UnknownObject<Mso::RefCountStrategy::SimpleNoQuery, IVoidFunctorThrow>;

} // namespace Mso

// We have to define swap function overrides in std namespace because they depend on a template parameter T.
namespace std {

template <typename T>
inline void swap(Mso::Functor<T> &left, Mso::Functor<T> &right) noexcept {
  left.Swap(right);
}

template <typename T>
inline void swap(Mso::FunctorThrow<T> &left, Mso::FunctorThrow<T> &right) noexcept {
  left.Swap(right);
}

} // namespace std

#endif // MSO_FUNCTIONAL_FUNCTOR_H
