// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "RawTextViewManager.h"
#include "TextViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <INativeUIManager.h>
#include <Utils/ValueUtils.h>

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

namespace react::uwp {

RawTextViewManager::RawTextViewManager(const Mso::React::IReactContext& context) : Super(context) {}

const char *RawTextViewManager::GetName() const {
  return "RCTRawText";
}

XamlView RawTextViewManager::CreateViewCore(int64_t /*tag*/) {
  winrt::Run run;
  return run;
}

bool RawTextViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  auto run = nodeToUpdate->GetView().as<winrt::Run>();
  if (run == nullptr)
    return true;

  if (propertyName == "text") {
    run.Text(asHstring(propertyValue));

    if (nodeToUpdate->GetParent() != -1) {
      if (auto uiManager = m_context->NativeUIManager()) {
        const ShadowNodeBase *parent = static_cast<ShadowNodeBase *>(uiManager->getHost()->FindShadowNodeForTag(nodeToUpdate->GetParent()));
        if (parent && parent->m_children.size() == 1) {
          auto view = parent->GetView();
          auto textBlock = view.try_as<winrt::TextBlock>();
          if (textBlock != nullptr) {
            textBlock.Text(run.Text());
          }
        }

        NotifyAncestorsTextChanged(nodeToUpdate);
      }
    }
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

void RawTextViewManager::NotifyAncestorsTextChanged(ShadowNodeBase *nodeToUpdate) {
  auto uiManager = GetReactContext().NativeUIManager();
  if (!uiManager)
    return;
  auto host = uiManager->getHost();
  ShadowNodeBase *parent = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(nodeToUpdate->GetParent()));
  while (parent) {
    auto viewManager = parent->GetViewManager();
    if (!std::strcmp(viewManager->GetName(), "RCTText")) {
      (static_cast<TextViewManager *>(viewManager))->OnDescendantTextPropertyChanged(parent);
    }
    parent = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(parent->GetParent()));
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

} // namespace react::uwp
