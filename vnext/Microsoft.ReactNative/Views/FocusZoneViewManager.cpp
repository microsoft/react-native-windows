// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "FocusZoneViewManager.h"

#include "JSValueReader.h"
#include "JSValueXaml.h"
#include "KeyboardEventHandler.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace Microsoft::ReactNative {

namespace win {
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Controls::Primitives;
using namespace winrt::Windows::UI::Xaml::Interop;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Media;
} // namespace win

namespace xaml = winrt::Windows::UI::Xaml;

static const std::unordered_map<std::string, xaml::Input::XYFocusNavigationStrategy>
    xyFocusNavigationStrategy = {
        {"projection", xaml::Input::XYFocusNavigationStrategy::Projection},
        {"navigationDirectionDistance",
         xaml::Input::XYFocusNavigationStrategy::NavigationDirectionDistance},
        {"rectilinearDistance", xaml::Input::XYFocusNavigationStrategy::RectilinearDistance}};

xaml::Input::XYFocusNavigationStrategy parseNavigationStrategyJson(const rn::JSValue& json) {
  auto iter = xyFocusNavigationStrategy.find(json.AsString());
  if (iter != xyFocusNavigationStrategy.end()) {
    return iter->second;
  }

  return xaml::Input::XYFocusNavigationStrategy::Auto;
}

bool IsElementChildOf(win::DependencyObject element, win::DependencyObject expectedParent) {
  win::DependencyObject parent = element;
  do {
    parent = win::VisualTreeHelper::GetParent(parent);
    if (parent == expectedParent) {
      return true;
    }
  } while (parent != nullptr);
  return false;
}

bool IsElementWithinAnotherFocusableElementInContainer(win::DependencyObject element,
                                                       win::FrameworkElement container) {
  auto parent = element;
  do {
    parent = win::VisualTreeHelper::GetParent(parent);
    auto control = parent.try_as<win::Control>();
    if (control != nullptr && control.IsTabStop() && control.IsEnabled()) {
      return true;
    }
  } while (parent != nullptr && parent != container);
  return false;
}

const win::TypeName focusZoneTypeName{winrt::hstring{L"FocusZoneProperties"},
                                      win::TypeKind::Metadata};

xaml::DependencyProperty FocusZoneLastFocusedElementTagProperty() {
  static xaml::DependencyProperty s_FocusZoneLastFocusedElementTagProperty =
      xaml::DependencyProperty::RegisterAttached(
          L"FocusZoneLastFocusedElementTag",
          winrt::xaml_typename<winrt::Windows::Foundation::IInspectable>(),
          focusZoneTypeName,
          win::PropertyMetadata(nullptr));

  return s_FocusZoneLastFocusedElementTagProperty;
}

xaml::DependencyProperty IsMovingFocusOutOfFocusZoneProperty() {
  static xaml::DependencyProperty s_IsMovingFocusOutOfFocusZoneProperty =
      xaml::DependencyProperty::RegisterAttached(L"IsMovingFocusOutOfFocusZone",
                                                 winrt::xaml_typename<bool>(),
                                                 focusZoneTypeName,
                                                 win::PropertyMetadata(winrt::box_value(false)));

  return s_IsMovingFocusOutOfFocusZoneProperty;
}

win::ScrollViewer GetFirstChildScrollViewer(win::DependencyObject element) {
  if (auto scrollViewer = element.try_as<win::ScrollViewer>()) {
    return scrollViewer;
  }

  auto childrenCount = win::VisualTreeHelper::GetChildrenCount(element);
  for (auto i = 0; i < childrenCount; i++) {
    auto child = win::VisualTreeHelper::GetChild(element, i);
    if (auto scrollViewer = GetFirstChildScrollViewer(child)) {
      return scrollViewer;
    }
  }

  return nullptr;
}

