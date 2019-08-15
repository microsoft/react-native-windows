// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ReactWindowsCore/BatchingMessageQueueThread.h>
#include <winrt/Windows.UI.Core.h>

namespace react {
namespace uwp {

// Executes the function on the provided UI Dispatcher
class UIMessageQueueThread
    : public facebook::react::BatchingMessageQueueThread {
 public:
  UIMessageQueueThread() = delete;
  UIMessageQueueThread(const UIMessageQueueThread &other) = delete;

  UIMessageQueueThread(winrt::Windows::UI::Core::CoreDispatcher dispatcher);
  virtual ~UIMessageQueueThread();

  virtual void runOnQueue(std::function<void()> &&func);
  virtual void runOnQueueSync(std::function<void()> &&func);
  virtual void quitSynchronous();

  void onBatchComplete() override;

 private:
  void ensureQueue();

 private:
  winrt::Windows::UI::Core::CoreDispatcher m_uiDispatcher{nullptr};

  typedef std::vector<std::function<void()>> WorkItemQueue;
  std::shared_ptr<WorkItemQueue> m_queue;
};

} // namespace uwp
} // namespace react
