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

        if (!strongThis->m_commandListener->IsListening()) {
          strongThis->StartListening();
        }

        ReactContext context(args.Context());
        strongThis->m_context = context;

        context.Notifications().Subscribe(
            TestModule::TestCompletedEvent(),
            context.UIDispatcher(),
            [weakThis](const auto & /*sender*/, ReactNotificationArgs<void> /*args*/) noexcept {
              if (auto strongThis = weakThis.get()) {
                strongThis->OnTestCompleted();
              }
            });

        context.Notifications().Subscribe(
            TestModule::TestPassedEvent(),
            context.UIDispatcher(),
            [weakThis](const auto & /*sender*/, ReactNotificationArgs<bool> args) noexcept {
              if (auto strongThis = weakThis.get()) {
                strongThis->OnTestPassed(*args.Data());
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

winrt::fire_and_forget TestHostHarness::OnTestCommand(TestCommand command, TestCommandResponse response) noexcept {
  // Keep ourselves alive while we have a test command
  auto strongThis = get_strong();

  if (m_pendingResponse) {
    response.Error("Received a test command while still processing the previous");
    co_return;
  }

  switch (command.Id) {
    case TestCommandId::RunTestComponent: {
      auto componentName = command.payload.GetString();
      m_rootView.ComponentName(componentName);

      // We might see multiple JS events for exceptions, failing tests, etc.
      // Flush everything pending from the JS thread to UI thread before
      // continuing to ensure all events from the past command are handled.

      // TODO: This only handles a subset of synchronization issues. We should
      // expose lifetime events so that we can wait until the root component is
      // loaded/unloaded before continuing
      co_await FlushJSQueue();

      m_pendingResponse = std::move(response);
      m_timeoutAction = TimeoutOnInactivty();
      break;
    }

    case TestCommandId::GoToComponent: {
      auto componentName = command.payload.GetString();
      m_rootView.ComponentName(componentName);
      co_await FlushJSQueue();
      response.Okay();
      break;
    }

    default: {
      ShowJSError("Unexpected command ID from test runner");
      response.Error("Unexpected command ID from test runner");
      break;
    }
  }
}

void TestHostHarness::OnTestCompleted() noexcept {
  OnTestPassed(true);
}

void TestHostHarness::OnTestPassed(bool passed) noexcept {
  if (m_pendingResponse) {
    if (passed) {
      m_pendingResponse->TestPassed();
    } else {
      m_pendingResponse->TestFailed();
    }

    CompletePendingResponse();
  }
}

IAsyncAction TestHostHarness::TimeoutOnInactivty() noexcept {
  constexpr auto CommandTimeout = 20s;

  auto weakThis = get_weak();
  co_await CommandTimeout;

  auto cancellationToken = co_await winrt::get_cancellation_token();
  if (auto strongThis = weakThis.get() && m_pendingResponse && !cancellationToken()) {
    m_pendingResponse->Timeout();
    CompletePendingResponse();
  }
}

IAsyncAction TestHostHarness::FlushJSQueue() noexcept {
  winrt::handle signal(CreateEvent(nullptr, false, false, nullptr));

  m_context.JSDispatcher().Post([&signal, uiDispatcher{m_context.UIDispatcher()} ]() noexcept {
    uiDispatcher.Post([&signal]() noexcept { SetEvent(signal.get()); });
  });

  co_await winrt::resume_on_signal(signal.get());
}

void TestHostHarness::ShowJSError(std::string_view err) noexcept {
  m_context.CallJSFunction(L"RCTLog", L"logToConsole", "error", err);
}

void TestHostHarness::CompletePendingResponse() noexcept {
  if (m_pendingResponse) {
    m_pendingResponse.reset();
  }

  if (m_timeoutAction) {
    m_timeoutAction.Cancel();
  }
}

void TestHostHarnessRedboxHandler::ShowNewError(IRedBoxErrorInfo info, RedBoxErrorType /*type*/) {
  if (auto strongHarness = m_weakHarness.get()) {
    if (strongHarness->m_context) {
      strongHarness->m_context.UIDispatcher().Post([info{std::move(info)}, strongHarness]() {
        if (strongHarness->m_pendingResponse) {
          strongHarness->m_pendingResponse->Exception(info);
          strongHarness->CompletePendingResponse();
        }
      });
    }
  }
}
bool TestHostHarnessRedboxHandler::IsDevSupportEnabled() {
  // We always want errors
  return true;
}
void TestHostHarnessRedboxHandler::UpdateError(IRedBoxErrorInfo /*info*/) {
  // Nothing to do
}
void TestHostHarnessRedboxHandler::DismissRedBox() {
  // Nothing to do
}

} // namespace IntegrationTest
