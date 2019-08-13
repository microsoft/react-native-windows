// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>
#include <winrt/Windows.UI.Core.h>

namespace react {
namespace uwp {

class BatchingUIMessageQueueThread : public facebook::react::MessageQueueThread {
public:
  BatchingUIMessageQueueThread() = delete;
  //  BatchingUIMessageQueueThread(const UIMessageQueueThread &other) = delete;

  BatchingUIMessageQueueThread(winrt::Windows::UI::Core::CoreDispatcher dispatcher);
  virtual ~BatchingUIMessageQueueThread();

  virtual void runOnQueue(std::function<void()> &&func);
  virtual void runOnQueueSync(std::function<void()> &&func);
  virtual void quitSynchronous();

  void ensureQueue();
  void onBatchComplete();

private:
  winrt::Windows::UI::Core::CoreDispatcher m_uiDispatcher{ nullptr };

  typedef std::vector<std::function<void()>> WorkItemQueue;
  std::shared_ptr<WorkItemQueue> m_queue;
};

} // namespace uwp
} // namespace react

