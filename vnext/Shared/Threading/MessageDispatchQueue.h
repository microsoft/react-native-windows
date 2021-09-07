// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IReactDispatcher.h>
#include <cxxreact/MessageQueueThread.h>
#include <functional/FunctorRef.h>
#include <future/Future.h>
#include <memory>

namespace Mso::React {

struct MessageDispatchQueue : facebook::react::MessageQueueThread, std::enable_shared_from_this<MessageDispatchQueue> {
  MessageDispatchQueue(
      Mso::DispatchQueue const &dispatchQueue,
      Mso::Functor<void(const Mso::ErrorCode &)> &&errorHandler,
      Mso::Promise<void> &&whenQuit = nullptr) noexcept;

  ~MessageDispatchQueue() noexcept override;

  Mso::DispatchQueue const &DispatchQueue() const noexcept {
    return m_dispatchQueue;
  }

 public: // FastMessageQueueThread implementation
  void runOnQueue(std::function<void()> &&func) override;

  // runOnQueueSync and quitSynchronous are dangerous.  They should only be
  // used for initialization and cleanup.
  void runOnQueueSync(std::function<void()> &&func) override;

  // Once quitSynchronous() returns, no further work should run on the queue.
  void quitSynchronous() override;

 private:
  void runSync(const Mso::VoidFunctorRef &func) noexcept;
  void tryFunc(const std::function<void()> &func) noexcept;

 private:
  std::atomic<bool> m_stopped;
  Mso::DispatchQueue m_dispatchQueue;
  Mso::Functor<void(const Mso::ErrorCode &)> m_errorHandler;
  const Mso::Promise<void> m_whenQuit;
};

struct MessageDispatchQueue2 : facebook::react::MessageQueueThread,
                               std::enable_shared_from_this<MessageDispatchQueue2> {
  MessageDispatchQueue2(
      Mso::React::IDispatchQueue2 &dispatchQueue,
      Mso::Functor<void(const Mso::ErrorCode &)> &&errorHandler,
      Mso::Promise<void> &&whenQuit = nullptr) noexcept;

  ~MessageDispatchQueue2() noexcept override;

 public: // FastMessageQueueThread implementation
  void runOnQueue(std::function<void()> &&func) override;

  // runOnQueueSync and quitSynchronous are dangerous.  They should only be
  // used for initialization and cleanup.
  void runOnQueueSync(std::function<void()> &&func) override;

  // Once quitSynchronous() returns, no further work should run on the queue.
  void quitSynchronous() override;

 private:
  void runSync(const Mso::VoidFunctorRef &func) noexcept;
  void tryFunc(const std::function<void()> &func) noexcept;

 private:
  std::atomic<bool> m_stopped;
  Mso::CntPtr<Mso::React::IDispatchQueue2> m_dispatchQueue;
  Mso::Functor<void(const Mso::ErrorCode &)> m_errorHandler;
  const Mso::Promise<void> m_whenQuit;
};

} // namespace Mso::React
