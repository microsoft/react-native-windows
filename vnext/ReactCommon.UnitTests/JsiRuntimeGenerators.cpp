#include <jsi/jsi/test/testlib.h>
#if defined(USE_V8)
#include <V8JsiRuntime.h>
#endif

// TODO: #2730 Introduces a vcxitem for shared test code and move this
// there.
#include <Threading/MessageQueueThreadFactory.h>

// Standard Library
#include <vector>

using facebook::jsi::Runtime;
using facebook::jsi::RuntimeFactory;
using facebook::react::MessageQueueThread;
using Microsoft::ReactNative::MakeJSQueueThread;

namespace facebook::jsi {
std::vector<RuntimeFactory> runtimeGenerators() {
  return {[]() -> std::unique_ptr<Runtime> {
    v8runtime::V8RuntimeArgs args;

    return v8runtime::makeV8Runtime(std::move(args));
  }};
}

} // namespace facebook::jsi
