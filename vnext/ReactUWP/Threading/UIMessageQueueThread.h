// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>
#include <winrt/Windows.UI.Core.h>

namespace react {
namespace uwp {

// Executes the function on the provided UI Dispatcher
class UIMessageQueueThread : public facebook::react::MessageQueueThread {
 public:
  UIMessageQueueThread() = delete;
  UIMessageQueueThread(const UIMessageQueueThread &other) = delete;

  UIMessageQueueThread(winrt::Windows::UI::Core::CoreDispatcher dispatcher);
  virtual ~UIMessageQueueThread();

  virtual void runOnQueue(std::function<void()> &&func);
  virtual void runOnQueueSync(std::function<void()> &&func);
  virtual void quitSynchronous();

 private:
  winrt::Windows::UI::Core::CoreDispatcher m_uiDispatcher{nullptr};
};

} // namespace uwp
} // namespace react

