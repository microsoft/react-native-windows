// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "JSQueueThread.h"

#include <CxxMessageQueue.h>

namespace react {
namespace uwp {

// From https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
  DWORD dwType; // Must be 0x1000.
  LPCSTR szName; // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
static void SetThreadName(DWORD dwThreadID, const char *threadName) {
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = threadName;
  info.dwThreadID = dwThreadID;
  info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable : 6320 6322)
  __try {
    RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR *)&info);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
#pragma warning(pop)
}

static void SetThreadName(std::thread &thread, const char *threadName) {
  DWORD threadId = ::GetThreadId(static_cast<HANDLE>(thread.native_handle()));
  SetThreadName(threadId, threadName);
}

std::shared_ptr<facebook::react::MessageQueueThread> CreateAndStartJSQueueThread() {
  auto q = std::make_shared<facebook::react::CxxMessageQueue>();
  std::thread t([q]() mutable {
    auto loop = facebook::react::CxxMessageQueue::getRunLoop(q);
    // Note: make sure that no stack frames above loop() have a strong reference
    // to q.
    q.reset();
    loop();
  });

  SetThreadName(t, "jsThread");
  t.detach();
  return q;
}

} // namespace uwp
} // namespace react
