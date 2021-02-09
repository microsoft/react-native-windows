// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactContext.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/NodeRpc.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <functional>

#include "TestCommand.h"
#include "TestTransaction.h"

namespace IntegrationTest {

//! Wrapper to allow co_await on a value that may be set in the future.
template <typename T>
struct Awaitable final : public std::experimental::suspend_always {
  bool await_ready() noexcept {
    return false;
  }

  void await_suspend(std::experimental::coroutine_handle<> handle) noexcept {
    m_valueSetEvent.add([handle{std::move(handle)}]() noexcept { handle(); });
  }

  T await_resume() noexcept {
    return std::move(*m_value);
  }

  void Set(const T &&value) noexcept {
    m_value = std::move(value);
    m_valueSetEvent();
  }

 private:
  winrt::event<winrt::delegate<>> m_valueSetEvent;
  std::optional<T> m_value;
};

//! Allows controlling the ReactHost based on messages from a TestRunner
class TestHostHarness : public winrt::implements<TestHostHarness, winrt::Windows::Foundation::IInspectable> {
  friend class TestHostHarnessRedboxHandler;

 public:
  TestHostHarness(const winrt::Microsoft::ReactNative::ReactNativeHost &reactHost) noexcept;
  ~TestHostHarness() noexcept;

  void SetRootView(winrt::Microsoft::ReactNative::ReactRootView &&rootView) noexcept;

 private:
  void ShowJSError(std::string_view err) noexcept;
  void OnInstanceLoaded(const winrt::Microsoft::ReactNative::InstanceLoadedEventArgs &args) noexcept;
  winrt::Windows::Foundation::IAsyncAction StartListening() noexcept;
  winrt::NodeRpc::Handler CreateRpcHander() noexcept;
  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Data::Json::IJsonValue> OnTestCommand(
      TestCommandId command,
      winrt::Windows::Data::Json::JsonValue payload) noexcept;
  winrt::fire_and_forget TimeoutOnInactivty(winrt::weak_ref<TestTransaction> transaction) noexcept;
  winrt::fire_and_forget HandleHostAction(HostAction action) noexcept;

  winrt::Windows::Foundation::IAsyncAction FlushJSQueue() noexcept;

  winrt::Microsoft::ReactNative::ReactRootView m_rootView;
  winrt::Microsoft::ReactNative::ReactContext m_context;
  winrt::Microsoft::ReactNative::IReactInstanceSettings::InstanceLoaded_revoker m_instanceLoadedRevoker;
  winrt::Windows::Foundation::IAsyncAction m_serverListenCoro;

  winrt::NodeRpc::Server m_rpcServer;
  winrt::com_ptr<TestTransaction> m_currentTransaction;
  winrt::Microsoft::ReactNative::IRedBoxHandler m_redboxHandler;
  std::optional<Awaitable<winrt::Windows::Data::Json::IJsonValue>> m_pendingResponse;

  bool m_instanceFailedToLoad{false};

  // Note: this may be accessed by other threads
  const winrt::Microsoft::ReactNative::ReactDispatcher m_dispatcher;
};

//! Redbox handler which feeds into the TestHostHarness to communicate exceptions to the test runner
class TestHostHarnessRedboxHandler
    : public winrt::implements<TestHostHarnessRedboxHandler, winrt::Microsoft::ReactNative::IRedBoxHandler> {
 public:
  TestHostHarnessRedboxHandler(winrt::weak_ref<TestHostHarness> &&weakHarness) noexcept
      : m_weakHarness(std::move(weakHarness)) {}

  void ShowNewError(
      const winrt::Microsoft::ReactNative::IRedBoxErrorInfo &info,
      winrt::Microsoft::ReactNative::RedBoxErrorType type) noexcept;
  bool IsDevSupportEnabled() noexcept;
  void UpdateError(const winrt::Microsoft::ReactNative::IRedBoxErrorInfo &info) noexcept;
  void DismissRedBox() noexcept;

 private:
  template <typename TFunc>
  void QueueToUI(TFunc &&func) noexcept;

  winrt::weak_ref<TestHostHarness> m_weakHarness;
};
} // namespace IntegrationTest
