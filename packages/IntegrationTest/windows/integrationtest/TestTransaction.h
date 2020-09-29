// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.ReactNative.h>
#include "TestCommandListener.h"

namespace IntegrationTest {

//! Describes the phase of running a test
enum class TestState {
  // The test has been started
  Started,

  // The test has a preliminary result, such as seeing that a test has passed
  // or seen an exception. This may not be the final result, such as if
  // exceptions get updated, or if exceptions happen after we pass.
  WaitingForCompletion,

  // The test is complete
  Complete,
};

//! An action the test host must take in response to an event
enum class HostAction {
  // No actions needed
  Continue,

  // Ensure that all pending events from the test are fired. The transaction
  // should be notifed after by calling OnEventsFlushed.
  FlushEvents,

  // A test result is ready to be submitted
  SubmitResult,
};

//! Describes the peding transaction result
enum class TestResult {
  None,
  FailNoMessage,
  Exception,
  Timeout,
  Pass,
};

//! Defines the state machine of interactions between an individual test and
//! external events
class TestTransaction final : public winrt::implements<TestTransaction, winrt::Windows::Foundation::IInspectable> {
  using IRedBoxErrorInfo = winrt::Microsoft::ReactNative::IRedBoxErrorInfo;

 public:
  //! The RedboxHandler has observed an error
  [[nodiscard]] HostAction OnNewError(const IRedBoxErrorInfo &errorInfo) noexcept;

  //! The RedboxHandler has updates to an existing error
  [[nodiscard]] HostAction OnUpdateError(const IRedBoxErrorInfo &errorInfo) noexcept;

  //! TestModule.testCompleted has been called
  [[nodiscard]] HostAction OnTestModuleTestCompleted() noexcept;

  //! TestModule.testPassed has been called
  [[nodiscard]] HostAction OnTestModuleTestPassed(bool passed) noexcept;

  //! A test has timed out without giving a response
  [[nodiscard]] HostAction OnTimeout() noexcept;

  //! All test events have been flushed
  [[nodiscard]] HostAction OnEventsFlushed() noexcept;

  //! Submits a result based on the completed transaction. Must only be
  //! called once the test is complete.
  void SubmitResult(TestCommandResponse &response) noexcept;

 private:
  TestState m_state{TestState::Started};
  TestResult m_result{TestResult::None};
  std::unique_ptr<ExceptionInfo> m_exception;
};
} // namespace IntegrationTest
