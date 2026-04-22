// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <windows.h>

#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.UI.Dispatching.h>

namespace Microsoft::React::Test {

namespace msrn = winrt::Microsoft::ReactNative;

struct TestUIDispatcher : public winrt::implements<TestUIDispatcher, msrn::IReactDispatcher> {
  TestUIDispatcher(winrt::Microsoft::UI::Dispatching::DispatcherQueue const &dispatcherQueue);

  bool HasThreadAccess();
  void Post(msrn::ReactDispatcherCallback const &callback);

 private:
  winrt::Microsoft::UI::Dispatching::DispatcherQueue m_dispatcherQueue;
  DWORD m_threadId{0};
};

} // namespace Microsoft::React::Test
