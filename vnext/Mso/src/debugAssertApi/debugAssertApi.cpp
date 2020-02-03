// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "debugAssertApi/debugAssertApi.h"
#include <algorithm>
#include <memory>
#include <set>
#include "compilerAdapters/cppMacrosDebug.h"
#include "cppExtensions/autoRestore.h"

#ifdef DEBUG

namespace Mso {
namespace DebugAsserts {

struct Data {
  std::set<uint32_t> DisabledTags;
  std::set<AssertIgnorer> Ignorers;
  std::set<AssertListener> Listeners;
  AssertHandler Handler = nullptr;

  static Data &Get() noexcept {
    // To avoid static initializer ordering issues, this object is created on demand and purposely leaked
    static auto s_data = Construct();
    return *s_data;
  }

 private:
  static Data *Construct() noexcept {
    // xlsrv doesn't allow calling new from static initializers. Using std::allocator instead, which has been modified
    // to call malloc.
    std::allocator<Data> alloc;
    auto p = alloc.allocate(1);
    alloc.construct(p);
    return p;
  }
};

void DisableAssertTag(uint32_t tag) noexcept {
  Data::Get().DisabledTags.insert(tag);
}

void EnableAssertTag(uint32_t tag) noexcept {
  Data::Get().DisabledTags.erase(tag);
}

bool IsAssertTagDisabled(uint32_t tag) noexcept {
  const auto &disabledTags = Data::Get().DisabledTags;
  return (disabledTags.find(tag) != std::end(disabledTags));
}

void AddAssertIgnorer(AssertIgnorer ignorer) noexcept {
  Data::Get().Ignorers.insert(ignorer);
}

void RemoveAssertIgnorer(AssertIgnorer ignorer) noexcept {
  Data::Get().Ignorers.erase(ignorer);
}

void AddAssertListener(AssertListener listener) noexcept {
  Data::Get().Listeners.insert(listener);
}

void RemoveAssertListener(AssertListener listener) noexcept {
  Data::Get().Listeners.erase(listener);
}

AssertHandler GetAssertHandler() noexcept {
  return Data::Get().Handler;
}

AssertHandler SetAssertHandler(AssertHandler handler) noexcept {
  std::swap(Data::Get().Handler, handler);
  return handler;
}

static bool v_isInAssertHandler;

extern "C" int32_t MsoAssertSzTagProc(
    const MsoAssertParams &params,
    _Printf_format_string_ const char *szFormat,
    va_list argList) noexcept {
  if (IsAssertTagDisabled(params.dwTag))
    return c_assertIgnore;

  char assertMessage[4096];
  (void)vsnprintf(assertMessage, sizeof(assertMessage), szFormat, argList);
  params.framesToSkip++;

  const auto &data = Data::Get();
  for (const auto &ignorer : data.Ignorers) {
    if (ignorer(params, assertMessage))
      return c_assertIgnore;
  }

  for (const auto &listener : data.Listeners) {
    listener(params, assertMessage);
  }

  if (data.Handler) {
    Mso::TRestorer<bool> restoreInAssertHandler(v_isInAssertHandler, true);
    return static_cast<int32_t>(data.Handler(params, assertMessage));
  }

  return c_assertIgnore;
}

bool IsInAssertHandler() noexcept {
  return v_isInAssertHandler;
}

} // namespace DebugAsserts
} // namespace Mso

// LIBLET_PUBLICAPI_APPLE MSOAPI_(void) MsoFAddIgnoredAssertTag(uint32_t tag) noexcept
//{
//	Mso::DebugAsserts::DisableAssertTag(tag);
//}
//
// LIBLET_PUBLICAPI_APPLE MSOAPI_(void) MsoFRemoveIgnoredAssertTag(uint32_t tag) noexcept
//{
//	Mso::DebugAsserts::EnableAssertTag(tag);
//}

#endif // DEBUG
