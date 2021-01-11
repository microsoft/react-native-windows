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

namespace Microsoft::ReactNative {

//
// GlyphShadowNode
//
class GlyphShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  GlyphShadowNode() = default;

  void createView() override;
  void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) override;

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

void GlyphShadowNode::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {
  m_updating = true;
  auto glyphs = GetView().as<winrt::Glyphs>();
  bool updateEmSize = false;

  for (auto &pair : props) {
    const std::string &propertyName = pair.first;
    const auto &propertyValue = pair.second;

    if (propertyName == "color") {
      if (react::uwp::IsValidColorValue(propertyValue))
        glyphs.Fill(react::uwp::BrushFrom(propertyValue));
#ifdef DEBUG
      else if (propertyValue.IsNull()) {
        // Log error, must have a color
        YellowBox("GlyphShadowNode - color property must be non-null");
      }
#endif
    } else if (propertyName == "fontUri") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        auto uri = winrt::Uri(react::uwp::asHstring(propertyValue));
        glyphs.FontUri(uri);
      }
    } else if (propertyName == "glyph") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        glyphs.Indices(react::uwp::asHstring(propertyValue));
      }
    } else if (propertyName == "colorEnabled") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean)
        glyphs.IsColorFontEnabled(propertyValue.AsBoolean());
    } else if (propertyName == "height") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
        m_height = propertyValue.AsDouble();

      updateEmSize = true;
    } else if (propertyName == "emSize") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
        m_emSize = propertyValue.AsDouble();
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

  Super::updateProperties(props);
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

const wchar_t *GlyphViewManager::GetName() const {
  return L"PLYIcon";
}

ShadowNode *GlyphViewManager::createShadow() const {
  return new GlyphShadowNode();
}

void GlyphViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  writer.WritePropertyName(L"emSize");
  writer.WriteString(L"number");
  writer.WritePropertyName(L"fontUri");
  writer.WriteString(L"string");
  writer.WritePropertyName(L"glyph");
  writer.WriteString(L"string");
  writer.WritePropertyName(L"colorEnabled");
  writer.WriteString(L"boolean");
}

XamlView GlyphViewManager::CreateViewCore(int64_t /*tag*/) {
  winrt::Glyphs glyphs = winrt::Glyphs();
  return glyphs;
}

} // namespace Microsoft::ReactNative