void OnFocusZoneKeyDown(const win::IInspectable& sender, const win::KeyRoutedEventArgs& e) {
  // Xaml has a bug (https://github.com/microsoft/microsoft-ui-xaml/issues/1363)
  // with KeyboardNavigationMode::Once where shift+tab won't work properly if
  // focusable elements are nested inside other elements. This fixes that and
  // also tracks the last focused element.

  if (e.Key() == winrt::Windows::System::VirtualKey::Tab) {
    auto senderFrameworkElement = sender.as<xaml::FrameworkElement>();
    auto xamlRoot = senderFrameworkElement.XamlRoot();

    // With XYFocusKeyboardNavigation enabled, the way to focus a child of a
    // focusable element is with tab. Then shift+tab focuses the parent. If
    // we're in that case then let Xaml handle the shift+tab.
    auto focusedElement =
        win::FocusManager::GetFocusedElement(xamlRoot).try_as<win::DependencyObject>();
    if (IsElementWithinAnotherFocusableElementInContainer(focusedElement, senderFrameworkElement)) {
      return;
    }

    auto const& coreWindow = win::CoreWindow::GetForCurrentThread();
    auto isShiftDown =
        KeyboardHelper::IsModifiedKeyPressed(coreWindow, winrt::Windows::System::VirtualKey::Shift);

    if (isShiftDown) {
      if (auto scrollViewer = GetFirstChildScrollViewer(senderFrameworkElement)) {
        // Since we leave the FocusZoneView by moving focus backwards
        // repeatedly, we don't want any ScrollViewer to scroll during
        // this process.
        scrollViewer.BringIntoViewOnFocusChange(false);
      }
      win::FindNextElementOptions findNextElementOptions = win::FindNextElementOptions();
      findNextElementOptions.SearchRoot(xamlRoot.Content());

      auto didMoveFocus = false;
      do {
        didMoveFocus = win::FocusManager::TryMoveFocus(win::FocusNavigationDirection::Previous,
                                                       findNextElementOptions);

        // As we move focus backwards through the focus zone, it triggers the
        // LosingFocus event which stores the last focused element. We don't want
        // to do this after the first time we move focus, so we set this flag.
        senderFrameworkElement.SetValue(IsMovingFocusOutOfFocusZoneProperty(),
                                        winrt::box_value(true));

        focusedElement =
            win::FocusManager::GetFocusedElement(xamlRoot).try_as<win::DependencyObject>();
      } while (didMoveFocus && IsElementChildOf(focusedElement, senderFrameworkElement));
      senderFrameworkElement.SetValue(IsMovingFocusOutOfFocusZoneProperty(),
                                      winrt::box_value(false));
      e.Handled(true);
    }
  }
}

void OnFocusZoneGettingFocus(const win::IInspectable& sender,
                             const win::GettingFocusEventArgs& e,
                             rn::IReactContext reactContext) {
  auto senderDependencyObject = sender.as<win::DependencyObject>();
  auto isOldFocusedElementOutsideFocusZoneView =
      e.OldFocusedElement() != nullptr &&
      !IsElementChildOf(e.OldFocusedElement(), senderDependencyObject);

  if (isOldFocusedElementOutsideFocusZoneView) {
    if (auto scrollViewer = GetFirstChildScrollViewer(senderDependencyObject)) {
      // Revert this property from above.
      scrollViewer.BringIntoViewOnFocusChange(true);
    }

    auto lastFocusedElementTag = winrt::unbox_value_or<int64_t>(
        senderDependencyObject.GetValue(FocusZoneLastFocusedElementTagProperty()), -1);
    if (lastFocusedElementTag == -1) {
      return;
    }

    auto xamlUIService = React::XamlUIService::FromContext(reactContext);
    const auto lastFocusedElement = xamlUIService.ElementFromReactTag(lastFocusedElementTag);
    if (lastFocusedElement != nullptr && e.NewFocusedElement() != lastFocusedElement) {
      e.TrySetNewFocusedElement(lastFocusedElement);
    }
  }
}

void OnFocusZoneLosingFocus(const win::IInspectable& sender, const win::LosingFocusEventArgs& e) {
  auto focusedElement = e.OldFocusedElement();
  if (!focusedElement) {
    return;
  }

  auto senderFrameworkElement = sender.as<xaml::FrameworkElement>();
  if (winrt::unbox_value<bool>(
          senderFrameworkElement.GetValue(IsMovingFocusOutOfFocusZoneProperty()))) {
    return;
  }
  auto tagPropertyValue =
      focusedElement.GetValue(win::FrameworkElement::TagProperty()).try_as<win::IPropertyValue>();
  if (!tagPropertyValue) {
    return;
  }
  auto focusedElementTag = tagPropertyValue.GetInt64();

  // We purposely don't store a reference to the element directly since it
  // would be a strong reference.
  senderFrameworkElement.SetValue(FocusZoneLastFocusedElementTagProperty(),
                                  winrt::box_value(focusedElementTag));
}

