#include <RuntimeOptions.h>

#include <mutex>
#include <unordered_map>

using std::mutex;
using std::scoped_lock;
using std::string;

namespace {
std::unordered_map<string, int32_t> g_runtimeOptionInts;
std::unordered_map<string, string> g_runtimeOptionStrings;
mutex g_runtimeOptionsMutex;
} // namespace

namespace Microsoft::React {

void __cdecl SetRuntimeOptionBool(string &&name, bool value) noexcept {
  scoped_lock lock{g_runtimeOptionsMutex};
  g_runtimeOptionInts.insert_or_assign(std::move(name), value ? 1 : 0);
}

void __cdecl SetRuntimeOptionInt(string &&name, int32_t value) noexcept {
  scoped_lock lock{g_runtimeOptionsMutex};
  g_runtimeOptionInts.insert_or_assign(std::move(name), value);
}

const bool __cdecl GetRuntimeOptionBool(const string &name) noexcept {
  scoped_lock lock{g_runtimeOptionsMutex};
  auto itr = g_runtimeOptionInts.find(name);
  if (itr != g_runtimeOptionInts.end())
    return itr->second == 1;

  return false;
}

const int32_t __cdecl GetRuntimeOptionInt(const string &name) noexcept {
  scoped_lock lock{g_runtimeOptionsMutex};
  auto itr = g_runtimeOptionInts.find(name);
  if (itr != g_runtimeOptionInts.end())
    return itr->second;

  return 0;
}

} // namespace Microsoft::React

void __cdecl Microsoft_React_SetRuntimeOptionBool(const char *name, bool value) noexcept {
  if (!name)
    return;

  scoped_lock lock{g_runtimeOptionsMutex};
  if (value)
    g_runtimeOptionInts.insert_or_assign(name, value ? 1 : 0);
  else
    g_runtimeOptionInts.erase(name);
}

void __cdecl Microsoft_React_SetRuntimeOptionInt(const char *name, int32_t value) noexcept {
  if (!name)
    return;

  scoped_lock lock{g_runtimeOptionsMutex};
  if (value)
    g_runtimeOptionInts.insert_or_assign(name, value);
  else
    g_runtimeOptionInts.erase(name);
}

void __cdecl Microsoft_React_SetRuntimeOptionString(const char *name, const char *value) noexcept {
  if (!name)
    return;

  scoped_lock lock{g_runtimeOptionsMutex};
  if (value)
    g_runtimeOptionStrings.insert_or_assign(name, value);
  else
    g_runtimeOptionStrings.erase(name);
}

const bool __cdecl Microsoft_React_GetRuntimeOptionBool(const char *name) noexcept {
  scoped_lock lock{g_runtimeOptionsMutex};
  auto itr = g_runtimeOptionInts.find(name);
  if (itr != g_runtimeOptionInts.end())
    return itr->second == 1;

  return false;
}

const int32_t __cdecl Microsoft_React_GetRuntimeOptionInt(const char *name) noexcept {
  scoped_lock lock{g_runtimeOptionsMutex};
  auto itr = g_runtimeOptionInts.find(name);
  if (itr != g_runtimeOptionInts.end())
    return itr->second;

  return 0;
}

const char *__cdecl Microsoft_React_GetRuntimeOptionString(const char *name) noexcept {
  scoped_lock lock{g_runtimeOptionsMutex};
  auto itr = g_runtimeOptionStrings.find(name);
  if (itr != g_runtimeOptionStrings.cend()) {
    auto size = itr->second.size() * sizeof(char) + 1 /*NULL termination*/;
    auto result = static_cast<char *>(malloc(size));
    strncpy_s(result, size, itr->second.c_str(), itr->second.size());

    return result;
  }

  return nullptr;
}
