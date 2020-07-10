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

const bool __cdecl GetFeatureGate(const string &name) noexcept {
  lock_guard<mutex> guard{g_featureGatesMutex};
  auto itr = g_featureGates.find(name);
  if (itr != g_featureGates.end())
    return itr->second;

  return false;
}

} // namespace Microsoft::React
