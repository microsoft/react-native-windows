#pragma once

#include <jsi/jsi.h>
#include <memory>

namespace facebook {
namespace jsi {

enum RuntimeType { Chakra = 0, Hermes = 1, V8 = 2, V8NAPI = 3 };

// An instance of this interface is expected to
// a. lazily create a JSI Runtime on the first call to getRuntime
// b. subsequent calls to getRuntime should return the Runtime created in (a)

// Note :: All calls to getRuntime() should happen on the same thread unless you are sure that
// the underlying Runtime instance is thread safe.

struct RuntimeHolderLazyInit {
  virtual std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept = 0;
  virtual RuntimeType getRuntimeType() noexcept = 0;
};

} // namespace jsi
} // namespace facebook
