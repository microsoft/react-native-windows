#include <FeatureGate.h>

#include <unordered_map>

using std::string;

namespace {
std::unordered_map<string, bool> g_featureGates;
}

namespace Microsoft::React {

void __cdecl SetFeatureGate(string &&name, bool value) noexcept {
  g_featureGates.insert_or_assign(std::move(name), value);
}

const bool __cdecl GetFeatureGate(string &&name) noexcept {
  try {
    return g_featureGates.at(std::move(name));
  } catch (const std::out_of_range &) {
    return false;
  }
}

} // namespace Microsoft::React
