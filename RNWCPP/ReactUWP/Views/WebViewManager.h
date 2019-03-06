// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>
#include "Utils/PropertyHandlerUtils.h"
#include <winrt/Windows.UI.Xaml.Media.h>

namespace winrt::Windows::UI::Xaml::Media { enum class Stretch; }

namespace react {
  namespace uwp {
    struct WebSource
    {
      std::string uri;
      bool packagerAsset;
    };
} }


template<>
struct json_type_traits<react::uwp::WebSource>
{
  static react::uwp::WebSource parseJson(folly::dynamic& json)
  {
    react::uwp::WebSource source;
    for (auto& item : json.items())
    {
      if (item.first == "uri")
        source.uri = item.second.asString();
      if (item.first == "__packager_asset")
        source.packagerAsset = item.second.asBool();
    }
    return source;
  }
};

namespace react { namespace uwp {

  class WebViewManager : public FrameworkElementViewManager
  {
    using Super = FrameworkElementViewManager;
  public:
    RCT_BEGIN_PROPERTY_MAP(WebViewManager)
      RCT_PROPERTY("source", setSource)
    RCT_END_PROPERTY_MAP()

    WebViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

    const char* GetName() const override;
    void UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap) override;

    folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
    folly::dynamic GetNativeProps() const override;

  protected:
    XamlView CreateViewCore(int64_t tag) override;

  private:
    void setSource(XamlView viewToUpdate, const WebSource& sources);
  };
} }
