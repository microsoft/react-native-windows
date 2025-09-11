#pragma once

#include <jsi/jsi.h>
#include <memory>

#include <DevSettings.h>
#include <jsinspector-modern/FallbackRuntimeTargetDelegate.h>
#include <jsinspector-modern/RuntimeTarget.h>

namespace Microsoft::JSI {

// An instance of this interface is expected to
// a. lazily create a JSI Runtime on the first call to getRuntime
// b. subsequent calls to getRuntime should return the Runtime created in (a)

// Note: all calls to getRuntime() should happen on the same thread unless you are sure that
// the underlying Runtime instance is thread safe.

struct RuntimeHolderLazyInit {
  virtual std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept = 0;
  virtual facebook::react::JSIEngineOverride getRuntimeType() noexcept = 0;

  virtual const std::shared_ptr<facebook::react::jsinspector_modern::RuntimeTargetDelegate> &
  getSharedRuntimeTargetDelegate() {
    if (!m_runtimeTargetDelegate) {
      m_runtimeTargetDelegate = std::make_shared<facebook::react::jsinspector_modern::FallbackRuntimeTargetDelegate>(
          getRuntime()->description());
    }
    return m_runtimeTargetDelegate;
  }

  virtual void teardown() noexcept {};

  // You can call this when a crash happens to attempt recording additional data
  // The fileDescriptor supplied is a raw file stream an implementation might write JSON to.
  virtual void crashHandler(int fileDescriptor) noexcept {};

 private:
  // Initialized by \c getRuntimeTargetDelegate if not overridden, and then never changes.
  std::shared_ptr<facebook::react::jsinspector_modern::RuntimeTargetDelegate> m_runtimeTargetDelegate;
};

} // namespace Microsoft::JSI
