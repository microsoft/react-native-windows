// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <atomic>

namespace Mso::React {

class CrashManager final {
 public:
  static void RegisterCustomHandler() noexcept;
  static void UnregisterCustomHandler() noexcept;
  static void OnUnhandledException() noexcept;

#if defined(DEBUG)
  static std::string TestCollectCrashData();
#endif

 private:
  static void CrashHandler(int fileDescriptor) noexcept;
  static std::atomic_uint _regCount;
};

} // namespace Mso::React
