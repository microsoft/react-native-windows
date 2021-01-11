// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <IReactInstance.h>

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <UI.Composition.h>
#include <Views/ExpressionAnimationStore.h>
#include <Views/ShadowNodeBase.h>
#include <Views/ViewManager.h>
#include <Views/ViewManagerBase.h>
#include <WindowsNumerics.h>
#include <XamlUIService.h>
#include "Views/KeyboardEventHandler.h"
#include "XamlFeatures.h"

using namespace std::placeholders;

namespace Microsoft::ReactNative {

ShadowNodeBase::ShadowNodeBase() : m_view(nullptr) {}

ViewManagerBase *ShadowNodeBase::GetViewManager() const {
  return static_cast<ViewManagerBase *>(m_viewManager);
}

void ShadowNodeBase::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {
  GetViewManager()->UpdateProperties(this, props);
}

void ShadowNodeBase::createView() {
  m_view = GetViewManager()->CreateView(this->m_tag);

  if (react::uwp::g_HasActualSizeProperty == react::uwp::TriBit::Undefined) {
    if (auto uielement = m_view.try_as<xaml::UIElement>()) {
      // ActualSize works on 19H1+ only
      react::uwp::g_HasActualSizeProperty =
          (uielement.try_as<xaml::IUIElement10>()) ? react::uwp::TriBit::Set : react::uwp::TriBit::NotSet;
    }
  }
}

bool ShadowNodeBase::NeedsForceLayout() {
  return false;
}

void ShadowNodeBase::dispatchCommand(
    const std::string &commandId,
    winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) {
  GetViewManager()->DispatchCommand(GetView(), commandId, std::move(commandArgs));
}

void ShadowNodeBase::removeAllChildren() {
  GetViewManager()->RemoveAllChildren(GetView());
}

void ShadowNodeBase::AddView(ShadowNode &child, int64_t index) {
  this->GetViewManager()->AddView(GetView(), static_cast<ShadowNodeBase &>(child).GetView(), index);
}

void ShadowNodeBase::RemoveChildAt(int64_t indexToRemove) {
  GetViewManager()->RemoveChildAt(GetView(), indexToRemove);
}

void ShadowNodeBase::onDropViewInstance() {
  m_handledKeyboardEventHandler = nullptr;
}

void ShadowNodeBase::ReplaceView(XamlView view) {
  SetTag(view, GetTag(m_view));

  m_view = view;

  if (m_handledKeyboardEventHandler) {
    m_handledKeyboardEventHandler->unhook();
    m_handledKeyboardEventHandler->hook(view);
  }
}

void ShadowNodeBase::ReplaceChild(const XamlView &oldChildView, const XamlView &newChildView) {
  GetViewManager()->ReplaceChild(m_view, oldChildView, newChildView);
}

void ShadowNodeBase::ReparentView(XamlView view) {
  GetViewManager()->TransferProperties(m_view, view);
  if (auto nativeUIManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
    int64_t parentTag = GetParent();
    auto host = nativeUIManager->getHost();
    auto pParentNode = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(parentTag));
    if (pParentNode != nullptr) {
      pParentNode->ReplaceChild(m_view, view);
    }
  }
  ReplaceView(view);

  // Let the UIManager know about this so it can update the yoga context.
  if (auto pNativeUiManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
    pNativeUiManager->ReplaceView(*static_cast<ShadowNode *>(this));
  }
}

comp::CompositionPropertySet ShadowNodeBase::EnsureTransformPS() {
  if (m_transformPS == nullptr) {
    m_transformPS = GetCompositor(GetView()).CreatePropertySet();
    UpdateTransformPS();
  }

  return m_transformPS;
}

// Create a PropertySet that will hold two properties:
// "center":  This is the center of the UIElement
// "transform": This will hold the un-centered TransformMatrix we want to apply
void ShadowNodeBase::UpdateTransformPS() {
  if (m_transformPS != nullptr) {
    // First build up an ExpressionAnimation to compute the "center" property,
    // like so: The input to the expression is UIElement.ActualSize/2, output is
    // a vector3 with [cx, cy, 0].
    auto uielement = m_view.try_as<xaml::UIElement>();
    assert(uielement != nullptr);
    m_transformPS = EnsureTransformPS();
    m_transformPS.InsertVector3(L"center", {0, 0, 0});

    auto centeringAnimation =
        GetViewManager()->GetExpressionAnimationStore()->GetElementCenterPointExpression(GetCompositor(GetView()));
    centeringAnimation.SetExpressionReferenceParameter(L"uielement", uielement);
    m_transformPS.StartAnimation(L"center", centeringAnimation);

    // Now insert the "transform" property with an initial value of identity.
    // The caller will handle populating this with the appropriate value (either
    // a static or animated value).
    winrt::Windows::Foundation::Numerics::float4x4 unused;
    // Take care not to stomp over any transform value we currently have set, as
    // we will use this value in the scenario where a View changed its backing
    // XAML element, here we will just transfer existing value to a new backing
    // XAML element.
    if (m_transformPS.TryGetMatrix4x4(L"transform", unused) == comp::CompositionGetValueStatus::NotFound) {
      m_transformPS.InsertMatrix4x4(L"transform", winrt::Windows::Foundation::Numerics::float4x4::identity());
    }
  }
}

void ShadowNodeBase::UpdateHandledKeyboardEvents(
    std::string const &propertyName,
    winrt::Microsoft::ReactNative::JSValue const &value) {
  EnsureHandledKeyboardEventHandler();
  m_handledKeyboardEventHandler->UpdateHandledKeyboardEvents(propertyName, value);
}

void ShadowNodeBase::EnsureHandledKeyboardEventHandler() {
  if (!m_handledKeyboardEventHandler) {
    assert(m_view);
    m_handledKeyboardEventHandler = std::make_unique<HandledKeyboardEventHandler>();
    m_handledKeyboardEventHandler->hook(m_view);
  }
}

void ShadowNodeBase::YellowBox(const std::string &message) const noexcept {
  GetViewManager()->GetReactContext().CallJSFunction("RCTLog", "logToConsole", folly::dynamic::array("warn", message));
}

void ShadowNodeBase::RedBox(const std::string &message) const noexcept {
  GetViewManager()->GetReactContext().CallJSFunction("RCTLog", "logToConsole", folly::dynamic::array("error", message));
}

} // namespace Microsoft::ReactNative
