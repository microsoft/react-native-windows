// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// It must go first because of VerifyElseCrash macro definition
#include "future/future.h"

#include <functional>
#include "CppWinRTIncludes.h"
#include "JSValue.h"
#include "NativeModules.h"

#undef GetCurrentTime

using namespace winrt;

namespace winrt::Microsoft::ReactNative {

struct ReactModuleBuilderMock {
  ReactModuleBuilderMock() noexcept;

  template <class... TArgs>
  void Call0(std::wstring const &methodName, TArgs &&... args) noexcept;

  template <class... TArgs, class... TReasolveArgs>
  Mso::Future<bool>
  Call1(std::wstring const &methodName, std::function<void(TReasolveArgs...)> &&resolve, TArgs &&... args) noexcept;

  template <class... TArgs, class... TResolveArgs, class... TRejectArgs>
  Mso::Future<bool> Call2(
      std::wstring const &methodName,
      std::function<void(TResolveArgs...)> const &resolve,
      std::function<void(TRejectArgs...)> const &reject,
      TArgs &&... args) noexcept;

  template <class TResult, class... TArgs>
  void CallSync(std::wstring const &methodName, TResult &result, TArgs &&... args) noexcept;

  JSValueObject GetConstants() noexcept;

  bool IsResolveCallbackCalled() const noexcept;
  void IsResolveCallbackCalled(bool value) noexcept;
  bool IsRejectCallbackCalled() const noexcept;
  void IsRejectCallbackCalled(bool value) noexcept;

  void ExpectEvent(
      std::wstring_view eventEmitterName,
      std::wstring_view eventName,
      Mso::Functor<void(JSValueArray const &)> &&checkValues) noexcept;

  void ExpectFunction(
      std::wstring_view moduleName,
      std::wstring_view functionName,
      Mso::Functor<void(JSValueArray const &)> &&checkValues) noexcept;

  void CallJSFunction(
      std::wstring_view moduleName,
      std::wstring_view functionName,
      JSValueArgWriter const &paramsArgWriter) noexcept;

  void EmitJSEvent(
      std::wstring_view eventEmitterName,
      std::wstring_view eventName,
      JSValueArgWriter const &paramsArgWriter) noexcept;

  Windows::Foundation::IInspectable CreateModule(
      ReactModuleProvider const &provider,
      IReactModuleBuilder const &moduleBuilder) noexcept;

 public: // IReactModuleBuilder
  void AddInitializer(InitializerDelegate const &initializer) noexcept;
  void AddConstantProvider(ConstantProviderDelegate const &constantProvider) noexcept;
  void AddMethod(hstring const &name, MethodReturnType returnType, MethodDelegate const &method) noexcept;
  void AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept;

 private:
  MethodDelegate GetMethod0(std::wstring const &methodName) const noexcept;
  MethodDelegate GetMethod1(std::wstring const &methodName) const noexcept;
  MethodDelegate GetMethod2(std::wstring const &methodName) const noexcept;
  SyncMethodDelegate GetSyncMethod(std::wstring const &methodName) const noexcept;

  static IJSValueWriter ArgWriter() noexcept;
  template <class... TArgs>
  static IJSValueReader ArgReader(TArgs &&... args) noexcept;
  static IJSValueReader CreateArgReader(std::function<void(IJSValueWriter const &)> const &argWriter) noexcept;

  template <class... TArgs, size_t... I>
  MethodResultCallback ResolveCallback(
      std::function<void(TArgs...)> const &resolve,
      std::index_sequence<I...>,
      Mso::Promise<bool> const &promise) noexcept;
  template <class... TArgs, size_t... I>
  MethodResultCallback RejectCallback(
      std::function<void(TArgs...)> const &reject,
      std::index_sequence<I...>,
      Mso::Promise<bool> const &promise) noexcept;

