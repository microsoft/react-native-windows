// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>

namespace IntegrationTest {

struct CallStackFrame {
  winrt::hstring File;
  winrt::hstring Method;
  uint32_t Line{};
  uint32_t Column{};
};

//! Mutable representation of an exception from a RedBoxHandler
struct ExceptionInfo {
  ExceptionInfo() noexcept = default;
  ExceptionInfo(const winrt::Microsoft::ReactNative::IRedBoxErrorInfo &errorInfo) noexcept {
    Id = errorInfo.Id();
    UpdateInfo(errorInfo);
  }

  void UpdateInfo(const winrt::Microsoft::ReactNative::IRedBoxErrorInfo &errorInfo) noexcept {
    assert(errorInfo.Id() == Id);

    if (!errorInfo.Message().empty()) {
      Message = errorInfo.Message();
    }
    if (!errorInfo.OriginalMessage().empty()) {
      OriginalMessage = errorInfo.OriginalMessage();
    }
    if (!errorInfo.Name().empty()) {
      Name = errorInfo.Name();
    }
    if (errorInfo.Callstack().Size() > 0) {
      Callstack.clear();
      for (const auto &frame : errorInfo.Callstack()) {
        Callstack.push_back({frame.File(), frame.Method(), frame.Line(), frame.Column()});
      }
    }
  }

  winrt::hstring Message;
  winrt::hstring OriginalMessage;
  winrt::hstring Name;
  std::vector<CallStackFrame> Callstack;
  uint32_t Id{};
};

} // namespace IntegrationTest
