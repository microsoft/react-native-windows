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
                if (strongThis->m_currentTransaction) {
                  strongThis->HandleHostAction(strongThis->m_currentTransaction->OnTestModuleTestCompleted());
                }
              }
            });

        context.Notifications().Subscribe(
            TestModule::TestPassedEvent(),
            context.UIDispatcher(),
            [weakThis](const auto & /*sender*/, ReactNotificationArgs<bool> args) noexcept {
              if (auto strongThis = weakThis.get()) {
                if (strongThis->m_currentTransaction) {
                  strongThis->HandleHostAction(strongThis->m_currentTransaction->OnTestModuleTestPassed(*args.Data()));
                }
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

  m_pendingResponse = std::move(response);

  switch (command.Id) {
    case TestCommandId::RunTestComponent: {
      auto componentName = command.payload.GetString();
      m_rootView.ComponentName(componentName);

      m_currentTransaction = winrt::make_self<TestTransaction>();
      TimeoutOnInactivty(m_currentTransaction->get_weak());
      break;
    }

    case TestCommandId::GoToComponent: {
      auto componentName = command.payload.GetString();
      m_rootView.ComponentName(componentName);

      co_await FlushJSQueue();
      m_pendingResponse->Okay();
      m_pendingResponse.reset();
      break;
    }

    default: {
      ShowJSError("Unexpected command ID from test runner");
      response.Error("Unexpected command ID from test runner");
      break;
    }
  }
}

winrt::fire_and_forget TestHostHarness::TimeoutOnInactivty(winrt::weak_ref<TestTransaction> transaction) noexcept {
  constexpr auto CommandTimeout = 20s;

  auto weakThis = get_weak();
  co_await CommandTimeout;

  if (auto strongTransaction = transaction.get()) {
    if (auto strongThis = weakThis.get()) {
      if (m_currentTransaction == strongTransaction) {
        HandleHostAction(m_currentTransaction->OnTimeout());
      }
    }
  }
}

winrt::fire_and_forget TestHostHarness::HandleHostAction(HostAction action) noexcept {
  switch (action) {
    case HostAction::Continue:
      break;

    case HostAction::FlushEvents:
      // TODO: This only handles a subset of synchronization issues. We should
      // expose lifetime events so that we can wait until the root component is
      // loaded/unloaded before continuing
      co_await FlushJSQueue();
      HandleHostAction(m_currentTransaction->OnEventsFlushed());
      break;

    case HostAction::SubmitResult:
      m_currentTransaction->SubmitResult(*m_pendingResponse);
      m_pendingResponse.reset();
      m_currentTransaction = nullptr;
      break;

    default:
      VerifyElseCrash(false);
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

void TestHostHarnessRedboxHandler::ShowNewError(const IRedBoxErrorInfo &info, RedBoxErrorType /*type*/) noexcept {
  QueueToUI([info](TestHostHarness & harness) noexcept {
    if (harness.m_currentTransaction) {
      harness.HandleHostAction(harness.m_currentTransaction->OnNewError(info));
    }
  });
}

bool TestHostHarnessRedboxHandler::IsDevSupportEnabled() noexcept {
  // We always want errors
  return true;
}

void TestHostHarnessRedboxHandler::UpdateError(const IRedBoxErrorInfo &info) noexcept {
  QueueToUI([info](TestHostHarness & harness) noexcept {
    if (harness.m_currentTransaction) {
      harness.HandleHostAction(harness.m_currentTransaction->OnUpdateError(info));
    }
  });
}

void TestHostHarnessRedboxHandler::DismissRedBox() noexcept {
  // Nothing to do
}

template <typename TFunc>
void TestHostHarnessRedboxHandler::QueueToUI(TFunc &&func) noexcept {
  if (auto strongHarness = m_weakHarness.get()) {
    if (strongHarness->m_context) {
      strongHarness->m_context.UIDispatcher().Post(
          [ strongHarness, func{std::move(func)} ]() noexcept { func(*strongHarness); });
    }
  }
}

} // namespace IntegrationTest
