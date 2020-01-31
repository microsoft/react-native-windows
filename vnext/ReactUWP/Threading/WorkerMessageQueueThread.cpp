// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "WorkerMessageQueueThread.h"

#include <wrl.h>
#include <atomic>
#include "AsyncWorkQueue.h"

namespace react::uwp {

struct AsyncCallback final : IAsyncCallback {
  AsyncCallback(std::function<void()> &&func) noexcept;

 public: // IUnknown
  ULONG __stdcall AddRef() noexcept override;
  ULONG __stdcall Release() noexcept override;
  HRESULT __stdcall QueryInterface(REFIID riid, _COM_Outptr_ void **ppvObject) noexcept override;

 public: // IAsyncCallback
  HRESULT __stdcall ProcessWorkItem(_In_opt_ IUnknown *pUserData) noexcept override;
  HRESULT __stdcall CancelWorkItem(_In_opt_ IUnknown *pUserData) noexcept override;

 private:
  std::atomic<ULONG> m_cRef{0};
  std::function<void()> m_func;
};

AsyncCallback::AsyncCallback(std::function<void()> &&func) noexcept : m_func{std::move(func)} {}

ULONG __stdcall AsyncCallback::AddRef() noexcept {
  return ++m_cRef;
}

ULONG __stdcall AsyncCallback::AsyncCallback::Release() noexcept {
  ULONG cRef = --m_cRef;

  if (cRef == 0) {
    delete this;
  }

  return cRef;
}

HRESULT __stdcall AsyncCallback::QueryInterface(REFIID riid, _COM_Outptr_ void **ppvObject) noexcept {
  if (IsEqualIID(riid, __uuidof(IAsyncCallback))) {
    *ppvObject = static_cast<IAsyncCallback *>(this);
    ((IAsyncCallback *)(*ppvObject))->AddRef();
  } else if (IsEqualIID(riid, __uuidof(IUnknown))) {
    *ppvObject = static_cast<IUnknown *>(this);
    ((IUnknown *)(*ppvObject))->AddRef();
  } else {
    *ppvObject = NULL;
    return E_NOINTERFACE;
  }

  return S_OK;
}

HRESULT __stdcall AsyncCallback::ProcessWorkItem(_In_opt_ IUnknown * /*pUserData*/) noexcept {
  try {
    m_func();
  } catch (...) {
    return E_FAIL;
  }

  return S_OK;
}

HRESULT __stdcall AsyncCallback::CancelWorkItem(_In_opt_ IUnknown * /*pUserData*/) noexcept {
  return S_OK;
}

struct WorkerMessageQueueThread::Impl {
  Impl() noexcept;
  ~Impl() noexcept;
  void runOnQueue(std::function<void()> &&func) noexcept;
  void runOnQueueSync(std::function<void()> &&func) noexcept;
  void quitSynchronous() noexcept;

 private:
  Microsoft::WRL::ComPtr<IAsyncWorkQueue> m_queue;
  std::atomic_bool m_stopped{false};
};

WorkerMessageQueueThread::Impl::Impl() noexcept {
  /*HRESULT hr =*/CreateAsyncWorkQueue(&m_queue);
  // TODO: Asserts
  // Assert(SUCCEEDED(hr))
}

WorkerMessageQueueThread::Impl::~Impl() noexcept {
  if (!m_stopped) {
    quitSynchronous();
  }
}

void WorkerMessageQueueThread::Impl::runOnQueue(std::function<void()> &&func) noexcept {
  // TODO: Asserts
  // Assert(!stopped)

  Microsoft::WRL::ComPtr<AsyncCallback> callback(new AsyncCallback(std::move(func)));
  m_queue->QueueWorkItem(callback.Get(), nullptr /*pUserData*/);
}

void WorkerMessageQueueThread::Impl::runOnQueueSync(std::function<void()> &&func) noexcept {
  // TODO: Asserts
  // Assert(!stopped)

  Microsoft::WRL::ComPtr<AsyncCallback> callback(new AsyncCallback(std::move(func)));
  m_queue->QueueWorkItem(callback.Get(), nullptr /*pUserData*/);
  m_queue->WaitForCallbacksToComplete();
}

void WorkerMessageQueueThread::Impl::quitSynchronous() noexcept {
  // TODO: Asserts
  // Assert(!stopped)
  m_queue->ShutDown();
  m_stopped = true;
}

WorkerMessageQueueThread::WorkerMessageQueueThread() noexcept {
  m_pimpl = std::make_unique<WorkerMessageQueueThread::Impl>();
}

WorkerMessageQueueThread::~WorkerMessageQueueThread() noexcept {}

void WorkerMessageQueueThread::runOnQueue(std::function<void()> &&func) noexcept {
  m_pimpl->runOnQueue(std::move(func));
}

void WorkerMessageQueueThread::runOnQueueSync(std::function<void()> &&func) noexcept {
  m_pimpl->runOnQueueSync(std::move(func));
}

void WorkerMessageQueueThread::quitSynchronous() noexcept {
  m_pimpl->quitSynchronous();
}

} // namespace react::uwp
