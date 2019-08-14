#include "pch.h"

#include "HermesRuntimeHolder.h"

#include <hermes/hermes.h>

using namespace facebook;

namespace react {
namespace uwp {

std::shared_ptr<jsi::Runtime> HermesRuntimeHolder::getRuntime() noexcept {
  std::call_once(once_flag_, [this]() { initRuntime(); });

  if (!runtime_)
    std::terminate();

  // ChakraJsiRuntime is not thread safe as of now.
  if (own_thread_id_ != std::this_thread::get_id())
    std::terminate();

  return runtime_;
}

void HermesRuntimeHolder::initRuntime() noexcept {
  runtime_ = facebook::hermes::makeHermesRuntime();
  own_thread_id_ = std::this_thread::get_id();
}
} // namespace uwp
} // namespace react
