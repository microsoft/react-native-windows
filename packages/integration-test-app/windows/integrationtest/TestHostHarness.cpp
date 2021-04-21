// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TestHostHarness.h"
#include "TestCommandResponse.h"
#include "TestModule.h"

#include <JSValue.h>

using namespace std::chrono;
using namespace std::chrono_literals;

using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::System;

namespace IntegrationTest {

TestHostHarness::TestHostHarness(const ReactNativeHost &reactHost) noexcept
    : m_dispatcher{ReactDispatcherHelper::UIThreadDispatcher()}, m_rpcServer(CreateRpcHander()) {
  VerifyElseCrash(m_dispatcher);
  VerifyElseCrash(m_dispatcher.HasThreadAccess());

  m_redboxHandler = winrt::make<TestHostHarnessRedboxHandler>(get_weak());

  reactHost.InstanceSettings().RedBoxHandler(m_redboxHandler);
  m_instanceLoadedRevoker = reactHost.InstanceSettings().InstanceLoaded(
      winrt::auto_revoke, [weakThis{get_weak()}](const auto & /*sender*/, auto &&args) noexcept {
        if (auto strongThis = weakThis.get()) {
          strongThis->m_dispatcher.Post(
              [strongThis, args{std::move(args)}]() noexcept { strongThis->OnInstanceLoaded(args); });
        }
      });
}

TestHostHarness::~TestHostHarness() noexcept {
  if (m_serverListenCoro) {
    m_serverListenCoro.Cancel();
  }
}

void TestHostHarness::SetRootView(ReactRootView &&rootView) noexcept {
  VerifyElseCrash(m_dispatcher.HasThreadAccess());

  m_rootView = std::move(rootView);
}

void TestHostHarness::OnInstanceLoaded(const InstanceLoadedEventArgs &args) noexcept {
  VerifyElseCrash(m_dispatcher.HasThreadAccess());

  if (!m_serverListenCoro) {
    m_serverListenCoro = StartListening();
  }

  ReactContext context(args.Context());
  m_context = context;
  m_instanceFailedToLoad = args.Failed();

  context.Notifications().Subscribe(
      TestModule::TestCompletedEvent(),
      m_dispatcher,
      [weakThis{get_weak()}](const auto & /*sender*/, ReactNotificationArgs<void> /*args*/) noexcept {
        if (auto strongThis = weakThis.get()) {
          if (strongThis->m_currentTransaction) {
            strongThis->HandleHostAction(strongThis->m_currentTransaction->OnTestModuleTestCompleted());
          }
        }
      });

  context.Notifications().Subscribe(
      TestModule::TestPassedEvent(),
      m_dispatcher,
      [weakThis{get_weak()}](const auto & /*sender*/, ReactNotificationArgs<bool> args) noexcept {
        if (auto strongThis = weakThis.get()) {
          if (strongThis->m_currentTransaction) {
            strongThis->HandleHostAction(strongThis->m_currentTransaction->OnTestModuleTestPassed(*args.Data()));
          }
        }
      });
}

IAsyncAction TestHostHarness::StartListening() noexcept {
  VerifyElseCrash(m_dispatcher.HasThreadAccess());

  auto cancellationToken = co_await winrt::get_cancellation_token();

  // Keep on polling for clients until we die
  while (!cancellationToken()) {
    auto processReqsTask = m_rpcServer.ProcessAllClientRequests(8305 /*port*/, 50ms /*pollInterval*/);

    // Foward cancellation to also cancel processing reqs (setting callback replaces the last)
    cancellationToken.callback([processReqsTask]() noexcept { processReqsTask.Cancel(); });
    co_await processReqsTask;
  }
}

winrt::NodeRpc::Handler TestHostHarness::CreateRpcHander() noexcept {
  winrt::NodeRpc::Handler handler;

  handler.BindAsyncOperation(
      L"RunTestComponent", [weakThis{get_weak()}](const JsonValue &payload) noexcept -> IAsyncOperation<IJsonValue> {
        auto strongThis = weakThis.get();
        VerifyElseCrash(strongThis);
        co_return co_await strongThis->OnTestCommand(TestCommandId::RunTestComponent, payload);
      });

  handler.BindAsyncOperation(
      L"GoToComponent", [weakThis{get_weak()}](const JsonValue &payload) noexcept -> IAsyncOperation<IJsonValue> {
        auto strongThis = weakThis.get();
        VerifyElseCrash(strongThis);
        co_return co_await strongThis->OnTestCommand(TestCommandId::GoToComponent, payload);
      });

  return handler;
}

IAsyncOperation<IJsonValue> TestHostHarness::OnTestCommand(TestCommandId command, JsonValue payload) noexcept {
  VerifyElseCrash(m_dispatcher.HasThreadAccess());

  // Keep ourselves alive while we have a test command
  auto strongThis = get_strong();

  if (m_pendingResponse) {
    co_return TestCommandResponse::Error("Received a test command while still processing the previous");
  }

  if (m_instanceFailedToLoad) {
    co_return TestCommandResponse::Error("The instance failed to load");
  }

  m_pendingResponse.emplace();

  switch (command) {
    case TestCommandId::RunTestComponent: {
      auto componentName = payload.GetObject().GetNamedString(L"component");
      m_rootView.ComponentName(componentName);

      m_currentTransaction = winrt::make_self<TestTransaction>();
      TimeoutOnInactivty(m_currentTransaction->get_weak());
      co_return co_await *m_pendingResponse;
      break;
    }

    case TestCommandId::GoToComponent: {
      auto componentName = payload.GetObject().GetNamedString(L"component");
      m_rootView.ComponentName(componentName);

      co_await FlushJSQueue();
      m_pendingResponse.reset();
      co_return TestCommandResponse::Okay();
      break;
    }

    default: {
      ShowJSError("Unexpected command ID from test runner");
      co_return TestCommandResponse::Error("Unexpected command ID from test runner");
      break;
    }
  }
}

winrt::fire_and_forget TestHostHarness::TimeoutOnInactivty(winrt::weak_ref<TestTransaction> transaction) noexcept {
  VerifyElseCrash(m_dispatcher.HasThreadAccess());

  winrt::apartment_context harnessContext;
  auto weakThis = get_weak();

  co_await 20s;
  co_await harnessContext;

  if (auto strongTransaction = transaction.get()) {
    if (auto strongThis = weakThis.get()) {
      if (m_currentTransaction == strongTransaction) {
        HandleHostAction(m_currentTransaction->OnTimeout());
      }
    }
  }
}

winrt::fire_and_forget TestHostHarness::HandleHostAction(HostAction action) noexcept {
  VerifyElseCrash(m_dispatcher.HasThreadAccess());

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

    case HostAction::ResultReady:
      m_pendingResponse->Set(m_currentTransaction->GetResult());
      m_pendingResponse.reset();
      m_currentTransaction = nullptr;
      break;

    default:
      VerifyElseCrash(false);
  }
}

IAsyncAction TestHostHarness::FlushJSQueue() noexcept {
  VerifyElseCrash(m_dispatcher.HasThreadAccess());

  winrt::handle signal(CreateEvent(nullptr, false, false, nullptr));

  m_context.JSDispatcher().Post([&signal, uiDispatcher{m_dispatcher}]() noexcept {
    uiDispatcher.Post([&signal]() noexcept { SetEvent(signal.get()); });
  });

  co_await winrt::resume_on_signal(signal.get());
}

void TestHostHarness::ShowJSError(std::string_view err) noexcept {
  VerifyElseCrash(m_dispatcher.HasThreadAccess());

  m_context.CallJSFunction(L"RCTLog", L"logToConsole", "error", err);
}

void TestHostHarnessRedboxHandler::ShowNewError(const IRedBoxErrorInfo &info, RedBoxErrorType /*type*/) noexcept {
  QueueToUI([info](TestHostHarness &harness) noexcept {
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
  QueueToUI([info](TestHostHarness &harness) noexcept {
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
    strongHarness->m_dispatcher.Post([strongHarness, func{std::move(func)}]() noexcept { func(*strongHarness); });
  }
}

} // namespace IntegrationTest
