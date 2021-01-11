// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "PopupViewManager.h"
#include "TouchEventHandler.h"

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>
#include <UI.Xaml.Input.h>
#include <UI.Xaml.Media.h>
#include <Utils/Helpers.h>
#include <Utils/ValueUtils.h>
#include <winrt/Windows.UI.Core.h>

namespace winrt {
using namespace xaml::Controls::Primitives;
} // namespace winrt

namespace Microsoft::ReactNative {

class PopupShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  PopupShadowNode() = default;
  virtual ~PopupShadowNode();

  void createView() override;
  void AddView(ShadowNode &child, int64_t index) override;
  virtual void removeAllChildren() override;
  virtual void RemoveChildAt(int64_t indexToRemove) override;
  void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) override;

  static void OnPopupClosed(const Mso::React::IReactContext &context, int64_t tag);
  winrt::Windows::Foundation::Size GetAppWindowSize();
  xaml::Controls::ContentControl GetControl();
  void UpdateTabStops();
  void UpdateLayout();

  bool IsWindowed() override {
    return true;
  }

 private:
  bool m_autoFocus{true};
  double m_verticalOffset{0};
  double m_horizontalOffset{0};
  std::unique_ptr<TouchEventHandler> m_touchEventHandler;
  std::unique_ptr<PreviewKeyboardEventHandlerOnRoot> m_previewKeyboardEventHandlerOnRoot;

  int64_t m_targetTag{0};
  winrt::Popup::Closed_revoker m_popupClosedRevoker{};
  winrt::Popup::Opened_revoker m_popupOpenedRevoker{};
  xaml::FrameworkElement::SizeChanged_revoker m_popupSizeChangedRevoker{};
  xaml::Window::SizeChanged_revoker m_windowSizeChangedRevoker{};
};

PopupShadowNode::~PopupShadowNode() {
  m_touchEventHandler->RemoveTouchHandlers();
  m_previewKeyboardEventHandlerOnRoot->unhook();
}

xaml::Controls::ContentControl PopupShadowNode::GetControl() {
  auto popup = GetView().as<winrt::Popup>();
  auto control = popup.Child().as<xaml::Controls::ContentControl>();

  return control;
}

void PopupShadowNode::createView() {
  Super::createView();

  auto popup = GetView().as<winrt::Popup>();
  auto control = GetControl();
  m_touchEventHandler = std::make_unique<TouchEventHandler>(GetViewManager()->GetReactContext());
  m_previewKeyboardEventHandlerOnRoot =
      std::make_unique<PreviewKeyboardEventHandlerOnRoot>(GetViewManager()->GetReactContext());

  m_popupClosedRevoker = popup.Closed(winrt::auto_revoke, [=](auto &&, auto &&) {
    if (!m_updating)
      OnPopupClosed(GetViewManager()->GetReactContext(), m_tag);
  });

  m_popupOpenedRevoker = popup.Opened(winrt::auto_revoke, [=](auto &&, auto &&) {
    if (!m_updating) {
      // When multiple flyouts/popups are overlapping, XAML's theme shadows
      // don't render properly. As a workaround we enable a z-index
      // translation based on an elevation derived from the count of open
      // popups/flyouts. We apply this translation on open of the popup.
      // (Translation is only supported on RS5+, eg. IUIElement9)
      if (auto uiElement9 = GetView().try_as<xaml::IUIElement9>()) {
        auto numOpenPopups = react::uwp::CountOpenPopups();
        if (numOpenPopups > 0) {
          winrt::Numerics::float3 translation{0, 0, (float)16 * numOpenPopups};
          popup.Translation(translation);
        }
      }

      UpdateLayout();
      UpdateTabStops();
    }
  });

  m_popupSizeChangedRevoker = popup.SizeChanged(winrt::auto_revoke, [=](auto &&, auto &&) {
    if (!m_updating) {
      UpdateLayout();
    }
  });

  m_windowSizeChangedRevoker = xaml::Window::Current().SizeChanged(winrt::auto_revoke, [=](auto &&, auto &&) {
    if (!m_updating) {
      UpdateLayout();
    }
  });
}

void PopupShadowNode::AddView(ShadowNode &child, int64_t index) {
  if (index != 0) {
    assert(false);
    return;
  }

  auto control = GetControl();
  auto childView = static_cast<ShadowNodeBase &>(child).GetView();

  control.Content(childView);

  m_touchEventHandler->AddTouchHandlers(childView);
  m_previewKeyboardEventHandlerOnRoot->hook(childView);
}

void PopupShadowNode::RemoveChildAt(int64_t indexToRemove) {
  if (indexToRemove != 0) {
    assert(false);
    return;
  }

  removeAllChildren();
}

void PopupShadowNode::removeAllChildren() {
  auto control = GetControl();

  control.Content(nullptr);

  m_touchEventHandler->RemoveTouchHandlers();
  m_previewKeyboardEventHandlerOnRoot->unhook();
}

