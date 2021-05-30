#include "pch.h"

#include "NativeTraceEventSource.h"

#include "facebook.react.NativeTraceEventSource.g.cpp"

#include <Tracing.h>
#include <Unicode.h>

using namespace ::Microsoft::Common::Unicode;

namespace winrt::facebook::react::implementation {
namespace {
::winrt::facebook::react::INativeTraceHandler g_abiHandler;
std::atomic<uint32_t> g_abiHandlerRegistrationCookie = 0;

class InternalHandler : public ::facebook::react::INativeTraceHandler{
  public : virtual void JSBeginSection(const char *profileName, const char *args) noexcept override{
      g_abiHandler.JSBeginSection(Utf8ToUtf16(profileName), Utf8ToUtf16(args));
} // namespace

virtual void JSEndSection() noexcept override {
  g_abiHandler.JSEndSection();
}

virtual void JSBeginAsyncSection(const char *profileName, int cookie) noexcept override {
  g_abiHandler.JSBeginAsyncSection(Utf8ToUtf16(profileName), cookie);
}

virtual void JSEndAsyncSection(const char *profileName, int cookie) noexcept override {
  g_abiHandler.JSEndAsyncSection(Utf8ToUtf16(profileName), cookie);
}

virtual void JSCounter(const char *profileName, int value) noexcept override {
  g_abiHandler.JSCounter(Utf8ToUtf16(profileName), value);
}

virtual void NativeBeginSection(const char *profileName, const char *args) noexcept override {
  g_abiHandler.NativeBeginSection(Utf8ToUtf16(profileName), Utf8ToUtf16(args));
}

virtual void NativeEndSection(const char *profileName, const char *args, std::chrono::nanoseconds duration) noexcept
    override {
  g_abiHandler.NativeEndSection(Utf8ToUtf16(profileName), Utf8ToUtf16(args), duration.count());
}

private:
} // namespace winrt::facebook::react::implementation
g_internalHandler;
}

uint32_t NativeTraceEventSource::InitializeTracing(::winrt::facebook::react::INativeTraceHandler const &handler) {
  g_abiHandler = handler;
  ::facebook::react::InitializeTracing(&g_internalHandler);
  return ++g_abiHandlerRegistrationCookie;
}

void NativeTraceEventSource::UninitializeTracing(uint32_t cookie) {
  assert(cookie == g_abiHandlerRegistrationCookie);
  g_abiHandler = nullptr;
  ::facebook::react::InitializeTracing(nullptr);
}
}
