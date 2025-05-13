#pragma once

#include <Fabric/ComponentView.h>
#include <Fabric/Composition/CompositionDynamicAutomationProvider.h>
#include <Fabric/Composition/ParagraphComponentView.h>
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

void UpdateUiaProperty(
    winrt::IInspectable provider,
    PROPERTYID propId,
    const std::optional<std::string> &oldValue,
    const std::optional<std::string> &newValue) noexcept;

long GetLiveSetting(const std::string &liveRegion) noexcept;

long GetAnnotationTypeId(const std::string &annotationType) noexcept;

bool accessibilityAnnotationHasValue(
    const std::optional<facebook::react::AccessibilityAnnotation> &annotation) noexcept;

void DispatchAccessibilityAction(::Microsoft::ReactNative::ReactTaggedView &view, const std::string &action) noexcept;

ExpandCollapseState GetExpandCollapseState(const bool &expanded) noexcept;

void AddSelectionItemsToContainer(CompositionDynamicAutomationProvider *provider) noexcept;

void RemoveSelectionItemsFromContainer(CompositionDynamicAutomationProvider *provider) noexcept;

ToggleState GetToggleState(const std::optional<facebook::react::AccessibilityState> &state) noexcept;

TextDecorationLineStyle GetTextDecorationLineStyle(facebook::react::TextDecorationStyle style) noexcept;
} // namespace winrt::Microsoft::ReactNative::implementation