 private:
  IReactContext m_reactContext{nullptr};
  std::vector<InitializerDelegate> m_initializers;
  std::vector<ConstantProviderDelegate> m_constantProviders;
  std::map<std::wstring, std::tuple<MethodReturnType, MethodDelegate>> m_methods;
  std::map<std::wstring, SyncMethodDelegate> m_syncMethods;
  bool m_isResolveCallbackCalled{false};
  bool m_isRejectCallbackCalled{false};
  Mso::Functor<void(std::wstring_view, std::wstring_view, JSValue const &)> m_jsFunctionHandler;
  Mso::Functor<void(std::wstring_view, std::wstring_view, JSValue const &)> m_jsEventHandler;
};

struct ReactContextMock : implements<ReactContextMock, IReactContext> {
  ReactContextMock(ReactModuleBuilderMock *builderMock) noexcept;

  IReactSettingsSnapshot SettingsSnapshot() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  IReactPropertyBag Properties() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  IReactNotificationService Notifications() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  IReactDispatcher UIDispatcher() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  IReactDispatcher JSDispatcher() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  IInspectable JSRuntime() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  void DispatchEvent(
      xaml::FrameworkElement const & /*view*/,
      hstring const & /*eventName*/,
      JSValueArgWriter const & /*eventDataArgWriter*/) noexcept {}

  void CallJSFunction(
      hstring const &moduleName,
      hstring const &functionName,
      JSValueArgWriter const &paramsArgWriter) noexcept;

  void EmitJSEvent(
      hstring const &eventEmitterName,
      hstring const &eventName,
      JSValueArgWriter const &paramsArgWriter) noexcept;

  uint16_t DebuggerPort() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  bool DebuggerBreakOnNextLine() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  bool UseDirectDebugger() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  bool UseFastRefresh() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  bool UseWebDebugger() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  hstring DebugBundlePath() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  hstring BundleRootPath() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  hstring SourceBundleHost() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  uint16_t SourceBundlePort() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  hstring JavaScriptBundleFile() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

 private:
  ReactModuleBuilderMock *m_builderMock;
};

struct ReactModuleBuilderImpl : implements<ReactModuleBuilderImpl, IReactModuleBuilder> {
  ReactModuleBuilderImpl(ReactModuleBuilderMock &mock) noexcept;

 public: // IReactModuleBuilder
  void AddInitializer(InitializerDelegate const &initializer) noexcept;
  void AddConstantProvider(ConstantProviderDelegate const &constantProvider) noexcept;
  void AddMethod(hstring const &name, MethodReturnType returnType, MethodDelegate const &method) noexcept;
  void AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept;

