#include <jsi/jsi.h>
#include <memory>

namespace facebook {
namespace v8runtime {

// This exists just to satisfy the export requirements, so that we can share the
// same .def file between GitHub and the Office internal build system
std::unique_ptr<jsi::Runtime> makeV8Runtime() {
  std::abort();
}

} // namespace v8runtime
} // namespace facebook
