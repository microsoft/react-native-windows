#include <FeatureGate.h>

#include <mutex>
#include <unordered_map>

using std::lock_guard;
using std::mutex;
using std::string;

namespace {
std::unordered_map<string, bool> g_featureGates;
mutex g_featureGatesMutex;
} // namespace

namespace Microsoft::React {

void __cdecl SetFeatureGate(string &&name, bool value) noexcept {
  lock_guard<mutex> guard{g_featureGatesMutex};
  g_featureGates.insert_or_assign(std::move(name), value);
}

const bool __cdecl GetFeatureGate(string &&name) noexcept {
  try {
    lock_guard<mutex> guard{g_featureGatesMutex};
    return g_featureGates.at(std::move(name));
  } catch (const std::out_of_range &) {
    return false;
  }
}

} // namespace Microsoft::React
