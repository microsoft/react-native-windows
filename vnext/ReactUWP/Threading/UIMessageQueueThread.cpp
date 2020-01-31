// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "UIMessageQueueThread.h"

using namespace winrt;
using namespace Windows::UI::Core;

namespace react::uwp {

UIMessageQueueThread::UIMessageQueueThread(CoreDispatcher const &dispatcher) noexcept : m_uiDispatcher{dispatcher} {}

UIMessageQueueThread::~UIMessageQueueThread() noexcept {}

void UIMessageQueueThread::runOnQueue(std::function<void()> &&func) noexcept {
  m_uiDispatcher.RunAsync(
      winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [func = std::move(func)]() noexcept {

//#define TRACK_UI_CALLS
#ifdef TRACK_UI_CALLS
        char buffer[1024];
        static uint32_t cCalls = 0;
        _snprintf_s(buffer, _countof(buffer), _TRUNCATE, "UIMessageQueueThread Calls: %u\r\n", ++cCalls);
        OutputDebugStringA(buffer);
#endif

        func();
      });
}

void UIMessageQueueThread::runOnQueueSync(std::function<void()> &&func) noexcept {
  assert(false && "Not supported");
  std::terminate();
}

void UIMessageQueueThread::quitSynchronous() noexcept {
  assert(false && "Not supported");
  std::terminate();
}

} // namespace react::uwp
