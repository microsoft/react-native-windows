// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ViewManagerBase.h>

#include "Unicode.h"
#include "ViewPanel.h"
#include "cdebug.h"

#include <Modules/NativeUIManager.h>

#include <IReactInstance.h>
#include <IXamlRootView.h>
#include <TestHook.h>
#include <Views/ShadowNodeBase.h>

namespace winrt {
using namespace xaml;
}

namespace react::uwp {

float GetConstrainedResult(float constrainTo, float measuredSize, YGMeasureMode measureMode) {
  // Round up to workaround truncation inside yoga
  measuredSize = ceil(measuredSize);

  if (measureMode == YGMeasureMode::YGMeasureModeExactly)
    return constrainTo;
  if (measureMode == YGMeasureMode::YGMeasureModeAtMost)
    return std::min(constrainTo, measuredSize);
  return measuredSize;
}

YGSize DefaultYogaSelfMeasureFunc(
    YGNodeRef node,
    float width,
    YGMeasureMode widthMode,
    float height,
    YGMeasureMode heightMode) {
  YogaContext *context = reinterpret_cast<YogaContext *>(YGNodeGetContext(node));

  // TODO: VEC context != nullptr, DefaultYogaSelfMeasureFunc expects a context.

  XamlView view = context->view;
  auto element = view.as<xaml::UIElement>();

  float constrainToWidth =
      widthMode == YGMeasureMode::YGMeasureModeUndefined ? std::numeric_limits<float>::max() : width;
  float constrainToHeight =
      heightMode == YGMeasureMode::YGMeasureModeUndefined ? std::numeric_limits<float>::max() : height;

  try {
    winrt::Windows::Foundation::Size availableSpace(constrainToWidth, constrainToHeight);

    // Clear out current size so it doesn't constrain the measurement
    auto widthProp = xaml::FrameworkElement::WidthProperty();
    auto heightProp = xaml::FrameworkElement::HeightProperty();
    auto origWidth = element.GetValue(widthProp);
    auto origHeight = element.GetValue(heightProp);
    element.ClearValue(widthProp);
    element.ClearValue(heightProp);

    element.Measure(availableSpace);

    element.SetValue(widthProp, origWidth);
    element.SetValue(heightProp, origHeight);
  } catch (winrt::hresult_error const &) {
    // TODO add more error handling here
    assert(false);
  }

  YGSize desiredSize = {GetConstrainedResult(constrainToWidth, element.DesiredSize().Width, widthMode),
                        GetConstrainedResult(constrainToHeight, element.DesiredSize().Height, heightMode)};
  return desiredSize;
}

ViewManagerBase::ViewManagerBase(const std::shared_ptr<IReactInstance> &reactInstance)
    : m_wkReactInstance(reactInstance) {}

folly::dynamic ViewManagerBase::GetExportedViewConstants() const {
  return folly::dynamic::object();
}

folly::dynamic ViewManagerBase::GetCommands() const {
  return folly::dynamic::object();
}

folly::dynamic ViewManagerBase::GetNativeProps() const {
  folly::dynamic props = folly::dynamic::object();
  props.update(folly::dynamic::object("onLayout", "function")("keyDownEvents", "array")("keyUpEvents", "array"));
  return props;
}

folly::dynamic ViewManagerBase::GetConstants() const {
  folly::dynamic constants = folly::dynamic::object("Constants", GetExportedViewConstants())("Commands", GetCommands())(
      "NativeProps", GetNativeProps());

  const auto bubblingEventTypesConstants = GetExportedCustomBubblingEventTypeConstants();
  if (!bubblingEventTypesConstants.empty())
    constants["bubblingEventTypes"] = std::move(bubblingEventTypesConstants);
  const auto directEventTypesConstants = GetExportedCustomDirectEventTypeConstants();
  if (!directEventTypesConstants.empty())
    constants["directEventTypes"] = std::move(directEventTypesConstants);

  return constants;
}

facebook::react::ShadowNode *ViewManagerBase::createShadow() const {
  // This class is the default ShadowNode that most view managers can use. If
  // they need special functionality
  //  they should override this function and create their own ShadowNodeBase
  //  sub-class.
  return new ShadowNodeBase();
}

void ViewManagerBase::destroyShadow(facebook::react::ShadowNode *node) const {
  delete node;
}

folly::dynamic ViewManagerBase::GetExportedCustomBubblingEventTypeConstants() const {
  const PCSTR standardEventBaseNames[] = {
      // Generic events
      "Press",
      "Change",
      "Focus",
      "Blur",
      "SubmitEditing",
      "EndEditing",
      "KeyPress",

      // Touch events
      "TouchStart",
      "TouchMove",
      "TouchCancel",
      "TouchEnd",

      // Keyboard events
      "KeyUp",
      "KeyDown",
  };

  folly::dynamic bubblingEvents = folly::dynamic::object();
  for (auto &standardEventBaseName : standardEventBaseNames) {
    using namespace std::string_literals;

    std::string eventName = "top"s + standardEventBaseName;
    std::string bubbleName = "on"s + standardEventBaseName;

    folly::dynamic registration = folly::dynamic::object();
    registration["captured"] = bubbleName + "Capture";
    registration["bubbled"] = std::move(bubbleName);

    bubblingEvents[std::move(eventName)] = folly::dynamic::object("phasedRegistrationNames", std::move(registration));
  }

  return bubblingEvents;
}

folly::dynamic ViewManagerBase::GetExportedCustomDirectEventTypeConstants() const {
  folly::dynamic eventTypes = folly::dynamic::object();
  eventTypes.update(folly::dynamic::object("topLayout", folly::dynamic::object("registrationName", "onLayout"))(
      "topMouseEnter", folly::dynamic::object("registrationName", "onMouseEnter"))(
      "topMouseLeave", folly::dynamic::object("registrationName", "onMouseLeave"))(
      "topAccessibilityAction", folly::dynamic::object("registrationName", "onAccessibilityAction"))
                    //    ("topMouseMove",
                    //    folly::dynamic::object("registrationName",
                    //    "onMouseMove"))
  );
  return eventTypes;
}

XamlView ViewManagerBase::CreateView(int64_t tag) {
  XamlView view = CreateViewCore(tag);

  OnViewCreated(view);
  // Set the tag if the element type supports it
  SetTag(view, tag);

  // In Debug, set the element name to the tag for convienent
  // searching within VisualStudio's Live Visual Tree pane
#ifdef DEBUG
  auto element = view.try_as<xaml::FrameworkElement>();
  if (element) {
    element.Name(L"<reacttag>: " + std::to_wstring(tag));
  }
#endif

  auto instance = m_wkReactInstance.lock();
  if (instance != nullptr)
    instance->CallXamlViewCreatedTestHook(view);

  return view;
}

void ViewManagerBase::AddView(const XamlView & /*parent*/, const XamlView & /*child*/, int64_t /*index*/) {
  // ASSERT: Child must either implement or not allow children.
  assert(false);
}

void ViewManagerBase::RemoveChildAt(const XamlView & /*parent*/, int64_t /*index*/) {
  // ASSERT: Child must either implement or not allow children.
  assert(false);
}

void ViewManagerBase::RemoveAllChildren(const XamlView &parent) {}

void ViewManagerBase::ReplaceChild(const XamlView &parent, const XamlView &oldChild, const XamlView &newChild) {
  // ASSERT: Child must either implement or not allow children.
  assert(false);
}

void ViewManagerBase::UpdateProperties(ShadowNodeBase *nodeToUpdate, const folly::dynamic &reactDiffMap) {
  // Directly dirty this node since non-layout changes like the text property do
  // not trigger relayout
  //  There isn't actually a yoga node for RawText views, but it will invalidate
  //  the ancestors which
  //  will include the containing Text element. And that's what matters.
  int64_t tag = GetTag(nodeToUpdate->GetView());
  auto instance = m_wkReactInstance.lock();
  if (instance != nullptr && instance->IsLoaded())
    static_cast<NativeUIManager *>(instance->NativeUIManager())->DirtyYogaNode(tag);

  for (const auto &pair : reactDiffMap.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;
    if (!UpdateProperty(nodeToUpdate, propertyName, propertyValue)) {
      NotifyUnimplementedProperty(nodeToUpdate, propertyName, propertyValue);
    }
  }
}

bool ViewManagerBase::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  if (propertyName == "onLayout") {
    nodeToUpdate->m_onLayout = !propertyValue.isNull() && propertyValue.asBool();
  } else if (propertyName == "keyDownEvents") {
    nodeToUpdate->UpdateHandledKeyboardEvents(propertyName, propertyValue);
  } else if (propertyName == "keyUpEvents") {
    nodeToUpdate->UpdateHandledKeyboardEvents(propertyName, propertyValue);
  } else {
    return false;
  }
  return true;
}

