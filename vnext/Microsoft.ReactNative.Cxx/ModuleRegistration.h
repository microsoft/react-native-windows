#pragma once
#include "winrt/Microsoft.ReactNative.Bridge.h"
#include "winrt/Microsoft.ReactNative.h"

#define INTERNAL_REACT_MODULE_3_ARGS(                                     \
    moduleClass, moduleName, eventEmitterName)                            \
  struct moduleClass;                                                     \
                                                                          \
  template <class TDummy>                                                 \
  struct moduleClass##_ModuleRegistration final                           \
      : ::Microsoft::ReactNative::ModuleRegistration {                    \
    moduleClass##_ModuleRegistration() noexcept                           \
        : ::Microsoft::ReactNative::ModuleRegistration(                   \
              moduleName,                                                 \
              eventEmitterName) {}                                        \
                                                                          \
    winrt::Microsoft::ReactNative::Bridge::ReactModuleProvider            \
    MakeModuleProvider() const noexcept override {                        \
      return ::Microsoft::ReactNative::MakeModuleProvider<moduleClass>(); \
    }                                                                     \
                                                                          \
    static const moduleClass##_ModuleRegistration Registration;           \
  };                                                                      \
                                                                          \
  template <class TDummy>                                                 \
  const moduleClass##_ModuleRegistration<TDummy>                          \
      moduleClass##_ModuleRegistration<TDummy>::Registration;             \
  template struct moduleClass##_ModuleRegistration<int>;

#define INTERNAL_REACT_MODULE_2_ARGS(moduleClass, moduleName) \
  INTERNAL_REACT_MODULE_3_ARGS(moduleClass, moduleName, moduleName)

#define INTERNAL_REACT_MODULE_1_ARGS(moduleClass) \
  INTERNAL_REACT_MODULE_2_ARGS(moduleClass, #moduleClass)

#define INTERNAL_REACT_MODULE_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4

#define INTERNAL_REACT_MODULE_RECOMPOSER(argsWithParentheses) \
  INTERNAL_REACT_MODULE_4TH_ARG argsWithParentheses

#define INTERNAL_REACT_MODULE_MACRO_CHOOSER(...) \
  INTERNAL_REACT_MODULE_RECOMPOSER(              \
      (__VA_ARGS__,                              \
       INTERNAL_REACT_MODULE_3_ARGS,             \
       INTERNAL_REACT_MODULE_2_ARGS,             \
       INTERNAL_REACT_MODULE_1_ARGS, ))

namespace Microsoft::ReactNative {

struct ModuleRegistration {
  ModuleRegistration(
      const char *moduleName,
      const char *eventEmitterName) noexcept;

  virtual winrt::Microsoft::ReactNative::Bridge::ReactModuleProvider
  MakeModuleProvider() const noexcept = 0;

  static const ModuleRegistration *Head() noexcept {
    return s_head;
  }

  const ModuleRegistration *Next() const noexcept {
    return m_next;
  }

  const char *ModuleName() const noexcept {
    return m_moduleName;
  }

 private:
  const char *m_moduleName;
  const char *m_eventEmitterName;
  const ModuleRegistration *m_next{nullptr};

  static const ModuleRegistration *s_head;
};

void AddAttributedModules(
    winrt::Microsoft::ReactNative::Bridge::IReactPackageBuilder const
        &packageBuilder) noexcept;

} // namespace Microsoft::ReactNative
