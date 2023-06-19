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

HRESULT UiaGetBoundingRectangleHelper(::Microsoft::ReactNative::ReactTaggedView &view, UiaRect &rect) noexcept;

HRESULT UiaSetFocusHelper(::Microsoft::ReactNative::ReactTaggedView &view) noexcept;

void UpdateUiaProperty(winrt::IInspectable provider, PROPERTYID propId, bool oldValue, bool newValue) noexcept;

void UpdateUiaProperty(
    winrt::IInspectable provider,
    PROPERTYID propId,
    const std::string &oldValue,
    const std::string &newValue) noexcept;

} // namespace winrt::Microsoft::ReactNative::implementation
