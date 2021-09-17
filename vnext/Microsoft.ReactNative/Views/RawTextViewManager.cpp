// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "RawTextViewManager.h"
#include "TextViewManager.h"
#include "VirtualTextViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <INativeUIManager.h>
#include <Utils/ShadowNodeTypeUtils.h>
#include <Utils/ValueUtils.h>

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>
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
    run.Text(asHstring(propertyValue));
    static_cast<RawTextShadowNode *>(nodeToUpdate)->originalText = winrt::hstring{};
    NotifyAncestorsTextChanged(nodeToUpdate);
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

void RawTextViewManager::NotifyAncestorsTextChanged(ShadowNodeBase *nodeToUpdate) {
  if (auto uiManager = GetNativeUIManager(GetReactContext()).lock()) {
    auto host = uiManager->getHost();
    ShadowNodeBase *parent = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(nodeToUpdate->GetParent()));
    TextTransform textTransform = TextTransform::Undefined;
    auto isNested = false;
    while (parent) {
      auto viewManager = parent->GetViewManager();
      const auto nodeType = viewManager->GetName();
      if (IsTextShadowNode(parent)) {
        const auto textViewManager = static_cast<TextViewManager *>(viewManager);
        if (textTransform == TextTransform::Undefined) {
          textTransform = textViewManager->GetTextTransformValue(parent);
        }

        VirtualTextShadowNode::ApplyTextTransform(
            *nodeToUpdate, textTransform, /* forceUpdate = */ false, /* isRoot = */ false);

        if (!isNested && parent->m_children.size() == 1) {
          auto view = parent->GetView();
          auto textBlock = view.try_as<winrt::TextBlock>();
          if (textBlock != nullptr) {
            const auto run = nodeToUpdate->GetView().try_as<winrt::Run>();
            if (run != nullptr) {
              textBlock.Text(run.Text());
            }
          }
        }

        (static_cast<TextViewManager *>(viewManager))->OnDescendantTextPropertyChanged(parent);

        // We have reached the parent TextBlock, so there're no more parent <Text> elements in this tree.
        break;
      } else if (IsVirtualTextShadowNode(parent) && textTransform == TextTransform::Undefined) {
        textTransform = static_cast<VirtualTextShadowNode *>(parent)->textTransform;
      }

      parent = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(parent->GetParent()));
      isNested = true;
    }
  }
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
