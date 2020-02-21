// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// The ABICxxModule implements the CxxModule interface and wraps up the ABI-safe
// NativeModule.
//

#pragma once

#include "DynamicReader.h"
#include "DynamicWriter.h"
#include "ReactHost/React.h"
#include "cxxreact/CxxModule.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct ABICxxModule : facebook::xplat::module::CxxModule {
  ABICxxModule(
      winrt::Windows::Foundation::IInspectable const &nativeModule,
      std::string &&name,
      std::vector<ConstantProviderDelegate> &&constantProviders,
      std::vector<facebook::xplat::module::CxxModule::Method> &&methods) noexcept;

 public: // CxxModule implementation
  std::string getName() noexcept override;
  std::map<std::string, folly::dynamic> getConstants() noexcept override;
  std::vector<facebook::xplat::module::CxxModule::Method> getMethods() noexcept override;

 private:
  winrt::Windows::Foundation::IInspectable m_nativeModule; // just to keep native module alive
  std::string m_name;
  std::vector<ConstantProviderDelegate> m_constantProviders;
  std::vector<facebook::xplat::module::CxxModule::Method> m_methods;
};

} // namespace winrt::Microsoft::ReactNative
