#pragma once

// forward declaration.
namespace facebook {
namespace jsi {
class Runtime;
}
} // namespace facebook

namespace facebook {
namespace react {
namespace tracing {
void initializeETW();
void initializeJSHooks(facebook::jsi::Runtime &runtime, bool isProfiling);

void log(const char *msg);
void error(const char *msg);
} // namespace tracing
} // namespace react
} // namespace facebook
