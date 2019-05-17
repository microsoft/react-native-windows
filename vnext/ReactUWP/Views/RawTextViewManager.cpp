// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "RawTextViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <Utils/ValueUtils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Media;
}

namespace react { namespace uwp {

RawTextViewManager::RawTextViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

const char* RawTextViewManager::GetName() const
{
  return "RCTRawText";
}

XamlView RawTextViewManager::CreateViewCore(int64_t tag)
{
  winrt::Run run;
  return run;
}

void RawTextViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap)
{
  auto run = nodeToUpdate->GetView().as<winrt::Run>();
  if (run == nullptr)
    return;

  for (auto& pair : reactDiffMap.items())
  {
    if (pair.first == "text")
    {
      run.Text(asHstring(pair.second));
    }
  }
  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void RawTextViewManager::SetLayoutProps(ShadowNodeBase& nodeToUpdate, XamlView viewToUpdate, float left, float top, float width, float height)
{
}

bool RawTextViewManager::RequiresYogaNode() const
{
  return false;
}

} }
