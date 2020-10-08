// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
//

#include "pch.h"

#include "GlyphViewManager.h"

#include <UI.Xaml.Documents.h>
#include <Utils/ValueUtils.h>
#include <Views/ShadowNodeBase.h>

#include <winrt/Windows.UI.ViewManagement.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace xaml;
using namespace xaml::Documents;
using namespace xaml::Media;
} // namespace winrt

namespace react::uwp {

//
// GlyphShadowNode
//
class GlyphShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  GlyphShadowNode() = default;

  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;

 private:
  static void UpdateFontColorProps(xaml::Documents::Glyphs glyphs);

  std::optional<double> m_emSize;
  double m_height = 24;
};

void GlyphShadowNode::createView() {
  Super::createView();
  auto glyphs = GetView().as<winrt::Glyphs>();

  glyphs.FontRenderingEmSize(24);
  UpdateFontColorProps(glyphs);
}

void GlyphShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;
  auto glyphs = GetView().as<winrt::Glyphs>();
  bool updateEmSize = false;

  for (auto &pair : props.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (propertyName == "color") {
      if (IsValidColorValue(propertyValue))
        glyphs.Fill(BrushFrom(propertyValue));
#ifdef DEBUG
      else if (propertyValue.isNull()) {
        // Log error, must have a color
        YellowBox("GlyphShadowNode - color property must be non-null");
      }
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

/*static*/ void GlyphShadowNode::UpdateFontColorProps(winrt::Glyphs glyphs) {
  glyphs.IsColorFontEnabled(false);

  auto application = winrt::Application::Current();
  if (!winrt::Windows::UI::ViewManagement::AccessibilitySettings().HighContrast()) {
    // 0 - Light, 1 - Light Disabled, 2 - Dark, 3 - Dark Disabled
    glyphs.ColorFontPaletteIndex(application.RequestedTheme() == winrt::ApplicationTheme::Light ? 0 : 2);
  }

  // Set default Fill color
  auto appDictionary = application.Resources();
  auto color = winrt::Colors::Black();
  if (appDictionary.HasKey(winrt::box_value(L"SystemAccentColor")))
    color = winrt::unbox_value<winrt::Windows::UI::Color>(appDictionary.Lookup(winrt::box_value(L"SystemAccentColor")));

  glyphs.Fill(xaml::Media::SolidColorBrush(color));
}

//
// GlyphViewManager
//
GlyphViewManager::GlyphViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const char *GlyphViewManager::GetName() const {
  return "PLYIcon";
}

facebook::react::ShadowNode *GlyphViewManager::createShadow() const {
  return new GlyphShadowNode();
}

folly::dynamic GlyphViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(
      folly::dynamic::object("emSize", "number")("fontUri", "string")("glyph", "string")("colorEnabled", "boolean"));

  return props;
}

XamlView GlyphViewManager::CreateViewCore(int64_t /*tag*/) {
  winrt::Glyphs glyphs = winrt::Glyphs();
  return glyphs;
}

} // namespace react::uwp