void MakeFocusZone(xaml::UIElement element, rn::IReactContext reactContext) {
  if (element.XYFocusKeyboardNavigation() == xaml::Input::XYFocusKeyboardNavigationMode::Enabled) {
    return;
  }

  element.TabFocusNavigation(xaml::Input::KeyboardNavigationMode::Once);
  element.XYFocusKeyboardNavigation(xaml::Input::XYFocusKeyboardNavigationMode::Enabled);
  element.KeyDown(OnFocusZoneKeyDown);
  element.GettingFocus(
      [reactContext](const win::IInspectable& sender, const win::GettingFocusEventArgs& e) {
        OnFocusZoneGettingFocus(sender, e, reactContext);
      });
  element.LosingFocus(OnFocusZoneLosingFocus);
}

void UpdateFocusZoneXYFocusNavigationStrategy(win::UIElement focusZoneElement,
                                              const rn::JSValue& propertyValue) {
  auto xyFocusNavigationStrategy = parseNavigationStrategyJson(propertyValue);
  focusZoneElement.XYFocusDownNavigationStrategy(xyFocusNavigationStrategy);
  focusZoneElement.XYFocusUpNavigationStrategy(xyFocusNavigationStrategy);
  focusZoneElement.XYFocusLeftNavigationStrategy(xyFocusNavigationStrategy);
  focusZoneElement.XYFocusRightNavigationStrategy(xyFocusNavigationStrategy);
}

// FocusZoneViewManager

winrt::hstring FocusZoneViewManager::Name() noexcept {
  return L"FocusZoneView";
}

bool DoubleEquals(double d1, double d2) {
  return std::fabs(d1 - d2) <= std::numeric_limits<double>().epsilon();
}

void GrowFocusZoneIfNeeded(win::ScrollViewer const& scrollViewer, rn::ViewPanel const& focusZone) {
  if (!scrollViewer.IsLoaded() || !focusZone.IsLoaded()) {
    return;
  }

  auto isSizedCorrectly = DoubleEquals(scrollViewer.ActualHeight() + 1, focusZone.ActualHeight()) &&
                          DoubleEquals(scrollViewer.ActualWidth() + 1, focusZone.ActualWidth());
  if (!isSizedCorrectly) {
    focusZone.Height(scrollViewer.ActualHeight() + 1);
    focusZone.Width(scrollViewer.ActualWidth() + 1);
  }
}

win::FrameworkElement FocusZoneViewManager::CreateView() noexcept {
  auto const& focusZone = rn::ViewPanel();

  MakeFocusZone(focusZone, m_reactContext);

  // Workaround XAML Islands bug with XYFocusKeyboardNavigation:
  // https://github.com/microsoft/microsoft-ui-xaml/issues/5664

  win::ScrollViewer scrollViewer;
  scrollViewer.VerticalScrollMode(xaml::Controls::ScrollMode::Disabled);
  scrollViewer.VerticalScrollBarVisibility(xaml::Controls::ScrollBarVisibility::Hidden);
  scrollViewer.HorizontalScrollMode(xaml::Controls::ScrollMode::Disabled);
  scrollViewer.HorizontalScrollBarVisibility(xaml::Controls::ScrollBarVisibility::Hidden);

  // We need to disable scrolling in this diabled ScrollViewer when children are focused.
  // Even though the size of this ScrollViewer and its content is such that scrolling should never
  // need to happen for focus changes, XAML still scrolls for some reason.
  // Usually we'd fix this with ScrollViewer.BringIntoViewOnFocusChange but we can't because if a
  // parent ScrollViewer is supposed to scroll due to focus changes, if this one has that property
  // set to false it will cancel the parent's ability to scroll into view. Instead we scroll back
  // to origin on any scroll events. This seems to happen fast enough that it's not noticeable.
  scrollViewer.ViewChanged([](auto const& sender, auto&&...) {
    sender.as<win::ScrollViewer>().ChangeView(0.0, 0.0, nullptr, true);
  });

  scrollViewer.Loaded([focusZone](auto const& sender, auto&&...) {
    GrowFocusZoneIfNeeded(sender.as<win::ScrollViewer>(), focusZone);
  });
  scrollViewer.SizeChanged([focusZone](auto const& sender, auto&&...) {
    GrowFocusZoneIfNeeded(sender.as<win::ScrollViewer>(), focusZone);
  });

  scrollViewer.Content(focusZone);
  return scrollViewer;
}

