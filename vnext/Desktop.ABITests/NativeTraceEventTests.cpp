// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <winrt/facebook.react.h>
#include <functional>
#include <stack>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::facebook::react;
using namespace winrt;

namespace ABITests {

TEST_CLASS(NativeTraceEventTests) {
  struct TestTraceHandler : public winrt::implements<TestTraceHandler, ::winrt::facebook::react::INativeTraceHandler> {
    void JSBeginSection(param::hstring const &profileName, param::hstring const &args) const {
      if (this->OnJSBeginSection) {
        this->OnJSBeginSection(((hstring)profileName).c_str(), ((hstring)args).c_str());
      }
    }

    void JSEndSection() const {
      if (this->OnJSEndSection) {
        this->OnJSEndSection();
      }
    };

    void JSBeginAsyncSection(param::hstring const &profileName, uint32_t cookie) const {
      if (this->OnJSBeginAsyncSection) {
        this->OnJSBeginAsyncSection(((hstring)profileName).c_str(), cookie);
      }
    }

    void JSEndAsyncSection(param::hstring const &profileName, uint32_t cookie) const {
      if (this->OnJSEndAsyncSection) {
        this->OnJSEndAsyncSection(((hstring)profileName).c_str(), cookie);
      }
    }

    void JSCounter(param::hstring const &profileName, uint32_t value) const {
      if (this->OnJSCounter) {
        this->OnJSCounter(((hstring)profileName).c_str(), value);
      }
    }

    void NativeBeginSection(param::hstring const &profileName, param::hstring const &args) const {
      if (this->OnNativeBeginSection) {
        this->OnNativeBeginSection(((hstring)profileName).c_str(), ((hstring)args).c_str());
      }
    }

    void NativeEndSection(param::hstring const &profileName, param::hstring const &args, int64_t durationInNanoseconds)
        const {
      if (this->OnNativeEndSection) {
        this->OnNativeEndSection(((hstring)profileName).c_str(), ((hstring)args).c_str(), durationInNanoseconds);
      }
    }

    std::function<void(const wchar_t *, const wchar_t *)> OnJSBeginSection;
    std::function<void()> OnJSEndSection;
    std::function<void(const wchar_t *, uint32_t)> OnJSBeginAsyncSection;
    std::function<void(const wchar_t *, uint32_t)> OnJSEndAsyncSection;
    std::function<void(const wchar_t *, uint32_t)> OnJSCounter;
    std::function<void(const wchar_t *, const wchar_t *)> OnNativeBeginSection;
    std::function<void(const wchar_t *, const wchar_t *, int64_t)> OnNativeEndSection;
  };

  // RAII helper to ensure trace handlers get unregistered
  struct NativeTraceInitializationGuard {
    NativeTraceInitializationGuard(const ::winrt::facebook::react::INativeTraceHandler &handler) noexcept {
      m_registrationCookie = NativeTraceEventSource::InitializeTracing(handler);
    }

    ~NativeTraceInitializationGuard() noexcept {
      NativeTraceEventSource::UninitializeTracing(m_registrationCookie);
    }

   private:
    uint32_t m_registrationCookie;
  };

  TEST_METHOD(NativeTraceEventHandler_Registered) {
    init_apartment(winrt::apartment_type::single_threaded);

    ::winrt::facebook::react::INativeTraceHandler handler = winrt::make<TestTraceHandler>();

    // anticipatory, see TODO below
    std::stack<std::pair<std::wstring, std::wstring>> jsSections;
    handler.as<TestTraceHandler>()->OnJSBeginSection = [&jsSections](const wchar_t *pn, const wchar_t *a) {
      jsSections.emplace(pn, a);
    };

    NativeTraceInitializationGuard initializationGuard(handler);

    // TODO:
    // Interact with RNW in such a way that it incurs tracing, then verify the
    // respective trace message. To do that, we might have to interact via the
    // existing unsafe ABI, then gradually replace those interactions with the
    // safe ABI as we build it up.
  }
};

} // namespace ABITests
