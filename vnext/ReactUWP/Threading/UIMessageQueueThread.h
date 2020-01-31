// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>
#include <winrt/Windows.UI.Core.h>

namespace react::uwp {

// Executes the function on the provided UI Dispatcher
struct UIMessageQueueThread final : facebook::react::MessageQueueThread {
  UIMessageQueueThread(winrt::Windows::UI::Core::CoreDispatcher const &dispatcher) noexcept;
  ~UIMessageQueueThread() noexcept override;

  UIMessageQueueThread() = delete;
  UIMessageQueueThread(const UIMessageQueueThread &other) = delete;

 public: // facebook::react::MessageQueueThread
  void runOnQueue(std::function<void()> &&func) noexcept override;
  void runOnQueueSync(std::function<void()> &&func) noexcept override;
  void quitSynchronous() noexcept override;

 private:
  winrt::Windows::UI::Core::CoreDispatcher m_uiDispatcher{nullptr};
};

} // namespace react::uwp
