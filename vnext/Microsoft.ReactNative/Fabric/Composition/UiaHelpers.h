#pragma once

#include <Fabric/ComponentView.h>
#include <Fabric/ReactTaggedView.h>
#include <UIAutomation.h>

namespace Microsoft::ReactNative {
struct winrt::Microsoft::ReactNative::implementation::ComponentView;
}

namespace winrt::Microsoft::ReactNative::implementation {

// Returns an AddRef'd neighbor of the provided ComponentView in the direction given
HRESULT
UiaNavigateHelper(
    const winrt::Microsoft::ReactNative::ComponentView &view,
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

long GetLiveSetting(const std::string &liveRegion) noexcept;

std::string extractAccessibilityValue(const facebook::react::AccessibilityValue &value) noexcept;

} // namespace winrt::Microsoft::ReactNative::implementation
