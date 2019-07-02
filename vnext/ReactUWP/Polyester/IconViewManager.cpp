// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// IconViewManager is a temporary implementation of render Icon from Font
// support final impl pending polyester design
//

#include "pch.h"

#include "IconViewManager.h"

#include <Utils/ValueUtils.h>
#include <Views/ShadowNodeBase.h>

#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Documents.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt

namespace react {
namespace uwp {
namespace polyester {

//
// IconShadowNode
//
class IconShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  IconShadowNode() = default;

  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;

 private:
  static void UpdateFontColorProps(
      winrt::Windows::UI::Xaml::Documents::Glyphs glyphs);

  std::optional<double> m_emSize;
  double m_height = 24;
};

void IconShadowNode::createView() {
  Super::createView();
  auto glyphs = GetView().as<winrt::Glyphs>();

  glyphs.FontRenderingEmSize(24);
  UpdateFontColorProps(glyphs);
}

void IconShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;
  auto glyphs = GetView().as<winrt::Glyphs>();
  bool updateEmSize = false;

  for (auto &pair : props.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (propertyName == "color") {
      if (IsValidColorValue(propertyValue))
        glyphs.Fill(BrushFrom(propertyValue));
#if FUTURE
      else if (propertyValue.isNull())
        ; // Log error, must have a color
#endif
    } else if (propertyName == "fontUri") {
      if (propertyValue.isString()) {
        auto uri = winrt::Uri(asHstring(propertyValue));
        glyphs.FontUri(uri);
      }
    } else if (propertyName == "glyph") {
      if (propertyValue.isString())
        glyphs.Indices(asHstring(propertyValue));
    } else if (propertyName == "colorEnabled") {
      if (propertyValue.isBool())
        glyphs.IsColorFontEnabled(propertyValue.asBool());
    } else if (propertyName == "height") {
      if (propertyValue.isNumber())
        m_height = propertyValue.asDouble();

      updateEmSize = true;
    } else if (propertyName == "emSize") {
      if (propertyValue.isNumber())
        m_emSize = propertyValue.asDouble();
      else
        m_emSize.reset();

      updateEmSize = true;
    }
  }

  if (updateEmSize) {
    // Default emSize to match height, unless its been explicitly set

    // FUTURE: enable some default scaling of 0.75 to convert height px to em?
    // The SegouUI Emoji font renders the full area of the emSize
    // where emSize should be height * 0.75, but office icon fonts
    // do not and applying a scale would be wrong

    double emSize = m_height;
    if (m_emSize.has_value())
      emSize = m_emSize.value();
    glyphs.FontRenderingEmSize(emSize);
  }

  Super::updateProperties(std::move(props));
  m_updating = false;
}

/*static*/ void IconShadowNode::UpdateFontColorProps(winrt::Glyphs glyphs) {
  glyphs.IsColorFontEnabled(false);

  auto application = winrt::Application::Current();
  if (!winrt::ViewManagement::AccessibilitySettings().HighContrast()) {
    // 0 - Light, 1 - Light Disabled, 2 - Dark, 3 - Dark Disabled
    glyphs.ColorFontPaletteIndex(
        application.RequestedTheme() == winrt::ApplicationTheme::Light ? 0 : 2);
  }

  // Set default Fill color
  auto appDictionary = application.Resources();
  auto color = winrt::Windows::UI::Colors::Black();
  if (appDictionary.HasKey(winrt::box_value(L"SystemAccentColor")))
    color = winrt::unbox_value<winrt::Windows::UI::Color>(
        appDictionary.Lookup(winrt::box_value(L"SystemAccentColor")));

  glyphs.Fill(winrt::SolidColorBrush(color));
}

//
// IconViewManager
//
IconViewManager::IconViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *IconViewManager::GetName() const {
  return "PLYIcon";
}

facebook::react::ShadowNode *IconViewManager::createShadow() const {
  return new IconShadowNode();
}

folly::dynamic IconViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("emSize", "number")("fontUri", "string")(
      "glyph", "string")("colorEnabled", "boolean"));

  return props;
}

XamlView IconViewManager::CreateViewCore(int64_t tag) {
  winrt::Glyphs glyphs = winrt::Glyphs();
  return glyphs;
}

} // namespace polyester
} // namespace uwp
} // namespace react
