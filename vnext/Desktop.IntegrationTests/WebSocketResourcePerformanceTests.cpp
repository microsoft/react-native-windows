// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <IWebSocket.h>

// Windows API includes
#include <Windows.h>
#include <TlHelp32.h>

// Standard library includes
#include <atomic>
#include <future>
#include <math.h>

using namespace facebook::react;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::string;
using std::unique_ptr;
using std::vector;

TEST_CLASS(WebSocketResourcePerformanceTest)
{
  // See http://msdn.microsoft.com/en-us/library/ms686701(v=VS.85).aspx
  int32_t GetCurrentThreadCount()
  {
    DWORD procId = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0 /*th32ProcessID*/);

    PROCESSENTRY32 entry = { 0 };
    entry.dwSize = sizeof(entry);

    BOOL procFound = true;
    procFound = Process32First(snapshot, &entry);
    while (procFound && entry.th32ProcessID != procId)
      procFound = Process32Next(snapshot, &entry);
    CloseHandle(snapshot);

    if (procFound)
      return entry.cntThreads;

    return -1;
  }

  ///
  /// Spawn a number of WebSocket resources, have it write and read a message several times,
  /// then measure the amount of allocated threads.
  /// Important. This test must be run in isolation (no other tests running concurrently).
  ///
  TEST_METHOD(ProcessThreadsPerResource)
  {
    const int resourceTotal = 50; // About 3 seconds total running time. 6 if we increase this value to 100.
    const int maxWriteCount = 10;
    const int expectedThreadsPerResource = 3;
    const int startThreadCount = GetCurrentThreadCount();

    std::atomic_int32_t threadCount = 0;

    // WebSocket resources scope.
    {
      vector<unique_ptr<IWebSocket>> resources;
      for (int i = 0; i < resourceTotal; i++)
      {
        auto ws = IWebSocket::Make("ws://localhost:5555/");
        ws->SetOnMessage([this, &threadCount](size_t size, const string& message)
        {
          auto count = this->GetCurrentThreadCount();
          if (count > threadCount.load())
            threadCount.store(count);
        });
        ws->SetOnSend([this, &threadCount](size_t)
        {
          auto count = this->GetCurrentThreadCount();
          if (count > threadCount.load())
            threadCount.store(count);
        });
        ws->SetOnClose([this, &threadCount](IWebSocket::CloseCode, const string& /*reason*/)
        {
          auto count = this->GetCurrentThreadCount();
          if (count > threadCount.load())
            threadCount.store(count);
        });
        ws->Connect();

        resources.push_back(std::move(ws));
      } // Create and store WS resources.

      // Send messages.
      for(auto& ws : resources)
      {
        ws->Send("some message");
      }
    }

    int64_t threadsPerResource = (threadCount.load() - startThreadCount) / resourceTotal;
    Assert::IsTrue(threadsPerResource <= expectedThreadsPerResource);
  }
};