 private:
  ReactModuleBuilderMock &m_mock;
};

//===========================================================================
// ReactModuleBuilderMock inline implementation
//===========================================================================

template <class... TArgs>
inline void ReactModuleBuilderMock::Call0(std::wstring const &methodName, TArgs &&... args) noexcept {
  if (auto method = GetMethod0(methodName)) {
    method(ArgReader(std::forward<TArgs>(args)...), ArgWriter(), nullptr, nullptr);
  }
}

template <class... TArgs, class... TResolveArgs>
inline Mso::Future<bool> ReactModuleBuilderMock::Call1(
    std::wstring const &methodName,
    std::function<void(TResolveArgs...)> &&resolve,
    TArgs &&... args) noexcept {
  Mso::Promise<bool> promise;
  if (auto method = GetMethod1(methodName)) {
    method(
        ArgReader(std::forward<TArgs>(args)...),
        ArgWriter(),
        ResolveCallback(resolve, std::make_index_sequence<sizeof...(TResolveArgs)>{}, promise),
        nullptr);
  }
  return promise.AsFuture();
}

template <class... TArgs, class... TResolveArgs, class... TRejectArgs>
inline Mso::Future<bool> ReactModuleBuilderMock::Call2(
    std::wstring const &methodName,
    std::function<void(TResolveArgs...)> const &resolve,
    std::function<void(TRejectArgs...)> const &reject,
    TArgs &&... args) noexcept {
  Mso::Promise<bool> promise;
  if (auto method = GetMethod2(methodName)) {
    method(
        ArgReader(std::forward<TArgs>(args)...),
        ArgWriter(),
        ResolveCallback(resolve, std::make_index_sequence<sizeof...(TResolveArgs)>{}, promise),
        RejectCallback(reject, std::make_index_sequence<sizeof...(TRejectArgs)>{}, promise));
  }
  return promise.AsFuture();
}

template <class TResult, class... TArgs>
inline void
ReactModuleBuilderMock::CallSync(std::wstring const &methodName, TResult &result, TArgs &&... args) noexcept {
  if (auto method = GetSyncMethod(methodName)) {
    auto writer = ArgWriter();
    method(ArgReader(std::forward<TArgs>(args)...), writer);
    ReadValue(MakeJSValueTreeReader(TakeJSValue(writer)), result);
  }
}

template <class... TArgs>
inline /*static*/ IJSValueReader ReactModuleBuilderMock::ArgReader(TArgs &&... args) noexcept {
  return CreateArgReader(
      [&args...](IJSValueWriter const &writer) mutable noexcept { WriteArgs(writer, std::forward<TArgs>(args)...); });
}

template <class... TArgs, size_t... I>
inline MethodResultCallback ReactModuleBuilderMock::ResolveCallback(
    std::function<void(TArgs...)> const &resolve,
    std::index_sequence<I...>,
    Mso::Promise<bool> const &promise) noexcept {
  return [this, resolve, promise](IJSValueWriter const &writer) noexcept {
    std::tuple<RemoveConstRef<TArgs>...> args;
    ReadArgs(MakeJSValueTreeReader(TakeJSValue(writer)), std::get<I>(args)...);
    resolve(std::get<I>(args)...);
    m_isResolveCallbackCalled = true;
    promise.TrySetValue(true);
  };
}

template <class... TArgs, size_t... I>
inline MethodResultCallback ReactModuleBuilderMock::RejectCallback(
    std::function<void(TArgs...)> const &reject,
    std::index_sequence<I...>,
    Mso::Promise<bool> const &promise) noexcept {
  return [this, reject, promise](IJSValueWriter const &writer) noexcept {
    std::tuple<RemoveConstRef<TArgs>...> args;
    ReadArgs(MakeJSValueTreeReader(TakeJSValue(writer)), std::get<I>(args)...);
    reject(std::get<I>(args)...);
    m_isRejectCallbackCalled = true;
    promise.TrySetValue(false);
  };
}

inline bool ReactModuleBuilderMock::IsResolveCallbackCalled() const noexcept {
  return m_isResolveCallbackCalled;
}

inline void ReactModuleBuilderMock::IsResolveCallbackCalled(bool value) noexcept {
  m_isResolveCallbackCalled = value;
}

inline bool ReactModuleBuilderMock::IsRejectCallbackCalled() const noexcept {
  return m_isRejectCallbackCalled;
}

inline void ReactModuleBuilderMock::IsRejectCallbackCalled(bool value) noexcept {
  m_isRejectCallbackCalled = value;
}

//===========================================================================
// ReactModuleBuilderImpl inline implementation
//===========================================================================

inline ReactModuleBuilderImpl::ReactModuleBuilderImpl(ReactModuleBuilderMock &mock) noexcept : m_mock{mock} {}

inline void ReactModuleBuilderImpl::AddInitializer(InitializerDelegate const &initializer) noexcept {
  m_mock.AddInitializer(initializer);
}

inline void ReactModuleBuilderImpl::AddConstantProvider(ConstantProviderDelegate const &constantProvider) noexcept {
  m_mock.AddConstantProvider(constantProvider);
}

inline void ReactModuleBuilderImpl::AddMethod(
    hstring const &name,
    MethodReturnType returnType,
    MethodDelegate const &method) noexcept {
  m_mock.AddMethod(name, returnType, method);
}

inline void ReactModuleBuilderImpl::AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept {
  m_mock.AddSyncMethod(name, method);
}

} // namespace winrt::Microsoft::ReactNative
