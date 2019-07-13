#include "pch.h"

#include "NativeLogEventSource.h"

#include "NativeLogEventSource.g.cpp"

#include <Logging.h>
#include <unicode.h>

using namespace facebook::react::unicode;

namespace winrt::facebook::react::implementation {
namespace {
::winrt::facebook::react::NativeLogHandler g_abiHandler;
std::atomic<uint32_t> g_abiHandlerRegistrationCookie = 0;
} // namespace

uint32_t NativeLogEventSource::InitializeLogging(
    ::winrt::facebook::react::NativeLogHandler const &handler) {
  g_abiHandler = handler;

  std::function<void(::facebook::react::RCTLogLevel, const char *)>
      internalHandler = [](::facebook::react::RCTLogLevel ll, const char *m) {
        assert(g_abiHandler);
        g_abiHandler(static_cast<LogLevel>(ll), utf8ToUtf16(m));
      };

  ::facebook::react::InitializeLogging(std::move(internalHandler));

  return ++g_abiHandlerRegistrationCookie;
}

void NativeLogEventSource::UninitializeLogging(uint32_t cookie) {
  assert(cookie == g_abiHandlerRegistrationCookie);
  g_abiHandler = nullptr;
}
} // namespace winrt::facebook::react::implementation
