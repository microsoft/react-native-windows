// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "RawTextViewManager.h"

#include <Views/ShadowNodeBase.h>
#include <Views/Text/TextVisitors.h>

#include <UI.Xaml.Documents.h>
#include <Utils/ValueUtils.h>
#include <winrt/Windows.Foundation.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Documents;
using namespace xaml::Media;
} // namespace winrt

namespace Microsoft::ReactNative {

RawTextViewManager::RawTextViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *RawTextViewManager::GetName() const {
  return L"RCTRawText";
}

XamlView RawTextViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &) {
  winrt::Run run;
  return run;
}

bool RawTextViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  auto run = nodeToUpdate->GetView().as<winrt::Run>();
  if (run == nullptr)
    return true;

  if (propertyName == "text") {
    MarkDirty(nodeToUpdate->m_tag);
    run.Text(asHstring(propertyValue));
    static_cast<RawTextShadowNode *>(nodeToUpdate)->originalText = winrt::hstring{};
    ApplyTextTransformToChild(nodeToUpdate);
    NotifyAncestorsTextPropertyChanged(nodeToUpdate, PropertyChangeType::Text);
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

void RawTextViewManager::SetLayoutProps(
    ShadowNodeBase & /*nodeToUpdate*/,
    const XamlView & /*viewToUpdate*/,
    float /*left*/,
    float /*top*/,
    float /*width*/,
    float /*height*/) {}

bool RawTextViewManager::RequiresYogaNode() const {
  return false;
}

} // namespace Microsoft::ReactNative