void PopupShadowNode::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {
  m_updating = true;

  auto popup = GetView().as<winrt::Popup>();
  if (popup == nullptr)
    return;

  bool needsLayoutUpdate = false;
  bool needsTabUpdate = false;
  const winrt::Microsoft::ReactNative::JSValue *isOpenProp = nullptr;

  for (auto &pair : props) {
    const std::string &propertyName = pair.first;
    const auto &propertyValue = pair.second;

    if (propertyName == "autoFocus") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean) {
        m_autoFocus = propertyValue.AsBoolean();
      } else if (propertyValue.IsNull()) {
        m_autoFocus = false;
      }
      needsTabUpdate = true;
    } else if (propertyName == "target") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
        m_targetTag = propertyValue.AsInt64();
      else
        m_targetTag = -1;
    } else if (propertyName == "isOpen") {
      isOpenProp = &propertyValue;
    } else if (propertyName == "isLightDismissEnabled") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean) {
        popup.IsLightDismissEnabled(propertyValue.AsBoolean());
      } else if (propertyValue.IsNull()) {
        popup.ClearValue(winrt::Popup::IsLightDismissEnabledProperty());
      }
      needsTabUpdate = true;
    } else if (propertyName == "horizontalOffset") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        m_horizontalOffset = propertyValue.AsDouble();
      } else if (propertyValue.IsNull()) {
        m_horizontalOffset = 0;
      }
      needsLayoutUpdate = true;
    } else if (propertyName == "verticalOffset") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        m_verticalOffset = propertyValue.AsDouble();
      } else if (propertyValue.IsNull()) {
        m_verticalOffset = 0;
      }
      needsLayoutUpdate = true;
    }
  }

  if (needsLayoutUpdate) {
    UpdateLayout();
  }
  if (needsTabUpdate) {
    UpdateTabStops();
  }

  // IsOpen needs to be set after IsLightDismissEnabled for light-dismiss to work
  if (isOpenProp != nullptr) {
    if (isOpenProp->Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean)
      popup.IsOpen(isOpenProp->AsBoolean());
    else if (isOpenProp->IsNull())
      popup.ClearValue(winrt::Popup::IsOpenProperty());
  }

  Super::updateProperties(props);
  m_updating = false;
}

/*static*/ void PopupShadowNode::OnPopupClosed(const Mso::React::IReactContext &context, int64_t tag) {
  folly::dynamic eventData = folly::dynamic::object("target", tag);
  context.DispatchEvent(tag, "topDismiss", std::move(eventData));
}

void PopupShadowNode::UpdateLayout() {
  auto popup = GetView().as<winrt::Popup>();

  // center relative to anchor
  if (m_targetTag > 0) {
    if (auto uiManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
      auto pNativeUIManagerHost = uiManager->getHost();
      ShadowNodeBase *pShadowNodeChild =
          static_cast<ShadowNodeBase *>(pNativeUIManagerHost->FindShadowNodeForTag(m_targetTag));

      if (pShadowNodeChild != nullptr) {
        auto targetView = pShadowNodeChild->GetView();
        auto targetElement = targetView.as<xaml::FrameworkElement>();

        auto popupTransform = targetElement.TransformToVisual(popup);
        winrt::Point bottomRightPoint(
            static_cast<float>(targetElement.Width()), static_cast<float>(targetElement.Height()));
        auto point = popupTransform.TransformPoint(bottomRightPoint);
        popup.HorizontalOffset(point.X + m_horizontalOffset);
        popup.VerticalOffset(point.Y + m_verticalOffset);
      }
    }
  } else // Center relative to app window
  {
    auto appWindow = xaml::Window::Current().Content();
    auto popupToWindow = appWindow.TransformToVisual(popup);
    auto appWindowSize = GetAppWindowSize();
    winrt::Point centerPoint;
    centerPoint.X = static_cast<float>((appWindowSize.Width - popup.ActualWidth()) / 2);
    centerPoint.Y = static_cast<float>((appWindowSize.Height - popup.ActualHeight()) / 2);
    auto point = popupToWindow.TransformPoint(centerPoint);
    popup.HorizontalOffset(point.X + m_horizontalOffset);
    popup.VerticalOffset(point.Y + m_verticalOffset);
  }
}

void PopupShadowNode::UpdateTabStops() {
  auto control = GetControl();
  if (m_autoFocus) {
    control.IsTabStop(true);
    control.TabFocusNavigation(xaml::Input::KeyboardNavigationMode::Cycle);
    xaml::Input::FocusManager::TryFocusAsync(control, xaml::FocusState::Programmatic);
  } else {
    control.IsTabStop(false);
    control.TabFocusNavigation(xaml::Input::KeyboardNavigationMode::Local);
  }
}

winrt::Size PopupShadowNode::GetAppWindowSize() {
  winrt::Size windowSize = winrt::SizeHelper::Empty();

  if (auto current = xaml::Window::Current()) {
    if (auto coreWindow = current.CoreWindow()) {
      windowSize.Width = coreWindow.Bounds().Width;
      windowSize.Height = coreWindow.Bounds().Height;
    }
  }

  return windowSize;
}

PopupViewManager::PopupViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *PopupViewManager::GetName() const {
  return L"RCTPopup";
}

void PopupViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"isOpen", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"isLightDismissEnabled", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"autoFocus", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"horizontalOffset", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"verticalOffset", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"target", L"number");
}

ShadowNode *PopupViewManager::createShadow() const {
  return new PopupShadowNode();
}

XamlView PopupViewManager::CreateViewCore(int64_t /*tag*/) {
  auto popup = winrt::Popup();
  auto control = xaml::Controls::ContentControl();

  popup.Child(control);

  return popup;
}

void PopupViewManager::SetLayoutProps(
    ShadowNodeBase &nodeToUpdate,
    const XamlView &viewToUpdate,
    float left,
    float top,
    float width,
    float height) {
  auto popup = viewToUpdate.as<winrt::Popup>();
  auto control = popup.Child().as<xaml::Controls::ContentControl>();

  control.Width(width);
  control.Height(height);

  Super::SetLayoutProps(nodeToUpdate, viewToUpdate, left, top, width, height);
}

void PopupViewManager::GetExportedCustomDirectEventTypeConstants(
    const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetExportedCustomDirectEventTypeConstants(writer);

  writer.WritePropertyName(L"topDismiss");
  writer.WriteObjectBegin();
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"registrationName", L"onDismiss");
  writer.WriteObjectEnd();
}

} // namespace Microsoft::ReactNative
