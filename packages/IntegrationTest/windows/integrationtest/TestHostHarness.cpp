// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TestHostHarness.h"
#include "TestModule.h"

#include <JSValue.h>

using namespace std::chrono;
using namespace std::chrono_literals;

using namespace winrt::integrationtest;
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Data::Json;

namespace IntegrationTest {

TestHostHarness::TestHostHarness() noexcept {
  m_redboxHandler = winrt::make<TestHostHarnessRedboxHandler>(get_weak());
  m_commandListener = winrt::make_self<TestCommandListener>();

  m_commandListener->OnTestCommand([weakThis{get_weak()}](
      const TestCommand &command, TestCommandResponse response) noexcept {
    if (auto strongThis = weakThis.get()) {
      strongThis->OnTestCommand(command, std::move(response));
    }
  });
}

void TestHostHarness::SetRootView(ReactRootView &&rootView) noexcept {
  m_rootView = std::move(rootView);
}

void TestHostHarness::SetReactHost(ReactNativeHost &&reactHost) noexcept {
  reactHost.InstanceSettings().RedBoxHandler(m_redboxHandler);
  m_instanceLoadedRevoker = reactHost.InstanceSettings().InstanceLoaded(
      winrt::auto_revoke, [weakThis{get_weak()}](const auto & /*sender*/, const auto &args) noexcept {
        auto strongThis = weakThis.get();
        if (!strongThis) {
          return;
        }

        ReactContext context(args.Context());

        strongThis->m_context = context;
        strongThis->StartListening();

        context.Notifications().Subscribe(
            TestModule::TestCompletedEvent(),
            context.UIDispatcher(),
            [weakThis](const auto & /*sender*/, ReactNotificationArgs<TestCompletedEventArgs> args) noexcept {
              if (auto strongThis = weakThis.get()) {
                strongThis->OnTestCompleted(*args.Data());
              }
            });

        context.Notifications().Subscribe(
            TestModule::TestPassedEvent(),
            context.UIDispatcher(),
            [weakThis](const auto & /*sender*/, ReactNotificationArgs<TestPassedEventArgs> args) noexcept {
              if (auto strongThis = weakThis.get()) {
                strongThis->OnTestPassed(args.Data()->first, args.Data()->second);
              }
            });
      });
}

winrt::fire_and_forget TestHostHarness::StartListening() noexcept {
  auto listenResult = co_await m_commandListener->StartListening();

  switch (listenResult) {
    case ListenResult::Success:
      co_return;

    case ListenResult::AddressInUse:
      ShowJSError("Test harness address is already in use. Is there another instance running?");
      co_return;

    case ListenResult::OtherError:
      ShowJSError("The test harness ran into an unexpected error trying to listen for commands.");
      co_return;

    default:
      ShowJSError("Unexpected ListenResult from TestCommandListener.");
      co_return;
  }
}

void TestHostHarness::OnTestCommand(const TestCommand &command, TestCommandResponse &&response) noexcept {
  m_pendingResponse = std::move(response);
  m_commandStartTime = std::chrono::steady_clock::now();

  switch (command.Id) {
    case TestCommandId::RunTestComponent: {
      auto componentName = command.payload.GetString();
      m_rootView.ComponentName(componentName);
      m_timeoutAction = TimeoutIfNoResponse();
      break;
    }

    case TestCommandId::GoToComponent: {
      auto componentName = command.payload.GetString();
      m_rootView.ComponentName(componentName);
      m_pendingResponse->Okay();
      CompletePendingResponse();
      break;
    }

    default: {
      ShowJSError("Unexpected command ID from test runner");
      m_pendingResponse->UnknownError("Unexpected command ID from test runner");
      CompletePendingResponse();
      break;
    }
  }
}

void TestHostHarness::OnTestCompleted(steady_clock::time_point eventTime) noexcept {
  OnTestPassed(eventTime, true);
}

void TestHostHarness::OnTestPassed(steady_clock::time_point eventTime, bool passed) noexcept {
  // Some tests like AppEventsTest can misbehave and fire many events at once
  // if reloaded. We don't have corelation IDs, so just try to filter out
  // events the JS thread saw before we started the current command.
  if (eventTime < m_commandStartTime) {
    return;
  }

  if (m_pendingResponse) {
    m_pendingResponse->TestPassed(passed);
    CompletePendingResponse();
  }
}

IAsyncAction TestHostHarness::TimeoutIfNoResponse() noexcept {
  constexpr auto CommandTimeout = 20s;

  auto weakThis = get_weak();
  co_await CommandTimeout;

  auto cancellationToken = co_await winrt::get_cancellation_token();
  if (auto strongThis = weakThis.get() && m_pendingResponse && !cancellationToken()) {
    m_pendingResponse->Timeout();
    CompletePendingResponse();
  }
}

void TestHostHarness::CompletePendingResponse() noexcept {
  if (m_pendingResponse) {
    m_pendingResponse.reset();

    if (m_timeoutAction) {
      m_timeoutAction.Cancel();
    }
  }
}

void TestHostHarness::ShowJSError(std::string_view err) noexcept {
  m_context.CallJSFunction(L"RCTLog", L"logToConsole", "error", err);
}

void TestHostHarnessRedboxHandler::ShowNewError(IRedBoxErrorInfo info, RedBoxErrorType /*type*/) {
  if (auto strongHarness = m_weakHarness.get()) {
    if (strongHarness->m_pendingResponse) {
      strongHarness->m_pendingResponse->Exception(info);
      strongHarness->CompletePendingResponse();
    }
  }
}
bool TestHostHarnessRedboxHandler::IsDevSupportEnabled() {
  // We always want errors
  return true;
}
void TestHostHarnessRedboxHandler::UpdateError(IRedBoxErrorInfo) {
  // Nothing to do
}
void TestHostHarnessRedboxHandler::DismissRedBox() {
  // Nothing to do
}

} // namespace IntegrationTest
