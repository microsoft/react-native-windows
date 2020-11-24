// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "FocusZoneViewManager.h"

#include "ViewControl.h"

#include "DynamicAutomationProperties.h"

#include <Modules/NativeUIManager.h>
#include <Utils/AccessibilityUtils.h>
#include <Utils/Helpers.h>
#include <Utils/PropertyUtils.h>

#include <INativeUIManager.h>
#include <IReactInstance.h>

#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>

#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Input.h>
#include <winrt/Windows.ApplicationModel.Resources.Core.h>

namespace winrt {
using namespace Windows::Foundation;
} // namespace winrt

namespace Microsoft {
namespace ReactNative {

void FocusZoneViewShadowNode::createView() {
  Super::createView();

  auto panel = GetViewPanel();
  m_panelKeyDownRevoker =
      panel.PreviewKeyDown(winrt::auto_revoke, [=](const auto &sender, const auto &e) { OnKeyDown(sender, e); });
  m_panelGettingFocusRevoker =
      panel.GettingFocus(winrt::auto_revoke, [=](const auto &sender, const auto &e) { OnGettingFocus(sender, e); });
  m_panelLosingFocusRevoker =
      panel.LosingFocus(winrt::auto_revoke, [=](const auto &sender, const auto &e) { OnLosingFocus(sender, e); });
}

void FocusZoneViewShadowNode::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {
  for (const auto &pair : props) {
    const std::string &propertyName = pair.first;
    const auto &propertyValue = pair.second;

    if (propertyName == "focusDirection") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        m_isHorizontal = propertyValue.AsString() == "horizontal";
      } else {
        m_isHorizontal = false;
      }
    }
  }
}

void FocusZoneViewShadowNode::OnKeyDown(const winrt::IInspectable &sender, const xaml::Input::KeyRoutedEventArgs &e) {
  auto senderFrameworkElement = sender.try_as<xaml::FrameworkElement>();
  auto xamlRoot = senderFrameworkElement.XamlRoot();
  auto focusedElement = xaml::Input::FocusManager::GetFocusedElement(xamlRoot).try_as<xaml::DependencyObject>();
  auto findNextElementOptions = xaml::Input::FindNextElementOptions();
  findNextElementOptions.SearchRoot(xamlRoot.Content());

  auto previousKey = m_isHorizontal ? winrt::system::VirtualKey::Left : winrt::system::VirtualKey::Up;
  auto nextKey = m_isHorizontal ? winrt::system::VirtualKey::Right : winrt::system::VirtualKey::Down;

  if (e.Key() == previousKey) {
    auto firstFocusableElement =
        xaml::Input::FocusManager::FindFirstFocusableElement(senderFrameworkElement).try_as<xaml::DependencyObject>();
    auto isFirstElementFocused = firstFocusableElement == focusedElement;
    if (!isFirstElementFocused) {
      xaml::Input::FocusManager::TryMoveFocus(xaml::Input::FocusNavigationDirection::Previous, findNextElementOptions);
      e.Handled(true);
    }
  } else if (e.Key() == nextKey) {
    auto lastFocusableElement =
        xaml::Input::FocusManager::FindLastFocusableElement(senderFrameworkElement).try_as<xaml::DependencyObject>();
    auto isLastElementFocused = lastFocusableElement == focusedElement;
    if (!isLastElementFocused) {
      xaml::Input::FocusManager::TryMoveFocus(xaml::Input::FocusNavigationDirection::Next, findNextElementOptions);
      e.Handled(true);
    }
  } else if (e.Key() == winrt::system::VirtualKey::Tab) {
    if (!react::uwp::IsXamlIsland()) {
      auto const &coreWindow = ui::Core::CoreWindow::GetForCurrentThread();
      auto isShiftDown = KeyboardHelper::IsModifiedKeyPressed(coreWindow, winrt::system::VirtualKey::Shift);

      winrt::Point anchorTopLeft = winrt::Point(0, 0);
      xaml::Media::GeneralTransform transform = senderFrameworkElement.TransformToVisual(xamlRoot.Content());
      winrt::Point anchorTopLeftConverted = transform.TransformPoint(anchorTopLeft);
      auto exclusionRect = winrt::Rect(
          anchorTopLeftConverted.X,
          anchorTopLeftConverted.Y,
          static_cast<float>(senderFrameworkElement.ActualWidth()),
          static_cast<float>(senderFrameworkElement.ActualHeight()));
      findNextElementOptions.ExclusionRect(exclusionRect);

      auto nextElement = xaml::Input::FocusManager::FindNextElement(
          isShiftDown ? xaml::Input::FocusNavigationDirection::Up : xaml::Input::FocusNavigationDirection::Down,
          findNextElementOptions);
      if (nextElement) {
        xaml::Input::FocusManager::TryFocusAsync(nextElement, xaml::FocusState::Programmatic);
        e.Handled(true);
      }
    } else {
      // TODO - need an alternative to ui::Core::CoreWindow::GetForCurrentThread(); for XAML Islands.
    }
  }
}

void FocusZoneViewShadowNode::OnGettingFocus(
    const winrt::IInspectable &sender,
    const xaml::Input::GettingFocusEventArgs &e) {
  if (e.FocusState() != winrt::FocusState::Programmatic && m_lastElement.get() != nullptr &&
      e.NewFocusedElement() != m_lastElement.get()) {
    e.TrySetNewFocusedElement(m_lastElement.get());
  }
}

void FocusZoneViewShadowNode::OnLosingFocus(
    const winrt::IInspectable &sender,
    const xaml::Input::LosingFocusEventArgs &e) {
  m_lastElement = e.OldFocusedElement();
}

// FocusZoneViewManager

FocusZoneViewManager::FocusZoneViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *FocusZoneViewManager::GetName() const {
  return L"FocusZoneView";
}

void FocusZoneViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"focusDirection", L"string");
}

ShadowNode *FocusZoneViewManager::createShadow() const {
  return new FocusZoneViewShadowNode();
}

} // namespace ReactNative
} // namespace Microsoft
