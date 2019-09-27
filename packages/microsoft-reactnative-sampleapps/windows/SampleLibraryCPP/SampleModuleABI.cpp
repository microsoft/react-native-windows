// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "SampleModuleABI.h"
#if __has_include("SampleModuleABI.g.cpp")
#include "SampleModuleABI.g.cpp"
#endif

using namespace winrt;

namespace winrt::SampleLibraryCPP::implementation {
void SampleModuleABI::Initialize() {}

IMapView<hstring, IInspectable> SampleModuleABI::Constants() {
  if (nullptr == m_constants) {
    auto constants = single_threaded_map<hstring, IInspectable>();
    m_constants = constants.GetView();
  }

  return m_constants;
}

IVectorView<MethodInfo> SampleModuleABI::Methods() {
  if (nullptr == m_methods) {
    auto methods = single_threaded_vector<MethodInfo>();
    m_methods = methods.GetView();
  }

  return m_methods;
}

} // namespace winrt::SampleLibraryCPP::implementation
