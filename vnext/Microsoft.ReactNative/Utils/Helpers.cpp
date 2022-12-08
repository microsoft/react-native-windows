// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/Helpers.h>
#include <winrt/Windows.Foundation.Metadata.h>

#include <appmodel.h>
#include <processthreadsapi.h>

#ifdef USE_FABRIC
#include <Fabric/Composition/CompositionUIService.h>
#endif // USE_FABRIC

namespace winrt {
using namespace xaml::Controls::Primitives;
using namespace xaml::Media;
using namespace Windows::Foundation::Metadata;
} // namespace winrt

namespace Microsoft::ReactNative {

template <uint16_t APIVersion>
bool IsAPIContractVxAvailable() {
  static bool isAPIContractVxAvailableInitialized = false;
  static bool isAPIContractVxAvailable = false;
  if (!isAPIContractVxAvailableInitialized) {
    isAPIContractVxAvailableInitialized = true;
    isAPIContractVxAvailable =
        winrt::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", APIVersion);
  }

  return isAPIContractVxAvailable;
}

bool IsAPIContractV5Available() {
  return IsAPIContractVxAvailable<5>();
}

bool IsAPIContractV6Available() {
  return IsAPIContractVxAvailable<6>();
}

bool IsAPIContractV7Available() {
  return IsAPIContractVxAvailable<7>();
}

bool IsAPIContractV8Available() {
  return IsAPIContractVxAvailable<8>();
}

bool IsAPIContractV12Available() {
  return IsAPIContractVxAvailable<12>();
}

bool IsRS3OrHigher() {
  return IsAPIContractV5Available();
}

bool IsRS4OrHigher() {
  return IsAPIContractV6Available();
}

bool IsRS5OrHigher() {
  return IsAPIContractV7Available();
}

bool Is19H1OrHigher() {
  return IsAPIContractV8Available();
}

bool Is21H1OrHigher() {
  return IsAPIContractV12Available();
}

bool IsXamlIsland() {
  AppPolicyWindowingModel e;
  if (FAILED(AppPolicyGetWindowingModel(GetCurrentThreadEffectiveToken(), &e)) ||
      e == AppPolicyWindowingModel_ClassicDesktop) {
    return true;
  }
  return false;
}

bool IsWinUI3Island() {
#ifndef USE_WINUI3
  return false;
#else
  return IsXamlIsland();
#endif
}

bool IsFabricEnabled(winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) {
#ifdef USE_FABRIC
  return winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
             properties) != nullptr;
#else
  return false;
#endif
}

} // namespace Microsoft::ReactNative