rn::ViewPanel GetFocusZone(win::FrameworkElement const& parent) noexcept {
  auto scrollViewer = parent.as<win::ScrollViewer>();
  return scrollViewer.Content().as<rn::ViewPanel>();
}

rn::IReactContext FocusZoneViewManager::ReactContext() noexcept {
  return m_reactContext;
}

void FocusZoneViewManager::ReactContext(rn::IReactContext reactContext) noexcept {
  m_reactContext = reactContext;
}

winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, rn::ViewManagerPropertyType>
FocusZoneViewManager::NativeProps() noexcept {
  auto nativeProps = winrt::single_threaded_map<winrt::hstring, win::ViewManagerPropertyType>();
  nativeProps.Insert(L"xyFocusNavigationStrategy", win::ViewManagerPropertyType::String);
  nativeProps.Insert(L"backgroundColor", win::ViewManagerPropertyType::Color);
  return nativeProps.GetView();
}

void FocusZoneViewManager::UpdateProperties(winrt::Windows::UI::Xaml::FrameworkElement const& view,
                                            rn::IJSValueReader const& propertyMapReader) noexcept {
  win::JSValueObject propertyMap = win::JSValueObject::ReadFrom(propertyMapReader);

  for (const auto& pair : propertyMap) {
    const auto& propertyName = winrt::to_hstring(pair.first);
    const auto& propertyValue = pair.second;

    if (propertyName == L"xyFocusNavigationStrategy") {
      UpdateFocusZoneXYFocusNavigationStrategy(GetFocusZone(view), propertyValue);
    } else if (propertyName == L"overflow") {
      // TODO(T95587553): Copied from ViewViewManager.cpp.
      // Can fix after MSFT resolves https://github.com/microsoft/react-native-windows/issues/8165.
      if (propertyValue.Type() == win::JSValueType::String) {
        bool clipChildren = propertyValue.AsString() == "hidden";
        GetFocusZone(view).ClipChildren(clipChildren);
      }
    } else if (propertyName == L"backgroundColor") {
      // TODO(T95587553): Copied from ViewViewManager.cpp.
      // Can fix after MSFT resolves https://github.com/microsoft/react-native-windows/issues/8165.
      const auto viewPanel = GetFocusZone(view);
      if (!propertyValue.IsNull()) {
        viewPanel.Background(propertyValue.To<win::Brush>());
      } else {
        viewPanel.ClearValue(win::Panel::BackgroundProperty());
      }
    }
  }
}

void FocusZoneViewManager::AddView(win::FrameworkElement const& parent,
                                   win::UIElement const& child,
                                   int64_t index) noexcept {
  if (auto const& panel = GetFocusZone(parent)) {
    panel.Children().InsertAt(index, child);
  }
}

void FocusZoneViewManager::RemoveAllChildren(win::FrameworkElement const& parent) noexcept {
  if (auto const& panel = GetFocusZone(parent)) {
    panel.Children().Clear();
  }
}

void FocusZoneViewManager::RemoveChildAt(win::FrameworkElement const& parent,
                                         int64_t index) noexcept {
  if (auto const& panel = GetFocusZone(parent)) {
    panel.Children().RemoveAt(index);
  }
}

void FocusZoneViewManager::ReplaceChild(win::FrameworkElement const& parent,
                                        win::UIElement const& oldChild,
                                        win::UIElement const& newChild) noexcept {
  if (auto const& panel = GetFocusZone(parent)) {
    uint32_t index;
    if (panel.Children().IndexOf(oldChild, index)) {
      panel.Children().RemoveAt(index);
      panel.Children().InsertAt(index, newChild);
    }
  }
}

} // namespace Microsoft::ReactNative
