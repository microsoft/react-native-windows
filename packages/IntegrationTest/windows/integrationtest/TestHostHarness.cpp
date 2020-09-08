// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TestHostHarness.h"
#include "TestModule.h"

#include <JSValue.h>

using namespace winrt::integrationtest;
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Data::Json;

namespace IntegrationTest {

TestHostHarness::TestHostHarness() noexcept {
  m_redboxHandler = winrt::make<TestHostHarnessRedboxHandler>(get_weak());
  m_commandListener = winrt::make_self<TestCommandListener>();

  m_commandListener->OnTestCommand(
      [weakThis{get_weak()}](const TestCommand &command, TestCommandResponse response) noexcept {
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
        if (auto strongThis = weakThis.get()) {
          strongThis->m_context = args.Context();
          strongThis->StartListening();
        }

        ReactNotificationService notifications(args.Context().Notifications());

        notifications.Subscribe(
            TestModule::TestCompletedEvent(),
            ReactDispatcher(args.Context().UIDispatcher()),
            [weakThis](const auto &/*sender*/, ReactNotificationArgs<void> /*args*/) noexcept {
              if (auto strongThis = weakThis.get()) {
                strongThis->OnTestCompleted();
              }
            });

        notifications.Subscribe(
            TestModule::TestPassedEvent(),
            ReactDispatcher(args.Context().UIDispatcher()),
            [weakThis](const auto &/*sender*/, ReactNotificationArgs<bool> args) noexcept {
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

void TestHostHarness::OnTestCommand(const TestCommand &command, TestCommandResponse &&response) noexcept {
  m_pendingResponse = std::move(response);

  switch (command.Id) {
    case TestCommandId::RunTestComponent: {
      auto componentName = command.payload.GetString();
      m_rootView.ComponentName(componentName);
      break;
    }

    default: {
      ShowJSError("Unexpected command ID from test runner");
      m_pendingResponse->UnknownError("Unexpected command ID from test runner");
      m_pendingResponse.reset();
      break;
    }
  }
}

void TestHostHarness::OnTestCompleted() noexcept {
  OnTestPassed(true);
}

void TestHostHarness::OnTestPassed(bool passed) noexcept {
  if (m_pendingResponse) {
    m_pendingResponse->TestPassed(passed);
    m_pendingResponse.reset();
  }
}

void TestHostHarness::ShowJSError(std::string_view err) noexcept {
  m_context.CallJSFunction(L"RCTLog", L"logToConsole", "error", err);
}

void TestHostHarnessRedboxHandler::ShowNewError(IRedBoxErrorInfo info, RedBoxErrorType /*type*/) {
  if (auto strongHarness = m_weakHarness.get()) {
    if (strongHarness->m_pendingResponse) {
      strongHarness->m_pendingResponse->Exception(info);
      strongHarness->m_pendingResponse.reset();
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
