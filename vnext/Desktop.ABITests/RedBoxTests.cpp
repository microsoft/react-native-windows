// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Internal.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.Collections.h>

using namespace winrt;
using namespace winrt::Microsoft::ReactNative;

namespace ABITests {

struct MockRedBoxHandler : implements<MockRedBoxHandler, Microsoft::ReactNative::IRedBoxHandler> {
  void ShowNewError(IRedBoxErrorInfo info, RedBoxErrorType type) {
    OnShowNewError(info, type);
  }
  bool IsDevSupportEnabled() {
    return OnIsDevSupportEnabled();
  };
  void UpdateError(IRedBoxErrorInfo info) {
    OnUpdateError(info);
  }
  void DismissRedBox() {
    OnDismissRedBox();
  }

  std::function<void(IRedBoxErrorInfo, RedBoxErrorType)> OnShowNewError;
  std::function<bool()> OnIsDevSupportEnabled;
  std::function<void(IRedBoxErrorInfo)> OnUpdateError;
  std::function<void()> OnDismissRedBox;
};

// The tests here are a development staging artifact owed to the incremental buildup of the C++/WinRT-based ABI of
// the Win32 DLL. They (or their logical equivalent) should probably get rolled into other tests once C++/WinRT-based
// instance management becomes available.
TEST_CLASS (RedBoxTest) {
 public:
  TEST_METHOD(Handler_IsCallable) {
    std::vector<std::tuple<IRedBoxErrorInfo, RedBoxErrorType>> showNewErrorCalls;
    auto handler = make<MockRedBoxHandler>();
    handler.as<MockRedBoxHandler>()->OnShowNewError = [&showNewErrorCalls](
                                                          IRedBoxErrorInfo info, RedBoxErrorType type) {
      showNewErrorCalls.emplace_back(info, type);
    };
    ReactInstanceSettings settings;
    settings.RedBoxHandler(handler);

    auto frameInfo1 = Microsoft::Internal::TestController::CreateRedBoxErrorFrameInfo(L"abc.js", L"foo", 123, 11);
    auto frameInfo2 = Microsoft::Internal::TestController::CreateRedBoxErrorFrameInfo(L"def.js", L"bar", 234, 22);
    auto errorInfo =
        Microsoft::Internal::TestController::CreateRedBoxErrorInfo(L"out of coffee", 1203, {frameInfo1, frameInfo2});
    settings.RedBoxHandler().ShowNewError(errorInfo, RedBoxErrorType::JavaScriptFatal);

    TestCheckEqual(1ul, showNewErrorCalls.size());
    TestCheckEqual(2ul, std::get<0>(showNewErrorCalls[0]).Callstack().Size());

    TestCheckEqual(L"abc.js", std::get<0>(showNewErrorCalls[0]).Callstack().GetAt(0).File());
    TestCheckEqual(L"foo", std::get<0>(showNewErrorCalls[0]).Callstack().GetAt(0).Method());
    TestCheckEqual(123ul, std::get<0>(showNewErrorCalls[0]).Callstack().GetAt(0).Line());
    TestCheckEqual(11ul, std::get<0>(showNewErrorCalls[0]).Callstack().GetAt(0).Column());

    TestCheckEqual(L"def.js", std::get<0>(showNewErrorCalls[0]).Callstack().GetAt(1).File());
    TestCheckEqual(L"bar", std::get<0>(showNewErrorCalls[0]).Callstack().GetAt(1).Method());
    TestCheckEqual(234ul, std::get<0>(showNewErrorCalls[0]).Callstack().GetAt(1).Line());
    TestCheckEqual(22ul, std::get<0>(showNewErrorCalls[0]).Callstack().GetAt(1).Column());

    TestCheckEqual(RedBoxErrorType::JavaScriptFatal, std::get<1>(showNewErrorCalls[0]));
  }
};

} // namespace ABITests