void ViewManagerBase::TransferProperties(const XamlView & /*oldView*/, const XamlView & /*newView*/) {}

void ViewManagerBase::DispatchCommand(
    const XamlView & /*viewToUpdate*/,
    const std::string & /*commandId*/,
    const folly::dynamic & /*commandArgs*/) {
  assert(false); // View did not handle its command
}

void ViewManagerBase::NotifyUnimplementedProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const folly::dynamic &value) {
#ifdef DEBUG
  auto viewManagerName = nodeToUpdate->GetViewManager()->GetName();
  auto element(nodeToUpdate->GetView().as<winrt::IInspectable>());

  if (element != nullptr) {
    auto className = Microsoft::Common::Unicode::Utf16ToUtf8(winrt::get_class_name(element));
    TestHook::NotifyUnimplementedProperty(viewManagerName, className, propertyName, value);
  } else {
    cdebug << "[NonIInspectable] viewManagerName = " << viewManagerName << std::endl;
  }
#endif // DEBUG
}

void ViewManagerBase::SetLayoutProps(
    ShadowNodeBase &nodeToUpdate,
    const XamlView &viewToUpdate,
    float left,
    float top,
    float width,
    float height) {
  auto element = viewToUpdate.as<xaml::UIElement>();
  if (element == nullptr) {
    // TODO: Assert
    return;
  }
  auto fe = element.as<xaml::FrameworkElement>();

  // Set Position & Size Properties
  ViewPanel::SetLeft(element, left);
  ViewPanel::SetTop(element, top);

  fe.Width(width);
  fe.Height(height);

  // Fire Events
  if (nodeToUpdate.m_onLayout) {
    int64_t tag = GetTag(viewToUpdate);
    folly::dynamic layout = folly::dynamic::object("x", left)("y", top)("height", height)("width", width);

    folly::dynamic eventData = folly::dynamic::object("target", tag)("layout", std::move(layout));

    auto instance = m_wkReactInstance.lock();
    if (instance != nullptr)
      instance->DispatchEvent(tag, "topLayout", std::move(eventData));
  }
}

YGMeasureFunc ViewManagerBase::GetYogaCustomMeasureFunc() const {
  return nullptr;
}

bool ViewManagerBase::RequiresYogaNode() const {
  return true;
}

bool ViewManagerBase::IsNativeControlWithSelfLayout() const {
  return GetYogaCustomMeasureFunc() != nullptr;
}
} // namespace react::uwp
