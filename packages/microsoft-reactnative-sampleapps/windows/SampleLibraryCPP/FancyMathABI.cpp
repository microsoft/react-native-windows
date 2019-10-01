// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "FancyMathABI.h"
#if __has_include("FancyMathABI.g.cpp")
#include "FancyMathABI.g.cpp"
#endif

using namespace winrt;

namespace winrt::SampleLibraryCPP::implementation
{
void FancyMathABI::Initialize() {}

IMapView<hstring, IInspectable> FancyMathABI::Constants() {
  if (nullptr == m_constants) {
    auto constants = single_threaded_map<hstring, IInspectable>();
    m_constants = constants.GetView();
  }

  return m_constants;
}

IVectorView<MethodInfo> FancyMathABI::Methods() {
  if (nullptr == m_methods) {
    auto methods = single_threaded_vector<MethodInfo>();
    m_methods = methods.GetView();
  }

  return m_methods;
}

}
