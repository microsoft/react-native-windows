// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "RawTextViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <Utils/ValueUtils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <ReactUWP\Base\UwpReactInstance.h>
#include <ReactUWP\Modules\NativeUIManager.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt

namespace react {
namespace uwp {

RawTextViewManager::RawTextViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *RawTextViewManager::GetName() const {
  return "RCTRawText";
}

XamlView RawTextViewManager::CreateViewCore(int64_t tag) {
  winrt::Run run;
  return run;
}

void RawTextViewManager::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    const folly::dynamic &reactDiffMap) {
  auto run = nodeToUpdate->GetView().as<winrt::Run>();
  if (run == nullptr)
    return;

  for (const auto &pair : reactDiffMap.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (propertyName == "text") {
      run.Text(asHstring(propertyValue));
      std::shared_ptr<IReactInstance> instance =
          this->m_wkReactInstance.lock();
      auto nativeUI =
          static_cast<UwpReactInstance &>(*instance).NativeUIManager();
      auto host = static_cast<NativeUIManager &>(*nativeUI).getHost();
      auto parent = host->FindShadowNodeForTag(nodeToUpdate->GetParent());     
      if (parent != nullptr && parent->m_children.size() == 1) {
        auto view = static_cast<ShadowNodeBase &>(*parent).GetView();
        auto textBlock = view.try_as<winrt::TextBlock>();
        if (textBlock != nullptr) {
          textBlock.Text(run.Text());
        }        
      }
    }
  }
  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void RawTextViewManager::SetLayoutProps(
    ShadowNodeBase &nodeToUpdate,
    XamlView viewToUpdate,
    float left,
    float top,
    float width,
    float height) {}

bool RawTextViewManager::RequiresYogaNode() const {
  return false;
}

} // namespace uwp
} // namespace react
