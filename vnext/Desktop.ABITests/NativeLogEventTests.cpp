// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.React.h>
#include <vector>
#include "ActivationFactory.h"

using namespace winrt::Microsoft::React;
using namespace winrt;

namespace ABITests {

TEST_CLASS (NativeLogEventTests) {
  // RAII helper to ensure log handlers get unregistered
  struct NativeLogInitializationGuard {
    NativeLogInitializationGuard(::winrt::Microsoft::React::NativeLogHandler const &handler) noexcept {
      m_registrationCookie = NativeLogEventSource::InitializeLogging(handler);
    }

    ~NativeLogInitializationGuard() noexcept {
      NativeLogEventSource::UninitializeLogging(m_registrationCookie);
    }

   private:
    uint32_t m_registrationCookie;
  };

 public:
  NativeLogEventTests() noexcept {
    winrt_activation_handler = WINRT_RoGetActivationFactory;
  }

  TEST_METHOD(NativeLogEventHandler_Registered) {
    init_apartment(winrt::apartment_type::single_threaded);

    // anticipatory, see TODO below
    std::vector<std::pair<::winrt::Microsoft::React::LogLevel, std::wstring>> logMessages;

    NativeLogHandler handler{[&logMessages](::winrt::Microsoft::React::LogLevel l, hstring const &m) {
      logMessages.emplace_back(l, m.c_str());
    }};

    NativeLogInitializationGuard initializationGuard{handler};

    // TODO:
    // Interact with RNW in such a way that it incurs logging, then verify the
    // respective log message(s). To do that, we might have to interact via the
    // existing unsafe ABI, then gradually replace those interactions with the
    // safe ABI as we build it up.
  }
};

} // namespace ABITests
