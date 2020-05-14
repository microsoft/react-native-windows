// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "PopupViewManager.h"
#include "TouchEventHandler.h"

#include <Modules/NativeUIManager.h>
#include <Utils/Helpers.h>
#include <Utils/ValueUtils.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>

namespace winrt {
using namespace Windows::UI::Xaml::Controls::Primitives;
} // namespace winrt

namespace react {
namespace uwp {

class PopupShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  PopupShadowNode() = default;
  virtual ~PopupShadowNode();

  void createView() override;
  void AddView(ShadowNode &child, int64_t index) override;
  virtual void removeAllChildren() override;
  virtual void RemoveChildAt(int64_t indexToRemove) override;
  void updateProperties(const folly::dynamic &&props) override;

  static void OnPopupClosed(IReactInstance &instance, int64_t tag);
  winrt::Windows::Foundation::Size GetAppWindowSize();
  winrt::ContentControl GetControl();
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

  int64_t m_targetTag;
  winrt::Popup::Closed_revoker m_popupClosedRevoker{};
  winrt::Popup::Opened_revoker m_popupOpenedRevoker{};
  winrt::FrameworkElement::SizeChanged_revoker m_popupSizeChangedRevoker{};
  winrt::Window::SizeChanged_revoker m_windowSizeChangedRevoker{};
};

PopupShadowNode::~PopupShadowNode() {
  m_touchEventHandler->RemoveTouchHandlers();
  m_previewKeyboardEventHandlerOnRoot->unhook();
}

winrt::ContentControl PopupShadowNode::GetControl() {
  auto popup = GetView().as<winrt::Popup>();
  auto control = popup.Child().as<winrt::ContentControl>();

  return control;
}

void PopupShadowNode::createView() {
  Super::createView();

  auto popup = GetView().as<winrt::Popup>();
  auto control = GetControl();
  auto wkinstance = GetViewManager()->GetReactInstance();
  m_touchEventHandler = std::make_unique<TouchEventHandler>(wkinstance);
  m_previewKeyboardEventHandlerOnRoot = std::make_unique<PreviewKeyboardEventHandlerOnRoot>(wkinstance);

  m_popupClosedRevoker = popup.Closed(winrt::auto_revoke, [=](auto &&, auto &&) {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
      OnPopupClosed(*instance, m_tag);
  });

  m_popupOpenedRevoker = popup.Opened(winrt::auto_revoke, [=](auto &&, auto &&) {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr) {
      // When multiple flyouts/popups are overlapping, XAML's theme shadows
      // don't render properly. As a workaround we enable a z-index
      // translation based on an elevation derived from the count of open
      // popups/flyouts. We apply this translation on open of the popup.
      // (Translation is only supported on RS5+, eg. IUIElement9)
      if (auto uiElement9 = GetView().try_as<winrt::IUIElement9>()) {
        auto numOpenPopups = CountOpenPopups();
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
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr) {
      UpdateLayout();
    }
  });

  m_windowSizeChangedRevoker = winrt::Window::Current().SizeChanged(winrt::auto_revoke, [=](auto &&, auto &&) {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr) {
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

void PopupShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;

  auto popup = GetView().as<winrt::Popup>();
  if (popup == nullptr)
    return;

  bool needsLayoutUpdate = false;
  bool needsTabUpdate = false;
  const folly::dynamic *isOpenProp = nullptr;

  for (auto &pair : props.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (propertyName == "autoFocus") {
      if (propertyValue.isBool()) {
        m_autoFocus = propertyValue.asBool();
      } else if (propertyValue.isNull()) {
        m_autoFocus = true;
      }
      needsTabUpdate = true;
    } else if (propertyName == "target") {
      if (propertyValue.isNumber())
        m_targetTag = static_cast<int64_t>(propertyValue.asDouble());
      else
        m_targetTag = -1;
    } else if (propertyName == "isOpen") {
      isOpenProp = &propertyValue;
    } else if (propertyName == "isLightDismissEnabled") {
      if (propertyValue.isBool()) {
        popup.IsLightDismissEnabled(propertyValue.getBool());
      } else if (propertyValue.isNull()) {
        popup.ClearValue(winrt::Popup::IsLightDismissEnabledProperty());
      }
      needsTabUpdate = true;
    } else if (propertyName == "horizontalOffset") {
      if (propertyValue.isNumber()) {
        m_horizontalOffset = propertyValue.asDouble();
      } else if (propertyValue.isNull()) {
        m_horizontalOffset = 0;
      }
      needsLayoutUpdate = true;
    } else if (propertyName == "verticalOffset") {
      if (propertyValue.isNumber()) {
        m_verticalOffset = propertyValue.asDouble();
      } else if (propertyValue.isNull()) {
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
    if (isOpenProp->isBool())
      popup.IsOpen(isOpenProp->getBool());
    else if (isOpenProp->isNull())
      popup.ClearValue(winrt::Popup::IsOpenProperty());
  }

  Super::updateProperties(std::move(props));
  m_updating = false;
}

/*static*/ void PopupShadowNode::OnPopupClosed(IReactInstance &instance, int64_t tag) {
  folly::dynamic eventData = folly::dynamic::object("target", tag);
  instance.DispatchEvent(tag, "topDismiss", std::move(eventData));
}

void PopupShadowNode::UpdateLayout() {
  auto wkinstance = static_cast<PopupViewManager *>(GetViewManager())->m_wkReactInstance;
  auto instance = wkinstance.lock();

  if (instance == nullptr)
    return;

  auto popup = GetView().as<winrt::Popup>();

  // center relative to anchor
  if (m_targetTag > 0) {
    auto pNativeUIManagerHost = static_cast<NativeUIManager *>(instance->NativeUIManager())->getHost();
    ShadowNodeBase *pShadowNodeChild =
        static_cast<ShadowNodeBase *>(pNativeUIManagerHost->FindShadowNodeForTag(m_targetTag));

    if (pShadowNodeChild != nullptr) {
      auto targetView = pShadowNodeChild->GetView();
      auto targetElement = targetView.as<winrt::FrameworkElement>();

      auto popupTransform = targetElement.TransformToVisual(popup);
      winrt::Point bottomRightPoint(
          static_cast<float>(targetElement.Width()), static_cast<float>(targetElement.Height()));
      auto point = popupTransform.TransformPoint(bottomRightPoint);
      popup.HorizontalOffset(point.X + m_horizontalOffset);
      popup.VerticalOffset(point.Y + m_verticalOffset);
    }
  } else // Center relative to app window
  {
    auto appWindow = winrt::Window::Current().Content();
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
    control.TabFocusNavigation(winrt::Windows::UI::Xaml::Input::KeyboardNavigationMode::Cycle);
    winrt::FocusManager::TryFocusAsync(control, winrt::Windows::UI::Xaml::FocusState::Programmatic);
  } else {
    control.IsTabStop(false);
    control.TabFocusNavigation(winrt::Windows::UI::Xaml::Input::KeyboardNavigationMode::Local);
  }
}

winrt::Size PopupShadowNode::GetAppWindowSize() {
  winrt::Size windowSize = winrt::SizeHelper::Empty();

  if (auto current = winrt::Window::Current()) {
    if (auto coreWindow = current.CoreWindow()) {
      windowSize.Width = coreWindow.Bounds().Width;
      windowSize.Height = coreWindow.Bounds().Height;
    }
  }

  return windowSize;
}

PopupViewManager::PopupViewManager(const std::shared_ptr<IReactInstance> &reactInstance) : Super(reactInstance) {}

const char *PopupViewManager::GetName() const {
  return "RCTPopup";
}

folly::dynamic PopupViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("isOpen", "boolean")("isLightDismissEnabled", "boolean")("autoFocus", "boolean")(
      "horizontalOffset", "number")("verticalOffset", "number")("target", "number"));

  return props;
}

facebook::react::ShadowNode *PopupViewManager::createShadow() const {
  return new PopupShadowNode();
}

XamlView PopupViewManager::CreateViewCore(int64_t /*tag*/) {
  auto popup = winrt::Popup();
  auto control = winrt::ContentControl();

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
  auto control = popup.Child().as<winrt::ContentControl>();

  control.Width(width);
  control.Height(height);

  Super::SetLayoutProps(nodeToUpdate, viewToUpdate, left, top, width, height);
}

folly::dynamic PopupViewManager::GetExportedCustomDirectEventTypeConstants() const {
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topDismiss"] = folly::dynamic::object("registrationName", "onDismiss");

  return directEvents;
}

} // namespace uwp
} // namespace react
