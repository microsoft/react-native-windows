// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "winrt/Microsoft.ReactNative.h"

#define INTERNAL_REACT_MODULE_3_ARGS(moduleStruct, moduleName, eventEmitterName)                                    \
  struct moduleStruct;                                                                                              \
                                                                                                                    \
  template <class TDummy>                                                                                           \
  struct moduleStruct##_ModuleRegistration final : winrt::Microsoft::ReactNative::ModuleRegistration {              \
    moduleStruct##_ModuleRegistration() noexcept : winrt::Microsoft::ReactNative::ModuleRegistration{moduleName} {} \
                                                                                                                    \
    winrt::Microsoft::ReactNative::ReactModuleProvider MakeModuleProvider() const noexcept override {               \
      return winrt::Microsoft::ReactNative::MakeModuleProvider<moduleStruct>();                                     \
    }                                                                                                               \
                                                                                                                    \
    static const moduleStruct##_ModuleRegistration Registration;                                                    \
  };                                                                                                                \
                                                                                                                    \
  template <class TDummy>                                                                                           \
  const moduleStruct##_ModuleRegistration<TDummy> moduleStruct##_ModuleRegistration<TDummy>::Registration;          \
  template struct moduleStruct##_ModuleRegistration<int>;                                                           \
                                                                                                                    \
  template <class TRegistry>                                                                                        \
  void RegisterModule(TRegistry &registry, moduleStruct *) noexcept {                                               \
    registry.RegisterModule<moduleStruct>(                                                                          \
        moduleName, eventEmitterName, winrt::Microsoft::ReactNative::ReactMemberId<__COUNTER__>{});                 \
  }

#define INTERNAL_REACT_MODULE_2_ARGS(moduleStruct, moduleName) \
  INTERNAL_REACT_MODULE_3_ARGS(moduleStruct, moduleName, nullptr)

#define INTERNAL_REACT_MODULE_1_ARGS(moduleStruct) INTERNAL_REACT_MODULE_2_ARGS(moduleStruct, L## #moduleStruct)

#define INTERNAL_REACT_MODULE_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4

#define INTERNAL_REACT_MODULE_RECOMPOSER(argsWithParentheses) INTERNAL_REACT_MODULE_4TH_ARG argsWithParentheses

#define INTERNAL_REACT_MODULE(...)  \
  INTERNAL_REACT_MODULE_RECOMPOSER( \
      (__VA_ARGS__, INTERNAL_REACT_MODULE_3_ARGS, INTERNAL_REACT_MODULE_2_ARGS, INTERNAL_REACT_MODULE_1_ARGS, ))

namespace winrt::Microsoft::ReactNative {

struct ModuleRegistration {
  ModuleRegistration(const wchar_t *moduleName) noexcept;

  virtual ReactModuleProvider MakeModuleProvider() const noexcept = 0;

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

void AddAttributedModules(IReactPackageBuilder const &packageBuilder) noexcept;

} // namespace winrt::Microsoft::ReactNative
