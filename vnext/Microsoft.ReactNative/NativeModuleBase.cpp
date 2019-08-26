// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeModuleBase.h"
#if __has_include("Bridge.NativeModuleBase.g.cpp")
#include "Bridge.NativeModuleBase.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::Microsoft::ReactNative::Bridge::implementation {

hstring NativeModuleBase::Name() {
  throw hresult_not_implemented(L"'Name' must be implemented.");
}

IMapView<hstring, IInspectable> NativeModuleBase::Constants() {
  return single_threaded_map<hstring, IInspectable>().GetView();
}

IVectorView<Bridge::MethodInfo> NativeModuleBase::Methods() {
  return single_threaded_vector<Bridge::MethodInfo>().GetView();
}

void NativeModuleBase::Initialize() {
  // no-op
}
} // namespace winrt::Microsoft::ReactNative::Bridge::implementation
