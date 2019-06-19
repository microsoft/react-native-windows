// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ViewManagerBase.h>

#include "ViewPanel.h"

#include <Modules/NativeUIManager.h>

#include <IReactInstance.h>
#include <IXamlRootView.h>
#include <Views/ShadowNodeBase.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace folly;

namespace winrt {
using namespace Windows::UI::Xaml;
}

namespace react { namespace uwp {

float GetConstrainedResult(float constrainTo, float measuredSize, YGMeasureMode measureMode)
{
  if (measureMode == YGMeasureMode::YGMeasureModeExactly)
    return constrainTo;
  if (measureMode == YGMeasureMode::YGMeasureModeAtMost)
    return std::min(constrainTo, measuredSize);
  return measuredSize;
}

YGSize DefaultYogaSelfMeasureFunc(YGNodeRef node, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode)
{
  YogaContext* context = reinterpret_cast<YogaContext*>(YGNodeGetContext(node));

  // TODO: VEC context != nullptr, DefaultYogaSelfMeasureFunc expects a context.

  XamlView view = context->view;
  auto element = view.as<winrt::UIElement>();

  float constrainToWidth = widthMode == YGMeasureMode::YGMeasureModeUndefined ? std::numeric_limits<float>::max() : width;
  float constrainToHeight = heightMode == YGMeasureMode::YGMeasureModeUndefined ? std::numeric_limits<float>::max() : height;

  try
  {
    winrt::Windows::Foundation::Size availableSpace(constrainToWidth, constrainToHeight);

    // Clear out current size so it doesn't constrain the measurement
    auto widthProp = winrt::FrameworkElement::WidthProperty();
    auto heightProp = winrt::FrameworkElement::HeightProperty();
    auto origWidth = element.GetValue(widthProp);
    auto origHeight = element.GetValue(heightProp);
    element.ClearValue(widthProp);
    element.ClearValue(heightProp);

    element.Measure(availableSpace);

    element.SetValue(widthProp, origWidth);
    element.SetValue(heightProp, origHeight);
  }
  catch (winrt::hresult_error const& )
  {
    // TODO add more error handling here
    assert(false);
  }

  YGSize desiredSize = {
    GetConstrainedResult(constrainToWidth, element.DesiredSize().Width, widthMode),
    GetConstrainedResult(constrainToHeight, element.DesiredSize().Height, heightMode) };
  return desiredSize;
}

ViewManagerBase::ViewManagerBase(const std::shared_ptr<IReactInstance>& reactInstance)
  : m_wkReactInstance(reactInstance)
{
}

dynamic ViewManagerBase::GetExportedViewConstants() const
{
  return dynamic::object();
}

dynamic ViewManagerBase::GetCommands() const
{
  return dynamic::object();
}

dynamic ViewManagerBase::GetNativeProps() const
{
  folly::dynamic props = folly::dynamic::object();
  props.update(folly::dynamic::object
    ("onLayout", "function")
    ("keyDownEvents", "array")
    ("keyUpEvents", "array")
  );
  return props;
}

dynamic ViewManagerBase::GetConstants() const
{
  folly::dynamic constants = folly::dynamic::object
    ("Constants", GetExportedViewConstants())
    ("Commands", GetCommands())
    ("NativeProps", GetNativeProps());

  const auto bubblingEventTypesConstants = GetExportedCustomBubblingEventTypeConstants();
  if (!bubblingEventTypesConstants.empty())
    constants["bubblingEventTypes"] = std::move(bubblingEventTypesConstants);
  const auto directEventTypesConstants = GetExportedCustomDirectEventTypeConstants();
  if (!directEventTypesConstants.empty())
    constants["directEventTypes"] = std::move(directEventTypesConstants);

  return constants;
}

facebook::react::ShadowNode* ViewManagerBase::createShadow() const
{
  // This class is the default ShadowNode that most view managers can use. If they need special functionality
  //  they should override this function and create their own ShadowNodeBase sub-class.
  return new ShadowNodeBase();
}

void ViewManagerBase::destroyShadow(facebook::react::ShadowNode* node) const
{
  delete node;
}

dynamic ViewManagerBase::GetExportedCustomBubblingEventTypeConstants() const
{
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
  for (auto& standardEventBaseName : standardEventBaseNames)
  {
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

dynamic ViewManagerBase::GetExportedCustomDirectEventTypeConstants() const
{
  folly::dynamic eventTypes = folly::dynamic::object();
  eventTypes.update(folly::dynamic::object
    ("topLayout", folly::dynamic::object("registrationName", "onLayout"))
    ("topMouseEnter", folly::dynamic::object("registrationName", "onMouseEnter"))
    ("topMouseLeave", folly::dynamic::object("registrationName", "onMouseLeave"))
//    ("topMouseMove", folly::dynamic::object("registrationName", "onMouseMove"))
  );
  return eventTypes;
}

XamlView ViewManagerBase::CreateView(int64_t tag)
{
  XamlView view = CreateViewCore(tag);

  // Set the tag if the element type supports it
  SetTag(view, tag);

  auto instance = m_wkReactInstance.lock();
  if (instance != nullptr)
    instance->CallXamlViewCreatedTestHook(view);

  return view;
}

void ViewManagerBase::AddView(XamlView parent, XamlView child, int64_t index)
{
  // ASSERT: Child must either implement or not allow children.
  assert(false);
}

void ViewManagerBase::RemoveChildAt(XamlView parent, int64_t index)
{
  // ASSERT: Child must either implement or not allow children.
  assert(false);
}

void ViewManagerBase::RemoveAllChildren(XamlView parent)
{
}

void ViewManagerBase::ReplaceChild(XamlView parent, XamlView oldChild, XamlView newChild)
{
  // ASSERT: Child must either implement or not allow children.
  assert(false);
}

void ViewManagerBase::UpdateProperties(ShadowNodeBase* nodeToUpdate, const dynamic& reactDiffMap)
{
  // Directly dirty this node since non-layout changes like the text property do not trigger relayout
  //  There isn't actually a yoga node for RawText views, but it will invalidate the ancestors which
  //  will include the containing Text element. And that's what matters.
  int64_t tag = GetTag(nodeToUpdate->GetView());
  auto instance = m_wkReactInstance.lock();
  if (instance != nullptr)
    static_cast<NativeUIManager*>(instance->NativeUIManager())->DirtyYogaNode(tag);

  for (const auto& pair : reactDiffMap.items())
  {
    const std::string& propertyName = pair.first.getString();
    const folly::dynamic& propertyValue = pair.second;

    if (propertyName == "onLayout")
    {
      nodeToUpdate->m_onLayout = !propertyValue.isNull() && propertyValue.asBool();
    }
    else if (propertyName == "keyDownEvents")
    {
      nodeToUpdate->UpdateHandledKeyboardEvents(propertyName, propertyValue);
    }
    else if (propertyName == "keyUpEvents")
    {
      nodeToUpdate->UpdateHandledKeyboardEvents(propertyName, propertyValue);
    }
  }
}

void ViewManagerBase::TransferProperties(XamlView /*oldView*/, XamlView /*newView*/)
{
}

void ViewManagerBase::DispatchCommand(XamlView viewToUpdate, int64_t commandId, const folly::dynamic& commandArgs)
{
  assert(false); // View did not handle its command
}

void ViewManagerBase::SetLayoutProps(ShadowNodeBase& nodeToUpdate, XamlView viewToUpdate, float left, float top, float width, float height)
{
  auto element = viewToUpdate.as<winrt::UIElement>();
  if (element == nullptr) {
    // TODO: Assert
    return;
  }
  auto fe = element.as<winrt::FrameworkElement>();

  bool changed =
    left != ViewPanel::GetLeft(element) ||
    top != ViewPanel::GetTop(element) ||
    width != fe.Width() ||
    height != fe.Height();

  if (changed) {
    // Set Position & Size Properties
    ViewPanel::SetLeft(element, left);
    ViewPanel::SetTop(element, top);

    fe.Width(width);
    fe.Height(height);

    // Fire Events
    if (nodeToUpdate.m_onLayout)
    {
      int64_t tag = GetTag(viewToUpdate);
      folly::dynamic layout = folly::dynamic::object
      ("x", left)
        ("y", top)
        ("height", height)
        ("width", width);

      folly::dynamic eventData = folly::dynamic::object
      ("target", tag)
        ("layout", std::move(layout));

      auto instance = m_wkReactInstance.lock();
      if (instance != nullptr)
        instance->DispatchEvent(tag, "topLayout", std::move(eventData));
    }
  }
}

YGMeasureFunc ViewManagerBase::GetYogaCustomMeasureFunc() const
{
  return nullptr;
}

bool ViewManagerBase::RequiresYogaNode() const
{
  return true;
}

bool ViewManagerBase::IsNativeControlWithSelfLayout() const
{
  return GetYogaCustomMeasureFunc() != nullptr;
}


} }
