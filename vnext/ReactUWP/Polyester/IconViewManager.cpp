// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// IconViewManager is a temporary implementation of render Icon from Font support
// final impl pending polyester design
//

#include "pch.h"

#include "IconViewManager.h"

#include <Utils/ValueUtils.h>
#include <Views/ShadowNodeBase.h>

#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Media;
}

namespace react { namespace uwp { namespace polyester {

IconViewManager::IconViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

const char* IconViewManager::GetName() const
{
  return "PLYIcon";
}

folly::dynamic IconViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object
    ("fontUri", "string")
    ("glyph", "string")
    ("colorEnabled", "boolean")
  );

  return props;
}

XamlView IconViewManager::CreateViewCore(int64_t tag)
{
  winrt::Glyphs glyphs = winrt::Glyphs();
  glyphs.FontRenderingEmSize(24);
  UpdateFontColorProps(glyphs);

  return glyphs;
}

void IconViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap)
{
  auto glyphs = nodeToUpdate->GetView().as<winrt::Glyphs>();
  if (glyphs == nullptr)
    return;

  for (const auto& pair : reactDiffMap.items())
  {
    const std::string& propertyName = pair.first.getString();
    const folly::dynamic& propertyValue = pair.second;

    if (propertyName == "color")
    {
      if (propertyValue.isInt())
        glyphs.Fill(BrushFrom(propertyValue));
#if FUTURE
      else if (propertyValue.isNull())
        ; // Log error, must have a color
#endif
    }
    else if (propertyName == "fontUri")
    {
      if (propertyValue.isString())
      {
        auto uri = winrt::Uri(asHstring(propertyValue));
        glyphs.FontUri(uri);
      }
    }
    else if (propertyName == "glyph")
    {
      if (propertyValue.isString())
        glyphs.Indices(asHstring(propertyValue));
    }
    else if (propertyName == "colorEnabled")
    {
      if (propertyValue.isBool())
        glyphs.IsColorFontEnabled(propertyValue.asBool());
    }
    else if (propertyName == "height")
    {
      if (propertyValue.isNumber())
        glyphs.FontRenderingEmSize(propertyValue.asDouble());
    }

    continue;
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

/*static*/ void IconViewManager::UpdateFontColorProps(winrt::Glyphs glyphs)
{
  glyphs.IsColorFontEnabled(false);

  auto application = winrt::Application::Current();
  if (!winrt::ViewManagement::AccessibilitySettings().HighContrast())
  {
    // 0 - Light, 1 - Light Disabled, 2 - Dark, 3 - Dark Disabled
    glyphs.ColorFontPaletteIndex(application.RequestedTheme() == winrt::ApplicationTheme::Light ? 0 : 2);
  }

  // Set default Fill color
  auto appDictionary = application.Resources();
  auto color = winrt::Windows::UI::Colors::Black();
  if (appDictionary.HasKey(winrt::box_value(L"SystemAccentColor")))
    color = winrt::unbox_value<winrt::Windows::UI::Color>(appDictionary.Lookup(winrt::box_value(L"SystemAccentColor")));

  glyphs.Fill(winrt::SolidColorBrush(color));
}

}}}
