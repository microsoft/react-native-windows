// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppRuntimeOptions.h>
#include <RuntimeOptions.h>

// Standard Library
#include <mutex>
#include <unordered_map>

using std::mutex;
using std::scoped_lock;
using std::string;

namespace {
std::unordered_map<string, int32_t> g_runtimeOptionInts;
std::unordered_map<string, string> g_runtimeOptionStrings;
mutex g_runtimeOptionsMutex;

void __cdecl GetStringCallback(const char *buffer, size_t length, void *state) {
  if (!buffer)
    return;

  *static_cast<char **>(state) = static_cast<char *>(malloc(length));
  strncpy_s(*static_cast<char **>(state), length, buffer, length);
}
} // namespace

namespace Microsoft::React {

void __cdecl SetRuntimeOptionBool(string &&name, bool value) noexcept {
  MicrosoftReactSetRuntimeOptionBool(name.c_str(), value);
}

void __cdecl SetRuntimeOptionInt(string &&name, int32_t value) noexcept {
  MicrosoftReactSetRuntimeOptionInt(name.c_str(), value);
}

void __cdecl SetRuntimeOptionString(string &&name, string &&value) noexcept {
  if (!value.empty())
    MicrosoftReactSetRuntimeOptionString(std::move(name).c_str(), std::move(value).c_str());
}

const bool __cdecl GetRuntimeOptionBool(const string &name) noexcept {
  return MicrosoftReactGetRuntimeOptionBool(name.c_str());
}

const int32_t __cdecl GetRuntimeOptionInt(const string &name) noexcept {
  return MicrosoftReactGetRuntimeOptionInt(name.c_str());
}

const string __cdecl GetRuntimeOptionString(const string &name) noexcept {
  char *payload{nullptr};
  MicrosoftReactGetRuntimeOptionString(name.c_str(), GetStringCallback, &payload);

  if (!payload)
    return string{};

  return string{std::move(payload)};
}

} // namespace Microsoft::React

void __cdecl MicrosoftReactSetRuntimeOptionBool(const char *name, bool value) noexcept {
  if (!name)
    return;

  scoped_lock lock{g_runtimeOptionsMutex};
  if (value)
    g_runtimeOptionInts.insert_or_assign(name, 1);
  else
    g_runtimeOptionInts.erase(name);
}

void __cdecl MicrosoftReactSetRuntimeOptionInt(const char *name, int32_t value) noexcept {
  if (!name)
    return;

  scoped_lock lock{g_runtimeOptionsMutex};
  if (value)
    g_runtimeOptionInts.insert_or_assign(name, value);
  else
    g_runtimeOptionInts.erase(name);
}

void __cdecl MicrosoftReactSetRuntimeOptionString(const char *name, const char *value) noexcept {
  if (!name)
    return;

  scoped_lock lock{g_runtimeOptionsMutex};
  if (value)
    g_runtimeOptionStrings.insert_or_assign(name, value);
  else
    g_runtimeOptionStrings.erase(name);
}

bool __cdecl MicrosoftReactGetRuntimeOptionBool(const char *name) noexcept {
  scoped_lock lock{g_runtimeOptionsMutex};
  auto itr = g_runtimeOptionInts.find(name);
  if (itr != g_runtimeOptionInts.end())
    return itr->second != 0;

  return false;
}

int32_t __cdecl MicrosoftReactGetRuntimeOptionInt(const char *name) noexcept {
  scoped_lock lock{g_runtimeOptionsMutex};
  auto itr = g_runtimeOptionInts.find(name);
  if (itr != g_runtimeOptionInts.end())
    return itr->second;

  return 0;
}

void __cdecl MicrosoftReactGetRuntimeOptionString(
    const char *name,
    MicrosoftReactGetStringCallback callBack,
    void *state) {
  scoped_lock lock{g_runtimeOptionsMutex};
  auto itr = g_runtimeOptionStrings.find(name);
  if (itr != g_runtimeOptionStrings.cend()) {
    callBack(itr->second.c_str(), itr->second.size() * sizeof(char) + 1 /*NULL termination*/, state);
  } else {
    callBack(nullptr, 0, state);
  }
}
