// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "WorkerMessageQueueThread.h"

#include "AsyncWorkQueue.h"
#include <atomic>
#include <wrl.h>

namespace react { namespace uwp {

class AsyncCallback : public IAsyncCallback
{
public:
  AsyncCallback(std::function<void()>&& func);

  // IUnknown
  STDMETHOD_(ULONG, AddRef)();
  STDMETHOD_(ULONG, Release)();
  STDMETHOD(QueryInterface)(REFIID riid, _Outptr_ void** ppvObject);

  // IAsyncCallback
  STDMETHOD(ProcessWorkItem)(_In_opt_ IUnknown* pUserData);
  STDMETHOD(CancelWorkItem)(_In_opt_ IUnknown* pUserData);

private:
  std::atomic<ULONG> m_cRef { 0 };

  std::function<void()> m_func;
};

AsyncCallback::AsyncCallback(std::function<void()>&& func)
  : m_func(std::move(func))
{
}

COM_DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) AsyncCallback::AddRef()
{
  return ++m_cRef;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) AsyncCallback::Release()
{
  ULONG cRef = --m_cRef;

  if (cRef == 0)
  {
    delete this;
  }

  return cRef;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP AsyncCallback::QueryInterface(
  REFIID riid,
  _Outptr_ void** ppvObject)
{
  if (IsEqualIID(riid, __uuidof(IAsyncCallback)))
  {
    *ppvObject = static_cast<IAsyncCallback*>(this);
    ((IAsyncCallback*)(*ppvObject))->AddRef();
  }
  else if (IsEqualIID(riid, __uuidof(IUnknown)))
  {
    *ppvObject = static_cast<IUnknown*>(this);
    ((IUnknown*)(*ppvObject))->AddRef();
  }
  else
  {
    *ppvObject = NULL;
    return E_NOINTERFACE;
  }

  return S_OK;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP AsyncCallback::ProcessWorkItem(_In_opt_ IUnknown* /*pUserData*/)
{
  try {
    m_func();
  }
  catch (...) {
    return E_FAIL;
  }

  return S_OK;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP AsyncCallback::CancelWorkItem(_In_opt_ IUnknown* /*pUserData*/)
{
  return S_OK;
}

struct WorkerMessageQueueThread::Impl
{
  Impl();
  ~Impl();
  void runOnQueue(std::function<void()>&& func);
  void runOnQueueSync(std::function<void()>&& func);
  void quitSynchronous();

  Microsoft::WRL::ComPtr<IAsyncWorkQueue> queue;
  std::atomic_bool stopped { false };
};

WorkerMessageQueueThread::Impl::Impl()
{
  HRESULT hr = CreateAsyncWorkQueue(&queue);
  // TODO: Asserts
  // Assert(SUCCEEDED(hr))
}

WorkerMessageQueueThread::Impl::~Impl()
{
  if (!stopped) {
    quitSynchronous();
  }
}

void WorkerMessageQueueThread::Impl::runOnQueue(std::function<void()>&& func)
{
  // TODO: Asserts
  // Assert(!stopped)

  Microsoft::WRL::ComPtr<AsyncCallback> callback(new AsyncCallback(std::move(func)));
  queue->QueueWorkItem(callback.Get(), nullptr /*pUserData*/);
}

void WorkerMessageQueueThread::Impl::runOnQueueSync(std::function<void()>&& func)
{
  // TODO: Asserts
  // Assert(!stopped)

  Microsoft::WRL::ComPtr<AsyncCallback> callback(new AsyncCallback(std::move(func)));
  queue->QueueWorkItem(callback.Get(), nullptr /*pUserData*/);
  queue->WaitForCallbacksToComplete();
}

void WorkerMessageQueueThread::Impl::quitSynchronous()
{
  // TODO: Asserts
  // Assert(!stopped)
  queue->ShutDown();
  stopped = true;
}

WorkerMessageQueueThread::WorkerMessageQueueThread()
{
  m_pimpl = std::make_unique<WorkerMessageQueueThread::Impl>();
}

WorkerMessageQueueThread::~WorkerMessageQueueThread()
{
}

void WorkerMessageQueueThread::runOnQueue(std::function<void()>&& func)
{
  m_pimpl->runOnQueue(std::move(func));
}

void WorkerMessageQueueThread::runOnQueueSync(std::function<void()>&& func)
{
  m_pimpl->runOnQueueSync(std::move(func));
}

void WorkerMessageQueueThread::quitSynchronous()
{
  m_pimpl->quitSynchronous();
}

} }

