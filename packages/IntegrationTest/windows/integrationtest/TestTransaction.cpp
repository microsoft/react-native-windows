// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TestTransaction.h"

#include <Crash.h>;

using namespace winrt::Microsoft::ReactNative;

namespace IntegrationTest {

HostAction TestTransaction::OnNewError(const IRedBoxErrorInfo &errorInfo) noexcept {
  VerifyElseCrash(m_state == TestState::Started || m_state == TestState::WaitingForCompletion);

  // Ignore this exception if we've already seen one
  if (m_result == TestResult::Exception) {
    return HostAction::Continue;
  }

  m_result = TestResult::Exception;
  m_exception = std::make_unique<ExceptionInfo>(errorInfo);

  // When __DEV__ is set, ExceptionsManager will always try to prettify the
  // stack trace, and will usually succeed. Wait for either a prettified
  // strack trace or timeout.
#if BUNDLE
  m_state = TestState::WaitingForCompletion;
  return HostAction::FlushEvents;
#else
  return HostAction::Continue;
#endif
}

HostAction TestTransaction::OnUpdateError(const IRedBoxErrorInfo &errorInfo) noexcept {
  // Note that we can get updates asynchronously some time after test
  // completion. Make sure the update correlates to the transactions exception.
  if (m_result != TestResult::Exception || m_exception->Id != errorInfo.Id()) {
    return HostAction::Continue;
  }

  VerifyElseCrash(m_state == TestState::Started || m_state == TestState::WaitingForCompletion);

  m_exception->UpdateInfo(errorInfo);

  if (m_state == TestState::Started) {
    m_state = TestState::WaitingForCompletion;
    return HostAction::FlushEvents;
  } else {
    return HostAction::Continue;
  }
}

HostAction TestTransaction::OnTestModuleTestCompleted() noexcept {
  return OnTestModuleTestPassed(true);
}

HostAction TestTransaction::OnTestModuleTestPassed(bool passed) noexcept {
  VerifyElseCrash(m_state == TestState::Started || m_state == TestState::WaitingForCompletion);

  switch (m_result) {
    case TestResult::None:
    case TestResult::Pass:
    case TestResult::Timeout:
      // Prefer the new result
      m_result = passed ? TestResult::Pass : TestResult::FailNoMessage;
      break;

    case TestResult::FailNoMessage:
    case TestResult::Exception:
      // Prefer the old result
      break;

    default:
      VerifyElseCrash(false);
  }

  if (m_state == TestState::Started) {
    m_state = TestState::WaitingForCompletion;
    return HostAction::FlushEvents;
  } else {
    return HostAction::Continue;
  }
}

HostAction TestTransaction::OnTimeout() noexcept {
  // Don't do anything if we already have a pending response
  if (m_state == TestState::WaitingForCompletion || m_state == TestState::Complete) {
    return HostAction::Continue;
  }

  if (m_result == TestResult::None) {
    m_result = TestResult::Timeout;
  }

  m_state = TestState::WaitingForCompletion;
  return HostAction::FlushEvents;
}

HostAction TestTransaction::OnEventsFlushed() noexcept {
  VerifyElseCrash(m_state == TestState::WaitingForCompletion);
  VerifyElseCrash(m_result != TestResult::None);

  m_state = TestState::Complete;
  return HostAction::SubmitResult;
}

void TestTransaction::SubmitResult(TestCommandResponse &response) noexcept {
  VerifyElseCrash(m_state == TestState::Complete);

  switch (m_result) {
    case TestResult::Pass:
      response.TestPassed();
      break;

    case TestResult::Timeout:
      response.Timeout();
      break;

    case TestResult::FailNoMessage:
      response.TestFailed();
      break;

    case TestResult::Exception:
      response.Exception(*m_exception);
      break;

    case TestResult::None:
    default:
      VerifyElseCrash(false);
  }
}

} // namespace IntegrationTest
