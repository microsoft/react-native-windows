// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <IWebSocketResource.h>
#include <Test/WebSocketServer.h>
#include <unicode.h>

// Windows API
#include <TlHelp32.h>
#include <Windows.h>

// Standard library includes
#include <math.h>
#include <atomic>
#include <future>

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Microsoft::Common::Unicode::Utf8ToUtf16;
using std::shared_ptr;
using std::string;
using std::vector;

// None of these tests are runnable
TEST_CLASS (WebSocketResourcePerformanceTest) {
  // See http://msdn.microsoft.com/en-us/library/ms686701(v=VS.85).aspx
  int32_t GetCurrentThreadCount() {
    DWORD procId = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0 /*th32ProcessID*/);

    PROCESSENTRY32 entry = {0};
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
  /// Spawn a number of WebSocket resources, have it write and read a message
  /// several times, then measure the amount of allocated threads. Important. This
  /// test must be run in isolation (no other tests running concurrently).
  ///
  TEST_METHOD(ProcessThreadsPerResource) {
    // About 3 seconds total running time.
    // 6, if we increase this value to 100.
    const int resourceTotal = 50;
    const int maxWriteCount = 10;
    const int expectedThreadsPerResource = 3;
    const int startThreadCount = GetCurrentThreadCount();

    std::atomic_int32_t threadCount = 0;
    bool errorFound = false;
    string errorMessage;

    auto server = std::make_shared<Test::WebSocketServer>(5556);
    server->SetMessageFactory([](string &&message) { return message + "_response"; });
    // TODO:  #4493 - Allow TestWebSocketServer to handle multiple incoming messages.
    // server->Start();

    // WebSocket resources scope.
    {
      vector<shared_ptr<IWebSocketResource>> resources;
      for (int i = 0; i < resourceTotal; i++) {
        auto ws = IWebSocketResource::Make("ws://localhost:5555/"); // TODO: Switch to port 5556
        ws->SetOnMessage([this, &threadCount, &errorFound](size_t size, const string &message, bool isBinary) {
          if (errorFound)
            return;

          auto count = this->GetCurrentThreadCount();
          if (count > threadCount.load())
            threadCount.store(count);
        });
        ws->SetOnSend([this, &threadCount, &errorFound](size_t) {
          if (errorFound)
            return;

          auto count = this->GetCurrentThreadCount();
          if (count > threadCount.load())
            threadCount.store(count);
        });
        ws->SetOnClose([this, &threadCount, &errorFound](IWebSocketResource::CloseCode, const string & /*reason*/) {
          if (errorFound)
            return;

          auto count = this->GetCurrentThreadCount();
          if (count > threadCount.load())
            threadCount.store(count);
        });
        ws->SetOnError([this, &errorFound, &errorMessage](IWebSocketResource::Error &&error) {
          if (errorFound)
            return;

          errorFound = true;
          errorMessage = error.Message;
        });
        ws->Connect();

        resources.push_back(std::move(ws));
      } // Create and store WS resources.

      // Send messages.
      for (auto &ws : resources) {
        ws->Send("some message");
      }

      // Close resources.
      for (auto &ws : resources) {
        ws->Close();
      }
    }
    // TODO:  #4493 - Allow TestWebSocketServer to handle multiple incoming messages.
    // server->Stop();

    int32_t finalThreadCount = threadCount.load();
    int64_t threadsPerResource = (finalThreadCount - startThreadCount) / resourceTotal;

    Assert::IsFalse(errorFound, Utf8ToUtf16(errorMessage).c_str());
    Assert::AreNotEqual(finalThreadCount, 0);
    Assert::IsTrue(threadsPerResource <= expectedThreadsPerResource);
  }
};
