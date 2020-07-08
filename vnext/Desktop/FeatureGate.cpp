#include <FeatureGate.h>

#include <unordered_set>

using std::string;

namespace {
std::unordered_set<string> g_featureGates;
}

namespace Microsoft::React {

void SetFeatureGate(string&& name) noexcept {
  g_featureGates.insert(std::move(name));
}

bool GetFeatureGate(string&& name) noexcept {
  return g_featureGates.find(std::move(name)) != g_featureGates.end();
}

} // namespace Microsoft::ReactNative
