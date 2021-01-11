// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ActivityIndicatorViewManager.h"

#include <JSValueWriter.h>
#include <UI.Xaml.Controls.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

namespace Microsoft::ReactNative {

ActivityIndicatorViewManager::ActivityIndicatorViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *ActivityIndicatorViewManager::GetName() const {
  return L"RCTActivityIndicatorView";
}

void ActivityIndicatorViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  winrt::Microsoft::ReactNative::WriteProperty(writer, L"animating", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"color", L"Color");
}

XamlView ActivityIndicatorViewManager::CreateViewCore(int64_t /*tag*/) {
  auto progressRing = xaml::Controls::ProgressRing();
  return progressRing;
}

bool ActivityIndicatorViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  auto progressRing = nodeToUpdate->GetView().as<xaml::Controls::ProgressRing>();
  if (progressRing == nullptr)
    return true;

  if (propertyName == "animating") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean)
      progressRing.IsActive(propertyValue.AsBoolean());
    if (propertyValue.IsNull())
      progressRing.ClearValue(xaml::Controls::ProgressRing::IsActiveProperty());
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

} // namespace Microsoft::ReactNative
