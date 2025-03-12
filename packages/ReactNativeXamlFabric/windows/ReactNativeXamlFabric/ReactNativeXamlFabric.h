#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/NativeReactNativeXamlFabricDataTypes.g.h")
#include "codegen/NativeReactNativeXamlFabricDataTypes.g.h"
#endif
// #include "codegen/NativeReactNativeXamlFabricSpec.g.h"

#include "NativeModules.h"

namespace winrt::ReactNativeXamlFabric {

REACT_MODULE(ReactNativeXamlFabric)
struct ReactNativeXamlFabric {
  // using ModuleSpec = ReactNativeXamlFabricCodegen::ReactNativeXamlFabricSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_SYNC_METHOD(multiply)
  double multiply(double a, double b) noexcept;

 private:
  React::ReactContext m_context;
};

} // namespace winrt::ReactNativeXamlFabric