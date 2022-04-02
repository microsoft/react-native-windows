#include <RuntimeOptions.h>

#include <mutex>
#include <unordered_map>

using std::lock_guard;
using std::mutex;
using std::string;

namespace {
std::unordered_map<string, int32_t> g_runtimeOptions;
std::unordered_map<string, string> g_runtimeOptionStrings;
mutex g_runtimeOptionsMutex;
} // namespace

namespace Microsoft::React {

void __cdecl SetRuntimeOptionBool(string &&name, bool value) noexcept {
  lock_guard<mutex> guard{g_runtimeOptionsMutex};
  g_runtimeOptions.insert_or_assign(std::move(name), value ? 1 : 0);
}

void __cdecl SetRuntimeOptionInt(string &&name, int32_t value) noexcept {
  lock_guard<mutex> guard{g_runtimeOptionsMutex};
  g_runtimeOptions.insert_or_assign(std::move(name), value);
}

void __cdecl SetRuntimeOptionString(const char *name, const char *value) noexcept {
  if (!name)
    return;

  lock_guard<mutex> guard{g_runtimeOptionsMutex};
  if (value)
    g_runtimeOptionStrings.insert_or_assign(name, value);
  else
    g_runtimeOptionStrings.erase(name);
}

const bool __cdecl GetRuntimeOptionBool(const string &name) noexcept {
  lock_guard<mutex> guard{g_runtimeOptionsMutex};
  auto itr = g_runtimeOptions.find(name);
  if (itr != g_runtimeOptions.end())
    return itr->second == 1;

  return false;
}

const int32_t __cdecl GetRuntimeOptionInt(const string &name) noexcept {
  lock_guard<mutex> guard{g_runtimeOptionsMutex};
  auto itr = g_runtimeOptions.find(name);
  if (itr != g_runtimeOptions.end())
    return itr->second;

  return 0;
}

const char *__cdecl GetRuntimeOptionString(const char *name) noexcept {
  lock_guard<mutex> guard{g_runtimeOptionsMutex};
  auto itr = g_runtimeOptionStrings.find(name);
  if (itr != g_runtimeOptionStrings.cend())
    return itr->second.c_str();

  return nullptr;
}

} // namespace Microsoft::React
