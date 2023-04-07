#pragma once

#include <Fabric/ComponentView.h>
#include <Fabric/ReactTaggedView.h>
#include <UIAutomation.h>

namespace winrt::Microsoft::ReactNative::implementation {

// Returns an AddRef'd neighbor of the provided IComponentView in the direction given
HRESULT
UiaNavigateHelper(
    ::Microsoft::ReactNative::ReactTaggedView &view,
    NavigateDirection direction,
    IRawElementProviderFragment *&retVal) noexcept;

} // namespace winrt::Microsoft::ReactNative::implementation
