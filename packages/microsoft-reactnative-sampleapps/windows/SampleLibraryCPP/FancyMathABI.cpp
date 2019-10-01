// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "FancyMathABI.h"
#if __has_include("FancyMathABI.g.cpp")
#include "FancyMathABI.g.cpp"
#endif

#define _USE_MATH_DEFINES

#include <math.h>

using namespace winrt;

namespace winrt::SampleLibraryCPP::implementation {
void FancyMathABI::Initialize() {
  OutputDebugStringW(L"FancyMathABI::Initialize()\n");
}

IMapView<hstring, IInspectable> FancyMathABI::Constants() {
  if (nullptr == m_constants) {
    auto constants = single_threaded_map<hstring, IInspectable>();

    constants.Insert(L"E", box_value(M_E));
    constants.Insert(L"Pi", box_value(M_PI));

    m_constants = constants.GetView();
  }

  return m_constants;
}

IVectorView<MethodInfo> FancyMathABI::Methods() {
  if (nullptr == m_methods) {
    auto methods = single_threaded_vector<MethodInfo>();

    methods.Append(MethodInfo(
        L"add",
        ReturnType::Callback,
        [](IVectorView<IInspectable> const &args,
           Callback const &callback,
           Callback const & /*ignored*/) {
          /*
          TODO: args are being sent as a json string, but should already be
          an object array, ie.

          double a = args.GetAt(0).as<IPropertyValue>().GetDouble();
          double b = args.GetAt(1).as<IPropertyValue>().GetDouble();
          */

          std::wstring json(args.GetAt(0).as<IPropertyValue>().GetString());

          size_t commaIndex = json.find_first_of(L",", 0);

          double a = std::stod(json.substr(1, commaIndex - 1));
          double b = std::stod(json.substr(commaIndex + 1, json.size() - (commaIndex + 2)));

          double result = a + b;
          callback(single_threaded_vector<IInspectable>({box_value(result)})
                       .GetView());
        }));

    m_methods = methods.GetView();
  }

  return m_methods;
}

} // namespace winrt::SampleLibraryCPP::implementation
