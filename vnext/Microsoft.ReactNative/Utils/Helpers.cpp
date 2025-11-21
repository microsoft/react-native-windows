// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/Helpers.h>
#include <winrt/Windows.Foundation.Metadata.h>

#include <appmodel.h>
#include <processthreadsapi.h>

#include <Fabric/Composition/CompositionUIService.h>

namespace winrt {
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

bool IsAPIContractV7Available() {
  return IsAPIContractVxAvailable<7>();
}

bool IsAPIContractV8Available() {
  return IsAPIContractVxAvailable<8>();
}

bool IsRS5OrHigher() {
  return IsAPIContractV7Available();
}

bool Is19H1OrHigher() {
  return IsAPIContractV8Available();
}

bool IsFabricEnabled(winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) {
  return winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
             properties) != nullptr;
}

} // namespace Microsoft::ReactNative
