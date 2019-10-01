// The ABICxxModule implements the CxxModule interface and wraps up the ABI-safe
// NativeModule.

#pragma once

#include "cxxreact/CxxModule.h"
#include "DynamicReader.h"
#include "DynamicWriter.h"
#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::Microsoft::ReactNative::Bridge {

struct ABICxxModuleEventSetter {
  std::string Name;
  EventSetter EventSetter;
};

struct ABICxxModule : facebook::xplat::module::CxxModule {
  ABICxxModule(
      winrt::Windows::Foundation::IInspectable &nativeModule,
      std::string name,
      std::string eventEmitterName,
      std::vector<facebook::xplat::module::CxxModule::Method> methods,
      std::vector<ConstantWriterDelegate> constants,
      std::vector<ABICxxModuleEventSetter> eventSetters) noexcept;

 public: // CxxModule implementation
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<facebook::xplat::module::CxxModule::Method> getMethods() override;

 private:
  void InitEvents(std::vector<ABICxxModuleEventSetter> eventSetters) noexcept;

 private:
  winrt::Windows::Foundation::IInspectable m_nativeModule;
  std::string m_name;
  std::string m_eventEmitterName;
  std::vector<facebook::xplat::module::CxxModule::Method> m_methods;
  std::vector<ConstantWriterDelegate> m_constants;
};

} // namespace winrt::Microsoft::ReactNative::Bridge
