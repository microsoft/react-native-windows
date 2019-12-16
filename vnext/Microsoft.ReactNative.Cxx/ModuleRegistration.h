// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "winrt/Microsoft.ReactNative.Bridge.h"

#define INTERNAL_REACT_MODULE_3_ARGS(moduleClass, moduleName, eventEmitterName)                               \
  struct moduleClass;                                                                                         \
                                                                                                              \
  template <class TDummy>                                                                                     \
  struct moduleClass##_ModuleRegistration final : winrt::Microsoft::ReactNative::Bridge::ModuleRegistration { \
    moduleClass##_ModuleRegistration() noexcept                                                               \
        : winrt::Microsoft::ReactNative::Bridge::ModuleRegistration(moduleName) {}                            \
                                                                                                              \
    winrt::Microsoft::ReactNative::Bridge::ReactModuleProvider MakeModuleProvider() const noexcept override { \
      return winrt::Microsoft::ReactNative::Bridge::MakeModuleProvider<moduleClass>();                        \
    }                                                                                                         \
                                                                                                              \
    static const moduleClass##_ModuleRegistration Registration;                                               \
  };                                                                                                          \
                                                                                                              \
  template <class TDummy>                                                                                     \
  const moduleClass##_ModuleRegistration<TDummy> moduleClass##_ModuleRegistration<TDummy>::Registration;      \
  template struct moduleClass##_ModuleRegistration<int>;                                                      \
                                                                                                              \
  template <class TRegistry>                                                                                  \
  void RegisterModule(TRegistry &registry, moduleClass *) noexcept {                                          \
    registry.RegisterModule<moduleClass>(                                                                     \
        moduleName, eventEmitterName, winrt::Microsoft::ReactNative::Bridge::ReactMemberId<__COUNTER__>{});   \
  }

#define INTERNAL_REACT_MODULE_2_ARGS(moduleClass, moduleName) INTERNAL_REACT_MODULE_3_ARGS(moduleClass, moduleName, L"")

#define INTERNAL_REACT_MODULE_1_ARGS(moduleClass) INTERNAL_REACT_MODULE_2_ARGS(moduleClass, L## #moduleClass)

#define INTERNAL_REACT_MODULE_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4

#define INTERNAL_REACT_MODULE_RECOMPOSER(argsWithParentheses) INTERNAL_REACT_MODULE_4TH_ARG argsWithParentheses

#define INTERNAL_REACT_MODULE(...)  \
  INTERNAL_REACT_MODULE_RECOMPOSER( \
      (__VA_ARGS__, INTERNAL_REACT_MODULE_3_ARGS, INTERNAL_REACT_MODULE_2_ARGS, INTERNAL_REACT_MODULE_1_ARGS, ))

namespace winrt::Microsoft::ReactNative::Bridge {

struct ModuleRegistration {
  ModuleRegistration(const wchar_t *moduleName) noexcept;

  virtual winrt::Microsoft::ReactNative::Bridge::ReactModuleProvider MakeModuleProvider() const noexcept = 0;

  static const ModuleRegistration *Head() noexcept {
    return s_head;
  }

  const ModuleRegistration *Next() const noexcept {
    return m_next;
  }

  const wchar_t *ModuleName() const noexcept {
    return m_moduleName;
  }

 private:
  const wchar_t *m_moduleName;
  const ModuleRegistration *m_next{nullptr};

  static const ModuleRegistration *s_head;
};

void AddAttributedModules(winrt::Microsoft::ReactNative::Bridge::IReactPackageBuilder const &packageBuilder) noexcept;

} // namespace winrt::Microsoft::ReactNative::Bridge
